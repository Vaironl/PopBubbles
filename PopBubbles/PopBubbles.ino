#include <Adafruit_GFX.h>    // Core graphics library
#include <SPI.h>       // this is needed for display
#include <Adafruit_ILI9341.h>
#include <Wire.h>      // this is needed for FT6206
#include <Adafruit_FT6206.h>

int MAX_WIDTH = 240;//The width of the screen
int MAX_HEIGHT = 320;//The height of the screen

// The FT6206 uses hardware I2C (SCL/SDA)
Adafruit_FT6206 ctp = Adafruit_FT6206();

// The display also uses hardware SPI, plus #9 & #10
#define TFT_CS 10
#define TFT_DC 9
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

//The following will allow the background color to change depending on the difficulty
int BACKGROUND = ILI9341_NAVY;


/* Bubble class, used to draw cool bubbles*/
class Bubble
{
private:
  int x;//x Position
  int y;//y position
  int radius;
  int color;//The color of the bubble. 
  static const int MIN_RADIUS = 5;
  static const int MAX_RADIUS = 20;
  boolean visible = true;

public:
  void drawBubble();// Takes care of drawing the bubble
  void clearBubble();//Draw over the bubble to clear it.
  void randomizePosition();//Sets the bubble to be at a random Position
  void setVisible(boolean visibility);//Should the bubble be drawn?
  Bubble();//Constructor
  Bubble(int color);//Constructor
};

Bubble::Bubble()
{
  x = 0;
  y = 0;
  radius = 0;
  color = ILI9341_YELLOW;
}

Bubble::Bubble(int initColor)
{
  Bubble();
  color = initColor;
}

void Bubble::drawBubble()
{
  tft.drawCircle(x, y, radius, color);
}

Bubble::setVisible(boolean newVisibility)
{
  visible = newVisibility;
}

void Bubble::clearBubble()
{
  //Clear the bubble by drawing over it using the background color
  tft.drawCircle(x,y,radius,BACKGROUND);
}

void Bubble::randomizePosition()
{
  radius = random((MAX_RADIUS-MIN_RADIUS)+1)+MIN_RADIUS;
  x = random((MAX_WIDTH-radius)+1);
  y = random((MAX_HEIGHT-radius)+1);
}

//The number of bubbles being drawn to the screen.
const int NUM_OF_BUBBLES = 20;
//Bad bubbles will be the same number of good bubbles
//const int NUM_OF_BAD_BUBBLES= NUM_OF_BUBBLES;

//Declare the bubbles array
Bubble *bubble[NUM_OF_BUBBLES];
Bubble *badBubbles[NUM_OF_BUBBLES];

void setup(void) {
  while (!Serial);     // used for leonardo debugging

  //115200 was the old .begin value
  Serial.begin(9600);
  Serial.println(("Pop Bubbles"));

  tft.begin();

  if (!ctp.begin(40)) {  // pass in 'sensitivity' coefficient
    Serial.println("Couldn't start FT6206 touchscreen controller");
    while (1);
  }

  Serial.println("Capacitive touchscreen started");

  tft.fillScreen(BACKGROUND);

  randomSeed(analogRead(0));

  //Bubble position should be random from

  int radius = 0;

  for(int i = 0; i < NUM_OF_BUBBLES; i++)
  {
    //Initialize constructors
    bubble[i] = new Bubble();
    //Because the # of BAD_BUBBLES is the same as good ones we also initialize them here.
    badBubbles[i] = new Bubble(ILI9341_BLACK);
  }
}

int counter = 0;//Counter variable for Debug purposes
int difficulty = 0;//Difficulty of the game
int VISIBLE_BUBBLES = NUM_OF_BUBBLES; // # of BUBBLES Visible at anytime
int VISIBLE_BAD_BUBBLES = NUM_OF_BUBBLES/4; // # of BUBBLES Visible at anytime
int DIFFICULTY_DELAY = 1000;

void loop() {  

  for(int i = 0; i < VISIBLE_BUBBLES; i++)
  {
    bubble[i]->randomizePosition();
    bubble[i]->drawBubble();
  }

  for(int i = 0; i < VISIBLE_BAD_BUBBLES; i++)
  {
    badBubbles[i]->randomizePosition();
    badBubbles[i]->drawBubble();
  }
  
  //Touch points
  TS_Point p = ctp.getPoint();
  // flip it around to match the screen.
  p.x = map(p.x, 0, 240, 240, 0);
  p.y = map(p.y, 0, 320, 320, 0);
  
  

  //Might have to make a separate function to clear circles
  delay(DIFFICULTY_DELAY);

  //Fill The screen after the difficulty changes
  if(counter == 5)
  {
    BACKGROUND = ILI9341_ORANGE;//ORANGE
    tft.fillScreen(BACKGROUND);
    difficulty = 1;
    VISIBLE_BUBBLES = NUM_OF_BUBBLES/2;
    VISIBLE_BAD_BUBBLES = NUM_OF_BUBBLES/2;
    DIFFICULTY_DELAY = 600;
  }
  else if(counter == 15)
  {
    BACKGROUND = ILI9341_RED;
    tft.fillScreen(BACKGROUND);
    VISIBLE_BUBBLES = NUM_OF_BUBBLES/4;
    VISIBLE_BAD_BUBBLES = NUM_OF_BUBBLES;
    difficulty = 2;
    DIFFICULTY_DELAY = 400;
  }
  else if(counter == 35)
  {
    BACKGROUND = ILI9341_NAVY;
    tft.fillScreen(BACKGROUND);
    difficulty = 0;
    counter = 0;
    VISIBLE_BUBBLES = NUM_OF_BUBBLES;
    VISIBLE_BAD_BUBBLES = NUM_OF_BUBBLES/4;
    DIFFICULTY_DELAY = 1000;
  }


  for(int i = 0; i < VISIBLE_BUBBLES; i++)
  {
    bubble[i]->clearBubble();
  }
  for(int i = 0; i < VISIBLE_BAD_BUBBLES; i++)
  {
    badBubbles[i]->clearBubble();
  }


  counter++;

}








