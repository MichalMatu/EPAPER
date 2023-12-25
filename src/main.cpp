// #include "heltec-eink-modules.h"
// #include "WiFi.h"

// #define PIN_BUSY 5
// #define PIN_CS 4
// #define PIN_DC 2

// DEPG0213RWS800 display(PIN_DC, PIN_CS, PIN_BUSY); // 2.13" V2 - BWR - Red Tab

// void setup()
// {
//   // init serial monitor
//   Serial.begin(115200);

//   // clear the display
//   display.clear();

//   // set the rotation of the display
//   display.setRotation(1);

//   // Connect to WiFi
//   WiFi.mode(WIFI_MODE_STA);
//   WiFi.begin("your_ssid", "your_password");

//   // Wait for the connection
//   while (WiFi.status() != WL_CONNECTED)
//   {
//     delay(1000);
//     Serial.println("Connecting to WiFi...");
//   }

//   // Connection successful, print MAC address
//   Serial.println("Connected to WiFi");
//   Serial.println("MAC Address: " + WiFi.macAddress());
// }

// // create variable to store refresh time
// unsigned long refreshTime = 0;
// int count = 0;

// void loop()
// {
//   // check if it's time to refresh the display
//   if (millis() - refreshTime >= 600)
//   {
//     DRAW(display)
//     {
//       // set text size to 2
//       display.setTextSize(2);
//       display.setCursor(10, 10);
//       display.print("First line.");

//       display.setCursor(10, 40);
//       display.print("Second line.");

//       // display count
//       display.setCursor(10, 70);
//       display.print(count);
//       count++;
//     }

//     // update the refresh time
//     refreshTime = millis();
//   }
// }

// E0:5A:1B:A1:9B:00
#include <esp_now.h>
#include <WiFi.h>
// include spi library
#include <SPI.h>

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
