#include <esp_now.h>
#include <WiFi.h>
#include <SPI.h>
#include <Arduino.h>

#include "heltec-eink-modules.h"
#define PIN_BUSY 5
#define PIN_CS 4
#define PIN_DC 14
DEPG0213RWS800 display(PIN_DC, PIN_CS, PIN_BUSY); // 2.13" V2 - BWR - Red Tab
// ****************************************************
#include <Wire.h>
#include <SensirionI2CScd4x.h>

SensirionI2CScd4x scd4x;

// Read Measurement
uint16_t co2 = 0;
float temperature = 0.0f;
float humidity = 0.0f;
bool isDataReady = false;

void printUint16Hex(uint16_t value)
{
  Serial.print(value < 4096 ? "0" : "");
  Serial.print(value < 256 ? "0" : "");
  Serial.print(value < 16 ? "0" : "");
  Serial.print(value, HEX);
}

void printSerialNumber(uint16_t serial0, uint16_t serial1, uint16_t serial2)
{
  Serial.print("Serial: 0x");
  printUint16Hex(serial0);
  printUint16Hex(serial1);
  printUint16Hex(serial2);
  Serial.println();
}
// ****************************************************
int analogMQ7 = 34;     // Analog input pin for MQ-7 sensor
int ledPin = 2;         // Device internal LED
int MQ7sensorValue = 0; // Value read from the sensor
int mq2Pin = 35;
int mq2Value = 0;
int mq9Pin = 32;
int mq9Value = 0;
// ****************************************************
// Structure example to receive data
// Must match the sender structure
typedef struct struct_message
{
  char a[32];
  float b;
  float c;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Char: ");
  Serial.println(myData.a);
  Serial.print("Float: ");
  Serial.println(myData.b);
  Serial.print("Float: ");
  Serial.println(myData.c);
}
// ****************************************************
void setup()
{
  // Initialize Serial Monitor
  Serial.begin(115200);

  // ****************************************************
  Wire.begin();

  uint16_t error;
  char errorMessage[256];

  scd4x.begin(Wire);
  scd4x.readMeasurement(co2, temperature, humidity);
  // stop potentially previously started measurement
  error = scd4x.stopPeriodicMeasurement();
  if (error)
  {
    Serial.print("Error trying to execute stopPeriodicMeasurement(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  }

  uint16_t serial0;
  uint16_t serial1;
  uint16_t serial2;
  error = scd4x.getSerialNumber(serial0, serial1, serial2);
  if (error)
  {
    Serial.print("Error trying to execute getSerialNumber(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  }
  else
  {
    printSerialNumber(serial0, serial1, serial2);
  }

  // Start Measurement
  error = scd4x.startPeriodicMeasurement();
  if (error)
  {
    Serial.print("Error trying to execute startPeriodicMeasurement(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  }

  Serial.println("Waiting for first measurement... (5 sec)");
  // ****************************************************

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // display mac adress in serial monitor
  Serial.print("ESP32 MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);

  Serial.println(F("MQ-7 Gas Sensor Flying-Fish started"));
  ledcWrite(analogMQ7, 255);
  delay(6000);
}
// ****************************************************
int mq7()
{
  ledcWrite(analogMQ7, 71); // 28% of 255
  MQ7sensorValue = analogRead(analogMQ7);
  return MQ7sensorValue;
}

// ****************************************************
void updateDisplay()
{
  DRAW(display)
  {
    // set size of text to be bigger
    // set rotation of text to be 90 degrees
    display.setRotation(3);
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("MQ-7:");
    display.print(MQ7sensorValue);
    display.drawLine(0, 20, 250, 20, BLACK);
    display.setCursor(0, 30);
    display.print("MQ-2:");
    mq2Value = analogRead(mq2Pin);
    display.print(mq2Value);
    display.drawLine(0, 50, 250, 50, BLACK);
    display.setCursor(0, 60);

    display.print("MQ-9:");
    mq9Value = analogRead(mq9Pin);
    display.print(mq9Value);

    // dissplay co2, temperature and humidity
    display.drawLine(0, 80, 250, 80, BLACK);
    display.setCursor(0, 90);
    display.print("CO2:");
    display.print(co2);
    display.drawLine(0, 110, 250, 110, BLACK);
    display.setCursor(0, 120);
    display.print("Temp:");
    display.print(temperature);
    display.drawLine(0, 140, 250, 140, BLACK);
    display.setCursor(0, 150);
    display.print("Humidity:");
    display.print(humidity);
    }
}

// ****************************************************
void scd41()
{
  // Read Measurement
  uint16_t error = scd4x.readMeasurement(co2, temperature, humidity);
  if (error)
  {
    char errorMessage[256];
    Serial.print("Error trying to execute readMeasurement(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  }
  else
  {
    Serial.print("CO2 concentration: ");
    Serial.print(co2);
    Serial.println(" ppm");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" degrees C");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  }
}
// ****************************************************
// use millis to read mq7 every 5 seconds
unsigned long previousMillis = 0;
const long interval = 30000;

void loop()
{

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    scd41();
    delay(100);
    mq7();
    delay(100);
    Serial.print(F("MQ-7: "));
    Serial.println(MQ7sensorValue);

    // serial print start and milis
    Serial.print("Start: ");
    Serial.println(millis());
    updateDisplay();
    // show time taken to update display
    Serial.print("End: ");
    Serial.println(millis());
    // calculate time taken to update display
    Serial.print("Time taken: ");
    Serial.println(millis() - previousMillis);
  }
}