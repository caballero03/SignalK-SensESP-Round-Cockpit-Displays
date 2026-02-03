#ifndef DIAL_GAUGE_H
#define DIAL_GAUGE_H

#include <TFT_eSPI.h>

class DialGauge {
public:
    // Constructor with member variable initialization and function calls
    explicit DialGauge(TFT_eSprite *frameBuffer, TFT_eSprite *dial, TFT_eSprite *needle, const unsigned short *gaugeImage) :
        frameBuffer(frameBuffer), dial(dial), needle(needle) {
        createFrameBuffer();
        createDial(gaugeImage);
        createNeedle();
    }

    void updateGauge(float value);
    void updateDialImage(const unsigned short *gaugeImage);

private:
    TFT_eSprite *frameBuffer;
    TFT_eSprite *dial;
    TFT_eSprite *needle;

    // Private member functions
    void createFrameBuffer();
    void createDial(const unsigned short *gaugeImage);
    void createNeedle();
};

#endif