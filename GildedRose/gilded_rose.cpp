#include "gilded_rose.h"

using std::vector;
using std::string;

static const string AGED_BRIE      = "Aged Brie";
static const string SULFURAS       = "Sulfuras, Hand of Ragnaros";
static const string BACKSTAGE_PASS = "Backstage passes to a TAFKAL80ETC concert";
static const string CONJURED       = "Conjured";
static const int    MAX_QUALITY    = 50;
static const int    MIN_QUALITY    = 0;

GildedRose::GildedRose(vector<Item>& items) : items(items)
{
}

void GildedRose::updateQuality()
{
    for (int i = 0; i < items.size(); i++)
    {
        if (items[i].name != AGED_BRIE && items[i].name != BACKSTAGE_PASS)
        {
            if (items[i].quality > MIN_QUALITY)
            {
                if (items[i].name != SULFURAS)
                {
                    items[i].quality = items[i].quality - 1;
                }
            }
        }
        else
        {
            if (items[i].quality < MAX_QUALITY)
            {
                items[i].quality = items[i].quality + 1;

                if (items[i].name == BACKSTAGE_PASS)
                {
                    if (items[i].sellIn < 11)
                    {
                        if (items[i].quality < MAX_QUALITY)
                        {
                            items[i].quality = items[i].quality + 1;
                        }
                    }

                    if (items[i].sellIn < 6)
                    {
                        if (items[i].quality < MAX_QUALITY)
                        {
                            items[i].quality = items[i].quality + 1;
                        }
                    }
                }
            }
        }

        if (items[i].name != SULFURAS)
        {
            items[i].sellIn = items[i].sellIn - 1;
        }

        if (items[i].sellIn < 0)
        {
            if (items[i].name != AGED_BRIE)
            {
                if (items[i].name != BACKSTAGE_PASS)
                {
                    if (items[i].quality > MIN_QUALITY)
                    {
                        if (items[i].name != SULFURAS)
                        {
                            items[i].quality = items[i].quality - 1;
                        }
                    }
                }
                else
                {
                    items[i].quality = items[i].quality - items[i].quality;
                }
            }
            else
            {
                if (items[i].quality < MAX_QUALITY)
                {
                    items[i].quality = items[i].quality + 1;
                }
            }
        }
    }
}
