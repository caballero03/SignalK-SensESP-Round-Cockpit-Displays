
#include "DialGauge.h"

void DialGauge::createFrameBuffer(uint8_t pivotX, uint8_t pivotY) {  // pivot point
    // Initialize the frame-buffer display sprite
    frameBuffer->createSprite(240, 240);
    frameBuffer->setPivot(pivotX, pivotY);
    frameBuffer->fillSprite(TFT_BLACK);
}

void DialGauge::createDial(const unsigned short *gaugeImage, uint8_t pivotX, uint8_t pivotY) { // pivot point, gauge bg img, size,pos
    dial->createSprite(240, 240);
    dial->setSwapBytes(true);
    dial->fillSprite(TFT_BLACK);
    dial->setPivot(pivotX, pivotY);
    dial->pushImage(0, 0, 240, 240, gaugeImage);
}

void DialGauge::updateDialImage(const unsigned short *gaugeImage) {
    dial->pushImage(0, 0, 240, 240, gaugeImage);
}

void DialGauge::createNeedle() { // Needle size, pivot point, draw function, needle color
    needle->createSprite(30, 110);
    needle->setSwapBytes(true);
    needle->fillSprite(TFT_BLACK);
    needle->drawWedgeLine(15, 0, 15, 107, 6, 2, 0xf381); // Orange color
    needle->setPivot(16, 0);
}

void DialGauge::updateGauge(float value) { // needle_angle function
    // Add the dial background to display sprite
    dial->pushToSprite(frameBuffer,0,0);

    // Draw the needle to the framebuffer. Subclass to override this fn()
    drawNeedle(value);

    // Push the display sprite to the screen (this method reduces flicker)
    frameBuffer->pushSprite(0,0);
}

void DialGauge::drawNeedle(float value) {
    // Add the needle to frameBuffer sprite at correct angle
    needle->pushRotated(frameBuffer, (value*3)+60, TFT_BLACK);

}