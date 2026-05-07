# Phase 2 Design: quality 경계값 처리 헬퍼 추출

## 목적

`updateQuality()` 내부에서 반복되는 quality 증감 산술 연산과 경계값 체크를  
헬퍼 함수로 추출하여 중복을 제거한다.  
동작 변경 없이 코드의 의도를 명확하게 한다.

---

## 변경 대상

### 파일: `GildedRose/gilded_rose.cpp`

#### 추가할 헬퍼 함수 (상수 선언 아래에 위치)

```cpp
static void increaseQuality(Item& item) {
    if (item.quality < MAX_QUALITY) item.quality++;
}

static void decreaseQuality(Item& item) {
    if (item.quality > MIN_QUALITY) item.quality--;
}
```

#### 교체 위치

| 기존 코드 | 교체 후 |
|-----------|---------|
| `if (items[i].quality < MAX_QUALITY) items[i].quality = items[i].quality + 1;` | `increaseQuality(items[i]);` |
| `if (items[i].quality > MIN_QUALITY) items[i].quality = items[i].quality - 1;` | `decreaseQuality(items[i]);` |
| `items[i].quality = items[i].quality - items[i].quality;` | `items[i].quality = MIN_QUALITY;` |

경계값 체크가 헬퍼 내부로 이동하므로, 호출부의 `if (quality < MAX_QUALITY)` /  
`if (quality > MIN_QUALITY)` 조건문을 제거한다.

---

## 변경하지 않는 것

- `gilded_rose.h` — 헤더 변경 없음 (헬퍼는 `.cpp` 내부 static 함수)
- `gilded_rose_test.cpp` — 테스트 코드 변경 없음
- `updateQuality()`의 로직 흐름 — 동작 변경 없음

---

## 완료 기준

- 빌드 성공
- Phase 1과 동일하게 16개 통과, Conjured 2개 실패 유지
