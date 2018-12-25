
#include "DCL.h"
#include <EEPROM.h>
#include "FastLED.h"



void setPower(String value)
{
  if (value == "ON") 
  { 
  power = 1;
  }
  else if (value == "OFF")
  {
  power = 0;
  }
  EEPROM.write(5, power);
  EEPROM.commit();
  
}


void setSolidColor(CRGB MQTTcolor)
{
  solidColor = MQTTcolor;

  EEPROM.write(2, MQTTcolor.red);
  EEPROM.write(3, MQTTcolor.green);
  EEPROM.write(4, MQTTcolor.blue);
  XL_mode = 1;
  
}

void setPattern(int value)
{
  currentPatternIndex = value;
  EEPROM.write(1, currentPatternIndex);
  EEPROM.commit();
}

void setBrightness(int value)
{
  // don't wrap around at the ends
  if (value > 255)
    value = 255;
  else if (value < 0) value = 0;

  brightness = value;

  FastLED.setBrightness(brightness);

  EEPROM.write(0, brightness);
  EEPROM.commit();
  
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

boolean isValidNumber(String str) 
{
  // TODO replace with regex check
  bool result = false;
  for (byte i = 0; i < str.length(); i++)
  {
    if (isDigit(str.charAt(i))) {
      result = true;
    } else {
      result = false;
      break;
    }
  }
  return result;
}

void String2char(String str, char* char_array)
{
    int str_len = str.length() + 1; 
    char_array[str_len];
    str.toCharArray(char_array, str_len);
}

