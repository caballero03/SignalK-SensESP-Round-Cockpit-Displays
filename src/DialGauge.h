#ifndef DIAL_GAUGE_H
#define DIAL_GAUGE_H

#include <TFT_eSPI.h>

class DialGauge {
public:
    // Constructor with member variable initialization and function calls
    DialGauge(TFT_eSprite *frameBuffer, TFT_eSprite *dial, TFT_eSprite *needle, const unsigned short *gaugeImage, uint8_t pivotX, uint8_t pivotY) :
        frameBuffer(frameBuffer), dial(dial), needle(needle) {
        createFrameBuffer(pivotX, pivotY);
        createDial(gaugeImage, pivotX, pivotY);
        createNeedle();
    }

    void updateGauge(float value);
    void updateDialImage(const unsigned short *gaugeImage);
    
    // Virtual method for drawing the updated value. To be implemented by subclasses
    virtual void drawNeedle(float value);

protected:
    TFT_eSprite *frameBuffer;
    TFT_eSprite *dial;
    TFT_eSprite *needle;

private:
    // Private member functions
    void createFrameBuffer(uint8_t pivotX, uint8_t pivotY);
    void createDial(const unsigned short *gaugeImage, uint8_t pivotX, uint8_t pivotY);
    void createNeedle();
};

#endif