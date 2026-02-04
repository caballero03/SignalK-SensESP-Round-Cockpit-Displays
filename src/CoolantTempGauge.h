
#include "DialGauge.h"

class CoolantTempGauge : public DialGauge  { 
public: 
    CoolantTempGauge(TFT_eSprite *frameBuffer, TFT_eSprite *dial, TFT_eSprite *needle, const unsigned short *gaugeImage, uint8_t pivotX, uint8_t pivotY) :
        DialGauge(frameBuffer, dial, needle, gaugeImage, pivotX, pivotY)
    {}

    // Override the drawNeedle method with a custom implementation
    void drawNeedle(float value) override  { 
        // Custom behavior for this subclass

        if(value < 10.0) value = 10.0;

        // Add the needle to frameBuffer sprite at correct angle
        needle->pushRotated(frameBuffer, (1 * 1.44 * (value - 10.0))+60, TFT_BLACK); // TODO: Take away the 3 * part

    }
    
};