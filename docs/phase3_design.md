# Phase 3 Design: 아이템 유형별 메서드 분리

## 목적

`updateQuality()`의 중첩 if-else 블록을 아이템 유형별 private 메서드로 분리한다.  
`updateQuality()`는 아이템 이름을 보고 적절한 메서드를 호출하는 dispatcher로 단순화한다.

---

## 변경 대상

### 파일: `GildedRose/gilded_rose.h`

`GildedRose` 클래스에 private 메서드 선언 추가:

```cpp
private:
    void updateNormal(Item& item);
    void updateAgedBrie(Item& item);
    void updateSulfuras(Item& item);
    void updateBackstagePass(Item& item);
    void updateConjured(Item& item);
```

### 파일: `GildedRose/gilded_rose.cpp`

#### updateQuality() — dispatcher로 단순화

```cpp
void GildedRose::updateQuality()
{
    for (auto& item : items)
    {
        if      (item.name == SULFURAS)       updateSulfuras(item);
        else if (item.name == AGED_BRIE)      updateAgedBrie(item);
        else if (item.name == BACKSTAGE_PASS) updateBackstagePass(item);
        else if (item.name == CONJURED)       updateConjured(item);
        else                                  updateNormal(item);
    }
}
```

#### 각 메서드 구현

```cpp
void GildedRose::updateNormal(Item& item)
{
    decreaseQuality(item);
    item.sellIn--;
    if (item.sellIn < 0)
        decreaseQuality(item);
}

void GildedRose::updateAgedBrie(Item& item)
{
    increaseQuality(item);
    item.sellIn--;
    if (item.sellIn < 0)
        increaseQuality(item);
}

void GildedRose::updateSulfuras(Item& item)
{
    // sellIn, quality 모두 변경하지 않음
}

void GildedRose::updateBackstagePass(Item& item)
{
    increaseQuality(item);
    if (item.sellIn < 11) increaseQuality(item);
    if (item.sellIn < 6)  increaseQuality(item);
    item.sellIn--;
    if (item.sellIn < 0)
        item.quality = MIN_QUALITY;
}

void GildedRose::updateConjured(Item& item)
{
    // Phase 4에서 구현 — 현재는 일반 아이템과 동일하게 동작
    decreaseQuality(item);
    item.sellIn--;
    if (item.sellIn < 0)
        decreaseQuality(item);
}
```

> `updateConjured`는 Phase 4 전까지 `updateNormal`과 동일하게 동작하여  
> Conjured 테스트는 Phase 3에서도 동일하게 실패 상태를 유지한다.

---

## 변경하지 않는 것

- `Item` 클래스 — 변경 없음
- `gilded_rose_test.cpp` — 테스트 코드 변경 없음
- 비즈니스 로직 — 동작 변경 없음

---

## 완료 기준

- 빌드 성공
- 16개 통과, Conjured 2개 실패 유지
