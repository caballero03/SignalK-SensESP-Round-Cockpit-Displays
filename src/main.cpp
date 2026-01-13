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
// #include "sensesp/system/lambda_consumer.h"
#include "sensesp_app_builder.h"

#include <TFT_eSPI.h>
#include "gauge1.h"
#include "myGauge_1.h"
#include "myGauge_1a.h"
#include "myGauge_1b.h"
#include "gauge2.h"
#include "gauge3.h"
#include "gauge4.h"
#include "gauge5.h"
#include "gauge6.h"
#include "font.h"

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

// use first channel of 16 channels (started from zero)
// #define LEDC_CHANNEL_0     0

// use 12 bit precission for LEDC timer
#define LEDC_TIMER_12_BIT  12

// use 5000 Hz as a LEDC base frequency
#define LEDC_BASE_FREQ     5000

TFT_eSPI tft = TFT_eSPI(); 
TFT_eSprite img = TFT_eSprite(&tft);
// TFT_eSprite ln = TFT_eSprite(&tft);

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


  pinMode(0,INPUT_PULLUP);

    // Native display CS pins
    pinMode(LCD_0_CS, OUTPUT);

    // Remote CS pins
    pinMode(LCD_1_CS, OUTPUT);
    pinMode(LCD_2_CS, OUTPUT);
    pinMode(LCD_3_CS, OUTPUT);
    pinMode(LCD_4_CS, OUTPUT);
   
    // tft.init();
    // tft.setRotation(3);
    // tft.setSwapBytes(true);
    // // img.setSwapBytes(true);
    // // img.fillScreen(TFT_ORANGE);TFT_RED
    // img.createSprite(240, 240);

    digitalWrite(LCD_0_CS, LOW); // native display

    digitalWrite(LCD_1_CS, HIGH); // remote display (test)
    digitalWrite(LCD_2_CS, LOW); // remote display (test)
    digitalWrite(LCD_3_CS, LOW); // remote display (test)
    digitalWrite(LCD_4_CS, LOW); // remote display (test)


    tft.init();
    tft.setRotation(4);
    tft.setSwapBytes(true);
    img.setSwapBytes(true);
    tft.fillScreen(TFT_ORANGE);
    img.createSprite(240, 240);


    // img.pushImage(0,0,240,240,gauge2);
  img.pushImage(0,0,240,240,myGauge_1);
  img.pushSprite(0, 0);
  

  digitalWrite(LCD_0_CS, HIGH); // native display

  digitalWrite(LCD_1_CS, HIGH); // remote display
  digitalWrite(LCD_2_CS, LOW); // remote display
  digitalWrite(LCD_3_CS, HIGH); // remote display
  digitalWrite(LCD_4_CS, HIGH); // remote display

  // img.pushImage(0,0,240,240,gauge1);
  img.pushImage(0,0,240,240,myGauge_1);
  img.pushSprite(0, 0);

  digitalWrite(LCD_1_CS, HIGH); // remote display
  digitalWrite(LCD_2_CS, HIGH); // remote display
  digitalWrite(LCD_3_CS, LOW); // remote display
  digitalWrite(LCD_4_CS, HIGH); // remote display

  // img.pushImage(0,0,240,240,gauge2);
  img.pushImage(0,0,240,240,myGauge_1b);
  img.pushSprite(0, 0);

  digitalWrite(LCD_1_CS, HIGH); // remote display
  digitalWrite(LCD_2_CS, HIGH); // remote display
  digitalWrite(LCD_3_CS, HIGH); // remote display
  digitalWrite(LCD_4_CS, LOW); // remote display

  // img.pushImage(0,0,240,240,gauge3);
  img.pushImage(0,0,240,240,myGauge_1a);
  img.pushSprite(0, 0);

  digitalWrite(LCD_1_CS, HIGH); // remote display
  digitalWrite(LCD_2_CS, HIGH); // remote display
  digitalWrite(LCD_3_CS, HIGH); // remote display
  digitalWrite(LCD_4_CS, HIGH); // remote display


  // digitalWrite(LCD_0_CS, LOW); // native display

  // digitalWrite(LCD_2_CS, LOW); // remote display (test)
  // digitalWrite(LCD_3_CS, LOW); // remote display (test)
  // digitalWrite(LCD_4_CS, LOW); // remote display (test)

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

  // ledcWrite(LCD_BACKLIGHT_PIN, (uint32_t)800);
  ledcWrite(LCD_BACKLIGHT_PIN, (uint32_t)200);
//   ledcWrite(LCD_BACKLIGHT_PIN, (uint32_t)2047);


  // To avoid garbage collecting all shared pointers created in setup(),
  // loop from here.
  while (true) {
    loop();
  }
}

void loop() { 
    event_loop()->tick(); 
}
