#include "gilded_rose.h"

using std::vector;
using std::string;

static const string AGED_BRIE      = "Aged Brie";
static const string SULFURAS       = "Sulfuras, Hand of Ragnaros";
static const string BACKSTAGE_PASS = "Backstage passes to a TAFKAL80ETC concert";
static const string CONJURED       = "Conjured";
static const int    MAX_QUALITY    = 50;
static const int    MIN_QUALITY    = 0;

static void increaseQuality(Item& item) {
    if (item.quality < MAX_QUALITY) item.quality++;
}

static void decreaseQuality(Item& item) {
    if (item.quality > MIN_QUALITY) item.quality--;
}

GildedRose::GildedRose(vector<Item>& items) : items(items)
{
}

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
    decreaseQuality(item);
    decreaseQuality(item);
    item.sellIn--;
    if (item.sellIn < 0)
    {
        decreaseQuality(item);
        decreaseQuality(item);
    }
}

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
