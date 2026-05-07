# Refactoring Plan: gilded_rose.cpp

## 목표

현재 `updateQuality()`의 중첩 if-else 구조를 단계적으로 개선한다.  
각 단계는 테스트를 통과한 상태에서만 다음으로 진행한다.

---

## 각 Phase 작업 순서

모든 Phase는 아래 순서를 반드시 따른다:

1. **Design 문서 작성** — 해당 Phase의 변경 내용을 `docs/design/phaseN.md`에 상세히 기술
2. **사람이 Design 문서 검토** — 진행 전 승인 필요
3. **구현** — Design 문서에 따라 코드 작성
4. **사람이 코드 검토** — 완료 전 승인 필요

---

## Phase 0 — 안전망 구축: Unit Test 작성

> Design 문서: `docs/phase0_design.md`

리팩터링 전 기존 동작을 모두 테스트로 고정한다.  
`gilded_rose_test.cpp`에 아래 케이스를 추가.

### 일반 아이템 (Normal Item)

| 케이스 | 초기값 | 기대 결과 |
|--------|--------|-----------|
| 하루 경과 후 quality/sellIn 각 1 감소 | sellIn=10, quality=20 | sellIn=9, quality=19 |
| sellIn=0 경과 후 quality 2배 감소 | sellIn=0, quality=10 | sellIn=-1, quality=8 |
| quality는 0 미만으로 내려가지 않음 | sellIn=0, quality=0 | quality=0 |
| quality는 0 미만으로 내려가지 않음 (sellIn 양수) | sellIn=5, quality=0 | quality=0 |

### Aged Brie

| 케이스 | 초기값 | 기대 결과 |
|--------|--------|-----------|
| 하루 경과 후 quality 1 증가 | sellIn=5, quality=10 | quality=11 |
| sellIn 만료 후 quality 2 증가 | sellIn=0, quality=10 | quality=12 |
| quality 50 초과 불가 | sellIn=5, quality=50 | quality=50 |

### Sulfuras

| 케이스 | 초기값 | 기대 결과 |
|--------|--------|-----------|
| quality/sellIn 변화 없음 | sellIn=5, quality=80 | sellIn=5, quality=80 |
| sellIn 음수여도 변화 없음 | sellIn=-1, quality=80 | sellIn=-1, quality=80 |

### Backstage passes

| 케이스 | 초기값 | 기대 결과 |
|--------|--------|-----------|
| sellIn > 10 → quality +1 | sellIn=15, quality=20 | quality=21 |
| sellIn = 10 → quality +2 | sellIn=10, quality=20 | quality=22 |
| sellIn = 5 → quality +3 | sellIn=5, quality=20 | quality=23 |
| sellIn = 0 → quality = 0 | sellIn=0, quality=20 | quality=0 |
| quality 50 초과 불가 | sellIn=5, quality=49 | quality=50 (cap at 50) |

### Conjured (미구현 → 테스트 먼저 작성, Phase 4에서 구현)

| 케이스 | 초기값 | 기대 결과 |
|--------|--------|-----------|
| 하루 경과 후 quality 2 감소 | sellIn=5, quality=10 | quality=8 |
| sellIn 만료 후 quality 4 감소 | sellIn=0, quality=10 | quality=6 |
| quality는 0 미만으로 내려가지 않음 | sellIn=0, quality=1 | quality=0 |

---

## Phase 1 — 소규모: 매직 스트링 상수화

> Design 문서: `docs/phase1_design.md`

**변경 범위**: `gilded_rose.cpp` 상단에 상수 추가, 문자열 리터럴 교체  
**리스크**: 최소 (동작 변경 없음)

```cpp
static const string AGED_BRIE        = "Aged Brie";
static const string SULFURAS         = "Sulfuras, Hand of Ragnaros";
static const string BACKSTAGE_PASS   = "Backstage passes to a TAFKAL80ETC concert";
static const string CONJURED         = "Conjured";
static const int    MAX_QUALITY      = 50;
static const int    MIN_QUALITY      = 0;
static const int    SULFURAS_QUALITY = 80;
```

**검증**: 전체 테스트 통과 확인

---

## Phase 2 — 소규모: quality 경계값 처리 헬퍼 추출

> Design 문서: `docs/phase2_design.md`

**변경 범위**: `gilded_rose.cpp`에 인라인 헬퍼 함수 2개 추가  
**리스크**: 최소 (로직 동일, 중복 제거)

```cpp
static void increaseQuality(Item& item) {
    if (item.quality < MAX_QUALITY) item.quality++;
}
static void decreaseQuality(Item& item) {
    if (item.quality > MIN_QUALITY) item.quality--;
}
```

`updateQuality()` 내부의 직접 산술 연산을 모두 헬퍼로 교체.

**검증**: 전체 테스트 통과 확인

---

## Phase 3 — 중규모: 아이템 유형별 메서드 분리

> Design 문서: `docs/phase3_design.md`

**변경 범위**: `updateQuality()` 루프 내부를 아이템별 private 메서드로 추출  
**리스크**: 중간 (로직 분리, 동작은 동일)

```cpp
// gilded_rose.h 에 추가
private:
    void updateNormal(Item& item);
    void updateAgedBrie(Item& item);
    void updateSulfuras(Item& item);
    void updateBackstagePass(Item& item);
    void updateConjured(Item& item);
```

`updateQuality()` 루프는 dispatcher로 단순화:

```cpp
void GildedRose::updateQuality() {
    for (auto& item : items) {
        if      (item.name == SULFURAS)       updateSulfuras(item);
        else if (item.name == AGED_BRIE)      updateAgedBrie(item);
        else if (item.name == BACKSTAGE_PASS) updateBackstagePass(item);
        else if (item.name == CONJURED)       updateConjured(item);
        else                                  updateNormal(item);
    }
}
```

**검증**: 전체 테스트 통과 + 커버리지 재측정

---

## Phase 4 — 대규모: Conjured 아이템 구현

> Design 문서: `docs/phase4_design.md`

**변경 범위**: `updateConjured()` 메서드 구현  
**리스크**: 낮음 (새 기능 추가, 기존 코드 불변)

Phase 0에서 작성해 둔 Conjured 테스트가 이 단계에서 통과되어야 한다.

```cpp
void GildedRose::updateConjured(Item& item) {
    decreaseQuality(item);
    decreaseQuality(item);
    item.sellIn--;
    if (item.sellIn < 0) {
        decreaseQuality(item);
        decreaseQuality(item);
    }
}
```

**검증**: Conjured 테스트 통과 + 전체 테스트 통과 + 커버리지 재측정

---

## 진행 원칙

1. 각 Phase는 반드시 **Design → 검토 → 구현 → 검토** 순서를 따른다.
2. 각 Phase 구현 완료 후 전체 테스트를 실행한다.
3. 테스트가 실패하면 해당 Phase에서 멈추고 원인을 수정한다.
4. `Item` 클래스와 `items` 속성은 수정하지 않는다.
5. 커버리지는 Phase 3, 4 완료 후 재측정하여 비교한다.
