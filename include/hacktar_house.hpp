#pragma once
#include "Arduino.h"
#include <FastLED.h>

const int NUM_NEOPIXELS = 3; // How many NeoPixels are attached to the Arduino?
const int NEO_BRIGHTNESS = 25; // Set BRIGHTNESS to about 1/5 (max = 255)
const int NEO_PIN = 17; // Which pin on the Arduino is connected to the NeoPixels?
const int DRONE_CAMERA = 10; // Drone Camera LED
const int HOUSE_RIGHT_EYE_RED = 13; //select the pin for the red LED
const int HOUSE_RIGHT_EYE_GREEN = 12; // select the pin for the green LED
const int HOUSE_RIGHT_EYE_BLUE = 11; // select the pin for the  blue LED
const int HOUSE_LEFT_EYE_RED = 5; //select the pin for the red LED
const int HOUSE_LEFT_EYE_GREEN = 4;// select the pin for the green LED
const int HOUSE_LEFT_EYE_BLUE = 3;  // select the pin for the  blue LED

/**
 * @brief High-level interface for working with the on-board indicators. Call HacktarHouse::setup() before use.
*/
class HacktarHouse {
    private:
        static CRGB neo_rgb_leds[NUM_NEOPIXELS];
    public:
        static void setup();
        static void set_eyes(const char * hexcolor);
        static void set_iot_brightness(int brightness);
        static void set_iot_device1(const char * hexcolor);
        static void set_iot_device2(const char * hexcolor);
        static void set_iot_device3(const char * hexcolor);
        static void set_iot_device(int device, const char * hexcolor);
        static void set_drone(bool watching);
        static void party();
        static void cylon();
};