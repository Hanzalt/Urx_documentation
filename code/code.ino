//(Snad posledni verze) Po druhym vyhorenim jsme se rozhodl udelat algoritmus smesi for cyklu(verze jedna) a smerem zapinani a zapinani(verze dva) JDU NA TO
//OLED displey https://s.click.aliexpress.com/e/_APw28Y
#include <Urx.h>
#include <EEPROM.h>
#include <U8g2lib.h>
U8G2_SH1106_128X32_VISIONOX_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); 
LedControl lc = LedControl(11, 12, 10, 4);
Operator op = Operator(4);
Images img = Images();
/*Structure 
SETUP FAZE -------------------
void Setup() - Runs on starting sketch
INPUT FAZE -------------------
void Button() - Function is for getting inputs from buttons
GAME FAZE --------------------
void Menu() - Function that takes care of menu
void GameOver() - Function for managing gamestate gameover
void SnakeMenu() - Function that takes care for snake menu that has settings   
void Snake() - Main function for game Snake
HELPING FAZE -----------------
void SettingGameSnake() - Setting starting conditions like length and speed of snake
void Warp() - Take care for warping through walls
void TurningLedArray() - Function for turning leds in game on/off
void ShowText() - Function for turning text on/off
void ShowOneDisplay() - Function for managing one display
void ShowBorders() - Function for showing borders on one display
void GenerateFood() - Function for setting random food position 
int IsButtonPressed() - Function for stoping the game untile some button is pressed and then return number corresponding to the direction
void PlaySound() - Function for playing chosen sound
LOOP FAZE --------------------
void Loop() - Runs every 1/100s or every 10 ms
*/

/****************************************************************/
//Snake
/****************************************************************/

//Food position variables
String versionUrxOS = "0.4.0";
int foodX = 0;
int foodY = 0;
//Delay in game is overrided when function SettingGameSnake is called
float delayTime = 300;
//GameState for setting menu - 0, gameover - 1, game - Snake - 2, game - Space Invators = 3, info - 4, nothing - 5
int gameState = 0;
//Menu variable
int whichIconShowed = 0;
//Variables for managing to turning game at specific time 
unsigned long allTime = 0;
unsigned long lastTime = 0;
unsigned long lastTime2 = 0;
unsigned long soundTime = 0;
unsigned long soundLastTime = 0;
unsigned long lastDebounceTime = 0; 
unsigned long interrupt_time = 0;
int debounceDelay = 80;
int soundDelay = 0;
//Buttons variables
const int forward = 7;//7,8
const int right = 8;//8,6
const int left = 6;//6,7
const int down = 5;//5,5
const int a = 9;
const int soundPin = 4;
const int b = 3;
//Battery analog pin
const int battery = A0;
//Constant variables that are use when SettingGameSnake is called
const int delayTimeConst = 230;
const int bodyLengthConst = 3;
//Variable for storing 8 bites
const int allLedTurnOn = 0b11111111;
const int allLedTurnOff = 0b00000000;
const int addressSound = 1;
const int addressSnake1 = 2;
const int addressSnake2 = 3;
const int addressSpace1 = 4;
const int addressSpace2 = 6;
const int addressGalaxian1 = 7;
const int addressGalaxian2 = 8;
//Movement variables
volatile int x = 0;
volatile int y = 0;
volatile int active = 0;
//volatile bool change = false;
int pX = 7;
int pY = 7;
volatile int sX = 0;
volatile int sY = 0;
//Variable for length of snake
int bodyLength = 3;
int opravaPohybu = 0;
int opravaPohybuf = 0;
int activatorV = 0;
int bulletTimer = 0;
int enBulletTimer1 = 0;
int enBulletTimer2 = 0;
int differenceCol = 0;
int moduloThree = 0;
int moveEnemy1 = 0;
int moveEnemy2 = 3;
int moveEnemyT1 = 2;
int moveEnemyT2 = 4;
int wave = 0;
int computer = 0;
int points = 0;
int prucho = 0;
byte congrats = 0;
bool congratsState = true;
bool soundState = true;
bool menuFirstWalkThrough = true;
bool goThrough = true;
bool startingLedOn = true;
bool load = true;
bool lastState = true;

/****************************************************************/
//Setup function
/****************************************************************/
void setup() {
  //Defining variables
  u8g2.begin();
  // Draw battery,voltage,version
  DrawMenu();
  //u8x8.refreshDisplay();    // only required for SSD1606/7  
  Serial.begin(9600);
  //Serial.print(EEPROM.read(addressSnake1));
  // Defining all buttons
  //EEPROM.write(addressSnake1,78);
  pinMode(forward, INPUT_PULLUP);
  pinMode(right, INPUT_PULLUP);
  pinMode(left, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(a, INPUT_PULLUP);
  pinMode(b, INPUT_PULLUP); 
  // Defining interrupts for all the buttons because we know that arduino nano every has all pins with interupts.
  attachInterrupt(digitalPinToInterrupt(forward), blink, FALLING);
  attachInterrupt(digitalPinToInterrupt(right), blink, FALLING);
  attachInterrupt(digitalPinToInterrupt(left), blink, FALLING);
  attachInterrupt(digitalPinToInterrupt(down), blink, FALLING);
  attachInterrupt(digitalPinToInterrupt(a), blink, FALLING);
  attachInterrupt(digitalPinToInterrupt(b), blink, FALLING);
  // Pin for piezzo buzzer
  pinMode(soundPin, OUTPUT);
  pinMode(LED_BUILTIN,OUTPUT);
  // Erasing and inicializing all displays
  for (int index = 0; index < 4; index++) {
    lc.shutdown(index, false);
    lc.setIntensity(index, 0);
    lc.clearDisplay(index);
  }
  // Making random random
  randomSeed(analogRead(0));
  // reading state of sound (on/off)
  soundState = EEPROM.read(addressSound);
  delay(100);
}
/****************************************************************/
//Input functions
/****************************************************************/
void blink() { //Function for all buttons it is called with interupts so that means that is will go here every time you press button.
  interrupt_time = millis();
  if (interrupt_time-lastDebounceTime > debounceDelay) {
    y=(!digitalRead(forward) * 1) | (!digitalRead(down) * -1);
    x=(!digitalRead(left) * 1) | (!digitalRead(right) * -1);
    active=(!digitalRead(a) * 1) | (!digitalRead(b) * -1);
    lastDebounceTime = interrupt_time; 
  }
}
/****************************************************************/
//Gamestate functions
/****************************************************************/

//Menu function
void Menu(bool firstGoThrough, bool go) {
  //When menu start show M in start
  if (firstGoThrough==true) {
    int noty[] = {175,233,73.5,220,73.5,92.5};
    whichIconShowed = 0;
    //Clearing all displays
    for (int index = 0; index < 4; index++) {
      lc.clearDisplay(index);
    }
    for (int i = 0; i < 6; i++) {
      for (int row = 0; row < 16; row++) {
        for (int col = 0; col < 16; col++) {
          int nazev[] = {pgm_read_byte(&(HolTyc1[row][col])),pgm_read_byte(&(HolTyc2[row][col])),pgm_read_byte(&(HolTyc3[row][col])),pgm_read_byte(&(HolTyc4[row][col])),pgm_read_byte(&(HolTyc4[row][col])),pgm_read_byte(&(HolTyc4[row][col]))};
          ShowText(row,col,nazev[i]);
          soundDelay = op.playSound(6,i);
        } 
        delay(5);
      }
    }
    DrawMenu();
    noTone(soundPin);
    ActiveButton();
    //When some button is pressed then show menu
    for (int index = 0; index < 4; index++) {
      lc.clearDisplay(index);
    }
    ShowOneDisplay(1,'s');
    ShowOneDisplay(0,'g');
    ShowOneDisplay(2,'n');
    ShowOneDisplay(3,'i');
  } else  if (go==true){
    whichIconShowed = 0;
    for (int index = 0; index < 4; index++) {
      lc.clearDisplay(index);
    }
    //Showing menu text M
    for (int row = 0; row < 16; row++) {
      for (int col = 0; col < 16; col++) {
        ShowText(row,col,pgm_read_byte(&(MenuMenu[row][col])));
      }
    }
    DrawMenu();
    ActiveButton();
    for (int index = 0; index < 4; index++) {
      lc.clearDisplay(index);
    }
    ShowOneDisplay(1,'s');
    ShowOneDisplay(0,'g');
    ShowOneDisplay(2,'n');
    ShowOneDisplay(3,'i');
  }
  byte xNula = 1;
  byte yNula = 1;
  IsButtonPressed();
  if (whichIconShowed!=0) {
    if (x==0) {
      xNula=0;
    } else if (y==0) {
      yNula=0;
    } else {
      xNula = 1;
      yNula = 1;
    }
    /*
     * Solving menu
     *  _____      
     * |     |
     * | 2,4 |
     * | 1,3 |
     * |_____|
     * I now that apps next to each other have difference 2,
     * and apps under each other have difference 1.
     * So with little of math that is under I could solve this problem.
     */
    if (whichIconShowed==1) {
      whichIconShowed += (abs(x) + 1) * xNula;
      whichIconShowed += (abs(y)) * yNula;
    } else if (whichIconShowed==2) {
      whichIconShowed += (abs(x) + 1) * xNula;
      whichIconShowed -= (abs(y)) * yNula;
    } else if (whichIconShowed==3) {
      whichIconShowed -= (abs(x) + 1) * xNula;
      whichIconShowed += (abs(y)) * yNula;
    } else if (whichIconShowed==4) {
      whichIconShowed -= (abs(x) + 1) * xNula;
      whichIconShowed -= (abs(y)) * yNula;
    }
  }
  if (whichIconShowed==0 and active==0) {
    whichIconShowed=2;
  }
  ShowBorders(whichIconShowed-1);
  if (active==1 and whichIconShowed!=0) {
    active=0;
    if (soundState==true) {
      tone(soundPin,147);
      delay(100);
      noTone(soundPin);
    }
    if (whichIconShowed==2) {
      //Showing menu text S - Snake
      SettingGameSnake();
      GenerateFood();
      gameState = 2;
    } else if (whichIconShowed==1) {
      //Showing menu text G - Galaxian
      SettingGameGalaxian();
      gameState = 6;
    } else if (whichIconShowed==3) {
      //Showing menu text N - nastaveni
      SettingSetting();
    } else if (whichIconShowed==4) {
      //Showing menu text I - Space Invators
      SettingGameSpace();
      gameState = 3;
    }
  }
  delay(10);
}
//GameOver function
void GameOver(int score, int game) {
  //Changing gameState for gameOver state
  noTone(soundPin);
  soundDelay = 0;
  gameState = 1;
  menuFirstWalkThrough = false;
  //Clearing all displays
  for (int index = 0; index < 4; index++) {
    lc.clearDisplay(index);
  }
  if (game==1 or game==2 or game==4) {
    //Showing text Game over
    for (int row = 0; row < 16; row++) {
      for (int col = 0; col < 16; col++) {
        ShowText(row,col,pgm_read_byte(&(GameOverMessage[row][col])));
      }
    }
  } else if (game==3) {
    //Showing text Game over
    for (int row = 0; row < 16; row++) {
      for (int col = 0; col < 16; col++) {
        ShowText(row,col,pgm_read_byte(&(Win[row][col])));
      }
    }
  }
  //Snake
  if (game == 1) {
    if (EEPROM.read(addressSnake1) <= score) {
      EEPROM.write(addressSnake2,EEPROM.read(addressSnake1));
      EEPROM.write(addressSnake1,score);
    } else if (EEPROM.read(addressSnake2) < score) {
      EEPROM.write(addressSnake2,score);
    }
  } else if (game == 2 or game == 3) {
    if (read2EEPROM(addressSpace1) <= score) {
      write2EEPROM(addressSpace2,read2EEPROM(addressSpace1));
      write2EEPROM(addressSpace1,score);
    } else if (read2EEPROM(addressSpace2) < score) {
      write2EEPROM(addressSpace2,score);
    }
  } else if (game == 4) {
    if (EEPROM.read(addressGalaxian1) <= score) {
      EEPROM.write(addressGalaxian2,EEPROM.read(addressGalaxian2));
      EEPROM.write(addressGalaxian1,score);
    } else if (EEPROM.read(addressGalaxian2) < score) {
      EEPROM.write(addressGalaxian2,score);
    }
  }
  ActiveButton();
  //Clearing all displays
  for (int index = 0; index < 4; index++) {
    lc.clearDisplay(index);
  }
  //Showing text score
  for (int row = 0; row < 16; row++) {
      for (int col = 0; col < 16; col++) {
        if (row<=7) {
          ShowText(row,col,pgm_read_byte(&(Score[row][col])));
        }
      }
  }
  ShowNumbers(score);
  ActiveButton();
  gameState=0;
  goThrough = true;
  for (int index = 0; index < 4; index++) {
    lc.clearDisplay(index);
  }
  delay(250);
}
//Main function for game Snake
void Snake() {
  //Local variables Snake
  allTime = millis();
  //Movement: left....x = 1, right....x = -1, forward....y = 1, down....y = -1. 
  if ((x==0 and y==0) and startingLedOn==true) {
    ShowText(pY,pX,1);
    //Serial.print()
  } else if (startingLedOn==true) {
    ShowText(pY,pX,0);
    startingLedOn=false;
  }
  if (activatorV == 1 and (x!=0 or y!=0)) {
    activatorV=0;
    lastTime = allTime;
  }
  if (active==-1) {
    activatorV=1;
    active=0;
    sX=0;
    sY=0;
    x=0;
    y=0;
    GameOver(bodyLength-3,1);
  }
  if (activatorV == 0 and active == 1 and startingLedOn==false) {
    activatorV = 1;
    y=0;
    x=0;
    noTone(soundPin);
  }
  if (((bodyLength-3) > EEPROM.read(addressSnake1) and congratsState==true) and congrats <= 3) {
    soundDelay = op.playSound(9,congrats);
    congratsState=false;
  }
  if (activatorV == 0) {
    // Smart blinking food led 
    ShowText(foodY,foodX,millis() % 1000 < 500 ? 1 : 0);
    // This will run every delayTime
    if (allTime - lastTime >= delayTime) {
      //sY=y;
      //sX=x;
      if (sY!=-y and y!=0) {
        sY=y;
        sX=0;
      } else if (sX!=-x and x!=0) {
        sX=x;
        sY=0;
      }
      // Making head bigger
      pY -= sY;
      pX -= sX;
      Warp();
      if (Array[pY][pX] > 0) {
        GameOver((bodyLength-3),1);
      }
      if (sX!=0 or sY!=0) {
        Array[pY][pX] = bodyLength + 1;
      }
      //This will run only if game snake is on - if you are not dead
      if (gameState==2) {
        //Big for loop that run for all pixel in array
        for (int row = 0; row < 16; row++) {
          for (int col = 0; col < 16; col++) {
            // if there is a body segment, decrement it's value
            if (Array[row][col] > 0 ) {
              Array[row][col]--;
              //If This pixel is zero (I know that zero means that it is snake ending) turn pixel off
              if (Array[row][col]==0) {
                ShowText(row,col,0);
              //If pixel is the biggest turn pixel on
              } else if (Array[row][col]==bodyLength) {
                ShowText(row,col,1);
              }
            }
          }
        }
        if (soundDelay == 0 and (sX !=0 or sY !=0)) {
            soundDelay = op.playSound(1,0);
          }
        //If snake eated the food
        if (Array[pY][pX] == Array[foodY][foodX] && Array[pY][pX]!=0) {
  //        Serial.println("snez");
          soundDelay = op.playSound(2,0);
          bodyLength += 1;
          delayTime -= 0.5;
          foodX=-1;
          foodY=-1;
          GenerateFood();
          u8g2.firstPage();
          do {
            String out1 = "Your Score: " + String(bodyLength-3);
            String out2 = "High Score: " + String(EEPROM.read(addressSnake1));
            u8g2.setFont(u8g2_font_helvR12_tr);
            u8g2.setCursor(0, 13);
            u8g2.print(out1);
            u8g2.setCursor(0,28);
            u8g2.print(out2);
          } while ( u8g2.nextPage() );
        }
        //Resetting timer
        lastTime = allTime;
      }
    }
  }
}
void SpaceInvators() {
  allTime = millis();
  if (active==1 and bulletTimer==0 and activatorV == 2) {
    for (int col = 0; col < 16; col++) {
      if (Array[15][col] == 1) {
        bulletTimer = 14;
        bulletPosition[0] = col - 1; 
        bulletPosition[1] = 13;
      }
    }
    active=0;
    soundDelay = op.playSound(4,0);
  } else if (bulletTimer>0) {
    bulletTimer -= 1;
    if (Array[bulletPosition[1]][bulletPosition[0]] == 1) {
      bulletTimer=0;
      if (bulletPosition[1] > 7) {
        bulletTimer=0;
        Array[bulletPosition[1]][bulletPosition[0]] = 0;
        soundDelay = op.playSound(5,0);
      }
      if (bulletPosition[1] < 7) {
        Destroy(bulletPosition[1],bulletPosition[0]);
        computer=0;
        for (int i = 0; i < 6; i++) {
          if (Enemys[i]==5 or Enemys[i]==4) {
            computer+=1;
          }
        }
        bulletTimer = 0;
      }
    }
    ShowText(bulletPosition[1],bulletPosition[0],1);
    bulletPosition[1] -=  1;    
    if (computer >=6) {
      computer = 0;
      noTone(soundPin);
      GameOver(points,3);
      activatorV = -5;
    } else {
      computer = 0;
    }
  }
  if (enBulletTimer1>0 and activatorV == 2) {
    enBulletTimer1 -= 1;
    if (Array[enBulletPosition1[1]][enBulletPosition1[0]] == 1) {
      if (enBulletPosition1[1] >= 8 and enBulletPosition1[1] < 14) {
        Array[enBulletPosition1[1]][enBulletPosition1[0]] = 0;
        Array[enBulletPosition1[1]][enBulletPosition1[0] + 1] = 0;
        Array[enBulletPosition1[1]][enBulletPosition1[0] - 1] = 0;
        Array[enBulletPosition1[1] + 1][enBulletPosition1[0]] = 0;
        enBulletTimer1 = 0;
        enBulletPosition1[0] = 0;
        enBulletPosition1[1] = 0;
        //PlaySound(5);
      } else if (enBulletPosition1[1] >= 14) {
          for (int index = 0; index < 4; index++) {
            lc.clearDisplay(index);
          }
          int noty[6] = {196,233,147,131,233,196};
          for (int d = 0; d < 3; d++) {
            for (int row = 0; row < 2; row++) {
              for (int col = 0; col < 3; col++) {
                int nazev1[3] = {pgm_read_byte(&(lod[row][col])),pgm_read_byte(&(ship_explosion1[row][col])),pgm_read_byte(&(ship_explosion2[row][col]))};
                ShowText(row+14,col+pX,nazev1[d]);
              }
            }
            soundDelay = op.playSound(8,noty[d]);
            soundDelay = op.playSound(8,noty[d+1]);
            noty[d] = noty[d+2];
            noty[d+1] = noty[d+3];
          }
          GameOver(0,2);
          activatorV = -5;
      }
    }
    if (enBulletPosition1[0]==0 and enBulletPosition1[1] == 0) {
      ShowText(enBulletPosition1[1],enBulletPosition1[0],0);
    } else {
      ShowText(enBulletPosition1[1],enBulletPosition1[0],1);
      enBulletPosition1[1] += 1;
    }
    if (enBulletPosition1[1] >= 16) {
      enBulletTimer1 = 0;
    }
  }
  if (enBulletTimer2>0 and activatorV == 2) {
    enBulletTimer2 -= 1;
    if (Array[enBulletPosition2[1]][enBulletPosition2[0]] == 1) {
      if (enBulletPosition2[1] >= 8 and enBulletPosition2[1] < 14) {
        Array[enBulletPosition2[1]][enBulletPosition2[0]] = 0;
        Array[enBulletPosition2[1]][enBulletPosition2[0] + 1] = 0;
        Array[enBulletPosition2[1]][enBulletPosition2[0] - 1] = 0;
        Array[enBulletPosition2[1] + 1][enBulletPosition2[0]] = 0;
        enBulletTimer2=0;
        enBulletPosition2[0] = 0;
        enBulletPosition2[1] = 0;
      } else if (enBulletPosition2[1] >= 14) {
        for (int index = 0; index < 4; index++) {
            lc.clearDisplay(index);
          }
          int noty[6] = {196,233,147,131,233,196};
          for (int d = 0; d < 3; d++) {
            for (int row = 0; row < 2; row++) {
              for (int col = 0; col < 3; col++) {
                int nazev1[3] = {pgm_read_byte(&(lod[row][col])),pgm_read_byte(&(ship_explosion1[row][col])),pgm_read_byte(&(ship_explosion2[row][col]))};
                ShowText(row + 14,col + pX,nazev1[d]);
              }
            }
            soundDelay = op.playSound(8,noty[d]);
            soundDelay = op.playSound(8,noty[d+1]);
            noty[d] = noty[d+2];
            noty[d+1] = noty[d+3]; 
          }
          GameOver(0,2);
          activatorV = -5;
      }
    }
    if (enBulletPosition2[0]==0 and enBulletPosition2[1] == 0) {
      ShowText(enBulletPosition2[1],enBulletPosition2[0],0);
    } else {
      ShowText(enBulletPosition2[1],enBulletPosition2[0],1);
      enBulletPosition2[1] += 1;
    }
    if (enBulletPosition2[1] >= 16) {
      enBulletTimer2 = 0;
    }
  }
  if (allTime - lastTime >= delayTime) {
    if (activatorV == 2) {
      points -= 1;
      u8g2.firstPage();
      do {
        String out1 = "Your Score: " + String(points);
        String out2 = "High Score: " + String(read2EEPROM(addressSpace1));
        u8g2.setFont(u8g2_font_helvR12_tr);
        u8g2.setCursor(0, 13);
        u8g2.print(out1);
        u8g2.setCursor(0,28);
        u8g2.print(out2);
      } while ( u8g2.nextPage() );
      if (points <= 0) {
        points = 1;
      }
      if (active == -1) {
        x=0;
        y=0;
        active=0;
        activatorV=3;
        GameOver(0,2);
      }
    } else if (x!=0) {
      activatorV=2;
    }
    moveEnemyT1 -= 1;
    moveEnemyT2 -= 1;
    lastTime=allTime;
    if (moveEnemyT1 == 0) {
      moveEnemyT1 = 2;
      MoveEnemys(1);
      for (int i = 0; i < 3; i++) {
        if (Enemys[i] == 5 or Enemys[i] == 4) {
          computer += 1;
        }
      }
      if (computer >= 3) {
        computer = 0;
      } else {
        ShootEnemy(random(0,3),1);
      }
    }
    if (moveEnemyT2 == 0) {
      moveEnemyT2 = 3;
      MoveEnemys(2);
      for (int i = 0; i < 3; i++) {
        if (Enemys[i + 3] == 5 or Enemys[i + 3] == 4) {
          computer += 1;
        } 
      }
      if (computer >= 3) {
        computer = 0;
      } else {
        ShootEnemy(random(3,6),2);
      }
    }
    if (x!=0 and (activatorV == 0 or activatorV==1)) {
      activatorV = 2;
    }
    if (activatorV == 0 or activatorV == 2) {
      pX-=x;
      x=0;
      Warp();
      for (int row = 0; row < 2; row++) {
        for (int col = 0; col < 16; col++) {
          Array[row+14][col] = 0;
        }
      }
      if (activatorV == 0) {
        activatorV = 1;
      }
      for (int row = 0; row < 16; row++) {
        for (int col = 0; col < 16; col++) {
          if (pgm_read_byte(&(lod[row][col]))==1 and col < 3 and row < 2) {
            Array[row + 14][col+pX] = 1;
          } else if (row < 2 and col < 4) {
            Array[row + 14][col+pX] = 0;
            Array[row + 14][col+pX-4] = 0;
          } 
          if (load == true) {
            if (row <= 1 and activatorV==2) {
              ShowSprite(row,col,0,0,moveEnemy1,Enemys[0]);
              ShowSprite(row,col,0,0,moveEnemy1+4,Enemys[1]);
              ShowSprite(row,col,0,0,moveEnemy1+8,Enemys[2]);
            } else if (row >= 3 and row <= 4 and activatorV==2) {
              ShowSprite(row,col,0,1,moveEnemy2,Enemys[3]);
              ShowSprite(row,col,0,1,moveEnemy2+4,Enemys[4]);
              ShowSprite(row,col,0,1,moveEnemy2+8,Enemys[5]);
            }
          }
          if (Array[row][col]==1) {
            ShowText(row,col,1);
          } else {
            ShowText(row,col,0);
          }
//          Serial.print(Array[row][col]);
//          Serial.print(",");
        }
//        Serial.println(";");
      }
      load = false;
//      Serial.println("-----------------");
    }
  }
}
void GalaxianGame() {
  allTime = millis();
  if (active == -1) {
    y=0;
    active=0;
    activatorV=0;
    GameOver(points,4);
  }
  if (y!=0 and activatorV==0) {
    activatorV=1;
    for (int index = 0; index < 4; index++) {
      lc.clearDisplay(index);
    }
  } else if (activatorV==0) {
    for (int row = 0; row < 3; row++) {
      for (int col = 0; col < 2; col++) {
        ShowText(row+pY,col,pgm_read_byte(&(ship[row][col]))); 
      }
    }
    u8g2.firstPage();
    do {
      String out1 = "Your Score: " + String(points);
      String out2 = "High Score: " + String(EEPROM.read(addressGalaxian1));
      u8g2.setFont(u8g2_font_helvR12_tr);
      u8g2.setCursor(0, 13);
      u8g2.print(out1);
      u8g2.setCursor(0,28);
      u8g2.print(out2);
    } while ( u8g2.nextPage() );
  }
  if (activatorV==1) {
    if (allTime-lastTime>=200) {
      pY-=y;
      y=0;
      Warp();
      for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 2; col++) {
          Array[row+pY][col] = pgm_read_byte(&(ship[row][col])) + 1; 
        }
      }
      if (active==1) {
        active=0;
        soundDelay = op.playSound(4,0);
        ShootG(pY+1);
      }
      // Cele vykreslovani hry
      for (int row = 0; row < 16; row++) {
        for (int col = 0; col < 16; col++) {
          if (Array[row][col] > 0) {
            Array[row][col]--;
            if (Array[row][col]==0) {
              ShowText(row,col,0);
            } else if (Array[row][col]==1) {
              int er = 0;
              for (int i = 0; i < prucho*2; i++) {
                if (i%2==0) {
                  if (row==EnemysG[i] and col==EnemysG[i+1]) {
                    er = 1;
                    soundDelay = op.playSound(5,0);
                    ShowText(EnemysG[i],EnemysG[i+1],0);
                    ShowText(EnemysG[i],EnemysG[i+1]+1,0);
                    EnemysG[i]=-EnemysG[i];
                    ShowText(row,col,0);
                    if (EnemyDouble[i]>1 and EnemysG[i]!=abs(EnemysG[i])) {
                      EnemysG[i]=-EnemysG[i];
                      EnemyDouble[i]-=1;
                      EnemysG[i+1]=EnemysG[i+1]-1;
                      ShowText(EnemysG[i],EnemysG[i+1]+2,0);
                      ShowText(EnemysG[i],EnemysG[i+1]+1,1);
                    } else if ((EnemysG[i]!=17 and EnemysG[i+1]!=17) and EnemysG[i]!=abs(EnemysG[i])) {
                      wave+=1;
                      points+=1;
                      u8g2.firstPage();
                      do {
                        String out1 = "Your Score: " + String(points);
                        String out2 = "High Score: " + String(EEPROM.read(addressGalaxian1));
                        u8g2.setFont(u8g2_font_helvR12_tr);
                        u8g2.setCursor(0, 13);
                        u8g2.print(out1);
                        u8g2.setCursor(0,28);
                        u8g2.print(out2);
                      } while ( u8g2.nextPage() );
                      EnemysG[i+1]=17;
                      EnemysG[i]=17;
                    }
                    for (int j = col; j < 16; j++) {
                      Array[row][j]=0;
                    }
                  }
                }
              } 
              if (er!=1) {
                ShowText(row,col,1);
              }
            }
          }
        }
      }
    }
    if (allTime-lastTime2>=delayTime) {
      if (wave>=prucho) {
        wave=0;
        moveEnemy1+=1;
        prucho = EnemyG(moveEnemy1);
      }
      for (int i = 0; i < prucho*2; i++) {
        if (i%2==0) {
          if (EnemysG[i]!=abs(EnemysG[i])) {
            if (EnemyDouble[i]>1) {
              EnemysG[i]=-EnemysG[i];
              EnemyDouble[i]-=1;
              EnemysG[i+1]=EnemysG[i+1]-1;
            } else if (EnemysG[i]!=17 and EnemysG[i+1]!=17) {
              wave+=1;
              points+=1;
              u8g2.firstPage();
              do {
                String out1 = "Your Score: " + String(points);
                String out2 = "High Score: " + String(EEPROM.read(addressGalaxian1));
                u8g2.setFont(u8g2_font_helvR12_tr);
                u8g2.setCursor(0, 13);
                u8g2.print(out1);
                u8g2.setCursor(0,28);
                u8g2.print(out2);
              } while ( u8g2.nextPage() );
              EnemysG[i+1]=17;
              EnemysG[i]=17;
            }
          }
          if (EnemysG[i]!=17 and EnemysG[i+1]!=17) {
            ShowText(EnemysG[i],EnemysG[i+1]+1,0);
            if (EnemyDouble[i]>1) {
              for (int j = 1; j < EnemyDouble[i]; j++) {
                ShowText(EnemysG[i],EnemysG[i+1]-j,1);
              }
            }
            ShowText(EnemysG[i],EnemysG[i+1],1);
            EnemysG[i+1]--;
          }
          if (EnemysG[i+1]<=1) {
            GameOver(points,4);
          }
        }
      }
      lastTime2=allTime;
    }
  }
}
void InfoFun() {
  for (int index = 0; index < 4; index++) {
    lc.clearDisplay(index);
  }
  for (int row = 0; row < 16; row++) {
    for (int col = 0; col < 16; col++) {
      ShowText(row,col,pgm_read_byte(&(InfoText[row][col])));
    }
  }
  String out[9] = {"Miki toto je", "4 generace", "nasi konzole.", "Programoval", "jsem ji na", "chalupe.", "UZIVEJ a dej", "vedet :)"};
  for (int i = 0; i < 8; i++) {
    u8g2.firstPage();
    do {
      u8g2.setFont(u8g2_font_helvR12_tr);
      u8g2.setCursor(0, 13);
      u8g2.print(out[i]);
    } while ( u8g2.nextPage() );
    ActiveButton();
    delay(200);
  }
  gameState=0;
  goThrough = true;
}
void NastaveniHer() {
  if (activatorV == 0) {
    if (soundState==true) {
      for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 6; col++) {
          ShowText(row + 1,col + 9,pgm_read_byte(&(Sound_on[row][col])));
        }
      }
    } else if (soundState==false) {
      for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 6; col++) {
          ShowText(row + 1,col + 9,pgm_read_byte(&(Sound_off[row][col])));
        }
      }
    }
    IsButtonPressed();
    if (active==-1) {
      gameState=0;
      goThrough=true;
      for (int index = 0; index < 4; index++) {
          lc.clearDisplay(index);
      }
    }
    byte xNula = 1;
    byte yNula = 1;
    if (whichIconShowed!=0) {
      if (x==0) {
        xNula=0;
      } else if (y==0) {
        yNula=0;
      } else {
        xNula = 1;
        yNula = 1;
      }
      if (whichIconShowed==1) {
      whichIconShowed += (abs(x) + 1) * xNula;
      whichIconShowed += (abs(y)) * yNula;
    } else if (whichIconShowed==2) {
      whichIconShowed += (abs(x) + 1) * xNula;
      whichIconShowed -= (abs(y)) * yNula;
    } else if (whichIconShowed==3) {
      whichIconShowed -= (abs(x) + 1) * xNula;
      whichIconShowed += (abs(y)) * yNula;
    } else if (whichIconShowed==4) {
      whichIconShowed -= (abs(x) + 1) * xNula;
      whichIconShowed -= (abs(y)) * yNula;
    }
    } else if (whichIconShowed==0 and active==0) {
      whichIconShowed=2;
    }
    ShowBorders(whichIconShowed-1);
    if (active==1 and whichIconShowed!=0) {
      active=0;
      if (whichIconShowed!=4) {
        if (soundState==true) {
          tone(soundPin,147);
          delay(100);
          noTone(soundPin);
        }
        for (int index = 0; index < 4; index++) {
            lc.clearDisplay(index);
          }
      }
      if (whichIconShowed==2) {
          gameState=4;
      } else if (whichIconShowed==1) {
          activatorV = 1;
          whichIconShowed = 0;
          for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
              ShowText(row,col,pgm_read_byte(&(Gold_medal[row][col])));
            }
          }
          for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
              ShowText(row,col + 8,pgm_read_byte(&(Silver_medal[row][col])));
            }
          }
      } else if (whichIconShowed==3) {
          activatorV = 2;
          whichIconShowed = 0;
          for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
              ShowText(row,col,pgm_read_byte(&(Gold_medal[row][col])));
            }
          }
          for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
              ShowText(row,col + 8,pgm_read_byte(&(Silver_medal[row][col])));
            }
          }
      } else if (whichIconShowed==4) {
          soundState=!soundState;
          if (soundState==true) {
            tone(soundPin,147);
            delay(100);
            noTone(soundPin);
          }
          EEPROM.write(addressSound,soundState);
      }
    }
  } else if (activatorV==1 or activatorV==2) {
    IsButtonPressed();
    byte xNula = 1;
    byte yNula = 1;
    if (active==-1) {
      SettingSetting();
    }
    if (whichIconShowed!=0) {
      if (x==0) {
        xNula=0;
      } else if (y==0) {
        yNula=0;
      } else {
        xNula = 1;
        yNula = 1;
      }
      if (whichIconShowed==2) {
        whichIconShowed += (abs(x) + 1) * xNula;
      } else if (whichIconShowed==4) {
        whichIconShowed -= (abs(x) + 1) * xNula;
      }
    } else if (whichIconShowed==0 and active==0) {
      whichIconShowed=2;
    }
    if (whichIconShowed==2) {
      ShowBorders(1);
      for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
          ShowText(row,col + 8,pgm_read_byte(&(Silver_medal[row][col])));
        }
      }
      if (activatorV==1) {
        ShowNumbers(EEPROM.read(addressSnake1));
      } else if (activatorV==2) {
        ShowNumbers(read2EEPROM(addressSpace1));
      }
    } else if (whichIconShowed==4) {
        ShowBorders(3);
        for (int row = 0; row < 8; row++) {
          for (int col = 0; col < 8; col++) {
            ShowText(row,col,pgm_read_byte(&(Gold_medal[row][col])));
          }
        }
        if (activatorV==1) {
          ShowNumbers(EEPROM.read(addressSnake2));
      } else if (activatorV==2) {
          ShowNumbers(read2EEPROM(addressSpace2));
        }
    }
  }
  delay(10);
}
/****************************************************************/
//Helping functions
/****************************************************************/
void SettingGameSnake() {
  allTime = millis();
  lastTime=allTime;
  opravaPohybu = 0;
  delayTime = delayTimeConst;
  bodyLength = bodyLengthConst;
  activatorV = 0;
  startingLedOn = true;
  congrats=0;
  pX = 0;
  pY = 15;
  x = 0;
  y = 0;
  sX=0;
  sY=0;
  active=0;
  for (int row = 0; row < 16; row++) {
    for (int col = 0; col < 16; col++) {
      Array[row][col] = 0;
    }
  }
  for (int index = 0; index < 4; index++) {
    lc.clearDisplay(index);
  }
  u8g2.firstPage();
  do {
    //u8g2.setFont(u8g2_font_helvR14_tr);
    //u8g2.drawStr(0,15,"Hello Wold!");
    //u8g2.drawStr(0,30,"12345E7890!");
    String out1 = "Your Score: " + String(bodyLength-3);
    String out2 = "High Score: " + String(EEPROM.read(addressSnake1));
    u8g2.setFont(u8g2_font_helvR12_tr);
    u8g2.setCursor(0, 13);
    u8g2.print(out1);
    u8g2.setCursor(0,28);
    u8g2.print(out2);
  } while ( u8g2.nextPage() );
}
void SettingGameSpace() {
  allTime = millis();
  lastTime=allTime;
  delayTime = 10;
  enBulletTimer1 = 0;
  enBulletTimer2 = 0;
  bulletTimer = 0;
  activatorV = 0;
  moveEnemyT1 = 2;
  moveEnemyT2 = 3;
  moveEnemy1 = 1;
  moveEnemy2 = 4;
  points = 999;
  load = true;
  pX = 0;
  x = 0;
  y = 0;
  u8g2.firstPage();
  do {
    String out1 = "Your Score: " + String(points);
    String out2 = "High Score: " + String(read2EEPROM(addressSpace1));
    u8g2.setFont(u8g2_font_helvR12_tr);
    u8g2.setCursor(0, 13);
    u8g2.print(out1);
    u8g2.setCursor(0,28);
    u8g2.print(out2);
  } while ( u8g2.nextPage() );
  for (int i = 0; i < 6; i++) {
    Enemys[i] = random(0,3);
  }
  for (int row = 0; row < 16; row++) {
    for (int col = 0; col < 16; col++) {
      if (row <= 11 and row >= 8) {
        if (col >= 1 and col <= 4) {
          Array[row][col] = pgm_read_byte(&(block[row % 8][col - 1]));
        }
        if (col >= 6 and col <= 9) {
          Array[row][col] = pgm_read_byte(&(block[row % 8][(col - 2) % 4]));
        }
        if (col >= 11 and col <= 14) {
          Array[row][col] = pgm_read_byte(&(block[row % 8][(col - 3) % 8]));
        }
      } else if (row <= 1) {
        ShowSprite(row,col,0,0,moveEnemy1,Enemys[0]);
        ShowSprite(row,col,0,0,moveEnemy1+4,Enemys[1]);
        ShowSprite(row,col,0,0,moveEnemy1+8,Enemys[2]);
      } else if (row >= 3 and row <= 4) {
        ShowSprite(row,col,0,1,moveEnemy2,Enemys[3]);
        ShowSprite(row,col,0,1,moveEnemy2+4,Enemys[4]);
        ShowSprite(row,col,0,1,moveEnemy2+8,Enemys[5]);
      } else {
        Array[row][col] = 0;
      }
      //Serial.print(Array[row][col]);
      //Serial.print(",");
    }
    //Serial.println(";");
  }
  //Serial.println("-----------------");
  for (int index = 0; index < 4; index++) {
    lc.clearDisplay(index);
  }
}
void SettingGameGalaxian() {
  for (int index = 0; index < 4; index++) {
    lc.clearDisplay(index);
  }
  allTime = millis();
  lastTime=allTime;
  lastTime2=allTime;
  pY=13;
  delayTime=750;
  moveEnemy1=0;
  wave=0;
  points=0;
  activatorV=0;
  prucho = 0;
  for (int i = 0; i < 100; i++) {
    EnemysG[i]=0;
    if (i <= 50) {
      EnemyDouble[i]=0;
    }
  }
  for (int row = 0; row < 16; row++) {
    for (int col = 0; col < 16; col++) {
      Array[row][col] = 0;
    }
  }
  u8g2.firstPage();
  do {
    String out1 = "Your Score: " + String(points);
    String out2 = "High Score: " + String(EEPROM.read(addressGalaxian1));
    u8g2.setFont(u8g2_font_helvR12_tr);
    u8g2.setCursor(0, 13);
    u8g2.print(out1);
    u8g2.setCursor(0,28);
    u8g2.print(out2);
  } while ( u8g2.nextPage() );
}
void SettingSetting(){
  for (int index = 0; index < 4; index++) {
    lc.clearDisplay(index);
  }
  ShowOneDisplay(1,'d');
  ShowOneDisplay(0,'s');
  ShowOneDisplay(2,'i');
  activatorV = 0;
  whichIconShowed = 0;
  gameState = 5;
}
//Making snake go through walls
void Warp() {
  pX < 0 ? pX += 16 : 0;
  pX > 15 ? pX -= 16 : 0;
  pY < 0 ? pY += 16 : 0;
  pY > 15 ? pY -= 16 : 0;
}
//This function is like TurningLedArray but has changed display orientation
void ShowText(int row,int col, int state) {
  if (row < 8 and col < 8) {
    lc.setLed(1, map(col,0,7,7,0), row, state);
  } else if (row < 8 and col >= 8) {
    lc.setLed(3, map(col % 8,0,7,7,0), row, state);
  } else if (row >= 8 and col < 8) {
    lc.setLed(0, map(col,0,7,7,0), row % 8, state);
  } else if (row >= 8 and col >= 8) {
    lc.setLed(2, map(col % 8,0,7,7,0), row % 8, state);
  }
}
void ShowSprite(int row, int col, int animState, int differenceRow, int positionE, int whichEnemy) {
  if (positionE % 2 == 1) {
    differenceCol = 1;
  } else if (positionE % 2 == 0) {
    differenceCol = 0;
  } 
  if (col >= 2) {
    moduloThree = 2;
  } else {
    //mozna problem
    moduloThree = 1000;
  }
  if (whichEnemy==4 or whichEnemy==5) {
    if (col >= positionE and col <= positionE+1) {
      Array[row][col] = 0;
    }
  } else if ((col >= positionE and col <= positionE+1) and (row >= 3 and row <=4)) {
    Array[row][col] = pgm_read_byte(&(enemy1[(row - 1) % 2][(col - differenceCol) % moduloThree]));
  } else if (col == positionE and row <= 1) {
    Array[row][col] = pgm_read_byte(&(enemy2[row][(col - differenceCol) % moduloThree]));
  }
}
void ShowOneDisplay(int display, char what) {
  int offset = 2;
  if (what=='s') {
    for (int row = 0; row < 4; row++) {
      for (int col = 0; col < 4; col++) {
        lc.setLed(display,map(col,0,7,7,0) - offset,row + offset,pgm_read_byte(&(SnakeMenu[row][col])));
      }
    }
  }
  if (what=='d') {
    for (int row = 0; row < 4; row++) {
      for (int col = 0; col < 4; col++) {
        lc.setLed(display,map(col,0,7,7,0) - offset,row + offset,pgm_read_byte(&(Info[row][col])));
      }
    }
  }
  if (what=='g') {
    for (int row = 0; row < 4; row++) {
      for (int col = 0; col < 4; col++) {
        lc.setLed(display,map(col,0,7,7,0) - offset,row + offset,pgm_read_byte(&(Galaxian[row][col])));
      }
    }
  }
  if (what=='n') {
    for (int row = 0; row < 4; row++) {
      for (int col = 0; col < 4; col++) {
        lc.setLed(display,map(col,0,7,7,0) - offset,row + offset,pgm_read_byte(&(Nastaveni[row][col])));
      }
    }
  }
  if (what=='i') {
    for (int row = 0; row < 4; row++) {
      for (int col = 0; col < 4; col++) {
        lc.setLed(display,map(col,0,7,7,0) - offset,row + offset,pgm_read_byte(&(SpaceInvatorsMenu[row][col])));
      }
    }
  }
}
void ShowBorders(int display) {
  for (int i = 0; i<4;i++) {
    lc.setRow(i,0,allLedTurnOff);
    lc.setRow(i,7,allLedTurnOff);
    lc.setColumn(i,7,allLedTurnOff);
    lc.setColumn(i,0,allLedTurnOff);
  }
  lc.setRow(display,0,allLedTurnOn);
  lc.setRow(display,7,allLedTurnOn);
  lc.setColumn(display,7,allLedTurnOn);
  lc.setColumn(display,0,allLedTurnOn);
}
void ShowNumbers(int number) {
  String gameScore = String(number);
  for (int row = 0; row < 7; row++) {
    for (int col = 0; col < 16; col++) {
      ShowText(row + 8,col,0);
    }
  }
  for (int row = 0; row < 7; row++) {
    for (int col = 0; col < 4; col++) {
      for (int i = 0; i < gameScore.length(); i++) {
        int movetCol = col + i*5;
        int nazev[] = {pgm_read_byte(&(Zerox4[row][col])),pgm_read_byte(&(Onex4[row][col])),pgm_read_byte(&(Twox4[row][col])),pgm_read_byte(&(Threex4[row][col])),pgm_read_byte(&(Fourx4[row][col])),pgm_read_byte(&(Fivex4[row][col])),pgm_read_byte(&(Sixx4[row][col])),pgm_read_byte(&(Sevenx4[row][col])),pgm_read_byte(&(Eightx4[row][col])),pgm_read_byte(&(Ninex4[row][col]))};
        ShowText(row+9,movetCol,nazev[gameScore[i] - '0']);
      }
    }
  }
}
void GenerateFood() {
  while (true) {
    for (int i = 0; i<5;i++) {
      foodX = random(0,16);
      foodY = random(0,16); 
    }
    if (Array[foodY][foodX]==0) {
      break;
    } 
  }
  for (int row = 0; row < 16; row++) {
    for (int col = 0; col < 16; col++) {
      //Making Snake timer longer
      if (Array[row][col] > 0 ) {
        Array[row][col]++;
      }
    }
  }
}
void Destroy(int row, int col) {
  int whichEnemy = -1;
  if (row <= 1) {
    bodyLength = 1;
    if(moveEnemy1 == col) {
      whichEnemy = 0;
    } else if (moveEnemy1 + 4 == col) {
      whichEnemy = 1;
    } else if (moveEnemy1 + 8 == col) {
      whichEnemy = 2;
    }
    
  } else if (row >= 3 and row <= 4) {
    bodyLength = 2;
    if(moveEnemy2 == col) {
      whichEnemy = 3;
    } else if(moveEnemy2 + 1 == col) {
      whichEnemy = 3;
    } else if (moveEnemy2 + 4 == col) {
      whichEnemy = 4;
    } else if (moveEnemy2 + 5 == col) {
      whichEnemy = 4;
    } else if (moveEnemy2 + 8 == col) {
      whichEnemy = 5;
    } else if (moveEnemy2 + 9 == col) {
      whichEnemy = 5;
    }
  }
  Enemys[whichEnemy] = 4;
  load = true;
  soundDelay = op.playSound(3,0);
}
void MoveEnemys(int rows) {
  load = true;
  if (rows==1) {
    if (moveEnemy1==5) {
      foodY = 1;
    } else if (moveEnemy1==1) {
      foodY = 0;
    }
    if (foodY==1) {
      moveEnemy1 -= 1;
    } else {
      moveEnemy1 += 1;
    }
    for (int row = 0; row < 7; row++) {
        for (int col = 0; col < 16; col++) {
          Array[row][col] = 0;
        }
      }
  } else if (rows==2) {
    if (moveEnemy2==5) {
      foodX = 1;
    } else if (moveEnemy2==1) {
      foodX = 0;
    }
    if (foodX==1) {
      moveEnemy2 -= 1;
    } else {
      moveEnemy2 += 1;
    }
    for (int row = 0; row < 7; row++) {
        for (int col = 0; col < 16; col++) {
          Array[row][col] = 0;
        }
      } 
  }
}
void ShootEnemy(int whichEnemy,int rows) {
  while (Enemys[whichEnemy] == 5 or Enemys[whichEnemy] == 4) {
    if (rows == 1) {
      whichEnemy = random(0,3);
    } else if (rows == 2) {
      whichEnemy = random(3,6);
    }
  }
  if (enBulletTimer1 == 0) {
    if (whichEnemy == 0) {
      enBulletTimer1 = 14;
      enBulletPosition1[1] = 2;
      enBulletPosition1[0] = moveEnemy1;
      return;
    } else if (whichEnemy == 1) {
      enBulletTimer1 = 14;
      enBulletPosition1[1] = 2;
      enBulletPosition1[0] = moveEnemy1 + 4;
      return;
    } else if (whichEnemy == 2) {
      enBulletTimer1 = 14;
      enBulletPosition1[1] = 2;
      enBulletPosition1[0] = moveEnemy1 + 8;
      return;
    } 
  }
  if (enBulletTimer2 == 0) {
    if (whichEnemy == 3) {
      enBulletTimer2 = 11;
      enBulletPosition2[1] = 5;
      enBulletPosition2[0] = moveEnemy2;
      return;
    } else if (whichEnemy == 4) {
      enBulletTimer2 = 11;
      enBulletPosition2[1] = 5;
      enBulletPosition2[0] = moveEnemy2 + 4;
      return;
    } else if (whichEnemy == 5) {
      enBulletTimer2 = 11;
      enBulletPosition2[1] = 5;
      enBulletPosition2[0] = moveEnemy2 + 8;
      return;
    }
  }
}
void ShootG(int canon) {
  for (int col = 0; col < 16; col++) {
    if (Array[canon][col] == 0) {
      Array[canon][col] = col;
    }
  }
}
int EnemyG(int pruchod) {
  int randomR = 0; // How many enemys - maximum
  int randomR2 = 0; // How many enemys - between randomR and 1
  int randomR3 = 0; // How many enemys in one line
  if (pruchod <= 5) {
     randomR = 5;
     randomR3 = 2;
  } else if (pruchod <= 10 and pruchod > 5) {
     randomR = 8;
     randomR3 = 3;
  } else if (pruchod >= 10) {
     randomR = 12;
     randomR3 = 3;
  }
  randomR2 = random(1,randomR+1);
  for (int i = 0; i < randomR2*2; i++) {
    if (i%2==0) {
      EnemysG[i] = random(1,15);
      EnemysG[i+1] = 15;
      EnemyDouble[i] = random(1, randomR3+1);
      /*
      Serial.print("Hlava y: ");
      Serial.print(EnemysG[i]);
      Serial.print(" I-y: ");
      Serial.println(i);
      Serial.print("Hlava x: ");
      Serial.print(EnemysG[i+1]);
      Serial.print(" I-x: ");
      Serial.println(i+1);
      */
      
    }
  }
  return randomR2;
}
void IsButtonPressed() {
  x=0;
  y=0;
  active=0;
  while(true) {
    if (x!=0 or y!=0 or active!=0) {
      break;
    }
  }
}
void ActiveButton() {
  active=0;
  while (true) {
    if (active==1) {
      break;
    }
  }
}
/*void PlaySound(int sound, int i) {
  if (soundState==true) {
    if (sound == 1 and congratsState == true) {
      opravaPohybuf+=1;
      tone(soundPin,70);
      soundDelay = 100;
    } else if (sound == 2) {
      tone (soundPin,260);
      soundDelay = 250;
    } else if (sound == 3) {
      tone (soundPin, 40);
      soundDelay = 10;
    } else if (sound == 4) {
      noTone(soundPin);
      tone (soundPin, 147);
      soundDelay = 100;
    } else if (sound == 5) {
      noTone(soundPin);
      tone (soundPin, 87);
      soundDelay = 9;
    } else if (sound == 6) {
      int noty[] = {175,233,73.5,220,73.5,92.5};
      tone(soundPin,noty[i]);
    } else if (sound == 7) {
      int noty[16] = {147,247,220,196,165,330,294,262,156,233,220,196,139,233,220,196};
      tone(soundPin,noty[i % 16]);
    } else if (sound == 8) {
      tone(soundPin,i);
      delay(150);
      soundDelay = 0;
    } else if (sound == 9) {
      float noty[5] = {130.813,164.814,195,998,329.621};
      tone(soundPin,noty[i]);
      soundDelay = 200;
    }
  }
}
*/
void write2EEPROM(int address, int number) {
  byte byte1 = (number >> 8) & 0xFF;
  byte byte2 = number & 0xFF;

  EEPROM.write(address, byte1);
  EEPROM.write(address+1, byte2);
  read2EEPROM(address);
}
int read2EEPROM(int address) {
  byte byte1 = EEPROM.read(address);
  byte byte2 = EEPROM.read(address+1);

  int result = (byte1 << 8) + byte2;
  return result;
}
void DrawMenu() {
  int whichIcon = 0x0;
  int batteryValue = analogRead(battery);
  float mapBattery = (batteryValue * 5.0) / 1024.0;
  if (mapBattery >= 3.8) {
    whichIcon = 0x34;
  } else if (mapBattery < 3.8 and mapBattery >= 3.2) {
    whichIcon = 0x33;
  } else if (mapBattery < 3.2 and mapBattery >= 2.9) {
    whichIcon = 0x32;
  } else if (mapBattery < 2.9 and mapBattery >= 2.6) {
    whichIcon = 0x31;
  } else if (mapBattery < 2.6) {
    whichIcon = 0x30;
  }  
  u8g2.firstPage();
  do {
    String out1 = String(mapBattery) + "V";
    String out2 = "Ver. - " + versionUrxOS;
    u8g2.setFont(u8g2_font_battery24_tr);
    u8g2.drawGlyph(100,24,whichIcon);
    u8g2.setFont(u8g2_font_helvR12_tr);
    u8g2.setCursor(0, 13);
    u8g2.print(out1);
    u8g2.setCursor(0, 28);
    u8g2.print(out2);
  } while ( u8g2.nextPage() );
}
/****************************************************************/
//Loop
/****************************************************************/
void loop() {
  //Turning Led_builtin on if button is pressed for debugging
  //digitalWrite(LED_BUILTIN, digitalRead(forward) | digitalRead(down) | digitalRead(right) | digitalRead(left));
  //Turning snake ON
  soundTime = millis();
  if (gameState==2) {
    Snake();
  } else if (gameState==0) {
    Menu(menuFirstWalkThrough, goThrough);
    menuFirstWalkThrough = false;
    goThrough = false;
  } else if (gameState==3) {
    SpaceInvators();
  } else if (gameState==4) {
    InfoFun();
  } else if (gameState==5) {
    NastaveniHer();
  } else if (gameState==6) {
    GalaxianGame();
  }
  if (soundTime - soundLastTime >= soundDelay) {
    noTone(soundPin);
    soundLastTime = soundTime;
    if (soundDelay == 10) {
      tone (soundPin, 24);
      soundDelay = 8;
    } else if (soundDelay == 9) {
      tone (soundPin, 97);
      soundDelay = 7;
    } else if (soundDelay == 200) {
        if (congrats > 3) {
          soundDelay=0;
        } else {
          congrats+=1;
          congratsState=true;
        }
    } else {
      soundDelay=0;
    }
  }
}
