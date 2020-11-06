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
    Wire.begin();
    if (airSensor.begin() == false) {
        Serial.println("Air sensor not detected. Please check wiring. Freezing...");
        while (1);
    }

    airSensor.setAutoSelfCalibration(false); // no auto calibration
    airSensor.setMeasurementInterval(2);     // CO2-Messung alle 5 s
    Serial.println("ZTL - Zentrum fuer Technikkultur Landau");
    Serial.println("http://ztl.space");
    Serial.println("Firmware Version 1.0");
    pixels.begin();
    pixels.setBrightness(255);
    setColor(0,0,0);

    //rainbow(10); //Party Mode :-)
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
 
  if((ledState == HIGH) && (currentMillis - previousMillis >= freq)){
    ledState = LOW;
    previousMillis = currentMillis;
    setColor(0,0,0);
  }
  else if ((ledState == LOW) && (currentMillis - previousMillis >= freq)){
    ledState = HIGH;  // turn it on
    previousMillis = currentMillis;
    setColor(255,0,0);
  }
}
void rainbow(int wait) {
  for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256) {
    for (int i = 0; i < pixels.numPixels(); i++) { // For each pixel in pixels...
      int pixelHue = firstPixelHue + (i * 65536L / pixels.numPixels());
      pixels.setPixelColor(i, pixels.gamma32(pixels.ColorHSV(pixelHue)));
    }
    pixels.show(); // Update pixels with new contents
    delay(wait);  // Pause for a moment
  }
}
int ppm =0;
void loop() {
   
    
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

    if (  ppm  <  1000 ) //0 ...1000 Grün
    {
      setColor(0,255,0);
      //Serial.print("green");
    }
    else
    {
        if (ppm  <  1400 ) //1001 ...1400 Gelb
        {
          setColor(255,255,0);
          //Serial.print("yellow");
        }
        else
        {
          if (ppm  <  2000 ) //1401 ...2000 Rot
          {
             setColor(255,0,0);
            //Serial.print("red");
          }
          else // > 2000 Rot blinkend
          {
            blinkRed(500);
            //Serial.print("red Blink");
          }
        }
    }
}
