#ifndef DCL_H
#define DCL_H
#include "FastLED.h"




// Wi-Fi network to connect to (if not in AP mode)
const char* ssid = "true_home2G_5ab";
const char* password = "yKsF4UjP";

#define DATA_PIN      1     // for Huzzah: Pins w/o special function:  #4,#5, #12, #13, #14; // #16 does not work :(
#define LED_TYPE      WS2812
#define COLOR_ORDER   GRB
#define NUM_LEDS      162
#define MILLI_AMPS         500     // IMPORTANT: set here the maxmilli-Amps of your power supply 5V 2A = 2000
#define FRAMES_PER_SECOND  120 // here you can control the speed. With the Access Point / Web Server the animations run a bit slower.
int PlaylistSecond =20;
// MQTT Broker settings
const char* mqtt_server = "192.168.1.2";
const int mqtt_port = 1883; // Change this if u don't use a SSL connection
const char* mqtt_user = "mooba";
const char* mqtt_password = "mooba869ztz";
const char* mqtt_topic = "1/XL01";
const char* mqtt_clientid = "XL01";
CRGB leds[NUM_LEDS];
WiFiClient espClient;
PubSubClient client(espClient);
bool updateMQTT =0;

int p1 = 0;
int r1 = 128;
int g1 = 0;
int b1 = 0;
int p2 = 128;
int r2 = 128;
int g2 = 128;
int b2 = 128;
int p3 = 255;
int r3 = 0;
int g3 = 0;
int b3 = 128;


//pattern declaration
char* currentPatternname = "test";
uint8_t currentPatternIndex = 0; // Index number of which pattern is current
uint8_t autoPlayDurationSeconds = 10;
unsigned int autoPlayTimeout = 0;
uint8_t patternIndex = 0;
uint8_t gHue = 0; // rotating "base color" used by many of the pattern
String playPA = "no data";

//Openhab declaration
String XL_mode = "color";
String XL_gradient;
String XL_effect ="normal";
String XL_playlist = "ON";

//fastled declaration
CRGB solidColor = CRGB::Blue;
FASTLED_USING_NAMESPACE
#define FASTLED_ALLOW_INTERRUPTS 0
//#define FASTLED_INTERRUPT_RETRY_COUNT 0
uint8_t power = 1;
uint8_t brightness;




#endif