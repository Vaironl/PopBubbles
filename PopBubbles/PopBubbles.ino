/***************************************************
  This is our touchscreen painting example for the Adafruit ILI9341
  captouch shield
  ----> http://www.adafruit.com/products/1947

  Check out the links above for our tutorials and wiring diagrams

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/


#include <Adafruit_GFX.h>    // Core graphics library
#include <SPI.h>       // this is needed for display
#include <Adafruit_ILI9341.h>
#include <Wire.h>      // this is needed for FT6206
#include <Adafruit_FT6206.h>

/*
Screen size:
240x320
*/
int MAX_WIDTH = 240;
int MAX_HEIGHT = 320;

// The FT6206 uses hardware I2C (SCL/SDA)
Adafruit_FT6206 ctp = Adafruit_FT6206();

// The display also uses hardware SPI, plus #9 & #10
#define TFT_CS 10
#define TFT_DC 9
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// Size of the color selection boxes and the paintbrush size
int oldcolor, currentcolor;

class Bubble
{
  private:
    int x;
    int y;
    int radius;
    int color;
  
  public:
    void drawBubble();
    void deltaX(int delta); //Changes x by delta
    void deltaY(int delta); //Changes y by delta

    Bubble(int initX, int initY, int initRadius, int initColor);
};

Bubble::Bubble(int initX, int initY, int initRadius, int initColor)
{
  x = initX;
  y = initY;
  radius = initRadius;
  color = initColor;
}

void Bubble::drawBubble()
{
   tft.fillCircle(x, y, radius, color);
   //Clear the inside of the circle to make it look like a bubble
   tft.fillCircle(x,y,radius,ILI9341_BLACK);
   
   if(x > MAX_WIDTH-radius)
   {
     x = 0;
   }
   else if(x < 0)
   {
     x = MAX_WIDTH- radius;
   }
}

void Bubble::deltaX(int delta)
{
  x+=delta;
}

void Bubble::deltaY(int delta)
{
  y+=delta;
}


//Bubbles
Bubble *bubble;
Bubble *bubble2;

void setup(void) {
  while (!Serial);     // used for leonardo debugging
 
 //115200 was the old .begin value
  Serial.begin(9600);
  Serial.println(F("Pop Bubbles"));
  
  tft.begin();

  if (! ctp.begin(40)) {  // pass in 'sensitivity' coefficient
    Serial.println("Couldn't start FT6206 touchscreen controller");
    while (1);
  }

  Serial.println("Capacitive touchscreen started");
  
  tft.fillScreen(ILI9341_BLACK);
  
  //tft.invertDisplay(true);
  
  bubble = new Bubble(10,10,20,ILI9341_YELLOW);
  bubble2 = new Bubble(10,100,20,ILI9341_YELLOW);
 
  // select the current color 'red'

  currentcolor = ILI9341_RED;
}

void loop() {
  
  bubble->drawBubble();
  bubble->deltaX(1);  
  bubble2->drawBubble();
  bubble2->deltaX(1);
}
