
#include "DialGauge.h"

void DialGauge::createFrameBuffer() {  // pivot point
    // Initialize the frame-buffer display sprite
    frameBuffer->createSprite(240, 240);
    // frameBuffer->setPivot(120, 120);
    frameBuffer->fillSprite(TFT_BLACK);
}

void DialGauge::createDial(const unsigned short *gaugeImage) { // pivot point, gauge bg img, size,pos
    dial->createSprite(240, 240);
    dial->setSwapBytes(true);
    dial->fillSprite(TFT_BLACK);
    dial->setPivot(120, 120);
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

    // TODO: move the following indented chunk to a subclass in a drawGauge(framebuffer) function
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
    
    // Push the display sprite to the screen (this method reduces flicker)
    frameBuffer->pushSprite(0,0);
}