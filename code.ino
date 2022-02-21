//(Snad posledni verze) Po druhym vyhorenim jsme se rozhodl udelat algoritmus smesi for cyklu(verze jedna) a smerem zapinani a zapinani(verze dva) JDU NA TO
//OLED displey https://s.click.aliexpress.com/e/_APw28Y
#include "LedControl.h"
#include <EEPROM.h>
LedControl lc = LedControl(11, 12, 10, 4);
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

//Array for remembering the hole pixel matrix
int Array[16][16] = {{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                    {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};
int bulletPosition[2] = {0,0};
int enBulletPosition1[4] = {0,0,0,0};
int enBulletPosition2[4] = {0,0,0,0};
int Enemys[6] = {random(0,3),random(0,3),random(0,3),random(0,3),random(0,3),random(0,3)};
/****************************************************************/
//Snake
/****************************************************************/

/****************************************************************/
//Space Invators
/****************************************************************/
const PROGMEM byte enemy1[2][2] = {
  {1,1},
  {1,1}};
const PROGMEM byte enemy2[2][1] = { 
  {1},
  {1}};

const PROGMEM byte block[4][4] = {
  {0,1,1,0},
  {1,1,1,1},
  {1,1,1,1},
  {0,1,1,0}};
const PROGMEM byte lod[2][3] = {
  {0,1,0},
  {1,1,1}};
  
const PROGMEM byte vybuch1[3][3] = {
  {1,1},
  {1,1}};  
const PROGMEM byte ship_explosion1[2][3] = {
  {0,0,0},
  {1,1,1}};
const PROGMEM byte ship_explosion2[2][3] = {
  {0,0,0},
  {1,0,1}};
/****************************************************************/
//Space Invators
/****************************************************************/

/****************************************************************/
//Menu
/****************************************************************/

const PROGMEM byte GameOverMessage[16][16] = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1},
  {1,0,0,0,0,1,0,1,0,1,1,1,0,1,0,0},
  {1,0,1,1,0,1,1,1,0,1,0,1,0,1,1,0},
  {1,0,0,1,0,1,0,1,0,1,0,1,0,1,0,0},
  {1,1,1,1,0,1,0,1,0,1,0,1,0,1,1,1},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {1,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1},
  {1,0,0,1,0,1,0,1,0,1,0,0,0,1,0,1},
  {1,0,0,1,0,1,0,1,0,1,1,0,0,1,1,1},
  {1,0,0,1,0,1,0,1,0,1,0,0,0,1,1,0},
  {1,1,1,1,0,0,1,0,0,1,1,1,0,1,0,1},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
const PROGMEM byte Win[16][16] = {
  {0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
  {0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,1,0,0,0,1,0,0,1,0,0,1,0,0,0,1},
  {0,1,0,0,0,1,0,0,1,0,0,1,0,0,0,1},
  {0,1,0,0,0,1,0,0,1,0,0,1,1,0,0,1},
  {0,1,0,1,0,1,0,0,1,0,0,1,0,1,0,1},
  {0,1,0,1,0,1,0,0,1,0,0,1,0,0,1,1},
  {0,1,1,0,1,1,0,0,1,0,0,1,0,0,0,1},
  {0,1,0,0,0,1,0,0,1,0,0,1,0,0,0,1}};
const PROGMEM byte Score[16][16] = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {1,1,0,1,1,0,1,1,1,0,1,1,0,0,1,1},
  {1,0,0,1,0,0,1,0,1,0,1,0,1,0,1,0},
  {1,0,0,1,0,0,1,0,1,0,1,0,1,0,1,0},
  {1,1,0,1,0,0,1,0,1,0,1,0,1,0,1,1},
  {0,1,0,1,0,0,1,0,1,0,1,1,0,0,1,0},
  {0,1,0,1,0,0,1,0,1,0,1,0,1,0,1,0},
  {1,1,0,1,1,0,1,1,1,0,1,0,1,0,1,1},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};;
const PROGMEM byte SnakeMenu[4][4] = {
  {1,1,1,1},
  {1,1,0,0},
  {0,0,1,1},
  {1,1,1,1}};
const PROGMEM byte Nothing[4][4] = {
  {1,0,0,1},
  {1,1,0,1},
  {1,0,1,1},
  {1,0,0,1}};
  /*
const PROGMEM byte PongMenu[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,1,1,1,0,0},
  {0,0,0,1,0,1,0,0},
  {0,0,0,1,1,1,0,0},
  {0,0,0,1,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}};
const PROGMEM byte TetrisMenu[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,1,1,1,0,0,0},
  {0,0,0,1,0,0,0,0},
  {0,0,0,1,0,0,0,0},
  {0,0,0,1,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}};
  */
const PROGMEM byte Dekujeme[4][4] = {
  {1,1,1,0},
  {1,0,0,1},
  {1,0,0,1},
  {1,1,1,0}};
const PROGMEM byte SpaceInvatorsMenu[4][4] = {
  {1,1,1,1},
  {0,1,1,0},
  {0,1,1,0},
  {1,1,1,1}};
const PROGMEM byte MenuMenu[16][16] = {
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,1,1,0,0,0,0,0,0,0,0,1,1,0,1},
  {1,0,1,1,1,0,0,0,0,0,0,1,1,1,0,1},
  {1,0,1,1,1,1,0,0,0,0,1,1,1,1,0,1},
  {1,0,1,1,1,1,1,0,0,1,1,1,1,1,0,1},
  {1,0,1,1,0,1,1,1,1,1,1,0,1,1,0,1},
  {1,0,1,1,0,0,1,1,1,1,0,0,1,1,0,1},
  {1,0,1,1,0,0,0,1,1,0,0,0,1,1,0,1},
  {1,0,1,1,0,0,0,0,0,0,0,0,1,1,0,1},
  {1,0,1,1,0,0,0,0,0,0,0,0,1,1,0,1},
  {1,0,1,1,0,0,0,0,0,0,0,0,1,1,0,1},
  {1,0,1,1,0,0,0,0,0,0,0,0,1,1,0,1},
  {1,0,1,1,0,0,0,0,0,0,0,0,1,1,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}};
const PROGMEM byte HolTyc1[16][16] = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
const PROGMEM byte HolTyc2[16][16] = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,0},
  {0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,0},
  {0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,0},
  {0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0},
  {0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0},
  {0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,0},
  {0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,0},
  {0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
const PROGMEM byte HolTyc3[16][16] = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,1,1,0,0,1,1,0,0,0,0},
  {0,0,0,0,0,0,1,1,0,0,1,1,0,0,0,0},
  {0,0,0,0,0,0,1,1,0,0,1,1,0,0,0,0},
  {0,0,0,0,0,0,1,1,0,0,1,1,0,0,0,0},
  {0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0},
  {0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,0,1,1,0,0,1,1,0,0,0,0},
  {0,0,0,0,0,0,1,1,0,0,1,1,0,0,0,0},
  {0,0,0,0,0,0,1,1,0,0,1,1,0,0,0,0},
  {0,0,0,0,0,0,1,1,0,0,1,1,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
const PROGMEM byte HolTyc4[16][16] = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0},
  {0,0,0,0,0,1,1,1,0,0,1,1,1,0,0,0},
  {0,0,0,0,0,1,1,1,0,0,1,1,1,0,0,0},
  {0,0,0,0,0,1,1,1,0,0,1,1,1,0,0,0},
  {0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0},
  {0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0},
  {0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0},
  {0,0,0,0,0,1,1,1,0,0,1,1,1,0,0,0},
  {0,0,0,0,0,1,1,1,0,0,1,1,1,0,0,0},
  {0,0,0,0,0,1,1,1,0,0,1,1,1,0,0,0},
  {0,0,0,0,0,1,1,1,0,0,1,1,1,0,0,0},
  {0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
const PROGMEM byte HolTyc5[16][16] = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0},
  {0,0,0,0,1,1,1,1,0,0,1,1,1,1,0,0},
  {0,0,0,0,1,1,1,1,0,0,1,1,1,1,0,0},
  {0,0,0,0,1,1,1,1,0,0,1,1,1,1,0,0},
  {0,0,0,0,1,1,1,1,0,0,1,1,1,1,0,0},
  {0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
  {0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
  {0,0,0,0,1,1,1,1,0,0,1,1,1,1,0,0},
  {0,0,0,0,1,1,1,1,0,0,1,1,1,1,0,0},
  {0,0,0,0,1,1,1,1,0,0,1,1,1,1,0,0},
  {0,0,0,0,1,1,1,1,0,0,1,1,1,1,0,0},
  {0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
const PROGMEM byte HolTyc6[16][16] = {
  {0,0,0,0,0,1,1,1,0,0,0,0,1,1,1,0},
  {0,0,0,0,1,1,1,1,1,0,0,1,1,1,1,1},
  {0,0,0,0,1,1,1,1,1,0,0,1,1,1,1,1},
  {0,0,0,0,1,1,1,1,1,0,0,1,1,1,1,1},
  {0,0,0,0,1,1,1,1,1,0,0,1,1,1,1,1},
  {0,0,0,0,1,1,1,1,1,0,0,1,1,1,1,1},
  {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {0,0,0,0,1,1,1,1,1,0,0,1,1,1,1,1},
  {0,0,0,0,1,1,1,1,1,0,0,1,1,1,1,1},
  {0,0,0,0,1,1,1,1,1,0,0,1,1,1,1,1},
  {0,0,0,0,1,1,1,1,1,0,0,1,1,1,1,1},
  {0,0,0,0,0,1,1,1,0,0,0,0,1,1,1,0}};
const PROGMEM byte Gold_medal[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,1,0,0,0},
  {0,0,0,1,1,0,0,0},
  {0,0,1,0,1,0,0,0},
  {0,0,0,0,1,0,0,0},
  {0,0,0,0,1,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}};
const PROGMEM byte Silver_medal[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,1,1,0,0,0},
  {0,0,1,0,0,1,0,0},
  {0,0,0,0,1,0,0,0},
  {0,0,0,1,0,0,0,0},
  {0,0,1,1,1,1,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}};
const PROGMEM byte Sound_icon[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,1,0,0,0},
  {0,1,1,1,1,0,0,0},
  {0,1,1,1,1,0,1,0},
  {0,1,1,1,1,0,1,0},
  {0,1,1,1,1,0,0,0},
  {0,0,0,0,1,0,0,0},
  {0,0,0,0,0,0,0,0}};
const PROGMEM byte Sound_on[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,1,1,1,1,0,0},
  {0,0,1,1,1,1,0,0},
  {0,0,1,1,1,1,0,0},
  {0,1,1,1,1,1,1,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}};
const PROGMEM byte Sound_off[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,1,1,1,1,0,0},
  {0,1,1,1,1,1,1,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}};
const PROGMEM byte Dekujeme_vam_za_4_spolecne_roky_uceni[8][242] = {
  {0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,1,0,0,0,0,0,0,1,0,0,1,1,1,1,0,0,1,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,1,1,1,1,0,0,1,0,1,0,1,0,0,0,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1,1,1,0,0,1,0,1,0,1,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1,1,1,1,0,0,0,1,1,1,1,0,0,1,0,0,0,1,1,1,1,0,0,0,1,1,1,1,0,0,1,0,1,1,1,0,0,0,1,1,1,1,0,0,0,0,0,0,0,1,0,1,1,1,0,0,0,1,1,1,1,0,0,1,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,},
  {0,0,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,1,0,1,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,1,0,1,1,0,1,0,1,0,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,1,0,0,1,0,0,0,1,0,1,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,1,0,0,0,0,1,0,1,0,0,0,0,1,0,1,0,0,1,0,0,0,0,1,0,1,0,0,0,0,1,0,1,1,0,0,0,1,0,1,0,0,0,0,1,0,0,0,0,0,0,1,1,0,0,0,1,0,1,0,0,0,0,1,0,1,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,},
  {0,0,0,0,1,0,0,0,0,0,0,1,0,1,1,1,1,1,1,0,1,1,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,1,0,1,1,1,1,1,1,0,1,0,0,1,0,1,0,1,1,1,1,1,1,0,0,0,0,0,0,1,0,0,0,0,1,0,1,0,0,0,0,1,0,1,0,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,0,0,0,0,1,0,1,0,0,0,0,1,0,1,0,0,1,1,1,1,1,1,0,1,0,0,0,0,0,0,1,0,0,0,0,1,0,1,1,1,1,1,1,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,1,0,1,1,1,0,0,0,0,1,0,0,0,0,1,0,0,0,},
  {0,0,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,1,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,1,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,0,0,1,0,1,0,0,1,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,0,0,0,1,0,1,0,0,0,0,1,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,1,0,1,0,0,1,0,0,0,0,1,0,0,0,1,0,0,0,},
  {0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0,1,0,0,0,1,0,0,1,0,0,0,0,1,0,1,0,0,0,0,1,0,1,0,0,0,0,1,0,1,0,0,1,0,1,0,1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,0,1,1,0,1,0,0,1,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,1,1,1,1,1,0,0,1,0,0,0,0,1,0,1,0,0,1,0,0,0,0,1,0,1,0,0,0,0,1,0,1,0,0,0,0,1,0,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,1,1,1,1,0,1,1,},
  {0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,0,0,1,0,0,0,0,1,0,0,1,1,1,1,0,0,0,1,1,1,1,0,0,0,1,1,1,1,0,0,1,0,0,1,0,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,1,1,0,1,0,1,0,0,1,0,1,0,0,0,0,0,0,1,1,1,1,1,1,0,0,1,1,1,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1,0,0,1,1,1,1,0,0,0,1,1,1,1,0,0,1,0,0,0,0,1,0,0,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,1,1,0,0,1,0,0,0,0,1,0,0,0,0,0,0,1,0,1,1,}};
/****************************************************************/
//Numbers
/****************************************************************/

const PROGMEM byte Onex4[7][4] = {
  {0,0,1,0},
  {0,1,1,0},
  {1,0,1,0},
  {0,0,1,0},
  {0,0,1,0},
  {0,0,1,0},
  {0,1,1,1}};
const PROGMEM byte Twox4[7][4] = {
  {0,1,1,0},
  {1,0,0,1},
  {0,0,0,1},
  {0,0,1,0},
  {0,1,0,0},
  {1,0,0,0},
  {1,1,1,1}};
const PROGMEM byte Threex4[7][4] = {
  {0,1,1,0},
  {1,0,0,1},
  {0,0,0,1},
  {0,0,1,0},
  {0,0,0,1},
  {1,0,0,1},
  {0,1,1,0}};
const PROGMEM byte Fourx4[7][4] = {
  {1,0,0,1},
  {1,0,0,1},
  {1,0,0,1},
  {0,1,1,1},
  {0,0,0,1},
  {0,0,0,1},
  {0,0,0,1}};
const PROGMEM byte Fivex4[7][4] = {
  {1,1,1,1},
  {1,0,0,0},
  {1,0,0,0},
  {0,1,1,1},
  {0,0,0,1},
  {1,0,0,1},
  {0,1,1,0}};
const PROGMEM byte Sixx4[7][4] = {
  {0,1,1,0},
  {1,0,0,1},
  {1,0,0,0},
  {1,1,1,0},
  {1,0,0,1},
  {1,0,0,1},
  {0,1,1,0}};
const PROGMEM byte Sevenx4[7][4] = {
  {1,1,1,1},
  {0,0,0,1},
  {0,0,0,1},
  {0,0,1,0},
  {0,0,1,0},
  {0,1,0,0},
  {0,1,0,0}};
const PROGMEM byte Eightx4[7][4] = {
  {0,1,1,0},
  {1,0,0,1},
  {1,0,0,1},
  {0,1,1,0},  
  {1,0,0,1},
  {1,0,0,1},
  {0,1,1,0}};
const PROGMEM byte Ninex4[7][4] = {
  {0,1,1,0},
  {1,0,0,1},
  {1,0,0,1},
  {0,1,1,1},
  {0,0,0,1},
  {1,0,0,1},
  {0,1,1,0}};
const PROGMEM byte Zerox4[7][4] = {
  {0,1,1,0},
  {1,0,0,1},
  {1,0,1,1},
  {1,1,0,1},
  {1,0,0,1},
  {1,0,0,1},
  {0,1,1,0}};
  
/****************************************************************/
//Numbers
/****************************************************************/

//Food position variables
int foodX = 0;
int foodY = 0;
//Delay in game is overrided when function SettingGameSnake is called
int delayTime = 300;
//GameState for setting menu - 0, gameover - 1, game - Snake - 2, game - Space Invators = 3, dekujeme - 4, nothing - 5
int gameState = 0;
//Menu variable
int whichIconShowed = 0;
//Variables for managing to turning game at specific time 
unsigned long allTime = 0;
unsigned long lastTime = 0;
unsigned long soundTime = 0;
unsigned long soundLastTime = 0;
int soundDelay = 0;
//Buttons variables
const int forward = 7;//7,8
const int right = 8;//8,6
const int left = 6;//6,7
const int down = 5;//5,5
const int activate = 9;
const int soundPin = 4;
//Constant variables that are use when SettingGameSnake is called
const int delayTimeConst = 300;
const int bodyLengthConst = 3;
//Variable for storing 8 bites
const int allLedTurnOn = 0b11111111;
const int allLedTurnOff = 0b00000000;
const int addressSound = 1;
const int addressSnake1 = 2;
const int addressSnake2 = 3;
const int addressSpace1 = 4;
const int addressSpace2 = 6;
//Movement variables
volatile int x = 0;
volatile int y = 0;
int pX = 7;
int pY = 7;
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
int computer = 0;
int points = 0;
bool soundState = true;
bool menuFirstWalkThrough = true;
bool goThrough = true;
bool startingLedOn = true;
bool load = true;

/****************************************************************/
//Setup function
/****************************************************************/
void setup() {
  //Defining variables
  pinMode(forward, INPUT_PULLUP);
  pinMode(right, INPUT_PULLUP);
  pinMode(left, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(activate, INPUT_PULLUP);
  //Pin for piezzo buzzer
  pinMode(soundPin, OUTPUT);
  //Erasing and inicializing all displays
  for (int index = 0; index < 4; index++) {
    lc.shutdown(index, false);
    lc.setIntensity(index, 0);
    lc.clearDisplay(index);
  }
  //Making random random
  randomSeed(analogRead(0));
  soundState = EEPROM.read(addressSound);
  delay(100);
  //SettingGameSpace();
}
/****************************************************************/
//Input functions
/****************************************************************/
//Getting input from buttons
int Buttons() {
  if (digitalRead(forward)) {
    //Serial.println("dopredu");
    if (y != - 1) {
      y = 1;
      x = 0;
      return 1;
    }
  } else if (digitalRead(down)) {
    //Serial.println("dozadu");
    if (y != 1) {
      y = -1;
      x = 0;
      return 1;
    }
  } else if (digitalRead(left)) {
    //Serial.println("doleva");
    if (x != - 1) {
      x = 1;
      y = 0;
      return 1;
    }
  } else if (digitalRead(right)) {
    //Serial.println("doprava");
    if (x != 1) {
      x = -1;
      y = 0;
      return 1;
    } 
  } else {
      return 0;
    }
}
int HorizontalButtons() {
  if (digitalRead(left)) {
    x = 1;
    return 1;
  } else if (digitalRead(right)) {
    x = -1;
    return 1;
  } else {
    x = 0;
    return 0;
  }
}
int FireButtons() {
  if (digitalRead(forward)) {
    y = 1;
    return 1;
  } else {
    y = 0;
    return 0;
  }
}
/****************************************************************/
//Gamestate functions
/****************************************************************/

//Menu function
void Menu(bool firstGoThrough, bool go) {
  //When menu start show M in start
  if (firstGoThrough==true) {
    int remainder = 0;
    int remainderL = 0;
    //int noty[12] = {466,440,392,349,440,392,349,70,70,70,70,7}; // 440,466,349,294,233
    //int noty[] = {349,466,147,440,147,185,0,0,0,0,0};
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
          if (row == 0 or row == 6 - remainder or row == 12 - remainder) {
            //noTone(soundPin);
          }
          PlaySound(6,i);
        } 
        delay(5);
      }
    }
    noTone(soundPin);
    ActiveButton();
    //When some button is pressed then show menu
    for (int index = 0; index < 4; index++) {
      lc.clearDisplay(index);
    }
    ShowOneDisplay(1,'s');
    ShowOneDisplay(0,'d');
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
    ActiveButton();
    for (int index = 0; index < 4; index++) {
      lc.clearDisplay(index);
    }
    ShowOneDisplay(1,'s');
    ShowOneDisplay(0,'d');
    ShowOneDisplay(2,'n');
    ShowOneDisplay(3,'i');
  }
  int direction = IsButtonPressed();
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
  }
  if (whichIconShowed==0 and direction!=5) {
    whichIconShowed=2;
  }
  //Serial.println(whichIconShowed);
  //Cool animation for changing game in menu
  if (whichIconShowed==2) {
    //Showing menu text S - Snake
    ShowBorders(1);
    if (direction==5) {
      SettingGameSnake();
      GenerateFood();
      gameState = 2;
    }
  } else if (whichIconShowed==1) {
    //Showing menu text D - dekujeme
    ShowBorders(0);
    if (direction==5) {
      gameState = 4;
    }
  } else if (whichIconShowed==3) {
    //Showing menu text N - nastaveni
    ShowBorders(2);
    if (direction==5) {
      SettingSetting();
    }
  } else if (whichIconShowed==4) {
    //Showing menu text I - Space Invators
    ShowBorders(3);
    if (direction==5) {
      SettingGameSpace();
      gameState = 3;
    }
  }
  delay(200);
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
  if (game==1 or game==2) {
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
  if (opravaPohybu == 0) {
    opravaPohybu = Buttons();
  }
  //Smart blinking food led 
  if (x==0 and y==0) {
    lc.setLed(0,pX,pY,1);
  } else if (startingLedOn==true) {
    lc.setLed(0,pX,pY,0);
    startingLedOn=false;
  }
  if (activatorV == 1 and digitalRead(activate) == 1) {
    activatorV = 0;
    lastTime = allTime;
    delay(500);
  } else if (activatorV == 0 and digitalRead(activate) == 1) {
    activatorV = 1;
    noTone(soundPin);
    delay(1000);
  }
  if (activatorV == 0) {
    TurningLedArray(foodX,foodY,millis() % 1000 < 500 ? 1 : 0);
    //This will run every delayTime
    if (allTime - lastTime >= delayTime) {
      opravaPohybu = 0;
      //Making head bigger
      if (x == 1) {
        pY += 1;
        Warp();
        //Collision for GameOver
        if (Array[pY][pX] > 0) {
          GameOver(bodyLength,1);
        }
        Array[pY][pX] = bodyLength + 1;
      //Making head bigger
      } else if (x == -1) {
        pY -= 1;
        Warp();
        //Collision for GameOver
        if (Array[pY][pX] > 0) {
          GameOver(bodyLength,1);
        }
        Array[pY][pX] = bodyLength + 1;
      //Making head bigger
      } else if (y == -1) {
        pX += 1;
        Warp();
        //Collision for GameOver
        if (Array[pY][pX] > 0) {
          GameOver(bodyLength,1);
        }
        Array[pY][pX] = bodyLength + 1;
      //Making head bigger
      } else if (y == 1) {
        pX -= 1;
        Warp();
        //Collision for GameOver
        if (Array[pY][pX] > 0) {
          GameOver(bodyLength,1);
        }
        Array[pY][pX] = bodyLength + 1;
      }
      //This will run only if game snake is on
      if (gameState==2) {
        //Big for loop that run for all pixel in array
        for (int row = 0; row < 16; row++) {
          for (int col = 0; col < 16; col++) {
            // if there is a body segment, decrement it's value
            if (Array[row][col] > 0 ) {
              Array[row][col]--;
              //If This pixel is zero (I know that zero means that it is snake ending) turn pixel off
              if (Array[row][col]==0) {
                TurningLedArray(row,col,0);
              //If pixel is the biggest turn pixel on
              } else if (Array[row][col]==bodyLength) {
                TurningLedArray(row,col,1);
              }
            }
          }
          if (soundDelay == 0 and (x>=1 or x <0 or y >=1 or y<0)) {
            PlaySound(1,0);
          }
        }
        //If snake ated the food
        if (Array[pY][pX] == Array[foodX][foodY] && Array[pY][pX]!=0) {
  //        Serial.println("snez");
          noTone(soundPin);
          PlaySound(2,0);
          bodyLength += 1;
          delayTime -= 1;
          foodX=-1;
          foodY=-1;
          GenerateFood();
        }
        //Resetting timer
        lastTime = allTime;
      }
    }
  }
}
void SpaceInvators() {
  allTime = millis();
  FireButtons();
  HorizontalButtons();
  if (y==1 and bulletTimer==0 and activatorV == 2) {
    for (int col = 0; col < 16; col++) {
      if (Array[15][col] == 1) {
        bulletTimer = 14;
        bulletPosition[0] = col - 1; 
        bulletPosition[1] = 13;
      }
    }
    PlaySound(4,0);
  } else if (bulletTimer>0) {
    bulletTimer -= 1;
    if (Array[bulletPosition[1]][bulletPosition[0]] == 1) {
      bulletTimer=0;
      if (bulletPosition[1] > 7) {
        bulletTimer=0;
        Array[bulletPosition[1]][bulletPosition[0]] = 0;
        PlaySound(5,0);
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
            PlaySound(8,noty[d]);
            PlaySound(8,noty[d+1]);
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
            PlaySound(8,noty[d]);
            PlaySound(8,noty[d+1]);
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
  if (activatorV == 2 and digitalRead(activate) == 1) {
    activatorV = 3;
    lastTime = allTime;
    delay(500);
  } else if (activatorV == 3 and digitalRead(activate) == 1) {
    activatorV = 0;
    noTone(soundPin);
    delay(1000);
  }
  if (allTime - lastTime >= delayTime) {
    if (activatorV == 2) {
      points -= 1;
      if (points <= 0) {
        points = 1;
      }
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
void DekujemeMoc() {
  for (int index = 0; index < 4; index++) {
    lc.clearDisplay(index);
  }
  for (int i = 0; i < 210; i++) {
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 16; col++) {
        ShowText(row,col,pgm_read_byte(&(Dekujeme_vam_za_4_spolecne_roky_uceni[row][col+i+4])));
      }
    }
    PlaySound(7,i);
    delay(50);
  }
  gameState=0;
  goThrough = true;
}
void NastaveniHer() {
  if (activatorV == 0) {
    if (soundState==true) {
      for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
          ShowText(row,col + 8,pgm_read_byte(&(Sound_on[row][col])));
        }
      }
    } else if (soundState==false) {
      for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
          ShowText(row,col + 8,pgm_read_byte(&(Sound_off[row][col])));
        }
      }
    }
    int direction = IsButtonPressed();
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
      if (whichIconShowed==2) {
        whichIconShowed -= (abs(y)) * yNula;
      } else if (whichIconShowed==1) {
        whichIconShowed += (abs(x) + 1) * xNula;
        whichIconShowed += (abs(y)) * yNula;
      } else if (whichIconShowed==3) {
        whichIconShowed -= (abs(x) + 1) * xNula;
      }
    } else if (whichIconShowed==0 and direction!=5) {
      whichIconShowed=2;
    }
    if (whichIconShowed==2) {
      ShowBorders(1);
      if (direction==5) {
        soundState=!soundState;
        EEPROM.write(addressSound,soundState);
        if (soundState==true) {
          tone(soundPin,147);
          delay(100);
          noTone(soundPin);
        }
      }
    } else if (whichIconShowed==1) {
      ShowBorders(0);
      if (direction==5) {
        activatorV = 1;
        whichIconShowed = 0;
        if (soundState==true) {
          tone(soundPin,147);
          delay(100);
          noTone(soundPin);
        }
        for (int index = 0; index < 4; index++) {
          lc.clearDisplay(index);
        }
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
      }
    } else if (whichIconShowed==3) {
      ShowBorders(2);
      if (direction==5) {
        activatorV = 2;
        whichIconShowed = 0;
        if (soundState==true) {
          tone(soundPin,147);
          delay(100);
          noTone(soundPin);
        }
        for (int index = 0; index < 4; index++) {
          lc.clearDisplay(index);
        }
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
      }
    }
  } else if (activatorV==1 or activatorV==2) {
    int direction = IsButtonPressed();
    byte xNula = 1;
    byte yNula = 1;
    if (direction==5) {
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
    } else if (whichIconShowed==0 and direction!=5) {
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
  delay(200);
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
  pX = 7;
  pY = 7;
  x = 0;
  y = 0;
  for (int row = 0; row < 16; row++) {
    for (int col = 0; col < 16; col++) {
      Array[row][col] = 0;
    }
  }
  for (int index = 0; index < 4; index++) {
    lc.clearDisplay(index);
  }
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
void SettingSetting(){
  for (int index = 0; index < 4; index++) {
    lc.clearDisplay(index);
  }
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      ShowText(row,col,pgm_read_byte(&(Sound_icon[row][col])));
    }
  }
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      ShowText(row + 10,col + 2,pgm_read_byte(&(SnakeMenu[row][col])));
    }
  }
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      ShowText(row + 10,col + 10,pgm_read_byte(&(SpaceInvatorsMenu[row][col])));
    }
  }
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
//Turning Leds Snake
void TurningLedArray(int row,int col, int state) {
  if (row < 8 and col < 8) {
    lc.setLed(0, row, col, state);
  } else if (row < 8 and col >= 8) {
    lc.setLed(1, row, col%8, state);
  } else if (row >= 8 and col < 8) {
    lc.setLed(2, row%8, col, state);
  } else if (row >= 8 and col >= 8) {
    lc.setLed(3, row%8, col%8, state);
  } 
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
        lc.setLed(display,map(col,0,7,7,0) - offset,row + offset,pgm_read_byte(&(Dekujeme[row][col])));
      }
    }
  }
  if (what=='n') {
    for (int row = 0; row < 4; row++) {
      for (int col = 0; col < 4; col++) {
        lc.setLed(display,map(col,0,7,7,0) - offset,row + offset,pgm_read_byte(&(Nothing[row][col])));
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
    if (Array[foodX][foodY]==0) {
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
  PlaySound(3,0);
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
int IsButtonPressed() {
  x=0;
  y=0;
  while(true) {
    Buttons();
    if (x>0) {
      return 1;
    }
    if (x<0) {
      return 2;
    }
    if (y>0) {
      return 3;
    }
    if (y<0) {
      return 4;
    }
    if (digitalRead(activate)==1) {
      return 5;
    }
    delay(75);
  }
}
void ActiveButton() {
  while (true) {
    if (digitalRead(activate) == 1) {
      break;
    }
  }
}
void PlaySound(int sound, int i) {
  if (soundState==true) {
    if (sound == 1) {
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
    }
  }
}
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
    DekujemeMoc();
  } else if (gameState==5) {
    NastaveniHer();
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
    } else {
      soundDelay=0;
    }
  }
  //delay(10);
}
