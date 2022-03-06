#include <Wire.h>
#include <Blynk.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <SoftwareSerial.h>
#include<SH1106.h>
#include "OLEDDisplayUi.h"
#define BLYNK_PRINT Serial
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SimpleTimer.h>
BlynkTimer timer;
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
SH1106 display(0x3C, D1, D2);
OLEDDisplayUi ui ( &display );
DHT dht(D3, DHT22);
float  temperature, humidity, temp, fahrenheit;
int Signal;
int PulseSensorPurplePin = 0;
char auth[] = "XIIpHMe8qTXOy79ZtDocJD2LcsVx5R1O";
char ssid[] = "xyz";
char pass[] = "123456789";
void serialPrint() {
  Serial.print("DHT Temperature = ");
  Serial.print(temperature);
  Serial.println(" *C");
  Serial.print("Humidity = ");
  Serial.print(humidity);
  Serial.println(" %");
  Serial.print(Signal);
  Serial.println(" bpm");
  Serial.print(" Temperature = ");
  Serial.print(temp);
  Serial.println(" *C");
  Serial.print("Fahrenheit");
  Serial.print(fahrenheit);
  Serial.println(" F");
  temp = DS18B20.getTempCByIndex(0); // Celcius
  fahrenheit = DS18B20.toFahrenheit(temp);
}
void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
}
void drawFrame1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_16);
  display->drawStringMaxWidth(64 + x, 0 + y, 128, "Health Monitoring System");
}
void drawFrame2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  display->drawStringMaxWidth(64 + x, 0 + y, 128, "*****Pulse Sensor*****");
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawStringMaxWidth(10 + x, 20 + y, 128, "Pulse: " + String(Signal) + "bpm");
}
void drawFrame3(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  display->drawStringMaxWidth(64 + x, 0 + y, 128, "*****DHT22 Sensor*****");
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawStringMaxWidth(10 + x, 13 + y, 128, "Temperature: " + String(temperature) + "*C");
  display->drawStringMaxWidth(10 + x, 23 + y, 128, "Humidity: " + String(humidity) + "%");
}
void drawFrame4(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  display->drawStringMaxWidth(64 + x, 0 + y, 128, "*Temperature Sensor*");
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawStringMaxWidth(10 + x, 13 + y, 128, "Temperature: " + String(temp) + "*C");
  display->drawStringMaxWidth(10 + x, 23 + y, 128, "Temperature: " + String(fahrenheit) + "F");
}
FrameCallback frames[] = { drawFrame1, drawFrame2, drawFrame3, drawFrame4};
int frameCount = 4;
OverlayCallback overlays[] = { msOverlay };
int overlaysCount = 1;
void setup()
{
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  Serial.print("Connecting........");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print("Waiting to connect with WiFi\n");
  }
  Serial.print("WiFi is connected \n");
  Serial.print(WiFi.localIP());
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Could not find a valid DHT sensor, check wiring!");
    return;
  }
  dht.begin();
  DS18B20.begin();
  DS18B20.requestTemperatures();
  ui.setTargetFPS(60);
  ui.setIndicatorPosition(BOTTOM);
  ui.setIndicatorDirection(LEFT_RIGHT);
  ui.setFrameAnimation(SLIDE_LEFT);
  ui.setFrames(frames, frameCount);
  ui.setOverlays(overlays, overlaysCount);
  ui.init();
  display.flipScreenVertically();
  timer.setInterval(1000, serialPrint);
}
void loop()
{
  timer.run(); // Initiates SimpleTimer
  Blynk.run();
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  DS18B20.requestTemperatures(); 
  temp = DS18B20.getTempCByIndex(0);  // Celcius
  fahrenheit = DS18B20.toFahrenheit(temp);
  Signal = analogRead(PulseSensorPurplePin);
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, Signal);
  Blynk.virtualWrite(V3, temp); //virtual pin V3
  Blynk.virtualWrite(V4, fahrenheit);
  int remainingTimeBudget = ui.update();
  if (remainingTimeBudget > 0) {
    delay(remainingTimeBudget);
  }
}
