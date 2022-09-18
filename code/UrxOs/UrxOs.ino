//(Snad posledni verze) Po druhym vyhorenim jsme se rozhodl udelat algoritmus smesi for cyklu(verze jedna) a smerem zapinani a zapinani(verze dva) JDU NA TO
//OLED displey https://s.click.aliexpress.com/e/_APw28Y
#include <Urx.h> 
//#include <EEPROM.h>
//#include <U8g2lib.h>
//#include <Wire.h>
U8G2_SH1106_128X32_VISIONOX_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); 
LedControl lc = LedControl(11, 12, 10, 4);
Operator op();
Images img();
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
String versionUrxOS = "0.7.92";
int foodX = 0;
int foodY = 0;
//Delay in game is overrided when function SettingGameSnake is called
float delayTime = 300;
//GameState for setting menu - 0, gameover - 1, game - Snake - 2, game - Space Invators = 3, info - 4, nothing - 5
int gameState = 0;
//Menu variable
int whichIconShowed = 0;
int whichMedal = 0;
//Variables for managing to turning game at specific time 
unsigned long allTime = 0;
unsigned long lastTime = 0;
unsigned long lastTime2 = 0;
unsigned long soundTime = 0;
unsigned long soundLastTime = 0;
unsigned long lastDebounceTime = 0;       
int debounceDelay = 50;
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
const int addressIntensity = 4;
const int addressGalaxian1 = 5;
const int addressGalaxian2 = 6;
const int addressDanceMan1 = 7;
const int addressDanceMan2 = 9;
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
int moveEnemy1 = 0;
int wave = 0;
int noteCounter = 0;
int points = 50;
int prucho = 0;
int intensity = 0;
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
  u8g2.setFlipMode(1);
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
  //EEPROM.write(addressIntensity,0);
  intensity = EEPROM.read(addressIntensity);
  Serial.print(intensity);
  for (int index = 0; index < 4; index++) {
    lc.shutdown(index, false);
    lc.setIntensity(index, intensity);
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
  if ((millis()-lastDebounceTime) > debounceDelay) {
    y=(!digitalRead(forward) * 1) | (!digitalRead(down) * -1);
    x=(!digitalRead(left) * 1) | (!digitalRead(right) * -1);
    active=(!digitalRead(a) * 1) | (!digitalRead(b) * -1);
    lastDebounceTime = millis();
  }
}
/****************************************************************/
//Helping functions
/****************************************************************/
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
//This function is like TurningLedArray but has changed display orientation
void displayImage(const word* image, int displaySizeX=8, int displaySizeY=0, int display=0, int offsetX=0, int offsetY=0) {
  int MirrorX = displaySizeX-1;
  int MirrorY = displaySizeY-1;
  for (int index = 0; index < 2; index++) {
    for (int rows = 0; rows < displaySizeX; rows++) {
      if (displaySizeX == 8 and displaySizeY==0) {
        byte byte1 = (pgm_read_word(&image[(rows) + (index*displaySizeX)]) >> displaySizeX) & 0xFF;
        byte byte2 = pgm_read_word(&image[(rows) + (index*displaySizeX)]) & 0xFF;
        lc.setRow(index + 1 + index, MirrorX-rows, byte1);
        lc.setRow(index + index, MirrorX-rows, byte2);
      } else {
        for (int cols = 0; cols < displaySizeY; cols++) {
          lc.setLed(display, MirrorX-rows - offsetX, MirrorY-cols - offsetY, bitRead(pgm_read_word(&image[rows]), cols));
        }
      }
    }
  }
}
void displayLed(int row,int col, int state) {
  if (row < 8 and col < 8) {
    lc.setLed(1, 7-col, row, state);
  } else if (row < 8 and col >= 8) {
    lc.setLed(3, 7-col%8, row, state);
  } else if (row >= 8 and col < 8) {
    lc.setLed(0, 7-col, row%8, state);
  } else if (row >= 8 and col >= 8) {
    lc.setLed(2, 7-col%8, row%8, state);
  }
}
void displayNumbers(int number) {
  String gameScore = String(number);
  const word* nazev[] = {Zerox4,Onex4,Twox4,Threex4,Fourx4,Fivex4,Sixx4,Sevenx4,Eightx4,Ninex4};
  for (int i = 0; i < gameScore.length(); i++) {
    if (i==2) {
      displayImage(nazev[gameScore[i] - '0'], 4, 7, 2, -3, -1);
    } else {
      displayImage(nazev[gameScore[i] - '0'], 4, 7, 0, -4+(i*4), -1);
    }
  }
}
void displayOled(String writing = "") {
  u8g2.firstPage();
  do {
    int index = writing.lastIndexOf('\n');
    int length = writing.length();
    String first = writing.substring(0, index);
    u8g2.setFont(u8g2_font_helvR12_tr);
    u8g2.setCursor(0, 13);
    u8g2.print(first);
    u8g2.setCursor(0,28);
    index = writing.lastIndexOf('\n');
    length = writing.length();
    String second = writing.substring(index, length);
    u8g2.print(second);
  } while ( u8g2.nextPage() );
}
void displayClear() {
  for (int index = 0; index < 4; index++) {
    lc.clearDisplay(index);
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
    if (gameState==0) {
      if (points>0) {
        displayImage(Nastaveni1, 4,4,2,-2,-2);
      if (points==1) {
        points=-40;
      } else {
        points--;
      }
      } else {
        displayImage(Nastaveni2, 4,4,2,-2,-2);
        if (points==0) {
        points=40;
      } else {
        points++;
      }
      }
    }
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
void PlaySound(int sound, int i) {
  if (soundState==true) {
    if (sound == 1 and congratsState == true) {
      opravaPohybuf+=1;
      tone(soundPin,70);
      soundDelay = 100;
    } else if (sound == 2) {
      tone (soundPin,260);
      soundDelay = 250;
    } 
    /*else if (sound == 3) {
      tone (soundPin, 40);
      soundDelay = 10;
    } 
    */else if (sound == 4) {
      noTone(soundPin);
      tone (soundPin, 147);
      soundDelay = 100;
    } else if (sound == 5) {
      noTone(soundPin);
      tone (soundPin, 87);
      soundDelay = 9;
    }/* else if (sound == 8) {
      tone(soundPin,i);
      delay(150);
      soundDelay = 0;
    }
    */else if (sound == 9) {
      const float noty[5] = {130.813,164.814,195,998,329.621};
      tone(soundPin,noty[i]);
      soundDelay = 200;
    }
  }
}
void write2EEPROM(int address, int number) {
  byte byte1 = (number >> 8) & 0xFF;
  byte byte2 = number & 0xFF;

  EEPROM.write(address, byte1);
  EEPROM.write(address+1, byte2);
  //read2EEPROM(address);
}
int read2EEPROM(int address) {
  byte byte1 = EEPROM.read(address);
  byte byte2 = EEPROM.read(address+1);

  int result = (byte1 << 8) + byte2;
  return result;
}
void DrawMenu() {
  displayClear();
  displayImage(SnakeMenu, 4,4,1,-2,-2);
  displayImage(Nastaveni1, 4,4,2,-2,-2);
  displayImage(Galaxian, 4,4,0,-2,-2);
  displayImage(DancemanMenu, 4,4,3,-2,-2);
  byte whichIcon = 0x0;
  const int batteryValue = analogRead(battery);
  const float mapBattery = (batteryValue * 5.0) / 1024.0;
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
    const String out1 = String(mapBattery) + "V";
    const String out2 = "Ver. - " + versionUrxOS;
    u8g2.setFont(u8g2_font_battery24_tr);
    u8g2.drawGlyph(100,24,whichIcon);
    u8g2.setFont(u8g2_font_helvR12_tr);
    u8g2.setCursor(0, 13);
    u8g2.print(out1);
    u8g2.setCursor(0, 28);
    u8g2.print(out2);
  } while ( u8g2.nextPage() );
}
void SettingSetting(){
  displayClear();
  if (soundState==true) {
    displayImage(Sound_on, 6, 6, 3, -1, -1);
  } else if (soundState==false) {
    displayImage(Sound_off, 6, 6, 3, -1, -1);
  }
  displayImage(Intensity, 4,4,2,-2,-2);
  displayImage(HighScore, 4,4,0,-2,-2);
  activatorV = 0;
  whichIconShowed = 0;
}
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
  displayClear();
  displayOled("Your Score: " + String(bodyLength-3) + "\nHigh Score: " + String(EEPROM.read(addressSnake1)));
}
void SettingGameGalaxian() {
  displayClear();
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
  displayOled("Your Score: " + String(points) + "\nHigh Score: " + String(EEPROM.read(addressGalaxian1)));
}
void SettingGameDanceMan() {
  displayClear();
  points=0; // How many points
  prucho = 0; // When the arrow will apear
  wave=0; // What song to play
  noteCounter=0; // for playSongDanceMan counting which note to play
  soundDelay=0;
  sX=0; // Counting time when pressed
  sY=0; // Direction variable
  activatorV=0; // Is game on?
  lastTime=millis();
  displayOled("\nAll Points: " + String(points));
}
//Making snake go through walls
void Warp() {
  pX < 0 ? pX += 16 : 0;
  pX > 15 ? pX -= 16 : 0;
  pY < 0 ? pY += 16 : 0;
  pY > 15 ? pY -= 16 : 0;
}
/****************************************************************/
//Gamestate functions
/****************************************************************/

//Menu function
void Menu(bool firstGoThrough, bool go) {
  //When menu start show M in start
  if (firstGoThrough==true) {
    const float noty[] = {61.735,261.625,261.625,293.664,329.627};
    whichIconShowed = 0;
    //Clearing all displays
    displayClear();
    for (int i = 0; i < 7; i++){
      const word* nazev[] = {HolTyc1,HolTyc2,HolTyc3,HolTyc4,HolTyc5,HolTyc6,HolTyc7};
      displayImage(nazev[i]);
      if ((i>=1 and i <=6) and soundState==true) {
        tone(soundPin, noty[i-1]);
      }
      delay(100);
      noTone(soundPin);
    }
    ActiveButton();
    DrawMenu();
  } else if (go==true) {
    DrawMenu();
  }
  IsButtonPressed();
  if (whichIconShowed!=0) {
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
      whichIconShowed += (abs(x) + 1) * abs(x);
      whichIconShowed += (abs(y)) * abs(y);
    } else if (whichIconShowed==2) {
      whichIconShowed += (abs(x) + 1) * abs(x);
      whichIconShowed -= (abs(y)) * abs(y);
    } else if (whichIconShowed==3) {
      whichIconShowed -= (abs(x) + 1) * abs(x);
      whichIconShowed += (abs(y)) * abs(y);
    } else if (whichIconShowed==4) {
      whichIconShowed -= (abs(x) + 1) * abs(x);
      whichIconShowed -= (abs(y)) * abs(y);
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
      gameState = 5;
    } else if (whichIconShowed==4) {
      //Showing menu text D - Dance Man
      SettingGameDanceMan();
      gameState = 3;
    }
  }
  delay(10);
}

//GameOver function
void GameOver(int score=0, int game = -1) {
  //Changing gameState for gameOver state
  noTone(soundPin);
  soundDelay = 0;
  gameState = 1;
  menuFirstWalkThrough = false;
  //Clearing all displays
  displayClear();
  if (game==1 or game==2 or game==4 or game==-1) {
    //Showing text Game over
    displayImage(GameOverMessage);
  } else if (game==3) {
    displayImage(Win);
  }
  //Snake
  if (game == 1) {
    if (EEPROM.read(addressSnake1) <= score) {
      EEPROM.write(addressSnake2,EEPROM.read(addressSnake1));
      EEPROM.write(addressSnake1,score);
    } else if (EEPROM.read(addressSnake2) < score) {
      EEPROM.write(addressSnake2,score);
    }
  } else if (game == 3) {
    if (read2EEPROM(addressDanceMan1) <= score) {
      write2EEPROM(addressDanceMan2,read2EEPROM(addressDanceMan2));
      write2EEPROM(addressDanceMan1,score);
    } else if (read2EEPROM(addressDanceMan2) < score) {
      write2EEPROM(addressDanceMan2,score);
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
  displayClear();
  //Showing text score
  if (game!=-1) {
    displayImage(Score);
    displayNumbers(score);
    ActiveButton();
  }
  goThrough=true;
  gameState=0;
}
//Main function for game Snake
void Snake() {
  //Local variables Snake
  allTime = millis();
  //Movement: left....x = 1, right....x = -1, forward....y = 1, down....y = -1. 
  if ((x==0 and y==0) and startingLedOn==true) {
    displayLed(pY,pX,1);
    //Serial.print()
  } else if (startingLedOn==true) {
    displayLed(pY,pX,0);
    startingLedOn=false;
  }
  if (activatorV == 1 and (x!=0 or y!=0)) {
    activatorV=0;
    lastTime = allTime;
  }
  if (active==-1) {
    goThrough=true;
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
  if (((bodyLength-3) >= EEPROM.read(addressSnake1) and congratsState==true) and congrats <= 3) {
    PlaySound(9,congrats);
    congratsState=false;
  }
  if (activatorV == 0) {
    // Smart blinking food led 
    displayLed(foodY,foodX,millis() % 1000 < 500 ? 1 : 0);
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
                displayLed(row,col,0);
              //If pixel is the biggest turn pixel on
              } else if (Array[row][col]==bodyLength) {
                displayLed(row,col,1);
              }
            }
          }
        }
        if (soundDelay == 0 and (sX !=0 or sY !=0)) {
            PlaySound(1,0);
          }
        //If snake eated the food
        if (Array[pY][pX] == Array[foodY][foodX] && Array[pY][pX]!=0) {
  //        Serial.println("snez");
          noTone(soundPin);
          PlaySound(2,0);
          bodyLength += 1;
          delayTime -= 0.5;
          foodX=-1;
          foodY=-1;
          GenerateFood();
          displayOled("Your Score: " + String(bodyLength-2) + "\nHigh Score: " + String(EEPROM.read(addressSnake1)));
        }
        //Resetting timer
        lastTime = allTime;
      }
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
    displayLed(15,0,0);
  } else if (activatorV==0) {
    for (int row = 0; row < 3; row++) {
      for (int col = 0; col < 2; col++) {
        displayLed(row+pY,col,pgm_read_byte(&(ship[row][col]))); 
      }
    }
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
        PlaySound(4,0);
        ShootG(pY+1);
      }
      // Cele vykreslovani hry
      for (int row = 0; row < 16; row++) {
        for (int col = 0; col < 16; col++) {
          if (Array[row][col] > 0) {
            Array[row][col]--;
            if (Array[row][col]==0) {
              displayLed(row,col,0);
            } else if (Array[row][col]==1) {
              int er = 0;
              for (int i = 0; i < prucho*2; i++) {
                if (i%2==0) {
                  if (row==EnemysG[i] and col==EnemysG[i+1]) {
                    er = 1;
                    PlaySound(5,0);
                    displayLed(EnemysG[i],EnemysG[i+1],0);
                    displayLed(EnemysG[i],EnemysG[i+1]+1,0);
                    EnemysG[i]=-EnemysG[i];
                    displayLed(row,col,0);
                    if (EnemyDouble[i]>1 and EnemysG[i]!=abs(EnemysG[i])) {
                      EnemysG[i]=-EnemysG[i];
                      EnemyDouble[i]-=1;
                      EnemysG[i+1]=EnemysG[i+1]-1;
                      displayLed(EnemysG[i],EnemysG[i+1]+2,0);
                      displayLed(EnemysG[i],EnemysG[i+1]+1,1);
                    } else if ((EnemysG[i]!=17 and EnemysG[i+1]!=17) and EnemysG[i]!=abs(EnemysG[i])) {
                      wave+=1;
                      points+=1;
                      displayOled("Your Score: " + String(points) + "\nHigh Score: " + String(EEPROM.read(addressGalaxian1)));
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
                displayLed(row,col,1);
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
          if (EnemysG[i]!=17 and EnemysG[i+1]!=17) {
            displayLed(EnemysG[i],EnemysG[i+1]+1,0);
            if (EnemyDouble[i]>1) {
              for (int j = 1; j < EnemyDouble[i]; j++) {
                displayLed(EnemysG[i],EnemysG[i+1]-j,1);
              }
            }
            displayLed(EnemysG[i],EnemysG[i+1],1);
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
int playSongDanceMan(String nameOfSong) {
  // Pisnicka z project hubu https://create.arduino.cc/projecthub/GeneralSpud/passive-buzzer-song-take-on-me-by-a-ha-0f04a8
  if ((nameOfSong=="AmongUs" or nameOfSong=="Among" or nameOfSong=="among" or nameOfSong=="Amongus") and soundDelay==0 and noteCounter<=36) {
    soundLastTime = millis();
    //Serial.println(noteCounter);
    int notes [37] =  {1046,1244,1400,1510,1400,1244,1046,0,  932,1174,1046,0,  780, 525, 0,   1046,1244,1400,1510,1400,1244,1400,0,  1510,1400,1244,1510,1400,1244,1510,1400,1244,1510,1400,1244,1510,0};
    int timing [37] = {250, 250, 250, 250, 250, 250, 250, 500,125,125, 250, 500,250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 500,125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 500};
    if (notes[noteCounter]==0) {
      noTone(soundPin);
    } else {
      tone(soundPin,notes[noteCounter]);
    }
    for (int i = 0; i < noteCounter+1; i++) {
      if (notes[i]==0) {
        soundDelay++;
      } 
      int index = soundDelay+noteCounter;
      if (soundDelay!=0 and i == noteCounter) {
        soundDelay = timing[i];
      } else if (i==noteCounter) {
        soundDelay = timing[i];
      }
    }
    noteCounter++;
    int ran = random(0,2);
    if (noteCounter==36) {
      noteCounter=0;
      wave=2;
      active==0;
      displayClear();
      displayImage(NewSong);
      return 2;
    }
    if (ran==0 and sX==0) {
      return 0;
    } else {
      return prucho;
    }
  } else if (nameOfSong=="Song1" and soundDelay==0 and noteCounter<=42) {
    int noty[42] = {147,247,220,196,165,330,294,262,156,233,220,196,139,233,220,196,147,247,220,196,165,330,294,262,156,233,220,196,139,233,220,196};
    tone(soundPin,noty[noteCounter]);
    soundDelay = 200;
    noteCounter++;
    int ran = random(0,2);
    if (noteCounter==42) {
      noteCounter=0;
      GameOver(points,3);
      return 2;
    }
    if (ran==0 and sX==0) {
      return 0;
    } else {
      return prucho;
    }
  } else {
    return prucho;
  }
}
void DanceMan() {
  allTime = millis();
  if (active==1 and activatorV==0) {
    activatorV=1;
    wave=1; // Random vybere pisnicku
  } else if (active==-1) {
    GameOver();
    Serial.println("Utekl jsem pres B");
  }
  if (activatorV==1) {
    if (prucho==0) {
      sY=random(0,4);
      const word* nazev[] = {ArrowUp,ArrowRight,ArrowDown,ArrowLeft};
      displayImage(nazev[sY]);
      sX=170;
      prucho=-1;
      x=0;
      y=0;
    } else if (prucho==-1 and sX<=160) {
      bool hit = false;
      if (x==1 and sY==3) {
        hit = true;
      } else if (x==-1 and sY==1) {
        hit = true;
      } else if (y==1 and sY==0) {
        hit = true;
      } else if (y==-1 and sY==2) {
        hit = true;
      } else if (x==1 or x==-1 or y==1 or y==-1) {
        noTone(soundPin);
        Serial.println(sY);
        Serial.println("Utekl jsem pres tlacitka");
        GameOver();
        /*
        prucho = random(10,80);
        sX=0;
        points-=200;
        displayClear();
        displayOled("Bad arrow: " + String(-200) + "\nAll Points: " + String(points));
        */
      }
      if (hit==true) {
        displayClear();
        if (sX<=170 and sX>110) {
          points+=20;
          displayOled("Perfect: " + String(20) + "\nAll Points: " + String(points));
        } else if (sX<=110 and sX>50) {
          points+=10;
          displayOled("Good: " + String(10) + "\nAll Points: " + String(points));
        } else if (sX<=50) {
          points+=0;
          displayOled("Ok: " + String(0) + "\nAll Points: " + String(points));
        }
        x=0;
        y=0;
        sX=0;
        prucho=1;
      }
    }
    if (allTime-lastTime>=10 and sX!=1 and sX!=0) {
      lastTime=millis();
      sX--;
    }
    if (wave==1 and prucho!=2) {
      prucho = playSongDanceMan("AmongUs");
    } else if (wave==2 and prucho!=2) {
      prucho = playSongDanceMan("Song1");
    } else if (prucho==2 and active==1) {
      prucho=0;
      active=0;
    }
  } else {
    displayImage(PushA);
  }
}
void InfoFun() {
  const word* nazev[] = {Low,Medium,High};
  if (intensity==13) {
    displayImage(nazev[2]);
  } else if (intensity==5) {
    displayImage(nazev[1]);
  } else if (intensity==0) {
    displayImage(nazev[0]);
  } 
  IsButtonPressed();
  if (active==-1) {
    gameState=0;
    DrawMenu();
  }
  if (x==-1 or y==1) {
    if (intensity==13) {
      intensity=0;
    } else if (intensity==5) {
      intensity=13;
    } else if (intensity==0) {
      intensity=5;
    } 
  } else if (x==1 or y==-1) {
    if (intensity==13) {
      intensity=5;
    } else if (intensity==5) {
      intensity=0;
    } else if (intensity==0) {
      intensity=13;
    } 
  }
  EEPROM.write(addressIntensity, intensity);
  for (int index = 0; index < 4; index++) {
    lc.setIntensity(index, EEPROM.read(addressIntensity));
  }
}
void NastaveniHer() {
  if (activatorV == 0) {
    if (soundState==true) {
      displayImage(Sound_on, 6, 6, 3, -1, -1);
    } else if (soundState==false) {
      displayImage(Sound_off, 6, 6, 3, -1, -1);
    }
    IsButtonPressed();
    if (active==-1) {
      goThrough=true;
      gameState=0;
    }
    if (whichIconShowed!=0) {
      if (whichIconShowed==1) {
      whichIconShowed += (abs(x) + 1) * abs(x);
      //whichIconShowed += (abs(y)) * abs(y);
    } else if (whichIconShowed==3) {
      whichIconShowed -= (abs(x) + 1) * abs(x);
      whichIconShowed += (abs(y)) * abs(y);
    } else if (whichIconShowed==4) {
      //whichIconShowed -= (abs(x) + 1) * abs(x);
      whichIconShowed -= (abs(y)) * abs(y);
    }
    } else if (whichIconShowed==0 and active==0) {
      whichIconShowed=1;
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
        displayClear();
      }
      if (whichIconShowed==3) {
          gameState=4;
      } else if (whichIconShowed==1) {
          activatorV = 1;
          whichIconShowed = 1;
          displayImage(SnakeMenu,4,4,1,-2,-2);
          displayImage(Galaxian,4,4,0,-2,-2);
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
  } else if (activatorV==1) {
    displayImage(Gold_medal,4,4,3,-2,-2);
    displayImage(Silver_medal,4,4,2,-2,-2);
    if (active==-1) {
      SettingSetting();
    }
    if (whichIconShowed==1 and whichMedal==0) {
      whichIconShowed += abs(y) * -1;
    } else if (whichIconShowed==0 and whichMedal==0) {
      whichIconShowed += abs(y);
    }
    if (whichIconShowed==0) {
      if (whichMedal==0) {
        ShowBorders(0); 
      }
      if (x==-1) {
        whichMedal = 3;
      } else if (x==1) {
        whichMedal = 0;
        ShowBorders(0);
        displayOled();
      }
      if (whichMedal==3) {
        whichMedal += abs(y) * -1;
      } else if (whichMedal==2) {
        whichMedal += abs(y);
      }
      if (whichMedal==3) {
        ShowBorders(3);
        displayOled("Score: " + String(EEPROM.read(addressGalaxian1)));
        
      } else if (whichMedal==2) {
        ShowBorders(2);
        displayOled("Score: " + String(EEPROM.read(addressGalaxian2)));
      }
      /*
      for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
          ShowText(row,col + 8,pgm_read_byte(&(Silver_medal[row][col])));
        }
      }
      if (activatorV==1) {
        ShowNumbers(EEPROM.read(addressSnake1));
      }
      */
    } else if (whichIconShowed==1) {
      if (whichMedal==0) {
        ShowBorders(1);
      }
      if (x==-1) {
        whichMedal = 3;
      } else if (x==1) {
        whichMedal = 0;
        ShowBorders(1);
        displayOled();
      }
      if (whichMedal==3) {
        whichMedal += abs(y) * -1;
      } else if (whichMedal==2) {
        whichMedal += abs(y);
      }
      if (whichMedal==3) {
        ShowBorders(3);
        displayOled("Score: " + String(EEPROM.read(addressSnake1)));
        
      } else if (whichMedal==2) {
        ShowBorders(2);
        displayOled("Score: " + String(EEPROM.read(addressSnake2)));
      }
    }
    IsButtonPressed();
  }
  delay(10);
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
    DanceMan();
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
    if (soundDelay == 9 and noteCounter==0) {
      tone (soundPin, 97);
      soundDelay = 7;
    } else if (soundDelay == 200 and noteCounter==0) {
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
