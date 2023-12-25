#include "heltec-eink-modules.h"
#define PIN_BUSY 5
#define PIN_CS 4
#define PIN_DC 2
DEPG0213RWS800 display(PIN_DC, PIN_CS, PIN_BUSY); // 2.13" V2 - BWR - Red Tab
void setup()
{

  // clear the display
  display.clear();

  // set the rotation of the display
  display.setRotation(1);
}

// create variable to store refresh time
unsigned long refreshTime = 0;
int count = 0;

void loop()
{
  // check if it's time to refresh the display
  if (millis() - refreshTime >= 60000)
  {
    DRAW(display)
    {
      // set text size to 2
      display.setTextSize(2);
      display.setCursor(10, 10);
      display.print("First line.");

      display.setCursor(10, 40);
      display.print("Second line.");

      // display count
      display.setCursor(10, 70);
      display.print(count);
      count++;
    }

    // update the refresh time
    refreshTime = millis();
  }
}