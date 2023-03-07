#include "hacktar_house.hpp"

#ifdef FIREBEETLE32
#define HIGH 0
#define LOW 1
#endif

CRGB HacktarHouse::neo_rgb_leds[NUM_NEOPIXELS];

/**
 * @brief Initalizes pin modes for various indicators.
*/
void HacktarHouse::setup() {
    pinMode(DRONE_CAMERA, OUTPUT);
    pinMode(HOUSE_RIGHT_EYE_RED, OUTPUT);
    pinMode(HOUSE_RIGHT_EYE_GREEN, OUTPUT);
    pinMode(HOUSE_RIGHT_EYE_BLUE, OUTPUT);
    pinMode(HOUSE_LEFT_EYE_RED, OUTPUT);
    pinMode(HOUSE_LEFT_EYE_GREEN, OUTPUT);
    pinMode(HOUSE_LEFT_EYE_BLUE, OUTPUT);
    digitalWrite(HOUSE_RIGHT_EYE_RED, HIGH);
    digitalWrite(HOUSE_RIGHT_EYE_GREEN, HIGH);
    digitalWrite(HOUSE_RIGHT_EYE_BLUE, HIGH);
    digitalWrite(HOUSE_LEFT_EYE_RED,HIGH);
    digitalWrite(HOUSE_LEFT_EYE_GREEN, HIGH);
    digitalWrite(HOUSE_LEFT_EYE_BLUE, HIGH);
    FastLED.addLeds<WS2812B, NEO_PIN>(neo_rgb_leds, NUM_NEOPIXELS); // Only try to realize the truth... there is no pin
    FastLED.setBrightness(10);
    HacktarHouse::neo_rgb_leds[0] = CRGB::Black;
    HacktarHouse::neo_rgb_leds[1] = CRGB::Black;
    HacktarHouse::neo_rgb_leds[2] = CRGB::Black;
    FastLED.show();
}

/**
 * @brief Enables/disables the drone camera light.
 * @param[in] drone_status True for on, False for off.
*/
void HacktarHouse::set_drone(bool drone_status) {
    digitalWrite(DRONE_CAMERA, drone_status);
}

/**
 * @brief Sets the color of the eyes on the front of the house.
 * @param[in] r Turn the RED LED on (True) or off (False)
 * @param[in] g Turn the GREEN LED on (True) or off (False)
 * @param[in] b Turn the BLUE LED on (True) or off (False)
*/
void HacktarHouse::set_eyes(const char * hexcolor) {
    int r, g, b;
    sscanf(hexcolor, "%02x%02x%02x", &r, &g, &b);

    digitalWrite(HOUSE_RIGHT_EYE_RED, (r > 1) ? LOW : HIGH);
    digitalWrite(HOUSE_LEFT_EYE_RED, (r > 1) ? LOW : HIGH);
    digitalWrite(HOUSE_RIGHT_EYE_GREEN, (g > 1) ? LOW : HIGH);
    digitalWrite(HOUSE_LEFT_EYE_GREEN, (g > 1) ? LOW : HIGH);
    digitalWrite(HOUSE_RIGHT_EYE_BLUE, (b > 1) ? LOW : HIGH);
    digitalWrite(HOUSE_LEFT_EYE_BLUE, (b > 1) ? LOW : HIGH);
}

void HacktarHouse::set_iot_brightness(int brightness) {
    if (brightness > 255)
        brightness = 255;
    FastLED.setBrightness(brightness);
    FastLED.show();
}

/**
 * @brief Sets the color of the IOT device LED indicator
 * @param[in] device IOT device indicator number
 * @param[in] hexcolor String hex color to set the IOT indicator to
*/
void HacktarHouse::set_iot_device(int device, const char * hexcolor) {
    int r, g, b, r_t;
    sscanf(hexcolor, "%02x%02x%02x", &r, &g, &b);
    r_t = r; r = g; g = r_t; // Swapping red and blue due to a pinning issue?
    HacktarHouse::neo_rgb_leds[device].setRGB(r, g, b);
    FastLED.show();
}

/**
 * @brief Sets the color of the 1st (right) IOT device LED indicator
 * @param[in] hexcolor String hex color to set the IOT indicator to
*/
void HacktarHouse::set_iot_device1(const char * hexcolor) {
    HacktarHouse::set_iot_device(0, hexcolor);
}

/**
 * @brief Sets the color of the 2nd (middle) IOT device LED indicator
 * @param[in] hexcolor String hex color to set the IOT indicator to
*/
void HacktarHouse::set_iot_device2(const char * hexcolor) {
    HacktarHouse::set_iot_device(1, hexcolor);
}

/**
 * @brief Sets the color of the 3rd (left) IOT device LED indicator
 * @param[in] hexcolor String hex color to set the IOT indicator to
*/
void HacktarHouse::set_iot_device3(const char * hexcolor) {
    HacktarHouse::set_iot_device(2, hexcolor);
}

void HacktarHouse::party() {
    char* colors[3] = {"ff0000", "00ff00", "0000ff"};
    /*char* colors[9] = { "FF6B6B",
        "FF6BB5",
        "FF81FF",
        "D081FF",
        "81ACFF",
        "81FFFF",
        "81FF81",
        "FFD081",
        "FF8181"
    };*/

    for(int i = 0; i < 20; i++) {
        HacktarHouse::set_eyes(colors[i % 3]); 
        HacktarHouse::set_iot_device1(colors[i % 3]);
        HacktarHouse::set_iot_device2(colors[(i + 1) % 3]);
        HacktarHouse::set_iot_device3(colors[(i + 2) % 3]);
        delay(300);
        HacktarHouse::set_eyes("000000");
        HacktarHouse::set_iot_device1("000000");
        HacktarHouse::set_iot_device2("000000");
        HacktarHouse::set_iot_device3("000000");
        delay(100);
    }
}

void HacktarHouse::cylon() {
    int time_between = 25;
    for (int n = 0; n < 3; n++) {
        for (int m = 0; m < NUM_NEOPIXELS; m++) {
            HacktarHouse::neo_rgb_leds[m].setRGB(0, 255, 0);
            FastLED.show();
            delay(time_between);
            HacktarHouse::neo_rgb_leds[m].setRGB(0, 0, 0);
            delay(time_between);
            FastLED.show();
        }
        for (int m = NUM_NEOPIXELS - 1; m > -1; m--) {
            HacktarHouse::neo_rgb_leds[m].setRGB(0, 255, 0);
            FastLED.show();
            delay(time_between);
            HacktarHouse::neo_rgb_leds[m].setRGB(0, 0, 0);
            delay(time_between);
            FastLED.show();
        }
    }
}