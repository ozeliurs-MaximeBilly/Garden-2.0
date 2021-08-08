#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include "Adafruit_SHT31.h"

/* Set these to your desired credentials. */
const char *ssid = "Humidity";

const int dry = 711; // value for dry sensor
const int wet = 291; // value for wet sensor

ESP8266WebServer server(80);
Adafruit_BMP280 bmp;
Adafruit_SHT31 sht31 = Adafruit_SHT31();

void handleRoot() {
  
  int sensorVal = analogRead(A0);
  int percentageHumididy = map(sensorVal, wet, dry, 100, 0); 

  int temp = bmp.readTemperature();
  int pres = bmp.readPressure();

  float t = sht31.readTemperature();
  float h = sht31.readHumidity();

  if (isnan(t)) {
    float t = 0;
  }

  if (isnan(h)) {
    float h = 0;
  }

  char buffer[600];
  sprintf(buffer, "<h1>Soil Humidity: %i %s</h1>\n<h1>Air Temperature (BMP280): %i *C</h1>\n<h1>Air Pressure: %i Pa</h1>\n<h1>Air Temperature (SHT31): %i *C</h1>\n<h1>Air Humidity: %i %s</h1>", percentageHumididy, "%", temp, pres, (int)t, (int)h, "%");
  
  server.send(200, "text/html", buffer);
}

void setup() {
  Serial.begin(9600);

  Serial.println(F("BMP280 test"));
  if (!bmp.begin(0x76)) { // Set to 0x77 for alternate i2c addr
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or try a different address!"));
    while (1) delay(10);
  }

  Serial.println(F("SHT31 test"));
  if (! sht31.begin(0x44)) { // Set to 0x45 for alternate i2c addr
    Serial.println(F("Could not find a valid SHT3x sensor, check wiring or try a different address!"));
    while (1) delay(10);
  }
  
  WiFi.softAP(ssid);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");

    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

void loop()
{
  server.handleClient();
}
