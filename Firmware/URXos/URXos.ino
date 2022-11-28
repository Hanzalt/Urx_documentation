#include <Urx.h>
U8G2_SH1106_128X32_VISIONOX_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
LedControl lc = LedControl(11, 12, 10, 4);
/*Structure
  SETUP FAZE --------------------------------------------------------------------------- 
  void Setup() - Runs on starting sketch
  
  INPUT FAZE ---------------------------------------------------------------------------
  void blink() - Function for getting inputs from buttons
  void IsButtonPressed() - Function for stopping Urx until someone press any button
  void ActiveButton() - Function for stopping Urx until someone press A button
  
  HELPING FAZE ------------------------------------------------------------------------- 
  Displaying Part ---------
  void ShowBorders(int display) - Function for showing borders on one display
  void displayImage(const word* image, int displaySizeX = 8, int displaySizeY = 0, int display = 0, int offsetX = 0, int offsetY = 0) - Function for displaying images on display
  void displayLed(int row, int col, int state) - Function for turning led on exact X and Y cordinations
  void displayOled(String writing = "") - Function for displaying text on OLED display 
  void displayClear() - Function for clearing big display
  
  Snake Part ---------
  void GenerateFood() - Snake function for generating food
  void SettingGameSnake() - Setting starting conditions for Snake like length and speed of snake
  void Warp() - Take care for warping through walls
  
  Galaxian Part ---------
  void ShootG(int canon) - Function for shooting in Galaxian
  void EnemyG(int passage) - Function for spawning enemys in Galaxian
  void SettingGameGalaxian() - Function for setting Galaxian
  
  DanceMan Part ---------
  void SettingGameDanceMan() - Setting starting conditions for DanceMan like speed song etc.
  
  Generic Part ---------
  void PlaySound(int sound, int i = 0) - Function for playing sound
  void write2EEPROM(int address, int number) - Function for writing 2byte int to EEPROM
  int read2EEPROM(int address) - Function for reading EEPROM from 2byte space
  void DrawMenu() - Function for drawing app menu
  void SettingSetting() - Function for setting setting
  
  GAME FAZE ----------------------------------------------------------------------------
  void Menu() - Function that takes care of menu
  void GameOver() - Function for managing gamestate gameover
  void SnakeMenu() - Function that takes care for snake menu that has settings
  void Snake() - Main function for game Snake
  
  LOOP FAZE ----------------------------------------------------------------------------
  void Loop() - Runs forever
*/

/****************************************************************/
//Snake
/****************************************************************/

//Version
String versionUrxOS = "0.8.0";
//Food position
int foodX = 0;
int foodY = 0;
//Super food position
int foodXSuper = 0;
int foodYSuper = 0;
//Delay in game is overrided when function SettingGameSnake is called
float delayTime = 300;
//GameState for setting menu - 0, gameover - 1, game - snake - 2, game - danceMan = 3, intensity - 4, difficulty - 41, settings - 5, galaxian - 6
int gameState = 0;
//Variables that is used in showing borders to apps
int whichIconShowed = 0;
int whichMedal = 0;
//Time variables
unsigned long allTime = 0;
unsigned long lastTime = 0;
unsigned long lastTime2 = 0;
unsigned long soundTime = 0;
unsigned long soundLastTime = 0;
unsigned long lastDebounceTime = 0;
const int debounceDelay = 100;
int soundDelay = 0;
int soundDelay1 = 0;
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
//Variables for turning one row ON and OFF
const int allLedTurnOn = 0b11111111;
const int allLedTurnOff = 0b00000000;
//EEPROM addresses
const int addressSound = 1;
const int addressIntensity = 2;
const int addressDifficulty = 3;
const int addressSnake1 = 4;
const int addressSnake2 = 6;
const int addressGalaxian1 = 8;
const int addressGalaxian2 = 10;
//Movement variables
volatile int x = 0;
volatile int y = 0;
volatile int active = 0;
//Helping movement variables
int shiftedX = 7;
int shiftedY = 7;
volatile int allwaysX = 0;
volatile int allwaysY = 0;
//Variable for length of snake
int bodyLength = 3;
//Timer variable
int timer = 0;
//Variable for moving enemy
int moveEnemy1 = 0;
//Variable for wave system
int wave = 0;
//Variable to set modes
int mode = 0;
//Variable that is used to count what note to play
int noteCounter = 0;
//Variable use to store points
int points = 6;
//Variable is for every use. If you need to have some helping variable then this is it.
int helpingVariable = 0;
//Variable for setting difficulty
int difficulty = 0;
//Variable for setting intensity
int intensity = 0;
//Variables for turning congrats song
byte congrats = 0;
bool congratsState = true;
//Variable for knowing if game is ON
bool isGameOn = 0;
//Variable that is used in sound making
bool soundState = true;
//Variable to turn on
bool menuFirstWalkThrough = true;
//Variable to draw apps
bool goThrough = true;
//Variable to know if you need to show staring led
bool startingLedOn = true;
bool load = true;
bool lastState = true;
const int melodyGalaxian[] PROGMEM = {


  // Bloody Tears, from Castlevania II
  // Arranged by Bobby Lee. THe flute part was used
  // https://musescore.com/user/263171/scores/883296

  //B-flat major Bb Eb
  REST, 4, NOTE_G5, 4,
  NOTE_A5, 4, NOTE_AS5, 4,
  NOTE_A5, 4, NOTE_F5, 4,
  NOTE_A5, 4, NOTE_G5, 4,
  REST, 4, NOTE_G5, 4,
  NOTE_A5, 4, NOTE_AS5, 4,
  NOTE_C6, 4, NOTE_AS5, 4,

  NOTE_A5, 4, NOTE_G5, 4, //8
  REST, 4, NOTE_G5, 4,
  NOTE_A5, 4, NOTE_AS5, 4,
  NOTE_A5, 4, NOTE_F5, 4,
  NOTE_A5, 4, NOTE_G5, 4,
  NOTE_D6, 4, REST, 8, NOTE_C6, 8,
  REST, 4, NOTE_AS5, 4,

  NOTE_A5, 4, NOTE_AS5, 8, NOTE_C6, 8, //15
  NOTE_F6, 8, REST, 8, REST, 4,
  NOTE_G5, 16, NOTE_D5, 16, NOTE_D6, 16, NOTE_D5, 16, NOTE_C6, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16,
  NOTE_A5, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, NOTE_A5, 16, NOTE_D5, 16, NOTE_G5, 16, NOTE_D5, 16,
  NOTE_A5, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, NOTE_C6, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16,

  NOTE_A5, 16, NOTE_D5, 16, NOTE_F5, 16, NOTE_D5, 16, NOTE_A5, 16, NOTE_D5, 16, NOTE_G5, 16, NOTE_D5, 16, //20
  NOTE_G5, 16, NOTE_D5, 16, NOTE_D6, 16, NOTE_D5, 16, NOTE_C6, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16,
  NOTE_A5, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, NOTE_A5, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16,
  NOTE_A5, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, NOTE_C6, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16,
  NOTE_A5, 16, NOTE_D5, 16, NOTE_F5, 16, NOTE_D5, 16, NOTE_A5, 16, NOTE_D5, 16, NOTE_G5, 16, NOTE_D5, 16,

  NOTE_G5, 16, NOTE_D5, 16, NOTE_D6, 16, NOTE_D5, 16, NOTE_C6, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, //25
  NOTE_A5, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, NOTE_A5, 16, NOTE_D5, 16, NOTE_G5, 16, NOTE_D5, 16,
  NOTE_A5, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, NOTE_C6, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16,
  NOTE_A5, 16, NOTE_D5, 16, NOTE_F5, 16, NOTE_D5, 16, NOTE_A5, 16, NOTE_D5, 16, NOTE_G5, 16, NOTE_D5, 16,
  NOTE_AS5, 16, NOTE_D5, 16, NOTE_D6, 16, NOTE_D5, 16, NOTE_C6, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16,

  NOTE_A5, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, NOTE_A5, 16, NOTE_D5, 16, NOTE_G5, 16, NOTE_D5, 16,
  NOTE_A5, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, NOTE_C6, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16,
  NOTE_A5, 16, NOTE_D5, 16, NOTE_F5, 16, NOTE_D5, 16, NOTE_A5, 16, NOTE_D5, 16, NOTE_G5, 16, NOTE_D5, 16,
  NOTE_C6, 16, NOTE_C6, 16, NOTE_F6, 16, NOTE_D6, 8, REST, 16, REST, 8,
  REST, 4, NOTE_C6, 16, NOTE_AS5, 16,

  NOTE_C6, -8,  NOTE_F6, -8, NOTE_D6, -4, //35
  NOTE_C6, 8, NOTE_AS5, 8,
  NOTE_C6, 8, NOTE_F6, 16, NOTE_D6, 8, REST, 16, REST, 8,
  REST, 4, NOTE_C6, 8, NOTE_D6, 8,
  NOTE_DS6, -8, NOTE_F6, -8,

  NOTE_D6, -8, REST, 16, NOTE_DS6, 8, REST, 8, //40
  NOTE_C6, 8, NOTE_F6, 16, NOTE_D6, 8, REST, 16, REST, 8,
  REST, 4, NOTE_C6, 8, NOTE_AS5, 8,
  NOTE_C6, -8,  NOTE_F6, -8, NOTE_D6, -4,
  NOTE_C6, 8, NOTE_AS5, 8,

  NOTE_C6, 8, NOTE_F6, 16, NOTE_D6, 8, REST, 16, REST, 8, //45
  REST, 4, NOTE_C6, 8, NOTE_D6, 8,
  NOTE_DS6, -8, NOTE_F6, -8,
  NOTE_D5, 8, NOTE_FS5, 8, NOTE_F5, 8, NOTE_A5, 8,
  NOTE_A5, -8, NOTE_G5, -4,

  NOTE_A5, -8, NOTE_G5, -4, //50
  NOTE_A5, -8, NOTE_G5, -4,
  NOTE_AS5, 8, NOTE_A5, 8, NOTE_G5, 8, NOTE_F5, 8,
  NOTE_A5, -8, NOTE_G5, -8, NOTE_D5, 8,
  NOTE_A5, -8, NOTE_G5, -8, NOTE_D5, 8,
  NOTE_A5, -8, NOTE_G5, -8, NOTE_D5, 8,

  NOTE_AS5, 4, NOTE_C6, 4, NOTE_A5, 4, NOTE_AS5, 4,
  NOTE_G5, 16, NOTE_D5, 16, NOTE_D6, 16, NOTE_D5, 16, NOTE_C6, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, //56 //r
  NOTE_A5, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, NOTE_A5, 16, NOTE_D5, 16, NOTE_G5, 16, NOTE_D5, 16,
  NOTE_A5, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, NOTE_C6, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16,
  NOTE_A5, 16, NOTE_D5, 16, NOTE_F5, 16, NOTE_D5, 16, NOTE_A5, 16, NOTE_D5, 16, NOTE_G5, 16, NOTE_D5, 16,

  NOTE_G5, 16, NOTE_D5, 16, NOTE_D6, 16, NOTE_D5, 16, NOTE_C6, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, //61
  NOTE_A5, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, NOTE_A5, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16,
  NOTE_A5, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, NOTE_C6, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16,
  NOTE_A5, 16, NOTE_D5, 16, NOTE_F5, 16, NOTE_D5, 16, NOTE_A5, 16, NOTE_D5, 16, NOTE_G5, 16, NOTE_D5, 16,
  NOTE_G5, 16, NOTE_D5, 16, NOTE_D6, 16, NOTE_D5, 16, NOTE_C6, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16,

  NOTE_A5, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, NOTE_A5, 16, NOTE_D5, 16, NOTE_G5, 16, NOTE_D5, 16, //66
  NOTE_A5, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, NOTE_C6, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16,
  NOTE_A5, 16, NOTE_D5, 16, NOTE_F5, 16, NOTE_D5, 16, NOTE_A5, 16, NOTE_D5, 16, NOTE_G5, 16, NOTE_D5, 16,
  NOTE_AS5, 16, NOTE_D5, 16, NOTE_D6, 16, NOTE_D5, 16, NOTE_C6, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16,
  NOTE_A5, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, NOTE_A5, 16, NOTE_D5, 16, NOTE_G5, 16, NOTE_D5, 16,

  NOTE_A5, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, NOTE_C6, 16, NOTE_D5, 16, NOTE_AS5, 16, NOTE_D5, 16, //71 //
  NOTE_A5, 16, NOTE_D5, 16, NOTE_F5, 16, NOTE_D5, 16, NOTE_A5, 8, NOTE_G5, 32, NOTE_A5, 32, NOTE_AS5, 32, NOTE_C6, 32,
  NOTE_D6, 16, NOTE_G5, 16, NOTE_AS5, 16, NOTE_G5, 16, NOTE_C6, 16, NOTE_G5, 16, NOTE_D6, 16, NOTE_G5, 16,
  NOTE_C6, 16, NOTE_G5, 16, NOTE_A5, 16, NOTE_G5, 16, NOTE_F6, 16, NOTE_G5, 16, NOTE_D6, 16, NOTE_DS5, 16,
  NOTE_D6, 4, REST, 4,

  NOTE_C5, 8, REST, 8, NOTE_A4, -16, NOTE_AS4, -16, NOTE_C5, 16, //76
  NOTE_D6, 16, NOTE_G4, 16, NOTE_AS4, 16, NOTE_G4, 16, NOTE_C5, 16, NOTE_G4, 16, NOTE_D6, 16, NOTE_G4, 16,
  NOTE_C6, 16, NOTE_F4, 16, NOTE_A4, 16, NOTE_F4, 16, NOTE_F5, 16, NOTE_F4, 16, NOTE_D6, 16, NOTE_DS4, 16,
  NOTE_D6, 16, REST, 8, NOTE_E4, 16, NOTE_F4, 16,

  //change of key B Major A# C# D# F# G#
  NOTE_GS4, 8, REST, 8, NOTE_AS4, 8, REST, 8,

  NOTE_DS5, 16, NOTE_GS4, 16, NOTE_B4, 16, NOTE_GS4, 16, NOTE_CS5, 16, NOTE_GS4, 16, NOTE_DS5, 16, NOTE_GS4, 16, //81
  NOTE_CS5, 16, NOTE_FS4, 16, NOTE_AS4, 16, NOTE_FS4, 16, NOTE_FS5, 16, NOTE_FS4, 16, NOTE_DS5, 16, NOTE_E5, 16,
  NOTE_D5, 4, REST, 4,
  NOTE_CS5, 8, REST, 8, NOTE_AS4, -16,  NOTE_B4, -16, NOTE_CS5, 16,
  NOTE_DS5, 16, NOTE_GS4, 16, NOTE_B4, 16, NOTE_GS4, 16, NOTE_CS5, 16, NOTE_GS4, 16, NOTE_DS5, 16, NOTE_GS4, 16,

  NOTE_CS5, 16, NOTE_FS4, 16, NOTE_AS4, 16, NOTE_FS4, 16, NOTE_FS5, 16, NOTE_FS4, 16, NOTE_DS5, 16, NOTE_E5, 16,
  NOTE_DS5, 4, REST, 8, NOTE_DS5, 16,  NOTE_E5, 16,
  NOTE_FS5, 16, NOTE_CS5, 16, NOTE_E5, 16, NOTE_CS4, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_G5, 16, NOTE_AS5, 16,
  NOTE_GS5, 16, NOTE_DS5, 16, NOTE_DS6, 16, NOTE_DS5, 16, NOTE_CS6, 16, NOTE_DS5, 16, NOTE_B5, 16, NOTE_DS5, 16,

  NOTE_AS5, 16, NOTE_DS5, 16, NOTE_B5, 16, NOTE_DS5, 16, NOTE_AS5, 16, NOTE_DS5, 16, NOTE_GS5, 16, NOTE_DS5, 16, //90
  NOTE_AS5, 16, NOTE_DS5, 16, NOTE_B5, 16, NOTE_DS5, 16, NOTE_CS6, 16, NOTE_DS5, 16, NOTE_B5, 16, NOTE_DS5, 16,
  NOTE_AS5, 16, NOTE_DS5, 16, NOTE_FS5, 16, NOTE_DS5, 16, NOTE_AS5, 16, NOTE_DS5, 16, NOTE_GS5, 16, NOTE_DS5, 16,
  NOTE_GS5, 16, NOTE_DS5, 16, NOTE_DS6, 16, NOTE_DS5, 16, NOTE_CS6, 16, NOTE_DS5, 16, NOTE_B5, 16, NOTE_DS5, 16,

  NOTE_AS5, 16, NOTE_DS5, 16, NOTE_B5, 16, NOTE_DS5, 16, NOTE_AS5, 16, NOTE_DS5, 16, NOTE_GS5, 16, NOTE_DS5, 16,//94
  NOTE_AS5, 16, NOTE_DS5, 16, NOTE_B5, 16, NOTE_DS5, 16, NOTE_CS6, 16, NOTE_DS5, 16, NOTE_B5, 16, NOTE_DS5, 16,
  NOTE_AS5, 16, NOTE_DS5, 16, NOTE_FS5, 16, NOTE_DS5, 16, NOTE_AS5, 16, NOTE_DS5, 16, NOTE_GS5, 16, NOTE_DS5, 16,
  NOTE_GS5, 16, NOTE_DS5, 16, NOTE_DS6, 16, NOTE_DS5, 16, NOTE_CS6, 16, NOTE_DS5, 16, NOTE_B5, 16, NOTE_DS5, 16,

  NOTE_AS5, 16, NOTE_DS5, 16, NOTE_B5, 16, NOTE_DS5, 16, NOTE_AS5, 16, NOTE_DS5, 16, NOTE_GS5, 16, NOTE_DS5, 16,//98
  NOTE_AS5, 16, NOTE_DS5, 16, NOTE_B5, 16, NOTE_DS5, 16, NOTE_CS6, 16, NOTE_DS5, 16, NOTE_B5, 16, NOTE_DS5, 16,
  NOTE_AS5, 16, NOTE_DS5, 16, NOTE_FS5, 16, NOTE_DS5, 16, NOTE_AS5, 16, NOTE_DS5, 16, NOTE_GS5, 16, NOTE_DS5, 16,
  NOTE_GS5, 16, NOTE_DS5, 16, NOTE_DS6, 16, NOTE_DS5, 16, NOTE_CS6, 16, NOTE_DS5, 16, NOTE_B5, 16, NOTE_DS5, 16,

  NOTE_AS5, 16, NOTE_DS5, 16, NOTE_B5, 16, NOTE_DS5, 16, NOTE_AS5, 16, NOTE_DS5, 16, NOTE_GS5, 16, NOTE_DS5, 16,//102
  NOTE_AS5, 16, NOTE_DS5, 16, NOTE_B5, 16, NOTE_DS5, 16, NOTE_CS6, 16, NOTE_DS5, 16, NOTE_B5, 16, NOTE_DS5, 16,
  NOTE_AS5, 16, NOTE_DS5, 16, NOTE_FS5, 16, NOTE_DS5, 16, NOTE_AS5, 16, NOTE_DS5, 16, NOTE_GS5, 16, NOTE_DS5, 16,

  NOTE_CS6, 8, NOTE_FS6, 16, NOTE_DS6, 8, REST, 16, REST, 8, //107
  REST, 4, NOTE_CS6, 8, NOTE_B5, 8,
  NOTE_CS6, -8, NOTE_FS6, -8, NOTE_DS6, -4,
  NOTE_CS6, 8, NOTE_B5, 8,
  NOTE_CS6, 8, NOTE_FS6, 16, NOTE_DS6, 8, REST, 16, REST, 8,
  REST, 4, NOTE_CS6, 8, NOTE_B5, 8,
  NOTE_E6, -8, NOTE_F6, -8,

  NOTE_DS6, -8, REST, 16, NOTE_E6, 8, REST, 16, REST, 16, //112
  NOTE_CS6, 8, NOTE_FS6, 16, NOTE_DS6, 8, REST, 16, REST, 8,
  REST, 4, NOTE_CS6, 8, NOTE_B5, 8,
  NOTE_CS6, -8, NOTE_FS6, -8, NOTE_DS6, -4,
  NOTE_CS6, 8, NOTE_B5, 8,

  NOTE_CS6, 8, NOTE_FS6, 16, NOTE_DS6, 8, REST, 16, REST, 8, //117
  REST, 4, NOTE_CS5, 8, NOTE_DS5, 8,
  NOTE_E5, -8, NOTE_F5, -8,
  NOTE_DS5, 8, NOTE_G5, 8, NOTE_GS5, 8, NOTE_AS5, 8,
  NOTE_AS5, -8, NOTE_GS5, -8,

  NOTE_AS5, -8, NOTE_GS5, -8,//122
  NOTE_AS5, -8, NOTE_GS5, -8,
  NOTE_B6, 8, NOTE_AS5, 8, NOTE_GS5, 8, NOTE_FS5, 8,
  NOTE_AS5, -8, NOTE_GS6, -8, NOTE_DS5, 8,
  NOTE_AS5, -8, NOTE_GS6, -8, NOTE_DS5, 8,
  NOTE_AS5, -8, NOTE_GS6, -8, NOTE_DS5, 8,

  NOTE_B5, 8, NOTE_CS6, 8, NOTE_AS5, 8, NOTE_B5, 8, //128
  NOTE_GS5, 8, REST, 8, REST, 16

};
const int melodyCat[] PROGMEM = {

  // Keyboard cat
  // Score available at https://musescore.com/user/142788/scores/147371

  NOTE_C4, 4, NOTE_E4, 4, NOTE_G4, 4, NOTE_E4, 4,
  NOTE_C4, 4, NOTE_E4, 8, NOTE_G4, -4, NOTE_E4, 4,
  NOTE_A3, 4, NOTE_C4, 4, NOTE_E4, 4, NOTE_C4, 4,
  NOTE_A3, 4, NOTE_C4, 8, NOTE_E4, -4, NOTE_C4, 4,
  NOTE_G3, 4, NOTE_B3, 4, NOTE_D4, 4, NOTE_B3, 4,
  NOTE_G3, 4, NOTE_B3, 8, NOTE_D4, -4, NOTE_B3, 4,

  NOTE_G3, 4, NOTE_G3, 8, NOTE_G3, -4, NOTE_G3, 8, NOTE_G3, 4,
  NOTE_G3, 4, NOTE_G3, 4, NOTE_G3, 8, NOTE_G3, 4,
  NOTE_C4, 4, NOTE_E4, 4, NOTE_G4, 4, NOTE_E4, 4,
  NOTE_C4, 4, NOTE_E4, 8, NOTE_G4, -4, NOTE_E4, 4,
  NOTE_A3, 4, NOTE_C4, 4, NOTE_E4, 4, NOTE_C4, 4,
  NOTE_A3, 4, NOTE_C4, 8, NOTE_E4, -4, NOTE_C4, 4,
  NOTE_G3, 4, NOTE_B3, 4, NOTE_D4, 4, NOTE_B3, 4,
  NOTE_G3, 4, NOTE_B3, 8, NOTE_D4, -4, NOTE_B3, 4,

  NOTE_G3, -1,

};
const int melodyDoom[] PROGMEM = {

  // At Doom's Gate (E1M1)
  // Score available at https://musescore.com/pieridot/doom

  NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //1
  NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
  NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
  NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, -2,

  NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //5
  NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
  NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
  NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, -2,

  NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //9
  NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
  NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
  NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, -2,

  NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //13
  NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
  NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
  NOTE_FS3, -16, NOTE_D3, -16, NOTE_B2, -16, NOTE_A3, -16, NOTE_FS3, -16, NOTE_B2, -16, NOTE_D3, -16, NOTE_FS3, -16, NOTE_A3, -16, NOTE_FS3, -16, NOTE_D3, -16, NOTE_B2, -16,

  NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //17
  NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
  NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
  NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, -2,

  NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //21
  NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
  NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
  NOTE_B3, -16, NOTE_G3, -16, NOTE_E3, -16, NOTE_G3, -16, NOTE_B3, -16, NOTE_E4, -16, NOTE_G3, -16, NOTE_B3, -16, NOTE_E4, -16, NOTE_B3, -16, NOTE_G4, -16, NOTE_B4, -16,

  NOTE_A2, 8, NOTE_A2, 8, NOTE_A3, 8, NOTE_A2, 8, NOTE_A2, 8, NOTE_G3, 8, NOTE_A2, 8, NOTE_A2, 8, //25
  NOTE_F3, 8, NOTE_A2, 8, NOTE_A2, 8, NOTE_DS3, 8, NOTE_A2, 8, NOTE_A2, 8, NOTE_E3, 8, NOTE_F3, 8,
  NOTE_A2, 8, NOTE_A2, 8, NOTE_A3, 8, NOTE_A2, 8, NOTE_A2, 8, NOTE_G3, 8, NOTE_A2, 8, NOTE_A2, 8,
  NOTE_F3, 8, NOTE_A2, 8, NOTE_A2, 8, NOTE_DS3, -2,

  NOTE_A2, 8, NOTE_A2, 8, NOTE_A3, 8, NOTE_A2, 8, NOTE_A2, 8, NOTE_G3, 8, NOTE_A2, 8, NOTE_A2, 8, //29
  NOTE_F3, 8, NOTE_A2, 8, NOTE_A2, 8, NOTE_DS3, 8, NOTE_A2, 8, NOTE_A2, 8, NOTE_E3, 8, NOTE_F3, 8,
  NOTE_A2, 8, NOTE_A2, 8, NOTE_A3, 8, NOTE_A2, 8, NOTE_A2, 8, NOTE_G3, 8, NOTE_A2, 8, NOTE_A2, 8,
  NOTE_A3, -16, NOTE_F3, -16, NOTE_D3, -16, NOTE_A3, -16, NOTE_F3, -16, NOTE_D3, -16, NOTE_C4, -16, NOTE_A3, -16, NOTE_F3, -16, NOTE_A3, -16, NOTE_F3, -16, NOTE_D3, -16,

  NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //33
  NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
  NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
  NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, -2,

  NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //37
  NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
  NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
  NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, -2,

  NOTE_CS3, 8, NOTE_CS3, 8, NOTE_CS4, 8, NOTE_CS3, 8, NOTE_CS3, 8, NOTE_B3, 8, NOTE_CS3, 8, NOTE_CS3, 8, //41
  NOTE_A3, 8, NOTE_CS3, 8, NOTE_CS3, 8, NOTE_G3, 8, NOTE_CS3, 8, NOTE_CS3, 8, NOTE_GS3, 8, NOTE_A3, 8,
  NOTE_B2, 8, NOTE_B2, 8, NOTE_B3, 8, NOTE_B2, 8, NOTE_B2, 8, NOTE_A3, 8, NOTE_B2, 8, NOTE_B2, 8,
  NOTE_G3, 8, NOTE_B2, 8, NOTE_B2, 8, NOTE_F3, -2,

  NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //45
  NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
  NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
  NOTE_B3, -16, NOTE_G3, -16, NOTE_E3, -16, NOTE_G3, -16, NOTE_B3, -16, NOTE_E4, -16, NOTE_G3, -16, NOTE_B3, -16, NOTE_E4, -16, NOTE_B3, -16, NOTE_G4, -16, NOTE_B4, -16,

  NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8, //49
  NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_B2, 8, NOTE_C3, 8,
  NOTE_E2, 8, NOTE_E2, 8, NOTE_E3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_D3, 8, NOTE_E2, 8, NOTE_E2, 8,
  NOTE_C3, 8, NOTE_E2, 8, NOTE_E2, 8, NOTE_AS2, -2,

};
/****************************************************************/
//Setup function
/****************************************************************/
void setup() {
  //Defining variables
  u8g2.begin();
  u8g2.setFlipMode(1);
  //u8x8.refreshDisplay();    // only required for SSD1606/7
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
  const int batteryValue = analogRead(battery);
  const float mapBattery = (batteryValue * 5.0) / 1024.0;
  if (mapBattery<2.9 and mapBattery>1) {
    u8g2.firstPage();
    do {
      const String out1 = "Your battery";
      const String out2 = "is empty";
      u8g2.setFont(u8g2_font_battery24_tr);
      u8g2.drawGlyph(100, 24, 0x30);
      u8g2.setFont(u8g2_font_helvR12_tr);
      u8g2.setCursor(0, 13);
      u8g2.print(out1);
      u8g2.setCursor(0, 28);
      u8g2.print(out2);
    } while ( u8g2.nextPage() );
    delay(999999999999999999999999999999999999);
  }

  // Erasing and inicializing all displays
  //EEPROM.write(addressIntensity,0);
  intensity = EEPROM.read(addressIntensity);
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
  if ((millis() - lastDebounceTime) > debounceDelay) {
    y = (!digitalRead(forward) * 1) | (!digitalRead(down) * -1);
    x = (!digitalRead(left) * 1) | (!digitalRead(right) * -1);
    active = (!digitalRead(a) * 1) | (!digitalRead(b) * -1);
    lastDebounceTime = millis();
  }
}

void IsButtonPressed() {
  x = 0;
  y = 0;
  active = 0;
  while (true) {
    if (x != 0 or y != 0 or active != 0) {
      break;
    }
  }
}

void ActiveButton() {
  active = 0;
  while (true) {
    if (active == 1) {
      break;
    }
  }
}
/****************************************************************/
//Helping functions
/****************************************************************/
void ShowBorders(int display) {
  for (int i = 0; i < 4; i++) {
    lc.setRow(i, 0, allLedTurnOff);
    lc.setRow(i, 7, allLedTurnOff);
    lc.setColumn(i, 7, allLedTurnOff);
    lc.setColumn(i, 0, allLedTurnOff);
  }
  lc.setRow(display, 0, allLedTurnOn);
  lc.setRow(display, 7, allLedTurnOn);
  lc.setColumn(display, 7, allLedTurnOn);
  lc.setColumn(display, 0, allLedTurnOn);
}
//This function is like TurningLedArray but has changed display orientation
void displayImage(const word* image, int displaySizeX = 8, int displaySizeY = 0, int display = 0, int offsetX = 0, int offsetY = 0) {
  int MirrorX = displaySizeX - 1;
  int MirrorY = displaySizeY - 1;
  for (int index = 0; index < 2; index++) {
    for (int rows = 0; rows < displaySizeX; rows++) {
      if (displaySizeX == 8 and displaySizeY == 0) {
        byte byte1 = (pgm_read_word(&image[(rows) + (index * displaySizeX)]) >> displaySizeX) & 0xFF;
        byte byte2 = pgm_read_word(&image[(rows) + (index * displaySizeX)]) & 0xFF;
        lc.setRow(index + 1 + index, MirrorX - rows, byte1);
        lc.setRow(index + index, MirrorX - rows, byte2);
      } else {
        for (int cols = 0; cols < displaySizeY; cols++) {
          lc.setLed(display, MirrorX - rows - offsetX, MirrorY - cols - offsetY, bitRead(pgm_read_word(&image[rows]), cols));
        }
      }
    }
  }
}
void displayLed(int row, int col, int state) {
  if (row < 8 and col < 8) {
    lc.setLed(1, 7 - col, row, state);
  } else if (row < 8 and col >= 8) {
    lc.setLed(3, 7 - col % 8, row, state);
  } else if (row >= 8 and col < 8) {
    lc.setLed(0, 7 - col, row % 8, state);
  } else if (row >= 8 and col >= 8) {
    lc.setLed(2, 7 - col % 8, row % 8, state);
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
    u8g2.setCursor(0, 28);
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
/****************************************************************/
//Snake functions
/****************************************************************/
void GenerateFood() {
  if (abs(mode) != mode and wave != -2) {
    for (int row = 0; row < 16; row++) {
      for (int col = 0; col < 16; col++) {
        //Making Snake timer longer
        if (Array[row][col] > 0 ) {
          Array[row][col]++;
        }
      }
    }
  }
  while (true) {
    for (int i = 0; i < 5; i++) {
      foodX = random(0, 16);
      foodY = random(0, 16);
    }
    if (Array[foodY][foodX] == 0) {
      break;
    }
  }
}

void SettingGameSnake() {
  allTime = millis();
  lastTime = allTime;
  delayTime = delayTimeConst - difficulty;
  bodyLength = bodyLengthConst;
  isGameOn = 0;
  startingLedOn = true;
  congrats = 0;
  shiftedX = 0;
  shiftedY = 15;
  mode = 1;
  wave = random(1, 5);
  points = 0;
  delay(100);
  x = 0;
  y = 0;
  allwaysX = 0;
  allwaysY = 0;
  foodYSuper = -1;
  foodXSuper = -1;
  active = 0;
  for (int row = 0; row < 16; row++) {
    for (int col = 0; col < 16; col++) {
      Array[row][col] = 0;
    }
  }
  displayClear();
  //displayOled("Your Score: " + String(bodyLength-3) + "\nHigh Score: " + String(EEPROM.read(addressSnake1)));
  displayOled("Snake CLASSIC\nFor start press A");
}

//Making snake go through walls
void Warp() {
  shiftedX < 0 ? shiftedX += 16 : 0;
  shiftedX > 15 ? shiftedX -= 16 : 0;
  shiftedY < 0 ? shiftedY += 16 : 0;
  shiftedY > 15 ? shiftedY -= 16 : 0;
}

/****************************************************************/
//Galaxian functions
/****************************************************************/
void ShootG(int canon) {
  for (int col = 0; col < 16; col++) {
    if (Array[canon][col] == 0) {
      Array[canon][col] = col;
    }
  }
}
int EnemyG(int passage) {
  int randomR = 0; // How many enemys - maximum
  int randomR2 = 0; // How many enemys - between randomR and 1
  int randomR3 = 0; // How many enemys in one line
  int difficultyCounter = 0;
  if (difficulty == -50) { // Easy
    difficultyCounter = -2;
  } else if (difficultyCounter == 60) {
    difficultyCounter = 2;
  }
  if (passage <= 5) {
    randomR = 4 + difficultyCounter;
    randomR3 = 2;
  } else if (passage <= 10 and passage > 5) {
    randomR = 7 + difficultyCounter;
    randomR3 = 3;
  } else if (passage >= 10) {
    randomR = 8 + difficultyCounter;
    randomR3 = 3;
  }
  randomR2 = random(1, randomR + 1);
  int EnemyPosRepetition[randomR2];
  for (int i = 0; i < randomR2 * 2; i++) {
    //noTone(soundPin);
    if (i % 2 == 0) {
      int randomPos = random(1, 15);
      while (true) {
        bool calibrateEnemysPosition = true;
        for (int x = 0; x < randomR2; x++) {
          if (EnemyPosRepetition[x] == randomPos) {
            randomPos = random(1, 15);
            calibrateEnemysPosition = false;
          }
        }
        if (calibrateEnemysPosition == true) {
          break;
        }
      }
      EnemysG[i] = randomPos;
      EnemyPosRepetition[i / 2] = EnemysG[i];
      EnemysG[i + 1] = 15;
      EnemyDouble[i] = random(1, randomR3 + 1);
    }
  }
  return randomR2;
}

void SettingGameGalaxian() {
  displayClear();
  allTime = millis();
  lastTime = allTime;
  lastTime2 = allTime;
  shiftedY = 13;
  delayTime = 750 - difficulty;
  moveEnemy1 = 0;
  noteCounter = 0;
  wave = 0;
  mode = 1;
  points = 0;
  isGameOn = 0;
  helpingVariable = 0;
  for (int i = 0; i < 100; i++) {
    EnemysG[i] = 0;
    if (i <= 50) {
      EnemyDouble[i] = 0;
    }
  }
  for (int row = 0; row < 16; row++) {
    for (int col = 0; col < 16; col++) {
      Array[row][col] = 0;
    }
  }
  displayOled("Galaxian song\nFor start press A");
}

/****************************************************************/
//DanceMan functions
/****************************************************************/
void SettingGameDanceMan() {
  displayClear();
  points = 0; // How many points
  //wave=0; // What song to play
  mode = 1;
  active = 0;
  noteCounter = 0; // for playSongDanceMan counting which note to play
  soundDelay = 0;
  allwaysX = 0; // Counting time when pressed
  allwaysY = 0; // Direction variable
  isGameOn = 0; // Is game on?
  lastTime = millis();
  if (helpingVariable != -2) {
    displayImage(ArrowUp);
    displayOled("Song: Among us\nFor start press A");
  } else if (helpingVariable == -2) {
    displayImage(ArrowDown);
    displayOled("Song: Uno\nFor start press A");
  }
  helpingVariable = 0; // When the arrow will apear
}
/****************************************************************/
//Generic functions
/****************************************************************/
void PlaySound(int sound, int i = 0) {
  if (soundState == true and abs(mode) != mode) {
    if (sound == 1) {
      tone(soundPin, 70);
      soundDelay = 100;
    } else if (sound == 11) {
      tone(soundPin, 150);
      soundDelay = 100;
    } else if (sound == 2) {
      tone (soundPin, 260);
      soundDelay = 250;
    } else if (sound == 3) {
      tone (soundPin, 500);
      soundDelay = 150;
    } else if (sound == 4) {
      noTone(soundPin);
      tone (soundPin, 170);
      soundDelay = 100;
    } else if (sound == 5) {
      tone (soundPin, 90);
      soundDelay = 90;
    } else if (sound == 9) {
      const float noty[5] = {130.813, 164.814, 195, 998, 329.621};
      tone(soundPin, noty[i]);
      soundDelay = 200;
    }
  }
}
void write2EEPROM(int address, int number) {
  byte byte1 = (number >> 8) & 0xFF;
  byte byte2 = number & 0xFF;

  EEPROM.write(address, byte1);
  EEPROM.write(address + 1, byte2);
  //read2EEPROM(address);
}
int read2EEPROM(int address) {
  byte byte1 = EEPROM.read(address);
  byte byte2 = EEPROM.read(address + 1);

  int result = (byte1 << 8) + byte2;
  return result;
}
void DrawMenu() {
  displayClear();
  displayImage(SnakeMenu, 4, 4, 1, -2, -2);
  displayImage(Nastaveni1, 4, 4, 2, -2, -2);
  displayImage(Galaxian, 4, 4, 0, -2, -2);
  displayImage(DancemanMenu, 4, 4, 3, -2, -2);
  whichIconShowed = 2;
  ShowBorders(1);
  active = 0;
}
void SettingSetting() {
  displayClear();
  if (soundState == true) {
    displayImage(Sound_on, 6, 6, 3, -1, -1);
  } else if (soundState == false) {
    displayImage(Sound_off, 6, 6, 3, -1, -1);
  }
  displayImage(Intensity, 4, 4, 2, -2, -2);
  displayImage(HighScore, 4, 4, 0, -2, -2);
  displayImage(DancemanMenu, 4, 4, 1, -2, -2);
  isGameOn = 0;
  active = 0;
  whichIconShowed = 2;
  ShowBorders(1);
}
/****************************************************************/
//Gamestate functions
/****************************************************************/

//Menu function
void Menu(bool firstGoThrough, bool go) {
  //When menu start show M in start
  if (firstGoThrough == true) {
    const float noty[] = {61.735, 261.625, 261.625, 293.664, 329.627};
    //Clearing all displays
    displayClear();
    byte whichIcon = 0x0;
    const int batteryValue = analogRead(battery);
    const float mapBattery = (batteryValue * 5.0) / 1024.0;
    if (mapBattery >= 3.8) {
      whichIcon = 0x34;
    } else if (mapBattery < 3.8 and mapBattery >= 3.5) {
      whichIcon = 0x33;
    } else if (mapBattery < 3.5 and mapBattery >= 3.3) {
      whichIcon = 0x32;
    } else if (mapBattery < 3.3 and mapBattery >= 3.15) {
      whichIcon = 0x31;
    } else if (mapBattery < 3.15 and mapBattery >= 2.9) {
      whichIcon = 0x30;
    } else if (mapBattery < 2.9 and mapBattery > 1) {
      whichIcon = 0;
    } else {
      whichIcon = -1;
    }
    u8g2.firstPage();
    do {
      const String out1 = String(mapBattery) + "V";
      const String out2 = "Ver. - " + versionUrxOS;
      u8g2.setFont(u8g2_font_battery24_tr);
      u8g2.drawGlyph(100, 24, whichIcon);
      u8g2.setFont(u8g2_font_helvR12_tr);
      u8g2.setCursor(0, 13);
      u8g2.print(out1);
      u8g2.setCursor(0, 28);
      u8g2.print(out2);
    } while ( u8g2.nextPage() );
    if (whichIcon == 0) {
      u8g2.firstPage();
      do {
        const String out1 = "Your battery";
        const String out2 = "is empty";
        u8g2.setFont(u8g2_font_battery24_tr);
        u8g2.drawGlyph(100, 24, 0x30);
        u8g2.setFont(u8g2_font_helvR12_tr);
        u8g2.setCursor(0, 13);
        u8g2.print(out1);
        u8g2.setCursor(0, 28);
        u8g2.print(out2);
      } while ( u8g2.nextPage() );
      displayClear();
      gameState = -1;
      delay(999999999999999999999999999999999999);
    }
    for (int i = 0; i < 7; i++) {
      const word* nazev[] = {HolTyc1, HolTyc2, HolTyc3, HolTyc4, HolTyc5, HolTyc6, HolTyc7};
      displayImage(nazev[i]);
      if ((i >= 1 and i <= 6) and soundState == true) {
        tone(soundPin, noty[i - 1]);
      }
      delay(100);
      noTone(soundPin);
    }
    int i = 0;
    active = 0;
    while (true) {
      i++;
      if (i == 100) {
        break;
      } else if (active == 1) {
        break;
      }
      delay(20);
    }
    DrawMenu();
  } else if (go == true) {
    DrawMenu();
  }
  if (points > 0) {
    displayImage(Nastaveni1, 4, 4, 2, -2, -2);
    if (points == 1) {
      points = -6;
    } else {
      points--;
    }
  } else {
    displayImage(Nastaveni2, 4, 4, 2, -2, -2);
    if (points == 0) {
      points = 6;
    } else {
      points++;
    }
  }
  /*
     Solving menu
      _____
     |     |
     | 2,4 |
     | 1,3 |
     |_____|
     I now that apps next to each other have difference 2,
     and apps under each other have difference 1.
     So with little of math that is under I could solve this problem.
  */
  if (whichIconShowed == 1) {
    displayOled("Galaxian");
    whichIconShowed += (abs(x) + 1) * abs(x);
    whichIconShowed += (abs(y)) * abs(y);
  } else if (whichIconShowed == 2) {
    displayOled("Snake");
    whichIconShowed += (abs(x) + 1) * abs(x);
    whichIconShowed -= (abs(y)) * abs(y);
  } else if (whichIconShowed == 3) {
    displayOled("Settings");
    whichIconShowed -= (abs(x) + 1) * abs(x);
    whichIconShowed += (abs(y)) * abs(y);
  } else if (whichIconShowed == 4) {
    displayOled("DanceMan");
    whichIconShowed -= (abs(x) + 1) * abs(x);
    whichIconShowed -= (abs(y)) * abs(y);
  }
  if (x != 0 or y != 0) {
    ShowBorders(whichIconShowed - 1);
  }
  x = 0;
  y = 0;
  if (active == 1 and whichIconShowed != 0) {
    active = 0;
    if (soundState == true) {
      tone(soundPin, 147);
      delay(100);
      noTone(soundPin);
    }
    if (whichIconShowed == 2) {
      //Showing menu text S - Snake
      SettingGameSnake();
      GenerateFood();
      gameState = 2;
    } else if (whichIconShowed == 1) {
      //Showing menu text G - Galaxian
      SettingGameGalaxian();
      gameState = 6;
    } else if (whichIconShowed == 3) {
      //Showing menu text N - nastaveni
      delay(250);
      SettingSetting();
      gameState = 5;
    } else if (whichIconShowed == 4) {
      //Showing menu text D - Dance Man
      SettingGameDanceMan();
      gameState = 3;
    }
  }
  delay(50);
}

//GameOver function
void GameOver(int score = 0, int game = -1) {
  //Changing gameState for gameOver state
  noTone(soundPin);
  points = 6;
  soundDelay = 0;
  gameState = 1;
  menuFirstWalkThrough = false;
  //Clearing all displays
  displayClear();
  if (game == 1 or game == 2 or game == 4 or game == -1) {
    //Showing text Game over
    displayImage(GameOverMessage);
    ActiveButton();
  } else if (game == 3 and score > 30) {
    displayImage(Win);
    ActiveButton();
  } else {
    displayImage(GameOverMessage);
    ActiveButton();
  }
  //Snake
  if (game == 1) {
    if (read2EEPROM(addressSnake1) <= score) {
      active=0;
      displayImage(Win);
      displayOled("New highscore\nYour are the best");
      while (true) {
        if (soundState==true) {
          tone(soundPin, NOTE_D6);
          delay(250);
          tone(soundPin, NOTE_G6);
          delay(250);
        }
        if (active==1) {
          break;
        }
      }
      write2EEPROM(addressSnake2, read2EEPROM(addressSnake1));
      write2EEPROM(addressSnake1, score);
    } else if (read2EEPROM(addressSnake2) < score) {
      write2EEPROM(addressSnake2, score);
    }
  } else if (game == 4) {
    if (read2EEPROM(addressGalaxian1) <= score) {
      active=0;
      displayImage(Win);
      displayOled("New highscore\nYour are the best");
      while (true) {
        if (soundState==true) {
          tone(soundPin, NOTE_D6);
          delay(250);
          tone(soundPin, NOTE_G6);
          delay(250);
        }
        if (active==1) {
          break;
        }
      }
      write2EEPROM(addressGalaxian2, read2EEPROM(addressGalaxian1));
      write2EEPROM(addressGalaxian1, score);
    } else if (read2EEPROM(addressGalaxian2) < score) {
      write2EEPROM(addressGalaxian2, score);
    }
  }
  //Clearing all displays
  displayClear();
  goThrough = true;
  gameState = 0;
}

//Main function for game Snake
void Snake() {
  //Local variables Snake
  allTime = millis();
  //Movement: left....x = 1, right....x = -1, forward....y = 1, down....y = -1.
  if ((mode == 1 or mode == 2) and active == 1) {
    noTone(soundPin);
    int mod = mode;
    SettingGameSnake();
    mode = -mod;
    displayOled("Your Score: " + String(bodyLength - 3 + points) + "\nHigh Score: " + String(read2EEPROM(addressSnake1)));
  } else if (mode == 1 and y != 0) {
    displayOled("Snake SUPER\nFor start press A");
    displayLed(foodY, foodX, 0);
    GenerateFood();
    foodXSuper = foodX;
    foodYSuper = foodY;
    GenerateFood();
    wave = -1;
    mode = 2;
    y = 0;
    delay(100);
  } else if (mode == 2 and y != 0) {
    displayOled("Snake CLASSIC\nFor start press A");
    mode = 1;
    y = 0;
    delay(100);
  }
  if ((x == 0 and y == 0) and startingLedOn == true and abs(mode) != mode) {
    displayLed(shiftedY, shiftedX, 1);
    //Serial.print()
  } else if (startingLedOn == true and abs(mode) != mode) {
    displayLed(shiftedY, shiftedX, 0);
    startingLedOn = false;
  }
  //Turning Snake ON
  if (isGameOn == 1 and (x != 0 or y != 0)) {
    isGameOn = 0;
    lastTime = allTime;
  }
  if (active == -1) {
    goThrough = true;
    isGameOn = 1;
    active = 0;
    allwaysX = 0;
    allwaysY = 0;
    x = 0;
    y = 0;
    displayOled("Your Score: " + String(bodyLength - 3 + points) + "\nHigh Score: " + String(read2EEPROM(addressSnake1)));
    GameOver(bodyLength - 3 + points, 1);
  }
  if (isGameOn == 0 and active == 1 and startingLedOn == false) {
    isGameOn = 1;
    y = 0;
    x = 0;
    noTone(soundPin);
  }
  if (((bodyLength - 4 + points) >= read2EEPROM(addressSnake1) and congratsState == true) and congrats <= 3) {
    PlaySound(9, congrats);
    congratsState = false;
  }
  if (isGameOn == 0) {
    // Smart blinking food led
    if (mode == 2 or wave == -2 and timer != 0) {
      displayLed(foodYSuper, foodXSuper, millis() % 100 < 50 ? 1 : 0);
      displayLed(foodY, foodX, millis() % 1000 < 500 ? 1 : 0);
    } else {
      displayLed(foodYSuper, foodXSuper, 0);
      displayLed(foodY, foodX, millis() % 1000 < 500 ? 1 : 0);
    }
    // This will run every delayTime
    if (allTime - lastTime >= delayTime) {
      if (abs(mode) != mode) {
        if (allwaysY != -y and y != 0) {
          allwaysY = y;
          allwaysX = 0;
        } else if (allwaysX != -x and x != 0) {
          allwaysX = x;
          allwaysY = 0;
        }
      } else {
        int y1 = random(-1, 2);
        int x1 = random(-1, 2);
        if (allwaysY != -y1 and y1 != 0) {
          allwaysY = y1;
          allwaysX = 0;
        } else if (allwaysX != -x1 and x1 != 0) {
          allwaysX = x1;
          allwaysY = 0;
        }
      }
      // Making head bigger
      shiftedY -= allwaysY;
      shiftedX -= allwaysX;
      Warp();
      if (Array[shiftedY][shiftedX] > 0) {
        displayOled("Your Score: " + String(bodyLength - 3 + points) + "\nHigh Score: " + String(read2EEPROM(addressSnake1)));
        GameOver((bodyLength - 3 + points), 1);
      }
      if (allwaysX != 0 or allwaysY != 0) {
        Array[shiftedY][shiftedX] = bodyLength + 1;
      }
      //This will run only if game snake is on - if you are not dead
      if (gameState == 2) {
        //Big for loop that run for all pixel in array
        for (int row = 0; row < 16; row++) {
          for (int col = 0; col < 16; col++) {
            // if there is a body segment, decrement it's value
            if (Array[row][col] > 0 ) {
              Array[row][col]--;
              //If This pixel is zero (I know that zero means that it is snake ending) turn pixel off
              if (Array[row][col] == 0) {
                displayLed(row, col, 0);
                //If pixel is the biggest turn pixel on
              } else if (Array[row][col] == bodyLength) {
                displayLed(row, col, 1);
              }
            }
          }
        }
        if (soundDelay == 0 and (allwaysX != 0 or allwaysY != 0)) {
          if (wave == -2) {
            PlaySound(11);
          } else {
            PlaySound(1);
          }
        }
        //If snake eated the food
        if (Array[shiftedY][shiftedX] == Array[foodY][foodX] && Array[shiftedY][shiftedX] != 0) {
          noTone(soundPin);
          PlaySound(2);
          if (abs(mode) != mode) {
            bodyLength += 1;
            delayTime -= 0.5;
          }
          foodX = -1;
          foodY = -1;
          GenerateFood();
          if (wave == 0 and mode == -2) {
            wave = -2;
            foodXSuper = foodX;
            foodYSuper = foodY;
            GenerateFood();
            timer = 20;
            displayOled("Super food: " + String(timer));
          } else if (wave != -2) {
            wave--;
          }
          if (abs(mode) != mode and wave != -2) {
            displayOled("Your Score: " + String(bodyLength - 3 + points) + "\nHigh Score: " + String(read2EEPROM(addressSnake1)));
          }
        }
        if (Array[shiftedY][shiftedX] == Array[foodYSuper][foodXSuper] && Array[shiftedY][shiftedX] != 0) {
          noTone(soundPin);
          PlaySound(3, 0);
          for (int row = 0; row < 16; row++) {
            for (int col = 0; col < 16; col++) {
              //Making Snake timer longer
              if (Array[row][col] > 0 ) {
                Array[row][col] += 2;
              }
            }
          }
          timer = 0;
          if (abs(mode) != mode) {
            bodyLength += 2;
            delayTime -= 0.5;
          }
          points++;
          foodYSuper = -1;
          foodXSuper = -1;
        }
        if (wave == -2 and timer != 0) {
          displayOled("Super food: " + String(timer + 1));
          timer--;
        } else if (wave == -2) {
          displayLed(foodYSuper, foodXSuper, 0);
          foodYSuper = -1;
          foodXSuper = -1;
          wave = random (2, 5);
          displayOled("Your Score: " + String(bodyLength - 3 + points) + "\nHigh Score: " + String(read2EEPROM(addressSnake1)));
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
    y = 0;
    active = 0;
    noTone(soundPin);
    GameOver(points, 4);
  }
  if ((mode == 1 or mode == 2) and active == 1) {
    noTone(soundPin);
    int mod = mode;
    SettingGameGalaxian();
    mode = -mod;
    displayOled("Your Score: " + String(points) + "\nHigh Score: " + String(read2EEPROM(addressGalaxian1)));
  } else if (mode == 1 and y != 0) {
    displayOled("Galaxian Classic\nFor start press A");
    noteCounter=0;
    soundDelay1=0;
    noTone(soundPin);
    mode = 2;
    y = 0;
    delay(100);
  } else if (mode == 2 and y != 0) {
    displayOled("Galaxian song\nFor start press A");
    mode = 1;
    y = 0;
    delay(100);
  }
  if (mode==1) {
    playSong("BloodyTears");
  }
  if (y != 0 and abs(mode)!=mode) {
    displayLed(15, 0, 0);
  } else if (abs(mode)==mode) {
    for (int row = 0; row < 3; row++) {
      for (int col = 0; col < 2; col++) {
        displayLed(row + shiftedY, col, pgm_read_byte(&(ship[row][col])));
      }
    }
  }
  if (abs(mode)!=mode) {
    if (mode==-1) {
      playSong("BloodyTears");
    }
    if (allTime - lastTime >= 200) {
      shiftedY -= y;
      y = 0;
      Warp();
      for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 2; col++) {
          Array[row + shiftedY][col] = pgm_read_byte(&(ship[row][col])) + 1;
        }
      }
      if (active == 1) {
        active = 0;
        if (mode==-2) {
          PlaySound(4);
        }
        ShootG(shiftedY + 1);
      }
      // Cele vykreslovani hry
      for (int row = 0; row < 16; row++) {
        for (int col = 0; col < 16; col++) {
          if (Array[row][col] > 0) {
            Array[row][col]--;
            if (Array[row][col] == 0) {
              displayLed(row, col, 0);
            } else if (Array[row][col] == 1) {
              int displayShip = 0;
              for (int i = 0; i < helpingVariable * 2; i++) {
                if (i % 2 == 0) {
                  if (row == EnemysG[i] and col == EnemysG[i + 1]) {
                    displayShip = 1;
                    if (EnemyDouble[i] == 1 and abs(EnemysG[i]) == EnemysG[i]) {
                      displayLed(EnemysG[i], EnemysG[i + 1] + 1, 0);
                      if (mode==-2) {
                        PlaySound(5);
                      }
                      EnemysG[i] = -EnemysG[i];
                      EnemysG[i + 1] = -EnemysG[i + 1];
                      points += 1;
                      displayOled("Your Score: " + String(points) + "\nHigh Score: " + String(read2EEPROM(addressGalaxian1)));
                    } else if (EnemyDouble[i] > 1 and abs(EnemysG[i]) == EnemysG[i]) {
                      if (mode==-2) {
                        PlaySound(5);
                      }
                      EnemyDouble[i] -= 1;
                      displayLed(EnemysG[i], EnemysG[i + 1], 0);
                      points += 1;
                      displayOled("Your Score: " + String(points) + "\nHigh Score: " + String(read2EEPROM(addressGalaxian1)));
                    }
                    if ((EnemysG[i] != 17 and EnemysG[i + 1] != 17) and EnemysG[i] != abs(EnemysG[i])) {
                      wave += 1;
                      EnemysG[i + 1] = 17;
                      EnemysG[i] = 17;
                    }
                    for (int j = col; j < 16; j++) {
                      Array[row][j] = 0;
                    }
                  }
                }
              }
              if (displayShip != 1) {
                displayLed(row, col, 1);
              }
            }
          }
        }
      }
    }
    if (allTime - lastTime2 >= delayTime) {
      if (wave >= helpingVariable) {
        wave = 0;
        moveEnemy1 += 1;
        helpingVariable = EnemyG(moveEnemy1);
      }
      for (int i = 0; i < helpingVariable * 2; i++) {
        if (i % 2 == 0) {
          if (EnemysG[i] != 17 and EnemysG[i + 1] != 17) {
            displayLed(EnemysG[i], EnemysG[i + 1] + 1, 0);
            if (EnemyDouble[i] > 1) {
              for (int j = 1; j < EnemyDouble[i]; j++) {
                displayLed(EnemysG[i], EnemysG[i + 1] - j, 1);
              }
            }
            displayLed(EnemysG[i], EnemysG[i + 1], 1);
            EnemysG[i + 1]--;
          }
          if (EnemysG[i + 1] <= 0 + EnemyDouble[i] - 1) {
            GameOver(points, 4);
            break;
          }
        }
      }
      lastTime2 = allTime;
    }
  }
}

void playSong(String nameOfSong) {
  // Pisnicka z project hubu https://create.arduino.cc/projecthub/GeneralSpud/passive-buzzer-song-take-on-me-by-a-ha-0f04a8
  if ((nameOfSong == "AmongUs" or nameOfSong == "Among" or nameOfSong == "among" or nameOfSong == "Amongus") and soundDelay == 0) {
    soundLastTime = millis();
    //Serial.println(noteCounter);
    int notes [37] =  {1046, 1244, 1400, 1510, 1400, 1244, 1046, 0  , 932, 1174, 1046, 0  , 780, 525, 0  , 1046, 1244, 1400, 1510, 1400, 1244, 1400, 0  , 1510, 1400, 1244, 1510, 1400, 1244, 1510, 1400, 1244, 1510, 1400, 1244, 1510, 0};
    int timing [37] = {250 , 250 , 250 , 250 , 250 , 250 , 250 , 500, 125, 125 , 250 , 500, 250, 250, 250, 250 , 250 , 250 , 250 , 250 , 250 , 250 , 500, 125 , 125 , 125 , 125 , 125 , 125 , 125 , 125 , 125 , 125 , 125 , 125 , 125 , 500};
    if (soundDelay == 0) {
      if (notes[noteCounter] == 0) {
        noTone(soundPin);
      } else if (soundState == true) {
        tone(soundPin, notes[noteCounter]);
      }
      soundDelay = timing[noteCounter];
    }
    noteCounter++;
    if (noteCounter == 37) {
      noteCounter = 0;
      displayClear();
      GameOver(points, 3);
    }
  } else if (nameOfSong == "Uno" and soundDelay == 0) {
    int noty[34] = {147, 247, 220, 196, 165, 330, 294, 262, 156, 233, 220, 196, 139, 233, 220, 196, 147, 247, 220, 196, 165, 330, 294, 262, 156, 233, 220, 196, 139, 233, 220, 196};
    if (soundState == true) {
      tone(soundPin, noty[noteCounter]);
    }
    soundDelay = 200;
    noteCounter++;
    //int ran = random(0,2);
    if (noteCounter == 34) {
      noteCounter = 0;
      displayClear();
      GameOver(points, 3);
    }
  } else if (nameOfSong == "BloodyTears" and soundDelay1 == 0) {
    int tempo = 144;
    // sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
    // there are two values per note (pitch and duration), so for each note there are four bytes
    int notes = sizeof(melodyGalaxian) / sizeof(melodyGalaxian[0]) / 2;

    // this calculates the duration of a whole note in ms
    int wholenote = (60000 * 4) / tempo;

    int divider = 0, noteDuration = 0;
    divider = pgm_read_word_near(melodyGalaxian + noteCounter + 1);
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    if (soundState==true) {
      tone(soundPin, pgm_read_word_near(melodyGalaxian + noteCounter), noteDuration * 0.9);
    }
    noteCounter += 2;
    // Wait for the specief duration before playing the next note.
    soundDelay1 = noteDuration;
    if (noteCounter == 1352) {
      noteCounter = 70;
    }
    // stop the waveform generation before the next note.
    //noTone(soundPin);
  } else if (nameOfSong == "KeyboardCat" and soundDelay1 == 0) {
    int tempo = 160;
    // sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
    // there are two values per note (pitch and duration), so for each note there are four bytes
    int notes = sizeof(melodyCat) / sizeof(melodyCat[0]) / 2;

    // this calculates the duration of a whole note in ms
    int wholenote = (60000 * 4) / tempo;

    int divider = 0, noteDuration = 0;
    divider = pgm_read_word_near(melodyCat + noteCounter + 1);
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    if (soundState==true) {
      tone(soundPin, pgm_read_word_near(melodyCat + noteCounter), noteDuration * 0.9);
    }
    noteCounter += 2;
    // Wait for the specief duration before playing the next note.
    soundDelay1 = noteDuration;
    if (noteCounter == 118) {
      noteCounter = 0;
      displayClear();
      GameOver(points, 3);
    }
    // stop the waveform generation before the next note.
    //noTone(soundPin);
  } else if (nameOfSong == "Doom" and soundDelay1 == 0) {
    int tempo = 225;
    // sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
    // there are two values per note (pitch and duration), so for each note there are four bytes
    int notes = sizeof(melodyDoom) / sizeof(melodyDoom[0]) / 2;

    // this calculates the duration of a whole note in ms
    int wholenote = (60000 * 4) / tempo;

    int divider = 0, noteDuration = 0;
    divider = pgm_read_word_near(melodyDoom + noteCounter + 1);
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    if (soundState==true) {
      tone(soundPin, pgm_read_word_near(melodyDoom + noteCounter), noteDuration * 0.9);
    }
    noteCounter += 2;
    // Wait for the specief duration before playing the next note.
    soundDelay1 = noteDuration;
    if (noteCounter == 500) { //1358
      noteCounter = 0;
      displayClear();
      GameOver(points, 3);
    }
    // stop the waveform generation before the next note.
    //noTone(soundPin);
  }
}

void DanceMan() {
  allTime = millis();
  if (active == -1) {
    GameOver();
  }
  if (abs(mode) == mode and active == 1) {
    noTone(soundPin);
    int mod = mode;
    helpingVariable = -mode;
    SettingGameDanceMan();
    helpingVariable = 1;
    mode = -mod;
    if (mode == -2 ) {
      displayOled("Song: Uno");
    } else if (mode == -1 ) {
      displayOled("Song: Among us");
    } else if (mode == -3) {
      displayOled("Song: DOOM");
    } else if (mode == -4) {
      displayOled("Song: Key. cat");
    }
  } else if (mode == 1 and y != 0) {
    mode = 3 + y;
    y = 0;
    delay(100);
  } else if (mode == 2 and y != 0) {
    mode -= y;
    y = 0;
    delay(100);
  } else if (mode == 3 and y != 0) {
    mode -= y;
    y = 0;
    delay(100);
  } else if (mode == 4 and y != 0) {
    mode = 2 + y;
    y = 0;
    delay(100);
  }
  if (mode == 1) {
    displayOled("Song: Among us\nFor start press A");
    displayImage(ArrowUp);
  } else if (mode == 2) {
    displayOled("Song: Uno\nFor start press A");
    displayImage(ArrowDown);
  } else if (mode == 3) {
    displayOled("Song: DOOM\nFor start press A");
    displayImage(ArrowLeft);
  } else if (mode == 4) {
    displayOled("Song: Key cat\nFor start press A");
    displayImage(ArrowRight);
  }
  //Serial.println(mode);
  if (abs(mode) != mode) {
    if (helpingVariable == 1) {
      allwaysY = random(0, 4);
      const word* nazev[] = {ArrowUp, ArrowRight, ArrowDown, ArrowLeft};
      displayImage(nazev[allwaysY]);
      helpingVariable = 0;
      allwaysX = 170;
      x = 0;
      y = 0;
    } else if (allwaysX <= 180) {
      bool hit = false;
      if (x == 1 or x == -1 or y == 1 or y == -1) {
        if (x == 1 and allwaysY == 3) {
          hit = true;
        } else if (x == -1 and allwaysY == 1) {
          hit = true;
        } else if (y == 1 and allwaysY == 0) {
          hit = true;
        } else if (y == -1 and allwaysY == 2) {
          hit = true;
        } else if (allwaysY == -2) {
          hit = true;
        } else {
          noTone(soundPin);
          GameOver();
        }
      }
      if (hit == true) {
        displayClear();
        if (allwaysX <= 170 and allwaysX > 110) {
          points += allwaysX / 10;
          displayOled("Perfect: " + String(allwaysX / 10) + "\nAll Points: " + String(points));
        } else if (allwaysX <= 110 and allwaysX > 50) {
          points += allwaysX / 10;
          displayOled("Good: " + String(allwaysX / 10) + "\nAll Points: " + String(points));
        } else if (allwaysX <= 50) {
          points += allwaysX / 10;
          displayOled("Ok: " + String(allwaysX / 10) + "\nAll Points: " + String(points));
        }
        x = 0;
        y = 0;
        allwaysX = 0;
        helpingVariable = 1;
        //delay(100);
      }
    }
    if (allTime - lastTime >= 10 and allwaysX != 1 and allwaysX != 0) {
      lastTime = millis();
      allwaysX--;
    }
    if (mode == -1 and gameState == 3) {
      playSong("AmongUs");
    } else if (mode == -2 and gameState == 3) {
      playSong("Uno");
    } else if (mode == -3 and gameState == 3) {
      playSong("Doom");
    } else if (mode == -4 and gameState == 3) {
      playSong("KeyboardCat");
    }
  }
}

void InfoFun(bool mode) {
  int Hard = 60;
  int Easy1 = -50;
  word* nazev[3];
  if (mode == true) {
    nazev[0] = {Low};
    nazev[1] = {Medium};
    nazev[2] = {High};
  } else {
    nazev[0] = {Easy};
    nazev[1] = {Medium};
    nazev[2] = {High/*Hard*/};
  }
  if (mode == true) {
    if (intensity == 13) {
      displayImage(nazev[2]);
      displayOled("Display intensity:\nHIGH");
    } else if (intensity == 3) {
      displayImage(nazev[1]);
      displayOled("Display intensity:\nMEDIUM");
    } else if (intensity == 0) {
      displayImage(nazev[0]);
      displayOled("Display intensity:\nLOW");
    }
    IsButtonPressed();
    if (active == -1 or active == 1) {
      gameState = 5;
      SettingSetting();
    }
    if (x == -1 or y == 1) {
      if (intensity == 13) {
        intensity = 0;
      } else if (intensity == 3) {
        intensity = 13;
      } else if (intensity == 0) {
        intensity = 3;
      }
    } else if (x == 1 or y == -1) {
      if (intensity == 13) {
        intensity = 3;
      } else if (intensity == 3) {
        intensity = 0;
      } else if (intensity == 0) {
        intensity = 13;
      }
    }
    EEPROM.write(addressIntensity, intensity);
    for (int index = 0; index < 4; index++) {
      lc.setIntensity(index, EEPROM.read(addressIntensity));
    }
    delay(200);
  } else {
    if (difficulty == Hard) {
      displayImage(nazev[2]);
      displayOled("Difficulty:\nHARD");
    } else if (difficulty == 0) {
      displayImage(nazev[1]);
      displayOled("Difficulty:\nMEDIUM");
    } else if (difficulty == Easy1) {
      displayImage(nazev[0]);
      displayOled("Difficulty:\nEASY");
    }
    IsButtonPressed();
    if (active == -1 or active == 1) {
      gameState = 5;
      SettingSetting();
    }
    if (x == -1 or y == 1) {
      if (difficulty == Hard) {
        difficulty = Easy1;
      } else if (difficulty == 0) {
        difficulty = Hard;
      } else if (difficulty == Easy1) {
        difficulty = 0;
      }
    } else if (x == 1 or y == -1) {
      if (difficulty == Hard) {
        difficulty = 0;
      } else if (difficulty == 0) {
        difficulty = Easy1;
      } else if (difficulty == Easy1) {
        difficulty = Hard;
      }
    }
    EEPROM.write(addressDifficulty, difficulty);
    delay(200);
  }
}

void NastaveniHer() {
  if (isGameOn == 0) {
    if (soundState == true) {
      displayImage(Sound_on, 6, 6, 3, -1, -1);
    } else if (soundState == false) {
      displayImage(Sound_off, 6, 6, 3, -1, -1);
    }
    //IsButtonPressed();
    if (active == -1) {
      goThrough = true;
      gameState = 0;
    }
    if (whichIconShowed == 1) {
      displayOled("High score");
      whichIconShowed += (abs(x) + 1) * abs(x);
      whichIconShowed += (abs(y)) * abs(y);
    } else if (whichIconShowed == 2) {
      displayOled("Difficulty");
      whichIconShowed += (abs(x) + 1) * abs(x);
      whichIconShowed -= (abs(y)) * abs(y);
    } else if (whichIconShowed == 3) {
      displayOled("Display intensity");
      whichIconShowed -= (abs(x) + 1) * abs(x);
      whichIconShowed += (abs(y)) * abs(y);
    } else if (whichIconShowed == 4) {
      if (soundState == true) {
        displayOled("Sound ON");
      } else if (soundState == false) {
        displayOled("Sound OFF");
      }
      whichIconShowed -= (abs(x) + 1) * abs(x);
      whichIconShowed -= (abs(y)) * abs(y);
    }
    if (x != 0 or y != 0) {
      ShowBorders(whichIconShowed - 1);
    }
    x = 0;
    y = 0;
    if (active == 1 and whichIconShowed != 0) {
      active = 0;
      if (whichIconShowed != 4) {
        if (soundState == true) {
          tone(soundPin, 147);
          delay(100);
          noTone(soundPin);
        }
        displayClear();
      }
      if (whichIconShowed == 3) {
        gameState = 4;
      } else if (whichIconShowed == 2) {
        gameState = 41;
      } else if (whichIconShowed == 1) {
        isGameOn = 1;
        whichIconShowed = 1;
        displayImage(SnakeMenu, 4, 4, 1, -2, -2);
        displayImage(Galaxian, 4, 4, 0, -2, -2);
        displayOled("Snake");
        ShowBorders(1);
      } else if (whichIconShowed == 4) {
        soundState = !soundState;
        if (soundState == true) {
          tone(soundPin, 147);
          delay(100);
          noTone(soundPin);
        }
        EEPROM.write(addressSound, soundState);
      }
    }
  } else if (isGameOn == 1) {
    displayImage(Gold_medal, 4, 4, 3, -2, -2);
    displayImage(Silver_medal, 4, 4, 2, -2, -2);
    if (active == -1) {
      SettingSetting();
    }
    if (whichIconShowed == 1 and whichMedal == 0) {
      whichIconShowed += abs(y) * -1;
    } else if (whichIconShowed == 0 and whichMedal == 0) {
      whichIconShowed += abs(y);
    }
    if (whichIconShowed == 0) {
      if (whichMedal == 0 and (x != 0 or y != 0)) {
        ShowBorders(0);
        displayOled("Galaxian");
      }
      if (x == -1) {
        whichMedal = 3;
      } else if (x == 1) {
        whichMedal = 0;
        ShowBorders(0);
        displayOled("Galaxian");
        //displayOled();
      }
      if (whichMedal == 3) {
        whichMedal += abs(y) * -1;
      } else if (whichMedal == 2) {
        whichMedal += abs(y);
      }
      if (whichMedal == 3) {
        if (x != 0 or y != 0) {
          ShowBorders(3);
        }
        displayOled("First\nScore: " + String(read2EEPROM(addressGalaxian1)));

      } else if (whichMedal == 2) {
        if (x != 0 or y != 0) {
          ShowBorders(2);
        }
        displayOled("Second\nScore: " + String(read2EEPROM(addressGalaxian2)));
      }
    } else if (whichIconShowed == 1) {
      if (whichMedal == 0 and (x != 0 or y != 0)) {
        ShowBorders(1);
        displayOled("Snake");
      }
      if (x == -1) {
        whichMedal = 3;
      } else if (x == 1) {
        whichMedal = 0;
        displayOled("Snake");
        ShowBorders(1);
        //displayOled();
      }
      if (whichMedal == 3) {
        whichMedal += abs(y) * -1;
      } else if (whichMedal == 2) {
        whichMedal += abs(y);
      }
      if (whichMedal == 3) {
        if (x != 0 or y != 0) {
          ShowBorders(3);
        }
        displayOled("First\nScore: " + String(read2EEPROM(addressSnake1)));
      } else if (whichMedal == 2) {
        if (x != 0 or y != 0) {
          ShowBorders(2);
        }
        displayOled("Second\nScore: " + String(read2EEPROM(addressSnake2)));
      }
    }
    x = 0;
    y = 0;
    //IsButtonPressed();
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
  if (gameState == -1) {
    displayClear();
  } else if (gameState == 2) {
    Snake();
  } else if (gameState == 0) {
    Menu(menuFirstWalkThrough, goThrough);
    menuFirstWalkThrough = false;
    goThrough = false;
  } else if (gameState == 3) {
    DanceMan();
  } else if (gameState == 4) {
    InfoFun(true);
  } else if (gameState == 41) {
    InfoFun(false);
  } else if (gameState == 5) {
    NastaveniHer();
  } else if (gameState == 6) {
    GalaxianGame();
  }
  if (soundTime - soundLastTime >= soundDelay and soundDelay1 == 0) {
    noTone(soundPin);
    soundLastTime = soundTime;
    if (soundDelay == 200 and noteCounter == 0) {
      if (congrats > 3) {
        soundDelay = 0;
      } else {
        congrats += 1;
        congratsState = true;
      }
    } else if (soundDelay == 150) {
      tone(soundPin, 200);
      soundDelay = 151;
    } else if (soundDelay == 151) {
      tone(soundPin, 360);
      soundDelay = 0;
    } else {
      soundDelay = 0;
    }
  }
  if (soundTime - soundLastTime >= soundDelay1 and soundDelay == 0) {
    noTone(soundPin);
    soundLastTime = soundTime;
    soundDelay1 = 0;
  }
}
