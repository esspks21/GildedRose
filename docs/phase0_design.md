# Phase 0 Design: Unit Test 작성

## 목적

리팩터링 전 기존 동작을 테스트로 고정하여 안전망을 구축한다.  
이후 모든 Phase에서 이 테스트가 통과하는 상태를 유지한다.

---

## 대상 파일

`GildedRose/gilded_rose_test.cpp`  
기존 `GildedRoseTest.Foo` 테스트는 유지하고, 아래 테스트를 추가한다.

---

## 테스트 케이스 설계

### 1. NormalItemTest — 일반 아이템

```cpp
TEST(NormalItemTest, QualityAndSellInDecreaseByOnePerDay)
// sellIn=10, quality=20 → sellIn=9, quality=19

TEST(NormalItemTest, QualityDecreasesTwiceAfterSellInExpires)
// sellIn=0, quality=10 → sellIn=-1, quality=8

TEST(NormalItemTest, QualityNeverGoesBelowZero_WhenSellInPositive)
// sellIn=5, quality=0 → quality=0

TEST(NormalItemTest, QualityNeverGoesBelowZero_WhenSellInExpired)
// sellIn=0, quality=0 → quality=0
```

### 2. AgedBrieTest

```cpp
TEST(AgedBrieTest, QualityIncreasesByOnePerDay)
// sellIn=5, quality=10 → quality=11

TEST(AgedBrieTest, QualityIncreasesByTwoAfterSellInExpires)
// sellIn=0, quality=10 → quality=12

TEST(AgedBrieTest, QualityNeverExceedsFifty)
// sellIn=5, quality=50 → quality=50
```

### 3. SulfurasTest

```cpp
TEST(SulfurasTest, QualityAndSellInNeverChange)
// sellIn=5, quality=80 → sellIn=5, quality=80

TEST(SulfurasTest, NeverChangesEvenWhenSellInIsNegative)
// sellIn=-1, quality=80 → sellIn=-1, quality=80
```

### 4. BackstagePassTest

```cpp
TEST(BackstagePassTest, QualityIncreasesByOneWhenSellInAboveTen)
// sellIn=15, quality=20 → quality=21

TEST(BackstagePassTest, QualityIncreasesByTwoWhenSellInIsTen)
// sellIn=10, quality=20 → quality=22

TEST(BackstagePassTest, QualityIncreasesByThreeWhenSellInIsFive)
// sellIn=5, quality=20 → quality=23

TEST(BackstagePassTest, QualityDropsToZeroAfterConcert)
// sellIn=0, quality=20 → quality=0

TEST(BackstagePassTest, QualityNeverExceedsFifty)
// sellIn=5, quality=49 → quality=50
```

### 5. ConjuredTest — 미구현 (테스트만 작성, Phase 4에서 구현)

Phase 4 구현 전까지 실패 상태로 유지한다.  
`EXPECT_EQ`에 기대값을 명시하여 실패 원인을 명확히 드러낸다.

```cpp
TEST(ConjuredTest, QualityDecreasesByTwoPerDay)
// sellIn=5, quality=10 → quality=8

TEST(ConjuredTest, QualityDecreasesByFourAfterSellInExpires)
// sellIn=0, quality=10 → quality=6

TEST(ConjuredTest, QualityNeverGoesBelowZero)
// sellIn=0, quality=1 → quality=0
```

---

## 테스트 작성 규칙

- Arrange / Act / Assert 주석으로 구분한다.
- 아이템 이름은 실제 문자열 그대로 사용한다 (상수는 Phase 1에서 도입).
- 테스트 하나당 검증 대상은 하나(`quality` 또는 `sellIn`)로 한정한다.

---

## 완료 기준

- Conjured 테스트 3개를 제외한 **17개 테스트 전체 통과**
- Conjured 테스트 3개는 **명확한 실패** 상태 확인
