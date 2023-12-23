#include "heltec-eink-modules.h"
#include <Adafruit_I2CDevice.h>
#define PIN_BUSY 5
#define PIN_CS 4
#define PIN_DC 2
DEPG0213RWS800 display(PIN_DC, PIN_CS, PIN_BUSY); // 2.13" V2 - BWR - Red Tab
void setup()
{
  DRAW(display)
  {
    display.fillCircle(75, 50, 50, BLACK);
    display.fillCircle(75, 200, 50, RED);
  }
}
void loop() {}