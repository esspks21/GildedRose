# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 리팩터링 계획

진행 중인 리팩터링 계획은 [docs/plan.md](docs/plan.md)를 참조한다.

### 작업 절차

각 Phase는 아래 순서를 반드시 따른다:

1. **Design 문서 작성** — `docs/phaseN_design.md`에 변경 내용을 상세히 기술
2. **사람이 Design 문서 검토** — 승인 후 구현 진행
3. **구현** — Design 문서에 따라 코드 작성
4. **사람이 코드 검토** — 승인 후 다음 Phase 진행

Design 문서와 구현은 사람의 검토·승인 없이 독립적으로 진행하지 않는다.

## Build

Visual Studio 2022 솔루션 파일(`GildedRose.slnx`)을 사용한다. MSBuild로 빌드:

```
msbuild GildedRose.slnx /p:Configuration=Debug /p:Platform=x64
```

또는 Visual Studio에서 솔루션을 열어 빌드(Ctrl+Shift+B).

## 테스트 실행

`Debug` 빌드 시 `main()`이 Google Mock 테스트 러너로 동작한다(`#ifdef _DEBUG` 분기).

```
.\x64\Debug\GildedRose.exe
```

테스트는 `GildedRose/gilded_rose_test.cpp`에 작성하며, `TEST(GildedRoseTest, ...)` 매크로를 사용한다.

## 커버리지 측정

OpenCppCoverage를 사용하여 코드 커버리지를 측정한다:

```
OpenCppCoverage.exe --sources C:\*.cpp --export_type=html:coverage -- .\x64\Debug\GildedRose.exe
```

커버리지 결과 리포트는 `.\coverage\` 디렉터리의 HTML 파일을 브라우저로 열어 확인한다.

## 아키텍처

핵심 도메인은 두 클래스로 구성된다:

- **`Item`** (`gilded_rose.h`): `name`, `sellIn`, `quality` 필드를 가진 단순 데이터 클래스. **절대 수정 금지** (고블린 소유).
- **`GildedRose`** (`gilded_rose.h` / `gilded_rose.cpp`): `items` 벡터의 레퍼런스를 보유하고, `updateQuality()`로 하루치 상태를 갱신한다.

`updateQuality()` 로직은 아이템 이름 문자열로 특수 동작을 분기한다:

| 아이템 | 동작 |
|--------|------|
| 일반 아이템 | 매일 quality -1, sellIn 0 이후 -2 |
| `Aged Brie` | 매일 quality +1, sellIn 0 이후 +2 |
| `Sulfuras, Hand of Ragnaros` | sellIn/quality 변하지 않음 (quality 고정 80) |
| `Backstage passes to a TAFKAL80ETC concert` | sellIn 11~: +1, sellIn 6~10: +2, sellIn 1~5: +3, 0 이하: quality → 0 |
| `Conjured` (미구현) | 일반 아이템의 2배 속도로 quality 감소 |

**구현 제약**: `Conjured` 아이템 지원이 요구사항에 있지만 현재 미구현 상태다. `Item` 클래스와 `items` 속성은 변경할 수 없으므로, `updateQuality()` 내부 로직에만 추가해야 한다.

## 의존성

- **Google Mock 1.11.0** — NuGet 패키지(`packages/gmock.1.11.0`). 복원은 Visual Studio의 NuGet 패키지 복원 기능 사용.
- C++20 (`/std:c++20`)
