# GildedRose Refactoring

> 레거시 코드를 단계적으로 개선하며 테스트 커버리지 100%를 달성한 C++ 리팩터링 프로젝트

---

## 프로젝트 개요

**Gilded Rose**는 판타지 상점의 재고 관리 시스템입니다.  
상점에는 다양한 아이템이 있고, 매일 `updateQuality()`라는 함수가 실행되어 각 아이템의 **판매 기한(sellIn)**과 **품질(quality)**을 갱신합니다.

이 프로젝트는 복잡하게 얽힌 기존 코드를 **읽기 쉽고 안전한 코드**로 개선하는 리팩터링 과정을 담고 있습니다.

---

## 아이템 규칙

| 아이템 | 동작 |
|--------|------|
| 일반 아이템 | 매일 품질 -1, 판매 기한 만료 후 -2 |
| Aged Brie (오래된 치즈) | 시간이 지날수록 품질 상승 |
| Sulfuras (전설 아이템) | 품질과 기한이 변하지 않음 |
| Backstage passes (백스테이지 입장권) | 콘서트가 가까워질수록 품질 급상승, 콘서트 후 품질 0 |
| Conjured (마법 아이템) | 일반 아이템의 2배 속도로 품질 감소 |

> **공통 제약**: 품질(quality)은 0 미만이 될 수 없고, 50을 초과할 수 없습니다.  
> (Sulfuras는 예외로 품질이 항상 80)

---

## 리팩터링 전 문제점

기존 `updateQuality()` 함수는 아래와 같이 **중첩된 if-else**가 5단계까지 쌓여 있었습니다.

```
if (아이템이 Aged Brie도 아니고 Backstage passes도 아니라면)
    if (품질이 0보다 크다면)
        if (Sulfuras가 아니라면)
            품질 감소
else
    if (품질이 50보다 작다면)
        품질 증가
        if (Backstage passes라면)
            if (sellIn이 11보다 작다면)
                ...
```

- 한 함수가 **모든 아이템의 규칙을 한꺼번에** 처리
- 조건문이 중첩되어 어느 경우에 무슨 동작이 일어나는지 **파악하기 어려움**
- `"Aged Brie"`, `"Sulfuras, Hand of Ragnaros"` 같은 긴 문자열이 코드 곳곳에 **반복** 등장
- **Conjured 아이템 미구현** 상태

---

## 리팩터링 과정 (Phase별)

### Phase 0 — 안전망 구축: 테스트 먼저 작성

> "코드를 바꾸기 전에 먼저 현재 동작이 맞는지 확인할 수 있는 테스트를 만든다"

리팩터링 중 실수로 동작이 바뀌더라도 즉시 감지할 수 있도록,  
모든 아이템 유형에 대한 **18개의 테스트**를 먼저 작성했습니다.

| 테스트 그룹 | 테스트 수 |
|------------|---------|
| 일반 아이템 | 4개 |
| Aged Brie | 3개 |
| Sulfuras | 2개 |
| Backstage passes | 5개 |
| Conjured (미구현 확인용) | 3개 |

이 단계에서 Conjured 테스트 2개는 **의도적으로 실패** 상태로 유지했습니다.  
구현이 없으니 당연히 실패해야 맞습니다.

---

### Phase 1 — 매직 스트링 상수화

> "코드 여기저기에 같은 문자열이 반복되면, 나중에 하나를 바꿀 때 다른 곳을 빠뜨리기 쉽다"

```
변경 전: items[i].name != "Aged Brie"  (여러 곳에 반복)
변경 후: items[i].name != AGED_BRIE    (상수 하나로 관리)
```

아이템 이름과 숫자(50, 0)를 모두 이름 있는 상수로 교체했습니다.  
**동작은 전혀 바뀌지 않고**, 코드의 의도가 명확해집니다.

---

### Phase 2 — 품질 증감 헬퍼 함수 추출

> "품질이 50을 넘지 않도록 체크하는 코드가 여러 곳에 복붙되어 있다면, 하나의 함수로 묶자"

```
변경 전:
  if (items[i].quality < 50)
      items[i].quality = items[i].quality + 1;  ← 여러 곳에 반복

변경 후:
  increaseQuality(item);  ← 경계값 체크가 함수 안에 숨겨짐
  decreaseQuality(item);
```

`increaseQuality()` / `decreaseQuality()` 두 함수가 품질 경계값(0~50)을 자동으로 지켜주므로,  
호출하는 쪽에서 매번 체크할 필요가 없어졌습니다.

---

### Phase 3 — 아이템 유형별 함수 분리

> "하나의 함수가 너무 많은 일을 하면, 아이템 하나의 규칙을 파악하기 위해 전체 코드를 읽어야 한다"

`updateQuality()`를 아이템 유형별 함수로 분리했습니다:

```
updateNormal()       ← 일반 아이템 규칙만
updateAgedBrie()     ← Aged Brie 규칙만
updateSulfuras()     ← Sulfuras 규칙만 (아무것도 안 함)
updateBackstagePass() ← Backstage passes 규칙만
updateConjured()     ← Conjured 규칙만
```

`updateQuality()`는 이제 아이템 이름을 보고 해당 함수를 호출하는 **교통 정리** 역할만 합니다:

```
아이템 종류에 따라 →  각자의 담당 함수 호출
```

---

### Phase 4 — Conjured 아이템 구현

> "이제 코드 구조가 명확해졌으니, 새 기능을 추가하기 쉬워졌다"

`updateConjured()` 함수 내부를 채웠습니다.  
일반 아이템의 2배 속도로 품질이 감소하므로 `decreaseQuality()`를 두 번 호출합니다.

Phase 0에서 미리 작성해 둔 Conjured 테스트 3개가 이 단계에서 모두 통과됩니다.

---

## 커버리지 변화

> **커버리지(Coverage)**란 테스트가 실행하는 코드의 비율입니다.  
> 100%에 가까울수록 테스트가 코드 전체를 검증하고 있다는 의미입니다.

| 단계 | 커버리지 | 통과 테스트 |
|------|---------|------------|
| 리팩터링 전 (테스트 1개) | **60%** | 1개 |
| Phase 0~4 완료 | **98%** | 18개 |
| print_item 테스트 추가 | **100%** | 19개 |

### 왜 Phase 0~4 후에도 2%가 남았나?

98%에서 미달된 2%는 비즈니스 로직과 무관한 **시연용 출력 함수** (`print_item`)였습니다.  
이 함수는 콘솔에 아이템 정보를 출력하는 역할인데, 테스트 실행 시에는 호출되지 않는 구조였습니다.  
별도 테스트를 추가하여 최종 **100%**를 달성했습니다.

---

## 파일 구조

```
GildedRose/
├── GildedRose/
│   ├── gilded_rose.h          # Item, GildedRose 클래스 선언
│   ├── gilded_rose.cpp        # 리팩터링된 핵심 로직
│   ├── gilded_rose_test.cpp   # 19개 단위 테스트
│   └── main.cpp               # 진입점 (Debug: 테스트 러너 / Release: 시연)
├── docs/
│   ├── plan.md                # 전체 리팩터링 계획
│   ├── phase0_design.md       # 각 Phase 설계 문서
│   ├── phase1_design.md
│   ├── phase2_design.md
│   ├── phase3_design.md
│   └── phase4_design.md
└── coverage/                  # 커버리지 리포트 (index.html로 확인)
```

---

## 빌드 및 테스트 실행

```bash
# 빌드 (Visual Studio 2022)
msbuild GildedRose.slnx /p:Configuration=Debug /p:Platform=x64

# 테스트 실행
.\x64\Debug\GildedRose.exe

# 커버리지 측정
OpenCppCoverage.exe --sources C:\*.cpp --export_type=html:coverage -- .\x64\Debug\GildedRose.exe
```

커버리지 결과는 `coverage/index.html`을 브라우저로 열어 확인합니다.

---

## 핵심 교훈

1. **테스트 먼저** — 코드를 바꾸기 전에 현재 동작을 테스트로 고정하면 리팩터링이 안전해진다.
2. **작은 단계씩** — 한 번에 크게 바꾸지 않고, 각 단계마다 테스트로 검증하며 진행한다.
3. **구조가 명확해지면 새 기능 추가가 쉬워진다** — Phase 3 이후 Conjured 구현이 단 몇 줄로 끝났다.
