
#include <Ucglib.h>
#include <Adafruit_FT6206.h>

Ucglib_ILI9341_18x240x320_HWSPI ucg(/*cd=*/ 9, /*cs=*/ 10, /*reset=*/ 8);//Hardware SPI
Adafruit_FT6206 ctp = Adafruit_FT6206();// The FT6206 uses hardware I2C (SCL/SDA)
#include <SPI.h>       // this is needed for display
#include <Wire.h>      // this is needed for FT6206


#define BLACK 0
#define WHITE 1
#define RED 2
#define YELLOW 3
#define NAVY 4
#define ORANGE 5

//The following will allow the background color to change depending on the difficulty
int BACKGROUND = BLACK;
//User score
int score = 0;
//Screen Dimensions
int MAX_WIDTH = 240, MAX_HEIGHT = 320;






//Sets the color

void setUCGColor(int color)
{
  //Use Switch statement
  if (color == WHITE)
  {
    ucg.setColor(255, 255, 255);
  }
  else if (color == RED)
  {
    ucg.setColor(255, 0, 0);

  }
  else if (color == YELLOW)
  {
    ucg.setColor(255, 255, 0);

  }
  else if (color == NAVY)
  {
    ucg.setColor(0, 0, 255);
  }
  else if (color == ORANGE)
  {
    ucg.setColor(255, 165, 0);
  }
}





/* Bubble class, used to draw cool bubbles*/
class Bubble
{
  private:
    int x;//x Position
    int y;//y position
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
  //Default color for bubbles
  color = YELLOW;
}

//Overloaded Constructor?
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
    setUCGColor(color);

    ucg.drawCircle(x, y, radius, UCG_DRAW_ALL);
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

  }
}

void Bubble::clearBubble()
{
  //Clear the bubble by drawing over it using the background color

  setUCGColor(BACKGROUND);
  ucg.drawBox(x - radius, y - radius, (radius * 2) + 3, (radius * 2) + 3);
}

void Bubble::randomizePosition()
{
  radius = random((MAX_RADIUS - MIN_RADIUS) + 1) + MIN_RADIUS;
  x = random((MAX_WIDTH - radius) + 1);
  y = random((MAX_HEIGHT - radius) + 1);
}


//The number of bubbles being drawn to the screen.
const int NUM_OF_BUBBLES = 10;

//Declare the bubbles array
Bubble *bubble[NUM_OF_BUBBLES];
Bubble *badBubbles[NUM_OF_BUBBLES];


void setup(void) {

  //delay(1000);
  Serial.begin(9600);
  ucg.begin(UCG_FONT_MODE_TRANSPARENT);
  //ucg.clearScreen();

  if (!ctp.begin(100)) {  // pass in 'sensitivity' coefficient
    Serial.println("Couldn't start FT6206 touchscreen controller");
    while (1);
  }


  setUCGColor(BACKGROUND);
  ucg.drawBox(0, 0, MAX_WIDTH, MAX_HEIGHT);

  for (int i = 0; i < NUM_OF_BUBBLES; i++)
  {
    //Initialize constructors
    bubble[i] = new Bubble();
    //Because the # of BAD_BUBBLES is the same as good ones we also initialize them here.
    badBubbles[i] = new Bubble(1, BLACK);
    /* FOR TESTING*/
    bubble[i]->randomizePosition();
    badBubbles[i]->randomizePosition();
  }


  ucg.setFont(ucg_font_ncenR24_tr);
  setUCGColor(ORANGE);


}

int counter = 0;//Counter variable for Debug purposes
int difficulty = 0;//Difficulty of the game
int VISIBLE_BUBBLES = NUM_OF_BUBBLES; // # of BUBBLES Visible at anytime
int VISIBLE_BAD_BUBBLES = NUM_OF_BUBBLES / 4; // # of BUBBLES Visible at anytime
int DIFFICULTY_DELAY = 1000;
int gamestate = 0; //State of the game



//START GAME COORDINATES
int gameStartX = (MAX_WIDTH / 2) - 35;
int startY = 87.5;
int quitY = 182.5;

int x = 0;

void loop(void) {

  setUCGColor(WHITE);
  ucg.setPrintPos(gameStartX, startY);
  ucg.println("Start");
  ucg.setPrintPos(gameStartX, quitY);
  ucg.println("Quit");
  
  setUCGColor(RED);
  ucg.drawBox(x, startY, 1,1);

  Serial.println(x);

  x++;

  if (! ctp.touched())
  {
    return;
  }


  TS_Point p = ctp.getPoint();

  p.x = map(p.x, 0, 240, 240, 0);
  p.y = map(p.y, 0, 320, 320, 0);

  if (p.x >= 47.5 && p.x <= gameStartX + 120)
  {
    if (p.y >= 87.5 && p.y <= 122.5)
    {
      setUCGColor(WHITE);
      ucg.drawBox(startX, 87.5, 145, 35);
      gamestate = 1;
    }
  }

  if (p.x >= gameStartX && p.x <= gameStartX + 120)
  {
    if (p.y >= quitY && p.y <= 217.5)
    {
      setUCGColor(WHITE);
      ucg.drawBox(gameStartX, quitY, 115, 35);
      gamestate = 0;
    }
  }













  //Start the game
  if (gamestate == 1)
  {

    for (int i = 0; i < VISIBLE_BUBBLES; i++)
    {
      //bubble[i]->randomizePosition();
      bubble[i]->drawBubble();
    }

    //Check the touch multiple times
    for (int PASS = 0; PASS < 30; PASS++)
    {

      //For each PASS check all of the bubbles
      for (int i = 0; i < VISIBLE_BUBBLES; i++)
      {


        //If the touch screen is touched. Check for collision with bubbles
        if (ctp.touched())
        {

          // flip it around to match the screen.
          TS_Point p = ctp.getPoint();// Get the point from the screep
          p.x = map(p.x, 0, 240, 240, 0);
          p.y = map(p.y, 0, 320, 320, 0);
          bubble[i]->checkCollision(p.x, p.y);
        }
      }
    }
    for (int i = 0; i < VISIBLE_BUBBLES; i++)
    {
      bubble[i]->clearBubble();
    }

    //End gamestate
  }
}

void bubbleTimer()
{
  for (int i = 60; i >= 0; i--)
  {
    ucg.drawBox(105, 0, 33, 21);
    ucg.setPrintPos(105, 0);
    //tft.setTextSize(3);
    ucg.println(i);
    //Wait 1 second
    delay(1000);
  }
}

void gameMenu()
{
  if (!ctp.touched())
  {
    return;
  }

  ucg.setPrintPos(47.5, 87.5);
  //ucg.setTextSize(5);
  ucg.println("Start");
  ucg.setPrintPos(62.5, 182.5);
  ucg.println("Quit");

  TS_Point p = ctp.getPoint();

  p.x = map(p.x, 0, 240, 240, 0);
  p.y = map(p.y, 0, 320, 320, 0);

  if (p.x >= 47.5 && p.x <= 192.5)
  {
    if (p.y >= 87.5 && p.y <= 122.5)
    {
      setUCGColor(WHITE);
      ucg.drawBox(47.5, 87.5, 145, 35);
      gamestate = 1;
    }
  }

  if (p.x >= 62.5 && p.x <= 177.5)
  {
    if (p.y >= 182.5 && p.y <= 217.5)
    {
      //Draw Whote
      setUCGColor(WHITE);
      ucg.drawBox(62.5, 182.5, 115, 35);
      gamestate = 0;
    }
  }
}
