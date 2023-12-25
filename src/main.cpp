#include <esp_now.h>
#include <WiFi.h>
// include spi library
#include <SPI.h>

#include "heltec-eink-modules.h"
#define PIN_BUSY 5
#define PIN_CS 4
#define PIN_DC 14
DEPG0213RWS800 display(PIN_DC, PIN_CS, PIN_BUSY); // 2.13" V2 - BWR - Red Tab

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
  DRAW(display)
  {
    // set size of text to be bigger
    display.setTextSize(2);
    display.setCursor(10, 10);
    display.print("T1:");
    display.print(myData.b);
    display.setCursor(10, 40);
    display.print("H1:");
    display.print(myData.c);
    display.setCursor(10, 50);
    display.drawLine(0, 70, 250, 70, BLACK);
    display.setTextSize(10);
    display.setCursor(5, 80);
    int temp = myData.b;
    display.print(temp);
    display.setCursor(5, 160);
    int hum = myData.c;
    display.print(hum);
  }
}

void setup()
{
  // Initialize Serial Monitor
  Serial.begin(115200);

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
}

void loop()
{
}
