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
#include "sensesp/transforms/time_counter.h"
#include "sensesp/sensors/digital_input.h"
#include "sensesp/transforms/lambda_transform.h"
#include "sensesp/signalk/signalk_output.h"
#include "sensesp/signalk/signalk_listener.h"
#include "sensesp/signalk/signalk_value_listener.h"
#include "sensesp/system/lambda_consumer.h"
#include "sensesp_app_builder.h"

#include "transforms/kelvintofahrenheit.h"

#include <TFT_eSPI.h>

#include "myGauge_1.h"
#include "myGauge_1b.h"
#include "myGauge_1g.h"
#include "mechanical_Digits_v1.1.h"
#include "engineHours_BG_v1.0b.h"

#include "Coolant_Temperature_Gauge_v1.2.h"
#include "Diesel_Fuel_Gauge_v1.0.h"

#include "DialGauge.h"
#include "CoolantTempGauge.h"
#include "OilPressureGauge.h"
#include "FuelLevelGauge.h"

// Native/local display CS line
#define LCD_0_CS 9

// This will be connected to the ignition switch power through an optoisolator
#define ENGINE_RUN_INPUT 46

// Remote displays' CS lines
#define LCD_1_CS 45   // J4
#define LCD_2_CS 42   // J5
#define LCD_3_CS 39   // J6

// Pin used for backlight PWM brightness control
#define LCD_BACKLIGHT_PIN 40

// use 12 bit precission for LEDC timer
#define LEDC_TIMER_12_BIT  12

// use 5000 Hz as a LEDC base frequency
#define LEDC_BASE_FREQ     5000

TFT_eSPI tft = TFT_eSPI();

TFT_eSprite img = TFT_eSprite(&tft); // For testing only -- delete later

TFT_eSprite display = TFT_eSprite(&tft);
TFT_eSprite needle = TFT_eSprite(&tft);
TFT_eSprite dial = TFT_eSprite(&tft);

TFT_eSprite coolantDisp = TFT_eSprite(&tft);
TFT_eSprite coolantNeedle = TFT_eSprite(&tft);
TFT_eSprite coolantDial = TFT_eSprite(&tft);

TFT_eSprite fuelDisp = TFT_eSprite(&tft);
TFT_eSprite fuelNeedle = TFT_eSprite(&tft);
TFT_eSprite fuelDial = TFT_eSprite(&tft);

TFT_eSprite mechDigDisp = TFT_eSprite(&tft);
TFT_eSprite mech_FG = TFT_eSprite(&tft);

TFT_eSprite mech_digits = TFT_eSprite(&tft);
TFT_eSprite mech_digit0 = TFT_eSprite(&tft);
TFT_eSprite mech_digit1 = TFT_eSprite(&tft);
TFT_eSprite mech_digit2 = TFT_eSprite(&tft);
TFT_eSprite mech_digit3 = TFT_eSprite(&tft);
TFT_eSprite mech_digit4 = TFT_eSprite(&tft);
// TFT_eSprite mech_digit5 = TFT_eSprite(&tft);

// Create a new DialGauge object with the sprites and the background
OilPressureGauge* oilPressureGauge;
CoolantTempGauge* coolantTempGauge;
FuelLevelGauge* fuelLevelGauge;

// This is the target digits for the mechanical display
uint8_t targetDigitArray[8];

void createMechDigits() {
    mech_digits.createSprite(33, 559);
    mech_digits.setSwapBytes(true);
    mech_digits.fillSprite(TFT_BLACK);
    mech_digits.pushImage(0, 0, 33, 559, mechanical_digits11);
}

void createMechFGImg() {
    mech_FG.createSprite(240, 240);
    mech_FG.setSwapBytes(true);
    mech_FG.fillSprite(TFT_BLACK);
    mech_FG.pushImage(0, 0, 240, 240, Engine_Hours_Gauge);
}


void selectDisplay(int displayId) {
    switch(displayId) {
        case 1: {
            digitalWrite(LCD_0_CS, HIGH); // native display
            digitalWrite(LCD_1_CS, LOW); // remote display
            digitalWrite(LCD_2_CS, HIGH); // remote display
            digitalWrite(LCD_3_CS, HIGH); // remote display
            break;
        }

        case 2: {
            digitalWrite(LCD_0_CS, HIGH); // native display
            digitalWrite(LCD_1_CS, HIGH); // remote display
            digitalWrite(LCD_2_CS, LOW); // remote display
            digitalWrite(LCD_3_CS, HIGH); // remote display
            break;
        }

        case 3: {
            digitalWrite(LCD_0_CS, HIGH); // native display
            digitalWrite(LCD_1_CS, HIGH); // remote display
            digitalWrite(LCD_2_CS, HIGH); // remote display
            digitalWrite(LCD_3_CS, LOW); // remote display
            break;
        }

        // This is for local display LCD_0_CS or any invalid display given
        default: {
            digitalWrite(LCD_0_CS, LOW); // native display
            digitalWrite(LCD_1_CS, HIGH); // remote display
            digitalWrite(LCD_2_CS, HIGH); // remote display
            digitalWrite(LCD_3_CS, HIGH); // remote display
        }

    }
}

void releaseDisplays() {
    digitalWrite(LCD_0_CS, HIGH); // native display
    digitalWrite(LCD_1_CS, HIGH); // remote display 2
    digitalWrite(LCD_2_CS, HIGH); // remote display 3
    digitalWrite(LCD_3_CS, HIGH); // remote display 4
}

void drawX(int x, int y)
{
  tft.drawLine(x-5, y-5, x+5, y+5, TFT_WHITE);
  tft.drawLine(x-5, y+5, x+5, y-5, TFT_WHITE);
}

// A function to peel the digits out of the engine hours to 
// display one-by-one on the mechanical digit display
std::vector<uint8_t> intToByteArray(int value) {
    std::vector<uint8_t> digits;

    while (value != 0) {
        digits.push_back(value % 10);
        value /= 10;
    }

    return digits;
}

// Bogus fuel level for testing gauges
float diesel_tank_level_read_callback() { 
    return 0.7097427;
}

// Bogus engine temp for testing gauges
float engine_coolant_temp_read_callback() { 
    return 289.8167; // 62 derees F
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

    // Input pin for engine state. This is how engine run time is counted.
    pinMode(ENGINE_RUN_INPUT, INPUT_PULLUP);

    //////////////////////////////////////////////
    // Select ALL displays

    // Native (built-in) display CS pin
    pinMode(LCD_0_CS, OUTPUT);

    // Remote CS pins
    pinMode(LCD_1_CS, OUTPUT);
    pinMode(LCD_2_CS, OUTPUT);
    pinMode(LCD_3_CS, OUTPUT);

    digitalWrite(LCD_0_CS, LOW); // native display
    digitalWrite(LCD_1_CS, LOW); // remote display (test)
    digitalWrite(LCD_2_CS, LOW); // remote display (test)
    digitalWrite(LCD_3_CS, LOW); // remote display (test)

    tft.init();
    tft.setRotation(4);
    tft.setSwapBytes(true);
    img.setSwapBytes(true);
    // tft.fillScreen(TFT_ORANGE);
    tft.fillScreen(TFT_BLACK);
    img.createSprite(240, 240);

    // Send this to all displays. Why? Because... that's why. LOL
    img.pushImage(0,0,240,240,myGauge_1);
    img.pushSprite(0, 0);

    // Test of individual displays
    // 
    selectDisplay(0);
    img.pushImage(0,0,240,240,Diesel_Fuel_Gauge_v10);
    img.pushSprite(0, 0);

    selectDisplay(3);
    //   img.pushImage(0,0,240,240,myGauge_1a);
    img.pushImage(0,0,240,240,Coolant_Temperature_Gauge_v12);
    img.pushSprite(0, 0);

    // Coolant temp. needle pivot point
    drawX(153,138);

    releaseDisplays();

    ///////////////////////////////////////////////////////////////////
    // 

    oilPressureGauge = new OilPressureGauge(&display, &dial, &needle, myGauge_1g, 120, 120);
    coolantTempGauge = new CoolantTempGauge(&coolantDisp, &coolantDial, &coolantNeedle, Coolant_Temperature_Gauge_v12, 153, 138);
    fuelLevelGauge = new FuelLevelGauge(&fuelDisp, &fuelDial, &fuelNeedle, Diesel_Fuel_Gauge_v10, 153, 138);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////////////////
    // Setup the sprites to make a simulated mechanical counter display for 
    // showing the engine hours.

    mechDigDisp.createSprite(240, 240);
    mechDigDisp.fillSprite(TFT_BLACK);

    // This creates a foreground image for the mech digit display to make a
    // rounded outline and labeling. The black regions are transparent
    createMechFGImg();

    // Create a tall image (33 x 559 px) to take digit sliding window sub-regions.
    // This gives the animation needed to simulate a mechanical counter action.
    createMechDigits();

    // Setup the rolling digit to be taller
    mech_digit0.createSprite(33, 70);

    // Setup the smaller digits
    mech_digit1.createSprite(33, 50);
    mech_digit2.createSprite(33, 50);
    mech_digit3.createSprite(33, 50);
    mech_digit4.createSprite(33, 50);
    // mech_digit5.createSprite(33, 50);
    
    //////////////////////////////////////////////////////////////////////////////////////
    // This is a 10fps animation routine to animate the mechanical digits 
    // in a somewhat realistic way. Why? Because it is fun and possibly 
    // has some "cool factor" as well.

    event_loop()->onRepeat(100, []() {
        // This array holds the current state of the digits
        static uint8_t curDigit[5] = {0, 0, 0, 0, 0};

        // First decimal place. This is a rolling digit representing 1/10th's of an hour
        float leastSigDigit = ((float)targetDigitArray[1] + ((float)targetDigitArray[0]/10.0f));
        mech_digits.pushToSprite(&mech_digit0,0,-30-(int)((leastSigDigit - 0.3) * 43.0));
        mech_digit0.pushToSprite(&mechDigDisp, 180-15,120-35-50);

        // Digit 1 -- hours
        mech_digits.pushToSprite(&mech_digit1,0,-40-((curDigit[0]/10.0) * 43));
        mech_digit1.pushToSprite(&mechDigDisp, 180-33-15,120-35+10-50);

        if((targetDigitArray[2] > curDigit[0]/10) || ((int)curDigit[0] >= 90 && (int)targetDigitArray[2] == 0)) {
            curDigit[0]++;

            if(curDigit[0] > 99) curDigit[0] = 0;
        }

        // Digit 2 -- 10's of hours
        mech_digits.pushToSprite(&mech_digit2,0,-40-((curDigit[1]/10.0) * 43));
        mech_digit2.pushToSprite(&mechDigDisp, 180-33-33-15,120-35+10-50);

        if((targetDigitArray[3] > curDigit[1]/10) || ((int)curDigit[1] >= 90 && (int)targetDigitArray[3] == 0)) {
            curDigit[1]++;

            if(curDigit[1] > 99) curDigit[1] = 0;
        }

        // Digit 3 -- 100's of hours
        mech_digits.pushToSprite(&mech_digit3,0,-40-((curDigit[2]/10.0) * 43));
        mech_digit3.pushToSprite(&mechDigDisp, 180-33-33-33-15,120-35+10-50);

        if((targetDigitArray[4] > curDigit[2]/10) || ((int)curDigit[2] >= 90 && (int)targetDigitArray[4] == 0)) {
            curDigit[2]++;

            if(curDigit[2] > 99) curDigit[2] = 0;
        }

        // Digit 4 -- 1000's of hours
        mech_digits.pushToSprite(&mech_digit4,0,-40-((curDigit[3]/10.0) * 43));
        mech_digit4.pushToSprite(&mechDigDisp, 180-33-33-33-33-15,120-35+10-50);

        if((targetDigitArray[5] > curDigit[3]/10) || ((int)curDigit[3] >= 90 && (int)targetDigitArray[5] == 0)) {
            curDigit[3]++;

            if(curDigit[3] > 99) curDigit[3] = 0;
        }

        // // Digit 5 -- Do we need this many digits? 
        // mech_digits.pushToSprite(&mech_digit5,0,-40-((curDigit[4]/10.0) * 43));
        // mech_digit5.pushSprite(180-33-33-33-33-33,120-35+10);

        // if((targetDigitArray[6] > curDigit[4]/10) || ((int)curDigit[4] >= 90 && (int)targetDigitArray[6] == 0)) {
        //     curDigit[4]++;

        //     if(curDigit[4] > 99) curDigit[4] = 0;
        // }

        /////////////////////////////////////////////////////////////////////
        // Draw a forground bezel image over the mech digits
        // 
        mech_FG.pushToSprite(&mechDigDisp,0,0, TFT_BLACK);

        // Now show the mech digit on the display
        selectDisplay(1);
        mechDigDisp.pushSprite(0,0);
    });

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

    ledcWrite(LCD_BACKLIGHT_PIN, (uint32_t)800);
    // ledcWrite(LCD_BACKLIGHT_PIN, (uint32_t)200);
    // ledcWrite(LCD_BACKLIGHT_PIN, (uint32_t)50);
    // ledcWrite(LCD_BACKLIGHT_PIN, (uint32_t)25);
    // ledcWrite(LCD_BACKLIGHT_PIN, (uint32_t)10);
    // ledcWrite(LCD_BACKLIGHT_PIN, (uint32_t)2047);


    //////////////////////////////////////////////////////////////////////////////////////
    // Using demo data from development SignalK server. Using the DBT for this test.
    // This will, of course, be connected to engine oil pressure when finished.

    auto* depthListener = new FloatSKListener("environment.depth.belowTransducer");

    depthListener
        ->connect_to(new LambdaConsumer<float>(
            [](float value) { 
                ESP_LOGD("MyApp", "DBT:     %f meters", value);

                selectDisplay(2);
                // digitalWrite(LCD_0_CS, LOW); // native display too, tee-hee Just wanted to show it on two screens at a time.
                oilPressureGauge->updateGauge(value);
            }));


    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // Data gathering of engine run state and runtime hours 
    //
    auto* input_state = new DigitalInputState(ENGINE_RUN_INPUT, INPUT_PULLUP, 500, "/Sensors/EngineState");

    // create a propulsion state lambda transform
    auto* propulsion_state = new LambdaTransform<float, String>(
        [](bool state) {
            if (!state) {
                return "running";
            } else {
                return "stopped";
            }
        },
        "/Transforms/PropulsionState");

    ConfigItem(propulsion_state)
        ->set_title("Propulsion State")
        ->set_sort_order(1200);

    input_state->connect_to(propulsion_state);

    auto* invert_state = new LambdaTransform<float, bool>(
        [](bool state) {
            if (state) {
                return false;
            } else {
                return true;
            }
        },
        "/Transforms/InvertPropulsionState");

    // create engine hours counter using PersistentDuration
    auto* engine_hours = new TimeCounter<float>("/Transforms/EngineHours");

    ConfigItem(engine_hours)->set_title("Engine Hours")->set_sort_order(1300);

    input_state
        ->connect_to(invert_state) // Need to invert this due to the input being reversed. Fix in hardware?
        ->connect_to(engine_hours);

    // create and connect the propulsion state output object
    propulsion_state->connect_to(
        new SKOutput<String>("propulsion.main.state", "", new SKMetadata("", "Main Engine State")));

    // create and connect the engine hours output object
    // Also, send the engine runtime seconds to a lambda funtion that sends the digits to the mech digit display
    engine_hours
        ->connect_to(new SKOutput<float>("propulsion.main.runTime", "", new SKMetadata("s", "Main Engine running time")))
        ->connect_to(new LambdaConsumer<float>(
            [](float engineSeconds) {
                std::vector<uint8_t> byteArray = intToByteArray((int)(engineSeconds / 1.0f));
                // std::vector<uint8_t> byteArray = intToByteArray((int)(engineSeconds / 36.0f)); // Convert seconds to hours  =====>> TODO: Do this when deployed.

                // Pad the array with some zeros for the leading digits
                for(int i=0; i<8; i++) {
                    byteArray.push_back(0);
                }

                // We only need six digits
                byteArray.resize(7);

                int count = 0;
                for (byte c : byteArray) {
                    targetDigitArray[count] = c;

                    count++;
                }
            }));

    //////////////////////////////////////////////////////////////////////////////////
    // engine_coolant_temp_read_callback

    // auto* engine_temp_input = new RepeatSensor<float>(1000, engine_coolant_temp_read_callback);

    // engine_temp_input
    //     ->connect_to(new SKOutputFloat("propulsion.main.temperature", "/engine/temp/sk"));

    // auto* diesel_tank_level_input = new RepeatSensor<float>(1000, diesel_tank_level_read_callback);

    // diesel_tank_level_input
    //     ->connect_to(new SKOutputFloat("tanks.fuel.main.currentLevel", "/diesel/tank/sk"));

    auto* engineTempListener = new FloatSKListener("propulsion.main.temperature");

    engineTempListener
        // ->connect_to(new KelvinToFahrenheit())
        ->connect_to(new LambdaConsumer<float>(
            [](float value) { 
                ESP_LOGD("MyApp", "Temp:     %f C", value - 273.15);

                selectDisplay(3);
                coolantTempGauge->updateGauge((value - 273.15));
            }));

    auto* fuelListener = new FloatSKListener("tanks.fuel.main.currentLevel");

    fuelListener
        ->connect_to(new LambdaConsumer<float>(
            [](float value) { 
                ESP_LOGD("MyApp", "Fuel:     %f %%", value);

                selectDisplay(0);
                fuelLevelGauge->updateGauge(value * 100);
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
