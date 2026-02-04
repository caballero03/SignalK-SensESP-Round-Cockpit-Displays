#include "DialGauge.h"

class OilPressureGauge : public DialGauge  { 
public: 
    OilPressureGauge(TFT_eSprite *frameBuffer, TFT_eSprite *dial, TFT_eSprite *needle, const unsigned short *gaugeImage, uint8_t pivotX, uint8_t pivotY) :
        DialGauge(frameBuffer, dial, needle, gaugeImage, pivotX, pivotY)
    {}

    // Override the drawNeedle method with a custom implementation
    void drawNeedle(float value) override  { 
        // Custom behavior for this subclass

        // 7-segment display border and background
        frameBuffer->fillRoundRect(60, 165, 120, 60, 8, TFT_DARKGREY);
        frameBuffer->fillRoundRect(63, 168, 114, 54, 6, 0x589d); // 0x1ad3 = #1b589d

        // 7-segment digits with one decimal place
        frameBuffer->setTextDatum(BC_DATUM); // place text based on bottom center origin
        frameBuffer->setTextSize(1);
        frameBuffer->setTextColor(TFT_LIGHTGREY, 0x589d); // 
        frameBuffer->drawFloat(value, 1, 120, 220, 7); // insert the 7-segment numerical text

        // Add the needle to frameBuffer sprite at correct angle
        needle->pushRotated(frameBuffer, (value*3)+60, TFT_BLACK);

    }
    
};