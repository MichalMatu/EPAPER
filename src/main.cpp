#include <esp_now.h>
#include <WiFi.h>
// include spi library
#include <SPI.h>

#include "heltec-eink-modules.h"
#define PIN_BUSY 5
#define PIN_CS 4
#define PIN_DC 2
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
    display.fillCircle(75, 50, 50, BLACK);
    display.fillCircle(75, 200, 50, RED);
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
