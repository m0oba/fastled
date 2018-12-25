
// TODO Flickering LED's ... https://github.com/FastLED/FastLED/issues/394 or https://github.com/FastLED/FastLED/issues/306

//library
#include "FastLED.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <EEPROM.h>
#include <PubSubClient.h>

//files
#include "GradientPalettes.h"
#include "Inits.h"
#include "DCL.h"
#include "MQTT.h"
#include "function.h"
#include "Pattern.h"

typedef void (*Pattern)();
typedef Pattern PatternList[];
typedef struct { Pattern prog;String name;} PatternAndName;
typedef PatternAndName PatternAndNameList[];
PatternAndNameList Playlistpa = 
{
  { colorwaves, "Color Waves" },
  { pride, "Pride" },
  { rainbow, "Rainbow" },
  { rainbowWithGlitter, "Rainbow With Glitter" },
  { confetti, "Confetti" },
  { sinelon, "Sinelon" },
  { juggle, "Juggle" },
  { bpm, "BPM" },
};
const uint8_t PlaylistpaCount = 
  sizeof( Playlistpa) / sizeof( Playlistpa[0] );


void setup(void) {
  Serial.begin(115200);
  delay(100);
  EEPROM.begin(512);
  
  loadSettings();
  initFastLED();
  logSys();
  initWlan();
  initOTA();
  ArduinoOTA.begin();
  initMQTT();

}
void callback(char* topic, byte* payload, unsigned int length) 
{
  // handle message arrived
  char tmp[length + 1];
  strncpy(tmp, (char*)payload, length);
  tmp[length] = '\0';
  String data(tmp);
  String topics(topic);
  Serial.print("Received Data from ");
  Serial.print(topics);
  Serial.printf(" : %s",data.c_str());
  Serial.println();
  
  if (topics == "1/XL01/color/cmd") 
  {
    Serial.println(topics);
    String h =  getValue(data, ',', 0);
    String s =  getValue(data, ',', 1);
    String v =  getValue(data, ',', 2);
    int hue = map(h.toInt(),0,360,0,255);
    int sat = map(s.toInt(),0,100,0,255);
    int val = map(v.toInt(),0,100,0,255);
    CHSV HSVtemp( hue, sat, val);
    solidColor = HSVtemp;
    CRGB RBBtemp = HSVtemp;
    XL_mode = "color";
    XL_playlist = "ON";
    Serial.println(solidColor.red);
    Serial.println(solidColor.green);
    Serial.println(solidColor.blue);
    client.publish("1/XL01/color",tmp);
    client.publish("1/XL01/mode","color");
    client.publish("1/XL01/playlist","OFF");
    EEPROM.write(1, solidColor.red);
    EEPROM.write(2, solidColor.green);
    EEPROM.write(3, solidColor.blue);
  }
  else if (topics == "1/XL01/mode/cmd")
  {
    XL_mode= data;
    client.publish("1/XL01/mode",tmp);
  }
  else if (topics == "1/XL01/power/cmd")
  {
    setPower(data);
    client.publish("1/XL01/power",tmp);
  }
  else if (topics == "1/XL01/pattern/cmd")
  {
      for ( int i = 0; i < 9; ++i ) 
    {
      if (Playlistpa[i].name == data )
      { 
        XL_mode = "pattern";
        XL_playlist = "OFF";
        currentPatternIndex = i;
        client.publish("1/XL01/pattern",tmp);
        client.publish("1/XL01/mode","pattern");
        client.publish("1/XL01/playlist","OFF");
        EEPROM.write(1, i);
        EEPROM.commit();
      }  
    } 
  } 
  else if (topics == "1/XL01/brightness/cmd")
  {
      int bri = map(data.toInt(),0,100,0,255);
      FastLED.setBrightness(bri);
      client.publish("1/XL01/brightness",tmp);
      EEPROM.write(0, bri);
      EEPROM.commit();
  } 
  else if (topics == "1/XL01/effect/cmd")
  {
      XL_effect = data;
      client.publish("1/XL01/effect",tmp);
  } 
  else if (topics == "1/XL01/gradient/cmd")
  {
      XL_mode = "gradient";
      XL_playlist = "OFF";
      XL_effect = "normal";
      XL_gradient = data.toInt();
      gCurrentPaletteNumber = data.toInt();
      client.publish("1/XL01/gradient",tmp);
      client.publish("1/XL01/mode","gradient");
      client.publish("1/XL01/playlist","OFF");
      client.publish("1/XL01/effect","normal");
  
  }
  else if (topics == "1/XL01/playlist/cmd")
  {
    XL_playlist = data;
    client.publish("1/XL01/playlist",tmp);    
  }
  else if (topics == "1/XL01/second/cmd")
  {
    PlaylistSecond = data.toInt();
    client.publish("1/XL01/second",tmp);    
  } 
}
void loop(void)
{
  yield(); // Avoid crashes on ESP8266
  // Add entropy to random number generator; we use a lot of it.
  random16_add_entropy(random(65535));
  ArduinoOTA.handle();
  if (!client.connected()) 
  {
    reconnectMqtt();
  }
  client.loop();

  if (power == 0) 
  {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    delay(15);
    return;
  }
  
  // EVERY_N_SECONDS(10) {
  //   Serial.print( F("Heap: ") ); Serial.println(system_get_free_heap_size());
  // }
  
  EVERY_N_MILLISECONDS( 20 ) 
  {
    gHue++;  // slowly cycle the "base color" through the rainbow
    
  }
  EVERY_N_SECONDS( PlaylistSecond ) 
  {
    if (XL_playlist == "ON")
    {
    currentPatternIndex = addmod8( currentPatternIndex, 1, PlaylistpaCount);  
    gCurrentPaletteNumber = addmod8( gCurrentPaletteNumber, 1, gGradientPaletteCount);
    }
  }

  if (XL_mode == "color") 
  {
    fill_solid(leds, NUM_LEDS, solidColor);
  }

  if (XL_mode == "pattern") 
  {
    Playlistpa[currentPatternIndex].prog();
  }

  if (XL_mode == "gradient")
  {
    gTargetPalette = gGradientPalettes[ gCurrentPaletteNumber ];
    
    if (XL_effect == "wave")
    {
      colorwaves( leds, NUM_LEDS, gCurrentPalette);
    }
    else if (XL_effect == "normal")
    {
      FillLEDsFromPaletteColors(255);
    }
  }
    
      
  // slowly blend the current cpt-city gradient palette to the next
  EVERY_N_MILLISECONDS(40) 
  {
    nblendPaletteTowardPalette( gCurrentPalette, gTargetPalette, 16); 
  }
    
  

  FastLED.show();    
  // insert a delay to keep the framerate modest
  delay(1000 / FRAMES_PER_SECOND);
}

/* Update MQTT
String XL_mode;
String XL_gradient;
String XL_effect;
String XL_playlistMode;
*/


void initMQTT() 
{
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  autoPlayTimeout = millis() + (autoPlayDurationSeconds * 1000);
}



