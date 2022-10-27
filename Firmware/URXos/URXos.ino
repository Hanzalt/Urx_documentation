///(Snad posledni verze) Po druhym vyhorenim jsme se rozhodl udelat algoritmus smesi for cyklu(verze jedna) a smerem zapinani a zapinani(verze dva) JDU NA TO
//OLED displey https://s.click.aliexpress.com/e/_APw28Y
#include <Urx.h> 
//#include <EEPROM.h>
//#include <U8g2lib.h>
//#include <Wire.h>
U8G2_SH1106_128X32_VISIONOX_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); 
URX::Games game;
URX::LedControl lc;
URX::Games::Snake snake;
URX::Games::OtherFunctions ot;
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
void playSound() - Function for playing chosen sound
LOOP FAZE --------------------
void Loop() - Runs every 1/100s or every 10 ms
*/

/****************************************************************/
//Snake
/****************************************************************/

//Food position variables
String versionUrxOS = "0.7.93";
//Delay in game is overrided when function SettingGameSnake is called

//GameState for setting menu - 0, gameover - 1, game - Snake - 2, game - Space Invators = 3, info - 4, nothing - 5

//Menu variable
int whichIconShowed = 0;
int whichMedal = 0;
//Variables for managing to turning game at specific time 

unsigned long lastTime2 = 0;
unsigned long soundTime = 0;
unsigned long soundLastTime = 0;
unsigned long lastDebounceTime = 0;       
int debounceDelay = 50;
//Battery analog pin
const int battery = A0;
//Constant variables that are use when SettingGameSnake is called
//Variable for length of snake
//int opravaPohybu = 0;
//int opravaPohybuf = 0;
int moveEnemy1 = 0;
int wave = 0;
int noteCounter = 0;
int points = 50;
int prucho = 0;
int intensity = 0;
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
  const int batteryValue = analogRead(battery);
  const float mapBattery = (batteryValue * 5.0) / 1024.0;
  if (mapBattery<2.9 and mapBattery>1) {
    u8g2.firstPage();
    do {
      const String out1 = "Your battery";
      const String out2 = "is empty";
      u8g2.setFont(u8g2_font_battery24_tr);
      u8g2.drawGlyph(100,24,0x30);
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
  
  lc.clearDisplay();
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
        lc.displayImage(Nastaveni1, 4,4,2,-2,-2);
      if (points==1) {
        points=-40;
      } else {
        points--;
      }
      } else {
        lc.displayImage(Nastaveni2, 4,4,2,-2,-2);
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
void DrawMenu() {
  lc.clearDisplay();
  lc.displayImage(SnakeMenu, 4,4,1,-2,-2);
  lc.displayImage(Nastaveni1, 4,4,2,-2,-2);
  lc.displayImage(Galaxian, 4,4,0,-2,-2);
  lc.displayImage(DancemanMenu, 4,4,3,-2,-2);
  byte whichIcon = 0x0;
  const int batteryValue = analogRead(battery);
  const float mapBattery = (batteryValue * 5.0) / 1024.0;
  if (mapBattery >= 3.8) {
    whichIcon = 0x34;
  } else if (mapBattery < 3.8 and mapBattery >= 3.2) {
    whichIcon = 0x33;
  } else if (mapBattery < 3.2 and mapBattery >= 2.9) {
    whichIcon = 0x32;
  } else if (mapBattery < 2.9  and mapBattery>1) {
    whichIcon = 0x30;
  } else {
    whichIcon=0;
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
  if (whichIcon==0x30) {
    u8g2.firstPage();
    do {
      const String out1 = "Your battery";
      const String out2 = "is empty";
      u8g2.setFont(u8g2_font_battery24_tr);
      u8g2.drawGlyph(100,24,0x30);
      u8g2.setFont(u8g2_font_helvR12_tr);
      u8g2.setCursor(0, 13);
      u8g2.print(out1);
      u8g2.setCursor(0, 28);
      u8g2.print(out2);
    } while ( u8g2.nextPage() );
    lc.clearDisplay();
    gameState=-1;
  }
}
void SettingSetting(){
  lc.clearDisplay();
  if (soundState==true) {
    lc.displayImage(Sound_on, 6, 6, 3, -1, -1);
  } else if (soundState==false) {
    lc.displayImage(Sound_off, 6, 6, 3, -1, -1);
  }
  lc.displayImage(Intensity, 4,4,2,-2,-2);
  lc.displayImage(HighScore, 4,4,0,-2,-2);
  activatorV = 0;
  whichIconShowed = 0;
}
void SettingGameGalaxian() {
  lc.clearDisplay();
  allTime = millis();
  lastTime=allTime;
  lastTime2=allTime;
  shiftedY=13;
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
  u8g2.displayOled("Your Score: " + String(points) + "\nHigh Score: " + String(EEPROM.read(addressGalaxian1)));
}
void SettingGameDanceMan() {
  lc.clearDisplay();
  points=0; // How many points
  prucho = 0; // When the arrow will apear
  wave=0; // What song to play
  noteCounter=0; // for playSongDanceMan counting which note to play
  soundDelay=0;
  allwaysX=0; // Counting time when pressed
  allwaysY=0; // Direction variable
  activatorV=0; // Is game on?
  lastTime=millis();
  u8g2.displayOled("\nAll Points: " + String(points));
}
/****************************************************************/
//Gamestate functions
/****************************************************************/

//Menu function
void Menu(bool firstGoThrough, bool go) {
  //When menu start show M in start
  //Games::Snake snake;
  if (firstGoThrough==true) {
    const float noty[] = {61.735,261.625,261.625,293.664,329.627};
    whichIconShowed = 0;
    //Clearing all displays
    lc.clearDisplay();
    for (int i = 0; i < 7; i++){
      const word* nazev[] = {HolTyc1,HolTyc2,HolTyc3,HolTyc4,HolTyc5,HolTyc6,HolTyc7};
      lc.displayImage(nazev[i]);
      if ((i>=1 and i <=6) and soundState==true) {
        tone(soundPin, noty[i-1]);
      }
      delay(100);
      noTone(soundPin);
    }
    ot.activeButton();
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
  lc.displayBorders(whichIconShowed-1);
  if (active==1 and whichIconShowed!=0) {
    active=0;
    if (soundState==true) {
      tone(soundPin,147);
      delay(100);
      noTone(soundPin);
    }
    if (whichIconShowed==2) {
      //Showing menu text S - Snake
      snake.begin();
      snake.generateFood();
      Serial.print("dobry");
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
void GalaxianGame() {
  allTime = millis();
  if (active == -1) {
    y=0;
    active=0;
    activatorV=0;
    ot.gameOver(points,4);
  }
  if (y!=0 and activatorV==0) {
    activatorV=1;
    lc.displayLed(15,0,0);
  } else if (activatorV==0) {
    for (int row = 0; row < 3; row++) {
      for (int col = 0; col < 2; col++) {
        lc.displayLed(row+shiftedY,col,pgm_read_byte(&(ship[row][col]))); 
      }
    }
  }
  if (activatorV==1) {
    if (allTime-lastTime>=200) {
      shiftedY-=y;
      y=0;
      ot.warp();
      for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 2; col++) {
          Array[row+shiftedY][col] = pgm_read_byte(&(ship[row][col])) + 1; 
        }
      }
      if (active==1) {
        active=0;
        ot.playSound(4,0);
        ShootG(shiftedY+1);
      }
      // Cele vykreslovani hry
      for (int row = 0; row < 16; row++) {
        for (int col = 0; col < 16; col++) {
          if (Array[row][col] > 0) {
            Array[row][col]--;
            if (Array[row][col]==0) {
              lc.displayLed(row,col,0);
            } else if (Array[row][col]==1) {
              int er = 0;
              for (int i = 0; i < prucho*2; i++) {
                if (i%2==0) {
                  if (row==EnemysG[i] and col==EnemysG[i+1]) {
                    er = 1;
                    ot.playSound(5,0);
                    lc.displayLed(EnemysG[i],EnemysG[i+1],0);
                    lc.displayLed(EnemysG[i],EnemysG[i+1]+1,0);
                    EnemysG[i]=-EnemysG[i];
                    lc.displayLed(row,col,0);
                    if (EnemyDouble[i]>1 and EnemysG[i]!=abs(EnemysG[i])) {
                      EnemysG[i]=-EnemysG[i];
                      EnemyDouble[i]-=1;
                      EnemysG[i+1]=EnemysG[i+1]-1;
                      lc.displayLed(EnemysG[i],EnemysG[i+1]+2,0);
                      lc.displayLed(EnemysG[i],EnemysG[i+1]+1,1);
                    } else if ((EnemysG[i]!=17 and EnemysG[i+1]!=17) and EnemysG[i]!=abs(EnemysG[i])) {
                      wave+=1;
                      points+=1;
                      u8g2.displayOled("Your Score: " + String(points) + "\nHigh Score: " + String(EEPROM.read(addressGalaxian1)));
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
                lc.displayLed(row,col,1);
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
            lc.displayLed(EnemysG[i],EnemysG[i+1]+1,0);
            if (EnemyDouble[i]>1) {
              for (int j = 1; j < EnemyDouble[i]; j++) {
                lc.displayLed(EnemysG[i],EnemysG[i+1]-j,1);
              }
            }
            lc.displayLed(EnemysG[i],EnemysG[i+1],1);
            EnemysG[i+1]--;
          }
          if (EnemysG[i+1]<=1) {
            ot.gameOver(points,4);
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
    } else if (soundState==true) {
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
      lc.clearDisplay();
      lc.displayImage(NewSong);
      return 2;
    }
    if (ran==0 and allwaysX==0) {
      return 0;
    } else {
      return prucho;
    }
  } else if (nameOfSong=="Song1" and soundDelay==0 and noteCounter<=42) {
    int noty[42] = {147,247,220,196,165,330,294,262,156,233,220,196,139,233,220,196,147,247,220,196,165,330,294,262,156,233,220,196,139,233,220,196};
    if (soundState==true){
      tone(soundPin,noty[noteCounter]);
    }
    soundDelay = 200;
    noteCounter++;
    int ran = random(0,2);
    if (noteCounter==42) {
      noteCounter=0;
      ot.gameOver(points,3);
      return 2;
    }
    if (ran==0 and allwaysX==0) {
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
    ot.gameOver();
    Serial.println("Utekl jsem pres B");
  }
  if (activatorV==1) {
    if (prucho==0) {
      allwaysY=random(0,4);
      Serial.print("allwaysY: ");
      Serial.println(allwaysY);
      const word* nazev[] = {ArrowUp,ArrowRight,ArrowDown,ArrowLeft};
      lc.displayImage(nazev[allwaysY]);
      allwaysX=170;
      prucho=-1;
      x=0;
      y=0;
    } else if (prucho==-1 and allwaysX<=170) {
      bool hit = false;
      if (x==1 and allwaysY==3) {
        hit = true;
      } else if (x==-1 and allwaysY==1) {
        hit = true;
      } else if (y==1 and allwaysY==0) {
        hit = true;
      } else if (y==-1 and allwaysY==2) {
        hit = true;
      } else if (allwaysY==-2) {
        Serial.println("Tohle je ta chyba");
        hit= true;
      }
        else if (x==1 or x==-1 or y==1 or y==-1) {
        noTone(soundPin);
        Serial.print("Smer sipky: ");
        Serial.println(allwaysY);
        Serial.print("y: ");
        Serial.println(y);
        Serial.print("x: ");
        Serial.println(x);
        Serial.println("Utekl jsem pres tlacitka");
        ot.gameOver();
      }
      if (hit==true) {
        lc.clearDisplay();
        if (allwaysX<=170 and allwaysX>110) {
          points+=20;
          u8g2.displayOled("Perfect: " + String(20) + "\nAll Points: " + String(points));
        } else if (allwaysX<=110 and allwaysX>50) {
          points+=10;
          u8g2.displayOled("Good: " + String(10) + "\nAll Points: " + String(points));
        } else if (allwaysX<=50) {
          points+=0;
          u8g2.displayOled("Ok: " + String(0) + "\nAll Points: " + String(points));
        }
        x=0;
        y=0;
        allwaysX=0;
        prucho=1;
      }
    }
    if (allTime-lastTime>=10 and allwaysX!=1 and allwaysX!=0) {
      lastTime=millis();
      allwaysX--;
    }
    if (wave==1 and prucho!=2 and gameState==3) {
      prucho = playSongDanceMan("AmongUs");
    } else if (wave==2 and prucho!=2 and gameState==3) {
      prucho = playSongDanceMan("Song1");
    } else if (prucho==2 and active==1) {
      prucho=0;
      active=0;
    }
  } else {
    lc.displayImage(PushA);
  }
}
void InfoFun() {
  const word* nazev[] = {Low,Medium,High};
  if (intensity==13) {
    lc.displayImage(nazev[2]);
  } else if (intensity==5) {
    lc.displayImage(nazev[1]);
  } else if (intensity==0) {
    lc.displayImage(nazev[0]);
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
      lc.displayImage(Sound_on, 6, 6, 3, -1, -1);
    } else if (soundState==false) {
      lc.displayImage(Sound_off, 6, 6, 3, -1, -1);
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
    lc.displayBorders(whichIconShowed-1);
    if (active==1 and whichIconShowed!=0) {
      active=0;
      if (whichIconShowed!=4) {
        if (soundState==true) {
          tone(soundPin,147);
          delay(100);
          noTone(soundPin);
        }
        lc.clearDisplay();
      }
      if (whichIconShowed==3) {
          gameState=4;
      } else if (whichIconShowed==1) {
          activatorV = 1;
          whichIconShowed = 1;
          lc.displayImage(SnakeMenu,4,4,1,-2,-2);
          lc.displayImage(Galaxian,4,4,0,-2,-2);
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
    lc.displayImage(Gold_medal,4,4,3,-2,-2);
    lc.displayImage(Silver_medal,4,4,2,-2,-2);
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
        lc.displayBorders(0); 
      }
      if (x==-1) {
        whichMedal = 3;
      } else if (x==1) {
        whichMedal = 0;
        lc.displayBorders(0);
        u8g2.displayOled();
      }
      if (whichMedal==3) {
        whichMedal += abs(y) * -1;
      } else if (whichMedal==2) {
        whichMedal += abs(y);
      }
      if (whichMedal==3) {
        lc.displayBorders(3);
        u8g2.displayOled("Score: " + String(EEPROM.read(addressGalaxian1)));
        
      } else if (whichMedal==2) {
        lc.displayBorders(2);
        u8g2.displayOled("Score: " + String(EEPROM.read(addressGalaxian2)));
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
        lc.displayBorders(1);
      }
      if (x==-1) {
        whichMedal = 3;
      } else if (x==1) {
        whichMedal = 0;
        lc.displayBorders(1);
        u8g2.displayOled();
      }
      if (whichMedal==3) {
        whichMedal += abs(y) * -1;
      } else if (whichMedal==2) {
        whichMedal += abs(y);
      }
      if (whichMedal==3) {
        lc.displayBorders(3);
        u8g2.displayOled("Score: " + String(EEPROM.read(addressSnake1)));
        
      } else if (whichMedal==2) {
        lc.displayBorders(2);
        u8g2.displayOled("Score: " + String(EEPROM.read(addressSnake2)));
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
  if (gameState==-1) {
    lc.clearDisplay();
  } else if (gameState==2) {
    //Serial.println("sa");
    snake.snakeStart();
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
