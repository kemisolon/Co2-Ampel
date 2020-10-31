/*
  Reading CO2, humidity and temperature from the SCD30
  By: Nathan Seidle
  SparkFun Electronics
  Date: May 22nd, 2018
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  Feel like supporting open source hardware?
  Buy a board from SparkFun! https://www.sparkfun.com/products/15112

  This example prints the current CO2 level, relative humidity, and temperature in C.

  Hardware Connections:
  Attach RedBoard to computer using a USB cable.
  Connect SCD30 to RedBoard using Qwiic cable.
  Open Serial Monitor at 115200 baud.
*/

#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include "SparkFun_SCD30_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD30
#define PIN         6
#define NUMPIXELS   5
SCD30 airSensor;
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  Serial.begin(115200);
  Serial.println("SCD30 Example");
  Wire.begin();
  pixels.begin();
  pixels.setBrightness(100);
  if (airSensor.begin() == false)
  {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1)
      ;
  }

  //The SCD30 has data ready every two seconds
}
bool blink=false; 
int ppm =0;
void loop()
{
  if (airSensor.dataAvailable())
  {
    ppm = airSensor.getCO2();
    Serial.print("co2(ppm):");
    Serial.print(ppm);

    Serial.print(" temp(C):");
    Serial.print(airSensor.getTemperature(), 1);

    Serial.print(" humidity(%):");
    Serial.print(airSensor.getHumidity(), 1);

    Serial.println();
    delay(500);

  }
  else
  {
    Serial.println("Waiting for new data");
  }
  switch(ppm){
    case 400 ... 700: //grün
      for(int i=0; i<NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0,255, 0));
      }
      break;
    case 701 ... 1000: //gelb
      for(int i=0; i<NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(255,255, 0));
      }
      break;
    case 1001 ... 2000: //rot
      for(int i=0; i<NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(255,0, 0));
      }
      break;
    case 2001 ... 5000:// rot -blinkend
      for(int i=0; i<NUMPIXELS; i++) {
        if(blink)
        {
          pixels.setPixelColor(i, pixels.Color(255,0, 0));
        }
        else
        {
          pixels.setPixelColor(i, pixels.Color(0,0, 0));
        }
      }
      blink= !blink;
      break;
  }
    pixels.show();
  delay(500);
}
