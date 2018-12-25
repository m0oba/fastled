
#include "DCL.h"





void reconnectMqtt() 
{
   // Loop until we're reconnected
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("XS01", "mooba", "mooba869ztz")) 
    {
      Serial.println("connected");
      client.subscribe("1/XL01/color/cmd");
      client.subscribe("1/XL01/brightness/cmd");
      client.subscribe("1/XL01/pattern/cmd");
      client.subscribe("1/XL01/power/cmd");
      client.subscribe("1/XL01/gradient/cmd");
      client.subscribe("1/XL01/effect/cmd");
      client.subscribe("1/XL01/mode/cmd");
      client.subscribe("1/XL01/playlist/cmd");
      client.subscribe("1/XL01/second/cmd");
    } 
    else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

