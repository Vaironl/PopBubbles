#include <Adafruit_GFX.h>    // Core graphics library
#include <SPI.h>       // this is needed for display
#include <Adafruit_ILI9341.h>
#include <Wire.h>      // this is needed for FT6206
#include <Adafruit_FT6206.h>

Adafruit_FT6206 ctp = Adafruit_FT6206();
Adafruit_ILI9341 tft = Adafruit_ILI9341(10, 9);

#define backColor ILI9341_BLACK

int gamestate = 0;

void setup(void)
{
  while (!Serial);     // used for leonardo debugging
 
  Serial.begin(115200);
  Serial.println(F("Cap Touch Paint!"));
  
  tft.begin();
  
  if (! ctp.begin(40))
  {  // pass in 'sensitivity' coefficient
    Serial.println("Couldn't start FT6206 touchscreen controller");
    while (1);
  }
  
  Serial.println("Capacitive touchscreen started");
  
  tft.fillScreen(backColor);
  tft.fillRect(47.5, 87.5, 145, 35, backColor);
  tft.fillRect(62.5, 182.5, 115, 35, backColor);
  tft.setTextColor(ILI9341_GREEN);
}

void loop()
{
  //gameMenu();
  
  tft.setCursor(47.5, 87.5);
  tft.setTextSize(5);
  tft.println("Start");
  tft.setCursor(62.5, 182.5);
  tft.println("Quit");
  
  if (! ctp.touched())
  {
    return;
  }
  
  TS_Point p = ctp.getPoint();
  
  p.x = map(p.x, 0, 240, 240, 0);
  p.y = map(p.y, 0, 320, 320, 0);
  
  if(p.x >= 47.5 && p.x <= 192.5)
  {
    if(p.y >= 87.5 && p.y <= 122.5)
    {
      tft.drawRect(47.5, 87.5, 145, 35, ILI9341_WHITE);
      gamestate = 1;
    }
  }
  
  if(p.x >= 62.5 && p.x <= 177.5)
  {
    if(p.y >= 182.5 && p.y <= 217.5)
    {
      tft.drawRect(62.5, 182.5, 115, 35, ILI9341_WHITE);
      gamestate = 0;
    }
  }
  
  if(gamestate == 1)
  {
    tft.fillScreen(backColor);
    bubbleTimer();
  }
  else
  {
    tft.fillScreen(backColor);
    tft.setTextColor(ILI9341_RED);
    tft.setTextSize(4);
    tft.setCursor(0, 140);
    tft.println("Game Ended");
    exit(0);
  }
}

void bubbleTimer()
{
  int i;
  for(i = 60; i >= 0; i--)
  {
    tft.fillRect(105, 0, 33, 21, backColor);
    tft.setCursor(105, 0);
    tft.setTextSize(3);
    tft.println(i);
    delay(1000);
  }
}

void gameMenu()
{
   if (!ctp.touched())
  {
    return;
  }
  
  tft.setCursor(47.5, 87.5);
  tft.setTextSize(5);
  tft.println("Start");
  tft.setCursor(62.5, 182.5);
  tft.println("Quit");
  
  TS_Point p = ctp.getPoint();
  
  p.x = map(p.x, 0, 240, 240, 0);
  p.y = map(p.y, 0, 320, 320, 0);
  
  if(p.x >= 47.5 && p.x <= 192.5)
  {
    if(p.y >= 87.5 && p.y <= 122.5)
    {
      tft.drawRect(47.5, 87.5, 145, 35, ILI9341_WHITE);
      gamestate = 1;
    }
  }
  
  if(p.x >= 62.5 && p.x <= 177.5)
  {
    if(p.y >= 182.5 && p.y <= 217.5)
    {
      tft.drawRect(62.5, 182.5, 115, 35, ILI9341_WHITE);
      gamestate = 0;
    }
  }
}
