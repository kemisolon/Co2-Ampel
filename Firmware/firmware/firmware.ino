/*
ZTL - Ampel
http://ztl.space
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include "SparkFun_SCD30_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD30
#define PIN         6
#define NUMPIXELS   9
SCD30 airSensor;
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {

    Serial.begin(115200);
    Serial.println("ZTL - Zentrum fuer Technikkultur Landau");
    Serial.println("http://ztl.space");
    Wire.begin();
    pixels.begin();
    pixels.setBrightness(150);
    setColor(0,0,255);
    if (airSensor.begin() == false) {
        Serial.println("Air sensor not detected. Please check wiring. Freezing...");
        while (1);
    }
}

void setColor(int r, int g, int b ){
  for(int i=0; i<NUMPIXELS; i++) {
          pixels.setPixelColor(i, pixels.Color(r,g, b));
    }
   pixels.show();
}

void blinkRed(long freq)
{
  static unsigned long previousMillis = 0;
  static bool ledState;

  unsigned long currentMillis = millis();
 
  if((ledState == HIGH) && (currentMillis - previousMillis >= freq))
  {
    ledState = LOW;
    previousMillis = currentMillis;
    setColor(0,0,0);
  }
  else if ((ledState == LOW) && (currentMillis - previousMillis >= freq))
  {
    ledState = HIGH;  // turn it on
    previousMillis = currentMillis;
    setColor(255,0,0);
  }

}

void loop() {
    int ppm =0;
    
     if(airSensor.dataAvailable()) {
        ppm = airSensor.getCO2();
        Serial.print("co2(ppm):");
        Serial.print(ppm);

        Serial.print(" temp(C):");
        Serial.print(airSensor.getTemperature(), 1);

        Serial.print(" humidity(%):");
        Serial.print(airSensor.getHumidity(), 1);

        Serial.println();
    }

    switch(ppm) {
        case 1 ... 700: //grün
            setColor(0,255,0);
            break;
        case 701 ... 1000: //gelb
            setColor(255,255,0);
            break;
        case 1001 ... 2000: //rot
            setColor(255,0,0);
            break;
        case 2001 ... 9999:// rot -blinkend
            blinkRed(500);
            break;
    }
  
}
