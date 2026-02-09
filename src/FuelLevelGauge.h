#ifndef FUEL_LEVEL_GAUGE_H
#define FUEL_LEVEL_GAUGE_H

#include "DialGauge.h"

class FuelLevelGauge : public DialGauge  { 
public: 
    FuelLevelGauge(TFT_eSprite *frameBuffer, TFT_eSprite *dial, TFT_eSprite *needle, const unsigned short *gaugeImage, uint8_t pivotX, uint8_t pivotY) :
        DialGauge(frameBuffer, dial, needle, gaugeImage, pivotX, pivotY)
    {}

    // Override the drawNeedle method with a custom implementation
    void drawNeedle(float value) override  { 
        // Custom behavior for this subclass

        // 
        // frameBuffer->fillRoundRect(60, 165, 120, 60, 8, TFT_DARKGREY);
        // frameBuffer->fillRoundRect(63, 168, 114, 54, 6, 0x589d); // 0x1ad3 = #1b589d

        frameBuffer->setTextDatum(MC_DATUM); // place text based on bottom center origin

        frameBuffer->setTextSize(2);
        frameBuffer->setTextColor(0x589d, TFT_LIGHTGREY); // 
        frameBuffer->drawFloat((value/100) * 19, 1, 200, 110, 2);

        frameBuffer->setTextSize(1);
        frameBuffer->setTextColor(0x589d, TFT_LIGHTGREY); // 
        frameBuffer->drawString("GALLONS", 200, 134, 2);

        // Calculate where the needle needs to be drawn in Y
        int needleYPos = 218 - (1.98 * value);

        // Draw the needle 
        frameBuffer->drawWedgeLine(100, needleYPos, 150, needleYPos, 6, 2, 0xf381); // Orange color

    }
    
};

#endif