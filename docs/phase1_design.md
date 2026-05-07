# Phase 1 Design: 매직 스트링 상수화

## 목적

`updateQuality()` 내부에 흩어진 문자열 리터럴과 숫자 리터럴을 상수로 교체한다.  
동작 변경 없이 가독성을 높이고, 이후 Phase에서 상수를 재사용할 수 있는 기반을 마련한다.

---

## 변경 대상

### 파일: `GildedRose/gilded_rose.cpp`

#### 추가할 상수 (`updateQuality()` 위에 선언)

```cpp
static const string AGED_BRIE        = "Aged Brie";
static const string SULFURAS         = "Sulfuras, Hand of Ragnaros";
static const string BACKSTAGE_PASS   = "Backstage passes to a TAFKAL80ETC concert";
static const string CONJURED         = "Conjured";
static const int    MAX_QUALITY      = 50;
static const int    MIN_QUALITY      = 0;
```

#### 교체 위치

| 기존 리터럴 | 교체 상수 |
|------------|----------|
| `"Aged Brie"` (3곳) | `AGED_BRIE` |
| `"Backstage passes to a TAFKAL80ETC concert"` (2곳) | `BACKSTAGE_PASS` |
| `"Sulfuras, Hand of Ragnaros"` (3곳) | `SULFURAS` |
| `50` (3곳) | `MAX_QUALITY` |
| `0` (quality 비교 시 2곳) | `MIN_QUALITY` |

---

## 변경하지 않는 것

- `gilded_rose.h` — `Item`, `GildedRose` 클래스 구조 변경 없음
- `gilded_rose_test.cpp` — 테스트 코드 변경 없음 (상수는 Phase 1에서 `.cpp` 내부에만 선언)
- `updateQuality()`의 로직 흐름 — 동작 변경 없음

---

## 완료 기준

- 빌드 성공
- Phase 0에서 통과한 16개 테스트 동일하게 통과
- Conjured 2개 동일하게 실패 유지
