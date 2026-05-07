# Phase 4 Design: Conjured 아이템 구현

## 목적

Phase 0에서 작성해 둔 Conjured 테스트를 통과시킨다.  
`updateConjured()` 메서드에 올바른 로직을 구현한다.

---

## 비즈니스 규칙

- 일반 아이템의 **2배** 속도로 quality가 감소한다.
- sellIn이 0 이하일 때는 추가로 **2배** 감소 (일반 아이템 만료 시 2배 감소의 2배).
- quality는 0 미만으로 내려가지 않는다.

| 상황 | quality 변화 |
|------|-------------|
| sellIn > 0 | -2 |
| sellIn ≤ 0 (만료) | -4 |

---

## 변경 대상

### 파일: `GildedRose/gilded_rose.cpp`

#### 현재 (Phase 3 임시 구현)

```cpp
void GildedRose::updateConjured(Item& item)
{
    decreaseQuality(item);
    item.sellIn--;
    if (item.sellIn < 0)
        decreaseQuality(item);
}
```

#### 변경 후

```cpp
void GildedRose::updateConjured(Item& item)
{
    decreaseQuality(item);
    decreaseQuality(item);
    item.sellIn--;
    if (item.sellIn < 0)
    {
        decreaseQuality(item);
        decreaseQuality(item);
    }
}
```

---

## 변경하지 않는 것

- `gilded_rose.h` — 변경 없음
- `gilded_rose_test.cpp` — 테스트 코드 변경 없음
- 다른 아이템 유형 메서드 — 변경 없음

---

## 완료 기준

- 빌드 성공
- 기존 16개 통과 유지
- Conjured 테스트 3개 **모두 통과** (총 18개 통과)
