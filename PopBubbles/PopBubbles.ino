#include <Adafruit_FT6206.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <SPI.h>       // this is needed for display
#include <Adafruit_ILI9341.h>
#include <Wire.h>      // this is needed for FT6206

int MAX_WIDTH = 240;//The width of the screen
int MAX_HEIGHT = 320;//The height of the screen

// The FT6206 uses hardware I2C (SCL/SDA)
Adafruit_FT6206 ctp = Adafruit_FT6206();

// The display also uses hardware SPI, plus #9 & #10
#define TFT_CS 10
#define TFT_DC 9
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

//The following will allow the background color to change depending on the difficulty
int BACKGROUND = ILI9341_BLACK;
//User score
int score = 0;


//Speed the bubbles will move at
int bubbleSpeed =  2;


/* Bubble class, used to draw cool bubbles*/
class Bubble
{
  private:
    int x;//x Position
    int y;//y position
    //Change in X of the bubble
    int dx = bubbleSpeed;
    //Change in Y of the bubble
    int dy = bubbleSpeed;
    int TYPE = 0; // 0 is a good bubble 1 is a bad bubble
    int radius;
    int color;//The color of the bubble.
    static const int MIN_RADIUS = 10;
    static const int MAX_RADIUS = 20;
    boolean visible = true;//Used to hide the bubble once it is tapped

  public:
    void drawBubble();// Takes care of drawing the bubble
    void clearBubble();//Draw over the bubble to clear it.
    void randomizePosition();//Sets the bubble to be at a random Position
    void setVisible(boolean visibility);//Should the bubble be drawn?
    boolean checkCollision(int touchX, int touchY);
    Bubble();//Constructor
    Bubble(int bubbleType, int color);//Constructor
};

//Default Constructor
Bubble::Bubble()
{
  x = 0;
  y = 0;
  radius = 0;

  //Gives a random color to the bubbles
  switch (random(4))
  {
    case 0:
      color = ILI9341_YELLOW;
      break;
    case 1:
      color = ILI9341_PINK;
      break;
    case 2:
      color = ILI9341_GREENYELLOW;
      break;
    case 3:
      color = ILI9341_WHITE;
      break;
    default:
      color = ILI9341_YELLOW;
  }

}

//Overloaded Constructor
Bubble::Bubble(int bubbleType, int initColor)
{
  Bubble();
  color = initColor;
  TYPE = bubbleType;
}

void Bubble::drawBubble()
{
  if (visible)
  {

    tft.drawCircle(x - dx, y - dy, radius, BACKGROUND);//clear the bubble
    //If the bubble is going outside of the screen range warp it back
    if (x + radius >= MAX_WIDTH + 1)
    {
      x = radius;//Reset to the left.

    }
    else if (x - radius <= -1) //left side of the screen. Move to the right side
    {
      x = MAX_WIDTH - radius; //Reset to the left.
    }
    if (y + radius >= MAX_HEIGHT + 1)
    {
      y = radius;//Reset to the left.

    }
    else if (y - radius <= -1) //left side of the screen. Move to the right side
    {
      y = MAX_HEIGHT - radius; //Reset to the left.
    }


    tft.drawCircle(x, y, radius, color);

    x += dx;
    y += dy;
  }

}

void Bubble::setVisible(boolean newVisibility)
{
  visible = newVisibility;
}

boolean Bubble::checkCollision(int touchX, int touchY)
{
  //Check if the tap was anywhere within a bubble
  //Only check the following if the bubble is vissible otherwise don't do it.
  if (visible && touchX >= (x - radius) && touchX <= (x + radius) && touchY >= (y - radius) && touchY <= (y + radius))
  {
    //Serial.println("Bubble Popped");
    visible = false;
    if (TYPE == 0)
    {
      score++;
    }
    else if (TYPE == 1)
    {
      score--;
    }

    tft.drawCircle(x - dx, y - dy, radius, BACKGROUND);

  }
}

void Bubble::clearBubble()
{
  //Clear the bubble by drawing over it using the background color
  tft.drawCircle(x, y, radius, BACKGROUND);
}

void Bubble::randomizePosition()
{
  radius = random((MAX_RADIUS - MIN_RADIUS) + 1) + MIN_RADIUS;
  x = random((MAX_WIDTH - radius) + 1);
  y = random((MAX_HEIGHT - radius) + 1);
  dx = dy = bubbleSpeed;


  //Random speed in the x direction
  if (random(2) == 1) // if random is 49+ we have positive speed if it's 1 we have negative speed
  {
    dx = -dx;
  }
  //Random speed in the y direction
  if (random(2) == 1) // if random is 0 we have positive speed if it's 1 we have negative speed
  {
    dy = -dy;

  }
}

//The number of bubbles being drawn to the screen.
const int NUM_OF_BUBBLES = 15;

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

  for (int i = 0; i < NUM_OF_BUBBLES; i++)
  {
    //Initialize constructors
    bubble[i] = new Bubble();
    //Because the # of BAD_BUBBLES is the same as good ones we also initialize them here.
    badBubbles[i] = new Bubble(1, ILI9341_BLACK);
    /* FOR TESTING*/
    bubble[i]->randomizePosition();
    badBubbles[i]->randomizePosition();
  }
}

/*
   Declare and initialize variables to be used to calculate game difficulty and advance game time
*/

int ticks = 100;//Number of ticks the game will go through
int counter = ticks;//Counter variable for Debug purposes
int VISIBLE_BUBBLES = NUM_OF_BUBBLES; // # of BUBBLES Visible at anytime
int VISIBLE_BAD_BUBBLES = NUM_OF_BUBBLES / 4; // # of BUBBLES Visible at anytime


//Make al of the bubbles visible again
void makeAllVisible()
{
  for (int i = 0; i < NUM_OF_BUBBLES; i++)
  {
    bubble[i]->setVisible(true);
    badBubbles[i]->setVisible(true);
  }
}

//Randomize the position of all bubbles
void randomizeAllBubbles()
{
  for (int i = 0; i < NUM_OF_BUBBLES; i++)
  {
    bubble[i]->randomizePosition();
    badBubbles[i]->randomizePosition();
  }

}


//Displays the counter & Score
void displayInfo()
{
  tft.fillRect(105 - 80, 0, 55, 21, BACKGROUND);
  tft.setCursor(105 - 80, 0);
  tft.setTextSize(3);
  tft.println(counter);

  tft.fillRect(105 - 20, 0, 33, 21, BACKGROUND);
  tft.setCursor(105 - 20, 0);
  tft.setTextSize(3);
  tft.print("score: ");

  tft.fillRect(105 + 90, 0, 33, 21, BACKGROUND);
  tft.setCursor(105 + 90, 0);
  tft.setTextSize(3);
  tft.print( + score);
}


//Inidicates the state of the game 0  = menu, 1  = playing, 2  = quit
int gamestate  = 0;

void displayGameMenu()
{

  tft.setTextColor(ILI9341_GREEN);
  tft.setCursor(47.5, 87.5);
  tft.setTextSize(5);
  tft.println("Start");
  tft.setCursor(62.5, 182.5);
  tft.println("Quit");
  tft.setTextColor(ILI9341_WHITE);

  if (!ctp.touched())
  {
    return;
  }

  TS_Point p = ctp.getPoint();

  p.x = map(p.x, 0, 240, 240, 0);
  p.y = map(p.y, 0, 320, 320, 0);

  if (p.x >= 47.5 && p.x <= 192.5)
  {
    if (p.y >= 87.5 && p.y <= 122.5)
    {
      tft.drawRect(47.5, 87.5, 145, 35, ILI9341_WHITE);
      gamestate = 1;
      BACKGROUND = ILI9341_NAVY;
      tft.fillScreen(BACKGROUND);
    }
  }

  if (p.x >= 62.5 && p.x <= 177.5)
  {
    if (p.y >= 182.5 && p.y <= 217.5)
    {
      tft.drawRect(62.5, 182.5, 115, 35, ILI9341_WHITE);
      gamestate = 2;
      tft.fillScreen(BACKGROUND);
    }
  }
}



void game()
{

  displayInfo();

  for (int i = 0; i < VISIBLE_BUBBLES; i++)
  {
    //If the touch screen is touched. Check for collision with bubbles
    if (ctp.touched())
    {
      TS_Point p = ctp.getPoint();// Get the point from the screep

      // flip it around to match the screen.
      p.x = map(p.x, 0, 240, 240, 0);
      p.y = map(p.y, 0, 320, 320, 0);
      bubble[i]->checkCollision(p.x, p.y);
    }
  }

  for (int i = 0; i < VISIBLE_BAD_BUBBLES; i++)
  {
    //If the touch screen is touched. Check for collision with bad bubbles
    if (ctp.touched())
    {
      TS_Point p = ctp.getPoint();// Get the point from the screep

      // flip it around to match the screen.
      p.x = map(p.x, 0, 240, 240, 0);
      p.y = map(p.y, 0, 320, 320, 0);
      badBubbles[i]->checkCollision(p.x, p.y);
    }
  }


  for (int i = 0; i < VISIBLE_BUBBLES; i++)
  {
    bubble[i]->drawBubble();
  }

  for (int i = 0; i < VISIBLE_BAD_BUBBLES; i++)
  {
    badBubbles[i]->drawBubble();
  }

  //Fill/Clear The screen only after the difficulty changes
  if (counter == ticks / 2)
  {
    BACKGROUND = ILI9341_ORANGE;
    tft.fillScreen(BACKGROUND);
    VISIBLE_BUBBLES = NUM_OF_BUBBLES / 2;
    VISIBLE_BAD_BUBBLES = NUM_OF_BUBBLES / 2;
    makeAllVisible();
    bubbleSpeed = 5;
    randomizeAllBubbles();
  }
  else if (counter == ticks / 5)
  {
    BACKGROUND = ILI9341_RED;
    tft.fillScreen(BACKGROUND);
    VISIBLE_BUBBLES = NUM_OF_BUBBLES / 4;
    VISIBLE_BAD_BUBBLES = NUM_OF_BUBBLES;
    makeAllVisible();
    bubbleSpeed = 10;
    randomizeAllBubbles();
  }
  else if (counter == 0)
  {
    counter = ticks;
    VISIBLE_BUBBLES = NUM_OF_BUBBLES;
    VISIBLE_BAD_BUBBLES = NUM_OF_BUBBLES / 4;
    makeAllVisible();
    bubbleSpeed = 2;
    randomizeAllBubbles();
    gamestate = 0;

    //Stop the game. Display user score & then go back to the main screen
    BACKGROUND = ILI9341_BLACK;
    tft.fillScreen(BACKGROUND);

    tft.setCursor(47.5, 87.5);
    tft.setTextSize(3);
    tft.println("Your score was...");
    tft.setCursor((MAX_WIDTH / 2) - 10, 182.5);
    tft.setTextSize(5);
    tft.println(score);

    delay(4000);
    tft.fillScreen(BACKGROUND);
    score = 0;

  }


  counter--;

}



void loop() {

  if (gamestate == 0)
  {
    displayGameMenu();
    return;
  }
  else if (gamestate == 1)
  {
    game();
    return;
  }
  else if (gamestate == 2)
  {

    tft.fillScreen(BACKGROUND);
    tft.setTextColor(ILI9341_RED);
    tft.setTextSize(4);
    tft.setCursor(0, 140);
    tft.println("Game Ended");

    exit(1);

  }


}
