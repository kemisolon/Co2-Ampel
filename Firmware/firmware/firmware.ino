/*
  ZTL - Ampel - Version 1.0
  http://ztl.space
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include "SparkFun_SCD30_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD30
#define PIN         6
#define NUMPIXELS   17 // oder 9 oder 17 LEDs
SCD30 airSensor;
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// für den loop without delay

unsigned long previousMillis = 0; // Zeitpunkt an dem zuletzt geschalten wurde
const long interval = 2000;       // Länge der Pause in ms, hier 2 sek.
int startPixel = 0;               // erster Pixel für initSequenz
int ppm = 0;                      // 0 = ppm nicht gesetzt

//
// SETUP

void setup() {

  Serial.begin(115200);

  // Sensor Init...

  Wire.begin();
  if (airSensor.begin() == false) {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    pixels.begin();
    pixels.setBrightness(150);
    setColor(234, 10, 142); // pink dauerhaft, wenn Sensor nicht erkannt.
    while (1);
  }
  else
  {
    Serial.println("Air sensor detected!");
  }

  airSensor.setAutoSelfCalibration(false); // no auto calibration
  airSensor.setMeasurementInterval(2);     // CO2-Messung alle 5 s

  Serial.println("ZTL - Zentrum fuer Technikkultur Landau");
  Serial.println("http://ztl.space");
  Serial.println("Firmware Version 1.0");

  // Starte mit Regenbogen...

  pixels.begin();
  pixels.setBrightness(255);
  rainbow(10); // Init Party Mode :-)
  setColor(0, 0, 0); // schalte alle LEDs aus

  Serial.println("Initialisierung abgeschlossen.");
}

//
// Funktion SETCOLOUR zum Setzen der LED-Farben auf allen LEDs

void setColor(int r, int g, int b ) {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  pixels.show();
}

//
// Funktion BLINKRED zum roten Blinken aller LEDs

void blinkRed(long freq)
{
  static unsigned long previousMillis = 0;
  static bool ledState;
  unsigned long currentMillis = millis();

  if ((ledState == HIGH) && (currentMillis - previousMillis >= freq)) {
    ledState = LOW;
    previousMillis = currentMillis;
    setColor(0, 0, 0);
  }
  else if ((ledState == LOW) && (currentMillis - previousMillis >= freq)) {
    ledState = HIGH;  // turn it on
    previousMillis = currentMillis;
    setColor(255, 0, 0);
  }
}

//
// Funktion RAINBOW für den Regenbogen-LED-Effekt am Anfang

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

//
// LOS GEHTS
//

void loop() {

  // Prüfe, ob Daten vom Sensor vorhanden sind

  if (airSensor.dataAvailable()) {
    ppm = airSensor.getCO2();
    Serial.print("co2(ppm):");
    Serial.print(ppm);

    Serial.print(" temp(C):");
    Serial.print(airSensor.getTemperature(), 1);

    Serial.print(" humidity(%):");
    Serial.print(airSensor.getHumidity(), 1);

    Serial.println();
  }


  // Loop without delay

  unsigned long currentMillis = millis(); // Aktuelle Zeit wird in currentMillis gespeichert
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis; // Zeitpunkt der letzten Ausfuehrung wird festgehalten

    // Alle 2 Sekunden...

    if ( ppm  <  1 ) // keine Daten vorhanden, LED Punkte leuchten nacheinander
    {
      pixels.setBrightness(255);
      pixels.setPixelColor(startPixel, pixels.Color(234, 10, 142));
      pixels.show();
      if ( startPixel < NUMPIXELS ) {
        startPixel++;
      }
      else {
        startPixel = 0;
        setColor(0, 0, 0);
        pixels.setPixelColor(startPixel, pixels.Color(234, 10, 142));
      }
      Serial.print("Status: no data - waiting for sensor-init - this takes up to 5 min...");
      Serial.println();
    }
    else if ( ppm  <  1000 ) // 0 ...1000 PPM Grün
    {
      setColor(0, 255, 0);
      Serial.print("Status: green");
      Serial.println();
    }
    else if ( ppm  <  1400 ) // 1001 ...1400 PPM Gelb
    {
      setColor(255, 215, 0);
      Serial.print("Status: yellow");
      Serial.println();
    }
    else if ( ppm  <  2000 ) // 1401 ...2000 PPM Rot
    {
      setColor(255, 0, 0);
      Serial.print("Status: red");
      Serial.println();
    }
    else // > 2000 PPM rot blinkend
    {
      blinkRed(500);
      Serial.print("Status: blink-red");
      Serial.println();
    }
  }
}
