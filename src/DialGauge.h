#ifndef DIAL_GAUGE_H
#define DIAL_GAUGE_H

#include <TFT_eSPI.h>

class DialGauge {
public:
    // Constructor with member variable initialization and function calls
    DialGauge(TFT_eSprite *frameBuffer, TFT_eSprite *dial, TFT_eSprite *needle) :
        frameBuffer(frameBuffer), dial(dial), needle(needle) {
        createFrameBuffer();
        createNeedle();
    }

    // DialGauge(TFT_eSPI *tft);

    void createDial(const unsigned short *gaugeImage);

    void updateGauge(float value);
    void updateDialImage(const unsigned short *gaugeImage);

private:
    TFT_eSprite *frameBuffer;
    TFT_eSprite *dial;
    TFT_eSprite *needle;

    // Private member functions
    void createNeedle();
    void createFrameBuffer();
};

#endif