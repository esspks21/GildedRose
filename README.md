# GildedRose Refactoring

레거시 코드를 단계적으로 개선하며 테스트 커버리지 100%를 달성한 C++ 리팩터링 프로젝트입니다.

---

## 리팩터링 전 문제점

`updateQuality()` 하나의 함수가 모든 아이템 규칙을 처리하며, 조건문이 5단계까지 중첩되어 있었습니다.

```cpp
if (name != "Aged Brie" && name != "Backstage passes to a TAFKAL80ETC concert") {
    if (quality > 0) {
        if (name != "Sulfuras, Hand of Ragnaros") {
            quality = quality - 1;
        }
    }
} else {
    if (quality < 50) {
        quality = quality + 1;
        if (name == "Backstage passes to a TAFKAL80ETC concert") {
            if (sellIn < 11) {
                if (quality < 50) { quality = quality + 1; }
            }
            ...
```

- 문자열 리터럴(`"Aged Brie"`, `"Sulfuras, Hand of Ragnaros"` 등)이 코드 여러 곳에 중복
- 품질 상한(50) · 하한(0) 체크 코드가 반복
- Conjured 아이템 미구현

---

## Phase 0 — 테스트 안전망 구축

**포인트**: 코드를 바꾸기 전, 현재 동작을 테스트로 고정해 리팩터링 중 실수를 즉시 감지할 수 있도록 합니다.

아이템 유형별로 18개의 테스트를 작성했습니다. Conjured 테스트 3개는 미구현 상태를 확인하기 위해 **의도적으로 실패** 상태로 작성했습니다.

```
커버리지: 60% → 리팩터링 전 테스트가 1개뿐이라 절반 가까운 코드가 검증되지 않은 상태
```

---

## Phase 1 — 매직 스트링 · 숫자 상수화

**포인트**: 동일한 문자열과 숫자가 코드 여러 곳에 흩어져 있으면, 하나를 바꿀 때 나머지를 빠뜨리기 쉽습니다.

```cpp
// Before
if (items[i].name != "Aged Brie" && ...)
if (items[i].quality < 50)

// After
static const string AGED_BRIE   = "Aged Brie";
static const string SULFURAS    = "Sulfuras, Hand of Ragnaros";
static const string BACKSTAGE_PASS = "Backstage passes to a TAFKAL80ETC concert";
static const string CONJURED    = "Conjured";
static const int    MAX_QUALITY = 50;
static const int    MIN_QUALITY = 0;

if (items[i].name != AGED_BRIE && ...)
if (items[i].quality < MAX_QUALITY)
```

동작 변경 없이 오타와 누락 가능성을 제거했습니다.

---

## Phase 2 — 품질 증감 헬퍼 함수 추출

**포인트**: 품질 경계값(0~50) 체크 코드가 총 7곳에서 반복됩니다. 함수로 묶으면 경계값 규칙을 한 곳에서만 관리할 수 있습니다.

```cpp
// Before — 품질을 올릴 때마다 경계값 체크를 매번 직접 작성
if (items[i].quality < 50)
    items[i].quality = items[i].quality + 1;

// After — 체크가 함수 내부로 이동, 호출부가 단순해짐
static void increaseQuality(Item& item) {
    if (item.quality < MAX_QUALITY) item.quality++;
}
static void decreaseQuality(Item& item) {
    if (item.quality > MIN_QUALITY) item.quality--;
}

increaseQuality(items[i]);  // 호출부에서 체크 불필요
```

또한 `items[i].quality = items[i].quality - items[i].quality`(품질을 0으로 만드는 코드)를 `items[i].quality = MIN_QUALITY`로 의도를 명확히 했습니다.

---

## Phase 3 — 아이템 유형별 함수 분리

**포인트**: 하나의 함수가 5가지 아이템 규칙을 모두 담당하면, 특정 아이템의 규칙을 파악하기 위해 전체 코드를 읽어야 합니다.

```cpp
// Before — updateQuality() 하나에 모든 규칙이 뒤섞임
void updateQuality() {
    for (...) {
        if (name != AGED_BRIE && name != BACKSTAGE_PASS) {
            if (name != SULFURAS) { ... }
        } else {
            if (name == BACKSTAGE_PASS) { ... }
        }
        if (name != SULFURAS) { sellIn--; }
        if (sellIn < 0) {
            if (name != AGED_BRIE) { ... }
            else { ... }
        }
    }
}

// After — dispatcher + 아이템별 전담 함수
void updateQuality() {
    for (auto& item : items) {
        if      (item.name == SULFURAS)       updateSulfuras(item);
        else if (item.name == AGED_BRIE)      updateAgedBrie(item);
        else if (item.name == BACKSTAGE_PASS) updateBackstagePass(item);
        else if (item.name == CONJURED)       updateConjured(item);
        else                                  updateNormal(item);
    }
}
```

각 아이템의 규칙이 독립된 함수로 분리되어, Aged Brie 규칙을 보려면 `updateAgedBrie()`만 읽으면 됩니다.

---

## Phase 4 — Conjured 아이템 구현

**포인트**: Phase 3으로 구조가 명확해진 덕분에 새 기능 추가가 단 몇 줄로 끝났습니다.

```cpp
// Before — updateConjured() 미구현 (일반 아이템과 동일하게 동작)
void updateConjured(Item& item) {
    decreaseQuality(item);
    item.sellIn--;
    if (item.sellIn < 0) decreaseQuality(item);
}

// After — 일반 아이템의 2배 속도로 품질 감소
void updateConjured(Item& item) {
    decreaseQuality(item);
    decreaseQuality(item);       // 하루 -2
    item.sellIn--;
    if (item.sellIn < 0) {
        decreaseQuality(item);
        decreaseQuality(item);   // 만료 후 추가 -2 (총 -4)
    }
}
```

Phase 0에서 미리 작성해 둔 Conjured 테스트 3개가 모두 통과됩니다.

---

## 커버리지 변화

| 단계 | 커버리지 | 통과 테스트 |
|------|---------|------------|
| 리팩터링 전 | 60% | 1개 |
| Phase 0~4 완료 | 98% | 18개 |
| print_item 테스트 추가 | **100%** | 19개 |

98%에서 멈춘 이유는 시연용 출력 함수(`print_item`)가 테스트 실행 중 한 번도 호출되지 않아서였습니다. 해당 함수에 대한 테스트를 추가해 100%를 달성했습니다.

---

## 핵심 교훈

1. **테스트 먼저** — 코드를 바꾸기 전에 테스트를 작성해 두면, 리팩터링 중 실수를 즉시 잡을 수 있다.
2. **작은 단계씩** — 한 번에 크게 바꾸지 않고, 각 단계마다 테스트로 검증하며 진행한다.
3. **구조가 명확해지면 확장이 쉬워진다** — Phase 3 이후 Conjured 구현이 단 몇 줄로 끝났다.
