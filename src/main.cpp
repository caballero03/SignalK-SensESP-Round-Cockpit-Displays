// Signal K application template file.
//
// This application demonstrates core SensESP concepts in a very
// concise manner. You can build and upload the application as is
// and observe the value changes on the serial port monitor.
//
// You can use this source file as a basis for your own projects.
// Remove the parts that are not relevant to you, and add your own code
// for external hardware libraries.

#include <memory>

#include "sensesp.h"
// #include "sensesp/sensors/analog_input.h"
// #include "sensesp/sensors/digital_input.h"
// #include "sensesp/sensors/sensor.h"
// #include "sensesp/signalk/signalk_output.h"
#include "sensesp/signalk/signalk_listener.h"
#include "sensesp/signalk/signalk_value_listener.h"
#include "sensesp/system/lambda_consumer.h"
#include "sensesp_app_builder.h"

#include <TFT_eSPI.h>
// #include "gauge1.h"
#include "myGauge_1.h"
#include "myGauge_1a.h"
#include "myGauge_1b.h"
#include "myGauge_1c.h"
#include "myGauge_1e.h"
#include "mechanical_Digits_v1.1.h"
// #include "gauge2.h"
// #include "gauge3.h"
// #include "gauge4.h"
// #include "gauge5.h"
// #include "gauge6.h"
// #include "font.h"

#define TOUCH_CS 0

// Native/local display CS line
#define LCD_0_CS 9

// Remote displays' CS lines
#define LCD_1_CS 46   // J3 on PCB
#define LCD_2_CS 45   // J4
#define LCD_3_CS 42   // J5
#define LCD_4_CS 39   // J6

// Pin used for backlight PWM brightness control
#define LCD_BACKLIGHT_PIN 40

// use 12 bit precission for LEDC timer
#define LEDC_TIMER_12_BIT  12

// use 5000 Hz as a LEDC base frequency
#define LEDC_BASE_FREQ     5000

TFT_eSPI tft = TFT_eSPI(); 
TFT_eSprite img = TFT_eSprite(&tft);
TFT_eSprite display = TFT_eSprite(&tft);
TFT_eSprite needle = TFT_eSprite(&tft);
TFT_eSprite dial = TFT_eSprite(&tft);
TFT_eSprite mech_digits = TFT_eSprite(&tft);
TFT_eSprite mech_digit = TFT_eSprite(&tft);

void drawX(int x, int y)
{
  tft.drawLine(x-5, y-5, x+5, y+5, TFT_WHITE);
  tft.drawLine(x-5, y+5, x+5, y-5, TFT_WHITE);
}

void createMechDigits() {
    mech_digits.createSprite(33, 559);
    mech_digits.setSwapBytes(true);
    mech_digits.fillSprite(TFT_BLACK);
    mech_digits.pushImage(0, 0, 33, 559, mechanical_digits11);
}

void createDial(const unsigned short *gaugeImage) {
    dial.createSprite(240, 240);
    dial.setSwapBytes(true);
    dial.fillSprite(TFT_BLACK);
    dial.setPivot(120, 120);
    dial.pushImage(0, 0, 240, 240, gaugeImage);
}

void updateDialImage(const unsigned short *gaugeImage) {
    dial.pushImage(0, 0, 240, 240, gaugeImage);
}

void createNeedle() {
    needle.createSprite(30, 110);
    needle.setSwapBytes(true);
    needle.fillSprite(TFT_BLACK);
    needle.drawWedgeLine(15, 0, 15, 107, 6, 2, 0xf381); // Orange color
    needle.setPivot(16, 0);
}

void updateGauge(float value) {
    dial.pushToSprite(&display,0,0); // add the dial background to display sprite
    display.setTextDatum(BC_DATUM); // place text based on bottom center origin

    display.fillRoundRect(60, 165, 120, 60, 8, TFT_DARKGREY);
    display.fillRoundRect(63, 168, 114, 54, 6, 0x589d); // 0x1ad3 = #1b589d

    display.setTextSize(1);
    display.setTextColor(TFT_LIGHTGREY, 0x589d); // 
    display.drawFloat(value, 1, 120, 220, 7); // insert the text

    needle.pushRotated(&display, (value*3)+60, TFT_BLACK); // add the needle to display sprite at correct angle
    
    display.pushSprite(0,0); // push the display sprite to the screen (this method reduces flicker)
}

void selectDisplay(int displayId) {
    switch(displayId) {
        case 1: {
            // This CS line doesn't work for some reason
            digitalWrite(LCD_0_CS, HIGH); // native display
            digitalWrite(LCD_1_CS, LOW); // remote display
            digitalWrite(LCD_2_CS, HIGH); // remote display
            digitalWrite(LCD_3_CS, HIGH); // remote display
            digitalWrite(LCD_4_CS, HIGH); // remote display
            break;
        }

        case 2: {
            digitalWrite(LCD_0_CS, HIGH); // native display
            digitalWrite(LCD_1_CS, HIGH); // remote display
            digitalWrite(LCD_2_CS, LOW); // remote display
            digitalWrite(LCD_3_CS, HIGH); // remote display
            digitalWrite(LCD_4_CS, HIGH); // remote display
            break;
        }

        case 3: {
            digitalWrite(LCD_0_CS, HIGH); // native display
            digitalWrite(LCD_1_CS, HIGH); // remote display
            digitalWrite(LCD_2_CS, HIGH); // remote display
            digitalWrite(LCD_3_CS, LOW); // remote display
            digitalWrite(LCD_4_CS, HIGH); // remote display
            break;
        }

        case 4: {
            digitalWrite(LCD_0_CS, HIGH); // native display
            digitalWrite(LCD_1_CS, HIGH); // remote display
            digitalWrite(LCD_2_CS, HIGH); // remote display
            digitalWrite(LCD_3_CS, HIGH); // remote display
            digitalWrite(LCD_4_CS, LOW); // remote display
            break;
        }

        // This is for local display LCD_0_CS or any invalid display given
        default: {
            digitalWrite(LCD_0_CS, LOW); // native display
            digitalWrite(LCD_1_CS, HIGH); // remote display
            digitalWrite(LCD_2_CS, HIGH); // remote display
            digitalWrite(LCD_3_CS, HIGH); // remote display
            digitalWrite(LCD_4_CS, HIGH); // remote display
        }

    }
}

void releaseDisplays() {
    digitalWrite(LCD_0_CS, HIGH); // native display
    digitalWrite(LCD_1_CS, HIGH); // remote display 1
    digitalWrite(LCD_2_CS, HIGH); // remote display 2
    digitalWrite(LCD_3_CS, HIGH); // remote display 3
    digitalWrite(LCD_4_CS, HIGH); // remote display 4
}

std::vector<uint8_t> intToByteArray(int value) {
    std::vector<uint8_t> digits;

    while (value != 0) {
        digits.push_back(value % 10);
        value /= 10;
    }

    return digits;
}


using namespace sensesp;

// The setup function performs one-time application initialization.
void setup() {
  SetupLogging(ESP_LOG_DEBUG);

  // Construct the global SensESPApp() object
  SensESPAppBuilder builder;
  sensesp_app = (&builder)
                    // Set a custom hostname for the app.
                    // ->set_hostname("my-sensesp-project")
                    ->set_hostname("cockpit-gauges")
                    // Optionally, hard-code the WiFi and Signal K server
                    // settings. This is normally not needed.
                    //->set_wifi_client("My WiFi SSID", "my_wifi_password")
                    //->set_wifi_access_point("My AP SSID", "my_ap_password")
                    //->set_sk_server("192.168.10.3", 80)
                    ->get_app();


    // Not sure what this is for yet. Maybe a mode or theme change? Backlight shutoff?
    pinMode(0,INPUT_PULLUP);

    //////////////////////////////////////////////
    // Select ALL displays

    // Native (built-in) display CS pin
    pinMode(LCD_0_CS, OUTPUT);

    // Remote CS pins
    pinMode(LCD_1_CS, OUTPUT);
    pinMode(LCD_2_CS, OUTPUT);
    pinMode(LCD_3_CS, OUTPUT);
    pinMode(LCD_4_CS, OUTPUT);

    digitalWrite(LCD_0_CS, LOW); // native display
    digitalWrite(LCD_1_CS, HIGH); // remote display (test) NOTE: This one doesn't work for some reason
    digitalWrite(LCD_2_CS, LOW); // remote display (test)
    digitalWrite(LCD_3_CS, LOW); // remote display (test)
    digitalWrite(LCD_4_CS, LOW); // remote display (test)

    tft.init();
    tft.setRotation(4);
    tft.setSwapBytes(true);
    img.setSwapBytes(true);
    tft.fillScreen(TFT_ORANGE);
    img.createSprite(240, 240);

    digitalWrite(LCD_2_CS, HIGH);

    // TODO: this needs to be initialized one(or more?) for each display
    // Initialize the frame-buffer display sprite
    display.createSprite(240, 240);
    display.setPivot(120, 120);
    display.fillSprite(TFT_BLACK);


    // Send this to all displays. Why? Because... that's why. LOL
    // img.pushImage(0,0,240,240,gauge2);
    img.pushImage(0,0,240,240,myGauge_1);
    img.pushSprite(0, 0);

    // Test of individual displays
    // selectDisplay(2);
    // // img.pushImage(0,0,240,240,gauge1);
    // img.pushImage(0,0,240,240,myGauge_1);
    // img.pushSprite(0, 0);

    selectDisplay(3);
    //   img.pushImage(0,0,240,240,gauge2);
    img.pushImage(0,0,240,240,myGauge_1b);
    img.pushSprite(0, 0);

    selectDisplay(4);
    // img.pushImage(0,0,240,240,gauge3);
    //   img.pushImage(0,0,240,240,myGauge_1a);
    img.pushImage(0,0,240,240,myGauge_1b);
    img.pushSprite(0, 0);

    releaseDisplays();

    ///////////////////////////////////////////////////////////////////
    // Tests to develop gauge stuff

    createDial(myGauge_1e);
    createNeedle();
    // updateGauge(300);


    createMechDigits();
    selectDisplay(2);
    mech_digit.createSprite(33, 70);
    mech_digits.pushToSprite(&mech_digit,0,-30-(9.5 * 43));
    mech_digit.pushSprite(120,120);

    

  ////////////////////////////////////////////////////////////////////
  //
  // Setting up the LEDC and configuring the Back light pin
  // NOTE: this needs to be done after tft.init()
#if ESP_IDF_VERSION_MAJOR == 5
  ledcAttach(LCD_BACKLIGHT_PIN, LEDC_BASE_FREQ, LEDC_TIMER_12_BIT);
#else
  ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_12_BIT);
  ledcAttachPin(LCD_BACKLIGHT_PIN, LEDC_CHANNEL_0);
#endif

//   ledcWrite(LCD_BACKLIGHT_PIN, (uint32_t)800);
//   ledcWrite(LCD_BACKLIGHT_PIN, (uint32_t)200);
//   ledcWrite(LCD_BACKLIGHT_PIN, (uint32_t)100);
  ledcWrite(LCD_BACKLIGHT_PIN, (uint32_t)2047);


auto* depthListener = new FloatSKListener("environment.depth.belowTransducer");

depthListener
    ->connect_to(new LambdaConsumer<float>(
        [](float value) { 
            ESP_LOGD("MyApp", "DBT:     %f meters", value);

            selectDisplay(3);
            digitalWrite(LCD_0_CS, LOW); // native display too, tee-hee
            updateGauge(value);
        }));

 // Produce integers every 1 seconds
  auto sensor_int = new RepeatSensor<int>(1000, []() {
    static int value = 0;
    value += 1;

    // std::vector<uint8_t> byteArray = intToByteArray(value);

    // for (byte c : byteArray) {
    //     ESP_LOGD("MyApp", "%d", c);
    // }
    // // ESP_LOGD("\n");


    if(value > 99) value = 0;

    return value;
  });

  //event_loop()->onRepeat(20, []() {
  //    
  //});
  sensor_int->connect_to(new LambdaConsumer<int>(
      [](int value) {
        ESP_LOGD("MyApp", "int:        %d", value); 

        selectDisplay(2);
        mech_digits.pushToSprite(&mech_digit,0,-30-(((float)value/10.0f) * 43));
        mech_digit.pushSprite(120,120);
    }));

    /////////////////////////////////////////////////////////////////////
    // To avoid garbage collecting all shared pointers created in setup(),
    // loop from here.
    while (true) {
        loop();
    }
}

void loop() { 
    event_loop()->tick(); 
}
