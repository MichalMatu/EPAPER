#include <esp_now.h>
#include <WiFi.h>
#include <SPI.h>
#include "heltec-eink-modules.h"

#define PIN_BUSY 5
#define PIN_CS 4
#define PIN_DC 14
DEPG0213RWS800 display(PIN_DC, PIN_CS, PIN_BUSY);

// Sender structure
typedef struct struct_message_send
{
  bool value;
} struct_message_send;

struct_message_send myDataSend;

// Receiver structure
typedef struct struct_message_recv
{
  char a[32];
  float b;
  float c;
} struct_message_recv;

struct_message_recv myDataRecv;

// Replace with the MAC address of your ESP-NOW receiver
uint8_t receiverMACAddress[] = {0x5C, 0xCF, 0x7F, 0x97, 0x72, 0x2B};

bool sendData = false;

void sendDataToRelay()
{
  sendData = !sendData;
  myDataSend.value = sendData;

  // Send data to the receiver
  esp_now_send(receiverMACAddress, (uint8_t *)&myDataSend, sizeof(myDataSend));
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&myDataRecv, incomingData, sizeof(myDataRecv));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Char: ");
  Serial.println(myDataRecv.a);
  Serial.print("Float: ");
  Serial.println(myDataRecv.b);
  Serial.print("Float: ");
  Serial.println(myDataRecv.c);

  // Your existing code for displaying data on the E-ink display
  DRAW(display)
  {
    // set size of text to be bigger
    display.setTextSize(2);
    display.setCursor(10, 10);
    display.print("T1:");
    display.print(myDataRecv.b);
    display.setCursor(10, 40);
    display.print("H1:");
    display.print(myDataRecv.c);
    display.setCursor(10, 50);
    display.drawLine(0, 70, 250, 70, BLACK);
    display.setTextSize(10);
    display.setCursor(5, 80);
    int temp = static_cast<int>(myDataRecv.b);
    int hum = static_cast<int>(myDataRecv.c);
    display.print(temp);
    display.setCursor(5, 160);
    display.print(hum);
  }
  sendDataToRelay();
}

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  Serial.print("ESP32 MAC Address: ");
  Serial.println(WiFi.macAddress());

  esp_now_register_recv_cb(OnDataRecv);

  // Your existing setup code for the E-ink display
}

void loop()
{

  // Your existing loop code
}
