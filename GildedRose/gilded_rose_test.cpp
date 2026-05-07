#include "gmock/gmock.h"
#include "gilded_rose.h"
#include <sstream>
#include <iostream>

using std::vector;
using std::string;

void print_item(const Item& item);

TEST(PrintItemTest, PrintsNameSellInQuality) {
    //Arrange
    Item item("Aged Brie", 5, 10);
    std::streambuf* orig = std::cout.rdbuf();
    std::ostringstream oss;
    std::cout.rdbuf(oss.rdbuf());

    //Act
    print_item(item);

    //Assert
    std::cout.rdbuf(orig);
    EXPECT_EQ("Aged Brie, 5, 10\n", oss.str());
}

TEST(GildedRoseTest, Foo) {
    //Arrange
    vector<Item> items;
    items.emplace_back("Foo", 0, 0);
    GildedRose app(items);

    //Act
    app.updateQuality();

    //Assert
    EXPECT_EQ("Foo", app.items[0].name);
}

// ===================== NormalItem =====================

TEST(NormalItemTest, QualityAndSellInDecreaseByOnePerDay) {
    //Arrange
    vector<Item> items;
    items.emplace_back("Normal Item", 10, 20);
    GildedRose app(items);

    //Act
    app.updateQuality();

    //Assert
    EXPECT_EQ(9, app.items[0].sellIn);
    EXPECT_EQ(19, app.items[0].quality);
}

TEST(NormalItemTest, QualityDecreasesTwiceAfterSellInExpires) {
    //Arrange
    vector<Item> items;
    items.emplace_back("Normal Item", 0, 10);
    GildedRose app(items);

    //Act
    app.updateQuality();

    //Assert
    EXPECT_EQ(8, app.items[0].quality);
}

TEST(NormalItemTest, QualityNeverGoesBelowZero_WhenSellInPositive) {
    //Arrange
    vector<Item> items;
    items.emplace_back("Normal Item", 5, 0);
    GildedRose app(items);

    //Act
    app.updateQuality();

    //Assert
    EXPECT_EQ(0, app.items[0].quality);
}

TEST(NormalItemTest, QualityNeverGoesBelowZero_WhenSellInExpired) {
    //Arrange
    vector<Item> items;
    items.emplace_back("Normal Item", 0, 0);
    GildedRose app(items);

    //Act
    app.updateQuality();

    //Assert
    EXPECT_EQ(0, app.items[0].quality);
}

// ===================== AgedBrie =====================

TEST(AgedBrieTest, QualityIncreasesByOnePerDay) {
    //Arrange
    vector<Item> items;
    items.emplace_back("Aged Brie", 5, 10);
    GildedRose app(items);

    //Act
    app.updateQuality();

    //Assert
    EXPECT_EQ(11, app.items[0].quality);
}

TEST(AgedBrieTest, QualityIncreasesByTwoAfterSellInExpires) {
    //Arrange
    vector<Item> items;
    items.emplace_back("Aged Brie", 0, 10);
    GildedRose app(items);

    //Act
    app.updateQuality();

    //Assert
    EXPECT_EQ(12, app.items[0].quality);
}

TEST(AgedBrieTest, QualityNeverExceedsFifty) {
    //Arrange
    vector<Item> items;
    items.emplace_back("Aged Brie", 5, 50);
    GildedRose app(items);

    //Act
    app.updateQuality();

    //Assert
    EXPECT_EQ(50, app.items[0].quality);
}

// ===================== Sulfuras =====================

TEST(SulfurasTest, QualityAndSellInNeverChange) {
    //Arrange
    vector<Item> items;
    items.emplace_back("Sulfuras, Hand of Ragnaros", 5, 80);
    GildedRose app(items);

    //Act
    app.updateQuality();

    //Assert
    EXPECT_EQ(5, app.items[0].sellIn);
    EXPECT_EQ(80, app.items[0].quality);
}

TEST(SulfurasTest, NeverChangesEvenWhenSellInIsNegative) {
    //Arrange
    vector<Item> items;
    items.emplace_back("Sulfuras, Hand of Ragnaros", -1, 80);
    GildedRose app(items);

    //Act
    app.updateQuality();

    //Assert
    EXPECT_EQ(-1, app.items[0].sellIn);
    EXPECT_EQ(80, app.items[0].quality);
}

// ===================== BackstagePass =====================

TEST(BackstagePassTest, QualityIncreasesByOneWhenSellInAboveTen) {
    //Arrange
    vector<Item> items;
    items.emplace_back("Backstage passes to a TAFKAL80ETC concert", 15, 20);
    GildedRose app(items);

    //Act
    app.updateQuality();

    //Assert
    EXPECT_EQ(21, app.items[0].quality);
}

TEST(BackstagePassTest, QualityIncreasesByTwoWhenSellInIsTen) {
    //Arrange
    vector<Item> items;
    items.emplace_back("Backstage passes to a TAFKAL80ETC concert", 10, 20);
    GildedRose app(items);

    //Act
    app.updateQuality();

    //Assert
    EXPECT_EQ(22, app.items[0].quality);
}

TEST(BackstagePassTest, QualityIncreasesByThreeWhenSellInIsFive) {
    //Arrange
    vector<Item> items;
    items.emplace_back("Backstage passes to a TAFKAL80ETC concert", 5, 20);
    GildedRose app(items);

    //Act
    app.updateQuality();

    //Assert
    EXPECT_EQ(23, app.items[0].quality);
}

TEST(BackstagePassTest, QualityDropsToZeroAfterConcert) {
    //Arrange
    vector<Item> items;
    items.emplace_back("Backstage passes to a TAFKAL80ETC concert", 0, 20);
    GildedRose app(items);

    //Act
    app.updateQuality();

    //Assert
    EXPECT_EQ(0, app.items[0].quality);
}

TEST(BackstagePassTest, QualityNeverExceedsFifty) {
    //Arrange
    vector<Item> items;
    items.emplace_back("Backstage passes to a TAFKAL80ETC concert", 5, 49);
    GildedRose app(items);

    //Act
    app.updateQuality();

    //Assert
    EXPECT_EQ(50, app.items[0].quality);
}

// ===================== Conjured (Phase 4에서 구현) =====================

TEST(ConjuredTest, QualityDecreasesByTwoPerDay) {
    //Arrange
    vector<Item> items;
    items.emplace_back("Conjured", 5, 10);
    GildedRose app(items);

    //Act
    app.updateQuality();

    //Assert
    EXPECT_EQ(8, app.items[0].quality);
}

TEST(ConjuredTest, QualityDecreasesByFourAfterSellInExpires) {
    //Arrange
    vector<Item> items;
    items.emplace_back("Conjured", 0, 10);
    GildedRose app(items);

    //Act
    app.updateQuality();

    //Assert
    EXPECT_EQ(6, app.items[0].quality);
}

TEST(ConjuredTest, QualityNeverGoesBelowZero) {
    //Arrange
    vector<Item> items;
    items.emplace_back("Conjured", 0, 1);
    GildedRose app(items);

    //Act
    app.updateQuality();

    //Assert
    EXPECT_EQ(0, app.items[0].quality);
}
