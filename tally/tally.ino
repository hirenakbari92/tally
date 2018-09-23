#include <Arduino.h>
#include <avr/pgmspace.h>
//#include <SPI.h>
#include "U8g2lib.h"

#include <SdFat.h>
#define SD_CS_PIN   A4
SdFat sd;

// object - display library.
U8G2_SSD1322_NHD_128X64_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 8, /* dc=*/ A3, /* reset=*/ 9);

// General definition
#define PRESSED     0 // when switch will be pressed arduino will receive this state
#define UNPRESSED   1

#define INC         1
#define DEC         0

// Switches interfacing
#define SW_POWER  1
#define SW_MENU   2
#define SW_UP     3
#define SW_DOWN   4
#define SW_LEFT   5
#define SW_RIGHT  6
#define SW_ENTER  7
//uint8_t SW_arr[7] = {SW_POWER, SW_MENU, SW_UP, SW_DOWN, SW_LEFT, SW_RIGHT, SW_ENTER};

// variables for state mechanism 
#define DISPLAY_OFF                     0
#define DISPLAY_INTRO                   1
#define DISPLAY_ERROR_SD_CARD           2
#define DISPLAY_CREATE_TALLY            3
#define DISPLAY_LOAD_TALLY              4
#define DISPLAY_LABEL_INPUT_1           5
#define DISPLAY_LABEL_INPUT_2           6
#define DISPLAY_LABEL_INPUT_3           7
#define DISPLAY_LABEL_INPUT_4           8
#define DISPLAY_CONFIRM_LABEL_OK        9
#define DISPLAY_CONFIRM_LABEL_CANCEL    10
#define DISPLAY_OFFSET_INPUT_1          11
#define DISPLAY_OFFSET_INPUT_2          12
#define DISPLAY_OFFSET_INPUT_3          13
#define DISPLAY_CONFIRM_OFFSET_OK       14
#define DISPLAY_CONFIRM_OFFSET_CANCEL   15

uint8_t curr_state, last_state;

// User variable
char nameOfTally_ch1[2] = {'A', 0};
char nameOfTally_ch2[2] = {'B', 0};
char nameOfTally_ch3[2] = {'C', 0};
char nameOfTally_ch4[2] = {'1', 0};
char offsetInMM_ch1[2] = {'0', 0};
char offsetInMM_ch2[2] = {'0', 0};
char offsetInMM_ch3[2] = {'0', 0};

void setup(void)
{
  // Switches - Enable internal pull-ups
  digitalWrite(SW_POWER, HIGH);
  digitalWrite(SW_MENU, HIGH);
  digitalWrite(SW_UP, HIGH);
  digitalWrite(SW_DOWN, HIGH);
  digitalWrite(SW_LEFT, HIGH);
  digitalWrite(SW_RIGHT, HIGH);
  digitalWrite(SW_ENTER, HIGH);

//  // Switches - set pin as INPUT pin.
  pinMode(SW_POWER, INPUT);
  pinMode(SW_MENU, INPUT);
  pinMode(SW_UP, INPUT);
  pinMode(SW_DOWN, INPUT);
  pinMode(SW_LEFT, INPUT);
  pinMode(SW_RIGHT, INPUT);
  pinMode(SW_ENTER, INPUT);

  Serial.begin(115200);
  Serial.println(F("Application is starting ... "));

  // Initialize display
  u8g2.begin();
  u8g2.clearDisplay();
   
  // while(digitalRead(SW_POWER) == UNPRESSED);
  delay(100);

  iniSDcard();
  sd.mkdir(".cache");
  
  displayErrorSdCard();
  displayCreateTally();
  displayLoadTally();
  displayConfirmLabelOk();
  displayConfirmLabelCancel();
  displayConfirmOffsetOk();
  displayConfirmOffsetCancel();
  displayMenuHud();
  displayMenuEdit();
  displayMenuMemory();
  displayMenuSend();
  displayMenuSettings();
  
  curr_state = last_state = DISPLAY_CREATE_TALLY;
  
  Serial.println(F("key navigation started ... "));
}

void loop(void)
{
  keySwitchNavigation();
}

// ********************************** Display function ************************************ //
// * Need to set font size and style (must be checked on actual display)

const char str_titleee[] PROGMEM = "titleee";
const char str_starting[] PROGMEM = "starting ...";
void displayIntro(void)
{
  char buffer[30];
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB14_tf);
  strcpy_P(buffer, str_titleee);
  u8g2.drawStr(32,26,buffer);
  u8g2.drawHLine(11,31,105);
  u8g2.setFont(u8g2_font_ncenB08_tf);
  strcpy_P(buffer, str_starting);
  u8g2.drawStr(38,43,buffer);
  u8g2.sendBuffer();
}

const char str_error[] PROGMEM = "error";
const char str_please_insert_an[] PROGMEM = "Please insert an";
const char str_sd_card_into_the[] PROGMEM = "sd card into the";
const char str_machine[] PROGMEM = "machine. . .";
const char str_OK[] PROGMEM = "OK";
void displayErrorSdCard(void)
{
  char buffer[30];
  u8g2.setDrawColor(1);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tf);
  strcpy_P(buffer, str_error);  
  u8g2.drawStr(13,8,buffer);
  strcpy_P(buffer, str_please_insert_an);  
  u8g2.drawStr(25,23,buffer);
  strcpy_P(buffer, str_sd_card_into_the);  
  u8g2.drawStr(25,36,buffer);
  strcpy_P(buffer, str_machine);  
  u8g2.drawStr(25,47,buffer);
  u8g2.drawHLine(3,5,4);
  u8g2.drawVLine(3,5,52);
  u8g2.drawHLine(3,57,79);
  u8g2.drawHLine(117,57,8);
  u8g2.drawVLine(124,5,52);
  u8g2.drawHLine(45,5,80);
  u8g2.drawBox(85,51,30,12);
  u8g2.setDrawColor(0);
  strcpy_P(buffer, str_OK);
  u8g2.drawStr(92,61,buffer);
  u8g2.sendBuffer();
}

const char str_create_new_tally[] PROGMEM = "Create new tally";
const char str_new[] PROGMEM = "new";
const char str_load[] PROGMEM = "load";
void displayCreateTally(void)
{
  char buffer[30];
  u8g2.setDrawColor(1);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tf);
  strcpy_P(buffer, str_create_new_tally);
  u8g2.drawStr(19,24,buffer);
  u8g2.drawFrame(75,36,32,13);
  strcpy_P(buffer, str_load);
  u8g2.drawStr(80,46,buffer);
  u8g2.drawBox(24,36,34,13);
  u8g2.setDrawColor(0);
  strcpy_P(buffer, str_new);
  u8g2.drawStr(30,45,buffer);
  u8g2.sendBuffer();
}
const char str_load_old_tally[] PROGMEM = "Load old tally";
void displayLoadTally(void)
{
  char buffer[30];
  u8g2.setDrawColor(1);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tf);
  strcpy_P(buffer, str_load_old_tally);
  u8g2.drawStr(24,22,buffer);
  u8g2.drawFrame(22,36,36,13);
  strcpy_P(buffer, str_new);
  u8g2.drawStr(30,45,buffer);
  u8g2.drawBox(75,36,32,13);
  u8g2.setDrawColor(0);
  strcpy_P(buffer, str_load);
  u8g2.drawStr(80,46,buffer);
  u8g2.sendBuffer();
}

void drawUpArrow(uint8_t x_pos, uint8_t y_pos)
{
  u8g2.drawLine(x_pos,y_pos,x_pos+3,y_pos-7);
  u8g2.drawLine(x_pos+3,y_pos-7,x_pos+6,y_pos);
  u8g2.drawHLine(x_pos,y_pos,6);
}
void drawDownArrow(uint8_t x_pos, uint8_t y_pos)
{
  u8g2.drawLine(x_pos,y_pos,x_pos+3,y_pos+7);
  u8g2.drawLine(x_pos+3,y_pos+7,x_pos+6,y_pos);
  u8g2.drawHLine(x_pos,y_pos,6);
}

const char str_enter[] PROGMEM = "enter";
const char str_label[] PROGMEM = "label";
void displayLabelInput1()
{
  char buffer[30];
  u8g2.setDrawColor(1);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tf);
  strcpy_P(buffer, str_enter);
  u8g2.drawStr(17,29,buffer);
  strcpy_P(buffer, str_label);
  u8g2.drawStr(17,43,buffer);

//  u8g2.drawTriangle(55,18,58,11,61,18);
//  u8g2.drawTriangle(55,43,58,50,61,43);
  drawUpArrow(55,18);
  drawDownArrow(55,43);
  u8g2.drawStr(55,35,nameOfTally_ch1);
  u8g2.drawStr(71,35,nameOfTally_ch2);
  u8g2.drawStr(90,35,nameOfTally_ch3);
  u8g2.drawStr(108,35,nameOfTally_ch4);
  u8g2.sendBuffer();
}
void displayLabelInput2()
{
  char buffer[30];
  u8g2.setDrawColor(1);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tf);
  strcpy_P(buffer, str_enter);
  u8g2.drawStr(17,29,buffer);
  strcpy_P(buffer, str_label);
  u8g2.drawStr(17,43,buffer);

  drawUpArrow(71,18);
  drawDownArrow(71,43);
  u8g2.drawStr(55,35,nameOfTally_ch1);
  u8g2.drawStr(71,35,nameOfTally_ch2);
  u8g2.drawStr(90,35,nameOfTally_ch3);
  u8g2.drawStr(108,35,nameOfTally_ch4);
  u8g2.sendBuffer();
}
void displayLabelInput3()
{
  char buffer[30];
  u8g2.setDrawColor(1);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tf);
  strcpy_P(buffer, str_enter);
  u8g2.drawStr(17,29,buffer);
  strcpy_P(buffer, str_label);
  u8g2.drawStr(17,43,buffer);

  drawUpArrow(90,18);
  drawDownArrow(90,43);
  u8g2.drawStr(55,35,nameOfTally_ch1);
  u8g2.drawStr(71,35,nameOfTally_ch2);
  u8g2.drawStr(90,35,nameOfTally_ch3);
  u8g2.drawStr(108,35,nameOfTally_ch4);
  u8g2.sendBuffer();
}
void displayLabelInput4()
{
  char buffer[30];
  u8g2.setDrawColor(1);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tf);
  strcpy_P(buffer, str_enter);
  u8g2.drawStr(17,29,buffer);
  strcpy_P(buffer, str_label);
  u8g2.drawStr(17,43,buffer);

  drawUpArrow(108,18);
  drawDownArrow(108,43);
  u8g2.drawStr(55,35,nameOfTally_ch1);
  u8g2.drawStr(71,35,nameOfTally_ch2);
  u8g2.drawStr(90,35,nameOfTally_ch3);
  u8g2.drawStr(108,35,nameOfTally_ch4);
  u8g2.sendBuffer();
}

const char str_confirm[] PROGMEM = "confirm";
const char str_do_you_want_to_name[] PROGMEM = "Do you want to name";
const char str_this_tally_var[] PROGMEM = "this tally";
const char str_CANCEL[] PROGMEM = "CANCEL";
void displayConfirmLabelOk(void)
{
  char buffer[30];
  u8g2.setDrawColor(1);
  u8g2.clearBuffer();
  u8g2.drawHLine(3,5,4);
  u8g2.drawVLine(3,5,52);
  u8g2.drawHLine(3,57,5);
  u8g2.drawHLine(65,57,18);
  u8g2.drawHLine(117,57,8);
  u8g2.drawVLine(124,5,52);
  u8g2.drawHLine(63,5,62);
  
  u8g2.setFont(u8g2_font_ncenB08_tf);
  strcpy_P(buffer, str_confirm);  
  u8g2.drawStr(13,8,buffer);
  strcpy_P(buffer, str_do_you_want_to_name);  
  u8g2.drawStr(13,23,buffer);
  strcpy_P(buffer, str_this_tally_var);  
  u8g2.drawStr(13,36,buffer);
  strcpy_P(buffer, str_CANCEL);  
  u8g2.drawStr(13,61,buffer);

  u8g2.drawStr(68,36,nameOfTally_ch1);
  u8g2.drawStr(77,36,nameOfTally_ch2);
  u8g2.drawStr(86,36,nameOfTally_ch3);
  u8g2.drawStr(95,36,nameOfTally_ch4);
  u8g2.drawStr(104,36,"?");
  
  u8g2.drawFrame(10,51,52,12);

  u8g2.drawBox(85,51,30,12);
  u8g2.setDrawColor(0);
  strcpy_P(buffer, str_OK);
  u8g2.drawStr(92,61,buffer);
  u8g2.sendBuffer();
}
void displayConfirmLabelCancel(void)
{
  char buffer[30];
  u8g2.setDrawColor(1);
  u8g2.clearBuffer();
  u8g2.drawHLine(3,5,4);
  u8g2.drawVLine(3,5,52);
  u8g2.drawHLine(3,57,5);
  u8g2.drawHLine(65,57,18);
  u8g2.drawHLine(117,57,8);
  u8g2.drawVLine(124,5,52);
  u8g2.drawHLine(63,5,62);
  
  u8g2.setFont(u8g2_font_ncenB08_tf);
  strcpy_P(buffer, str_confirm);  
  u8g2.drawStr(13,8,buffer);
  strcpy_P(buffer, str_do_you_want_to_name);  
  u8g2.drawStr(13,23,buffer);
  strcpy_P(buffer, str_this_tally_var);  
  u8g2.drawStr(13,36,buffer);

  u8g2.drawStr(68,36,nameOfTally_ch1);
  u8g2.drawStr(77,36,nameOfTally_ch2);
  u8g2.drawStr(86,36,nameOfTally_ch3);
  u8g2.drawStr(95,36,nameOfTally_ch4);
  u8g2.drawStr(104,36,"?");
  
  u8g2.drawFrame(85,51,30,12);  
  strcpy_P(buffer, str_OK);
  u8g2.drawStr(92,61,buffer);

  u8g2.drawBox(10,51,52,12);
  u8g2.setDrawColor(0);
  strcpy_P(buffer, str_CANCEL);  
  u8g2.drawStr(13,61,buffer);
  
  u8g2.sendBuffer();
}

const char str_offset[] PROGMEM = "offset";
const char str_mm[] PROGMEM = "mm";
const char str_b_mm[] PROGMEM = "(mm)";
void displayOffsetInput1()
{
  char buffer[30];
  u8g2.setDrawColor(1);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tf);
  strcpy_P(buffer, str_enter);
  u8g2.drawStr(12,20,buffer);
  strcpy_P(buffer, str_offset);
  u8g2.drawStr(10,35,buffer);
  strcpy_P(buffer, str_b_mm);
  u8g2.drawStr(10,48,buffer);
  strcpy_P(buffer, str_mm);
  u8g2.drawStr(105,33,buffer);

  u8g2.setFont(u8g2_font_ncenB14_tf);
  drawUpArrow(57,18);
  drawDownArrow(57,43);
  u8g2.drawStr(55,38,offsetInMM_ch1);
  u8g2.drawStr(71,38,offsetInMM_ch2);
  u8g2.drawStr(87,38,offsetInMM_ch3);
  u8g2.sendBuffer();
}
void displayOffsetInput2()
{
  char buffer[30];
  u8g2.setDrawColor(1);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tf);
  strcpy_P(buffer, str_enter);
  u8g2.drawStr(12,20,buffer);
  strcpy_P(buffer, str_offset);
  u8g2.drawStr(10,35,buffer);
  strcpy_P(buffer, str_b_mm);
  u8g2.drawStr(10,48,buffer);
  strcpy_P(buffer, str_mm);
  u8g2.drawStr(105,33,buffer);

  u8g2.setFont(u8g2_font_ncenB14_tf);
  drawUpArrow(73,18);
  drawDownArrow(73,43);
  u8g2.drawStr(55,38,offsetInMM_ch1);
  u8g2.drawStr(71,38,offsetInMM_ch2);
  u8g2.drawStr(87,38,offsetInMM_ch3);
  u8g2.sendBuffer();
}
void displayOffsetInput3()
{
  char buffer[30];
  u8g2.setDrawColor(1);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tf);
  strcpy_P(buffer, str_enter);
  u8g2.drawStr(12,20,buffer);
  strcpy_P(buffer, str_offset);
  u8g2.drawStr(10,35,buffer);
  strcpy_P(buffer, str_b_mm);
  u8g2.drawStr(10,48,buffer);
  strcpy_P(buffer, str_mm);
  u8g2.drawStr(105,33,buffer);

  u8g2.setFont(u8g2_font_ncenB14_tf);
  drawUpArrow(89,18);
  drawDownArrow(89,43);
  u8g2.drawStr(55,38,offsetInMM_ch1);
  u8g2.drawStr(71,38,offsetInMM_ch2);
  u8g2.drawStr(87,38,offsetInMM_ch3);
  u8g2.sendBuffer();
}

const char str_you_entered_an[] PROGMEM = "You entered an";
const char str_offset_of[] PROGMEM = "offset of ";
const char str_Is_this_correct[] PROGMEM = "Is this correct?";
const char str_Is_mm_d[] PROGMEM = "mm.";
void displayConfirmOffsetOk(void)
{
  char buffer[30];
  u8g2.setDrawColor(1);
  u8g2.clearBuffer();
  u8g2.drawHLine(3,5,4);
  u8g2.drawVLine(3,5,52);
  u8g2.drawHLine(3,57,5);
  u8g2.drawHLine(65,57,18);
  u8g2.drawHLine(117,57,8);
  u8g2.drawVLine(124,5,52);
  u8g2.drawHLine(63,5,62);
  
  u8g2.setFont(u8g2_font_ncenB08_tf);
  strcpy_P(buffer, str_confirm);  
  u8g2.drawStr(13,8,buffer);
  strcpy_P(buffer, str_you_entered_an);  
  u8g2.drawStr(23,20,buffer);
  strcpy_P(buffer, str_offset_of);  
  u8g2.drawStr(23,33,buffer);
  strcpy_P(buffer, str_Is_this_correct);  
  u8g2.drawStr(23,46,buffer);

  u8g2.drawStr(74,33,offsetInMM_ch1);
  u8g2.drawStr(80,33,offsetInMM_ch2);
  u8g2.drawStr(86,33,offsetInMM_ch3);
  strcpy_P(buffer, str_Is_mm_d);  
  u8g2.drawStr(94,33,buffer);

  strcpy_P(buffer, str_CANCEL);  
  u8g2.drawStr(13,61,buffer);
  u8g2.drawFrame(10,51,52,12);

  u8g2.drawBox(85,51,30,12);
  u8g2.setDrawColor(0);
  strcpy_P(buffer, str_OK);
  u8g2.drawStr(92,61,buffer);
  u8g2.sendBuffer();
}
void displayConfirmOffsetCancel(void)
{
  char buffer[30];
  u8g2.setDrawColor(1);
  u8g2.clearBuffer();
  u8g2.drawHLine(3,5,4);
  u8g2.drawVLine(3,5,52);
  u8g2.drawHLine(3,57,5);
  u8g2.drawHLine(65,57,18);
  u8g2.drawHLine(117,57,8);
  u8g2.drawVLine(124,5,52);
  u8g2.drawHLine(63,5,62);
  
  u8g2.setFont(u8g2_font_ncenB08_tf);
  strcpy_P(buffer, str_confirm);  
  u8g2.drawStr(13,8,buffer);
  strcpy_P(buffer, str_you_entered_an);  
  u8g2.drawStr(23,20,buffer);
  strcpy_P(buffer, str_offset_of);  
  u8g2.drawStr(23,33,buffer);
  strcpy_P(buffer, str_Is_this_correct);  
  u8g2.drawStr(23,46,buffer);

  u8g2.drawStr(74,33,offsetInMM_ch1);
  u8g2.drawStr(80,33,offsetInMM_ch2);
  u8g2.drawStr(86,33,offsetInMM_ch3);
  strcpy_P(buffer, str_Is_mm_d);  
  u8g2.drawStr(94,33,buffer);

  u8g2.drawFrame(85,51,30,12);
  strcpy_P(buffer, str_OK);
  u8g2.drawStr(92,61,buffer);
  
  u8g2.drawBox(10,51,52,12);
  u8g2.setDrawColor(0);  
  strcpy_P(buffer, str_CANCEL);  
  u8g2.drawStr(13,61,buffer);
  u8g2.sendBuffer();
}

const char str_menu[] PROGMEM = "menu";
const char str_HUD[] PROGMEM = "HUD";
const char str_EDIT[] PROGMEM = "EDIT";
const char str_MEMORY[] PROGMEM = "MEMORY";
const char str_SEND[] PROGMEM = "SEND";
const char str_SETTINGS[] PROGMEM = "SETTINGS";
void displayMenuHud(void)
{
  char buffer[30];
  u8g2.setDrawColor(1);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tf);
  u8g2.drawHLine(42,12,36);
  
  strcpy_P(buffer, str_menu);
  u8g2.drawStr(45,10,buffer);
  strcpy_P(buffer, str_EDIT);
  u8g2.drawStr(47,42,buffer);
  strcpy_P(buffer, str_MEMORY);
  u8g2.drawStr(35,57,buffer);
  
  u8g2.drawBox(8,18,112,10);
  u8g2.setDrawColor(0);  
  strcpy_P(buffer, str_HUD);  
  u8g2.drawStr(48,27,buffer);
  u8g2.sendBuffer();
}
void displayMenuEdit(void)
{
  char buffer[30];
  u8g2.setDrawColor(1);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tf);
  u8g2.drawHLine(42,12,36);
  
  strcpy_P(buffer, str_menu);
  u8g2.drawStr(45,10,buffer);
  strcpy_P(buffer, str_HUD);  
  u8g2.drawStr(48,27,buffer);
  strcpy_P(buffer, str_MEMORY);
  u8g2.drawStr(35,57,buffer);
  
  u8g2.drawBox(8,33,112,10);
  u8g2.setDrawColor(0);  
  strcpy_P(buffer, str_EDIT);
  u8g2.drawStr(47,42,buffer);
  u8g2.sendBuffer();
}
void displayMenuMemory(void)
{
  char buffer[30];
  u8g2.setDrawColor(1);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tf);
  u8g2.drawHLine(42,12,36);
  
  strcpy_P(buffer, str_menu);
  u8g2.drawStr(45,10,buffer);
  strcpy_P(buffer, str_EDIT);  
  u8g2.drawStr(47,27,buffer);
  strcpy_P(buffer, str_SEND);
  u8g2.drawStr(47,57,buffer);
  
  u8g2.drawBox(8,33,112,10);
  u8g2.setDrawColor(0);  
  strcpy_P(buffer, str_MEMORY);
  u8g2.drawStr(35,42,buffer);
  u8g2.sendBuffer();
}
void displayMenuSend(void)
{
  char buffer[30];
  u8g2.setDrawColor(1);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tf);
  u8g2.drawHLine(42,12,36);
  
  strcpy_P(buffer, str_menu);
  u8g2.drawStr(45,10,buffer);
  strcpy_P(buffer, str_MEMORY);  
  u8g2.drawStr(35,27,buffer);
  strcpy_P(buffer, str_SETTINGS);
  u8g2.drawStr(34,57,buffer);
  
  u8g2.drawBox(8,33,112,10);
  u8g2.setDrawColor(0);  
  strcpy_P(buffer, str_SEND);
  u8g2.drawStr(47,42,buffer);
  u8g2.sendBuffer();
}
void displayMenuSettings(void)
{
  char buffer[30];
  u8g2.setDrawColor(1);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tf);
  u8g2.drawHLine(42,12,36);
  
  strcpy_P(buffer, str_menu);
  u8g2.drawStr(45,10,buffer);
  strcpy_P(buffer, str_MEMORY);  
  u8g2.drawStr(35,27,buffer);
  strcpy_P(buffer, str_SEND);
  u8g2.drawStr(47,42,buffer);
  
  u8g2.drawBox(8,48,112,10);
  u8g2.setDrawColor(0);  
  strcpy_P(buffer, str_SETTINGS);
  u8g2.drawStr(34,57,buffer);
  u8g2.sendBuffer();
}

// **************************** Key navigation and state cycle functions *************************** //
/*
 * Key switch detection can be improved.
 * Label and offset display cursor position change mechanism can be optimized to eliminate full screen
 *      refresh time.
 * Menu display is not yet added in state cycle.
 */

void keySwitchNavigation(void)
{
  // To remove de-bouncing, 100 miliSecond waiting time is added after each key press detection.
  // If multiple keys are pressed, it will be served in sequence - MENU, UP, DOWN, LEFT, RIGHT, ENTER
  if(digitalRead(SW_POWER))
  {
    delay(100);
    if(digitalRead(SW_POWER))
    {
      state_update(SW_POWER);
    }
  }
  if(digitalRead(SW_MENU))
  {
    delay(100);
    if(digitalRead(SW_MENU))
    {
      state_update(SW_MENU);
    }
  }
  if(digitalRead(SW_UP))
  {
    delay(100);
    if(digitalRead(SW_UP))
    {
      state_update(SW_UP);
    }
  }
  if(digitalRead(SW_DOWN))
  {
    delay(100);
    if(digitalRead(SW_DOWN))
    {
      state_update(SW_DOWN);
    }
  }
  if(digitalRead(SW_LEFT))
  {
    delay(100);
    if(digitalRead(SW_LEFT))
    {
      state_update(SW_LEFT);
    }
  }
  if(digitalRead(SW_RIGHT))
  {
    delay(100);
    if(digitalRead(SW_RIGHT))
    {
      state_update(SW_RIGHT);
    }
  }
  if(digitalRead(SW_ENTER))
  {
    delay(100);
    if(digitalRead(SW_ENTER))
    {
      state_update(SW_ENTER);
    }
  }
}
void state_update(uint8_t pressedKey)
{
  // if user press power key after device is already initialized, then it will clear display and reset
  // status. After that if user press power button again, it will allow to re-initialized thorugh Switch-case.
  if((pressedKey == SW_POWER) && (curr_state != DISPLAY_OFF))
  {
    u8g2.clearDisplay();
    curr_state = DISPLAY_OFF;
    return;
  }
  switch(curr_state)
  {
    case DISPLAY_OFF:
    {
      if(pressedKey == SW_POWER)
      {
        iniSDcard();
        displayCreateTally();
        curr_state = last_state = DISPLAY_CREATE_TALLY; 
      }
    }break;
    case DISPLAY_INTRO:
    {
      displayIntro();
    }break;
    case DISPLAY_ERROR_SD_CARD:
    {
      displayErrorSdCard();
    }break;
    case DISPLAY_CREATE_TALLY:
    {
      if(pressedKey == SW_RIGHT)
      {
        displayLoadTally();
        curr_state = DISPLAY_LOAD_TALLY;
      }
      else if(pressedKey == SW_ENTER)
      {
        displayLabelInput1();
        curr_state = DISPLAY_LABEL_INPUT_1;
      }
    }break;
    case DISPLAY_LOAD_TALLY:
    {
      if(pressedKey == SW_LEFT)
      {
        displayCreateTally();
        curr_state = DISPLAY_CREATE_TALLY;
      }
      else if(pressedKey == SW_ENTER)
      {
        // Screen 10 - Yet to finalize.
      }
    }break;
    case DISPLAY_LABEL_INPUT_1:
    {
      if(pressedKey == SW_RIGHT)
      {
        displayLabelInput2();
        curr_state = DISPLAY_LABEL_INPUT_2;
      }
      else if(pressedKey == SW_UP)
      {
        updateTallyChar(nameOfTally_ch1, INC);
        displayLabelInput1();
      }
      else if(pressedKey == SW_DOWN)
      {
        updateTallyChar(nameOfTally_ch1, DEC);
        displayLabelInput1();
      }
      else if(pressedKey == SW_ENTER)
      {
        displayConfirmLabelOk();
        curr_state = DISPLAY_CONFIRM_LABEL_OK;
      }
    }break;
    case DISPLAY_LABEL_INPUT_2:
    {
      if(pressedKey == SW_LEFT)
      {
        displayLabelInput1();
        curr_state = DISPLAY_LABEL_INPUT_1;
      }
      else if(pressedKey == SW_RIGHT)
      {
        displayLabelInput3();
        curr_state = DISPLAY_LABEL_INPUT_3;
      }
      else if(pressedKey == SW_UP)
      {
        updateTallyChar(nameOfTally_ch2, INC);
        displayLabelInput2();
      }
      else if(pressedKey == SW_DOWN)
      {
        updateTallyChar(nameOfTally_ch2, DEC);
        displayLabelInput2();
      }
      else if(pressedKey == SW_ENTER)
      {
        displayConfirmLabelOk();
        curr_state = DISPLAY_CONFIRM_LABEL_OK;
      }
    }break;
    case DISPLAY_LABEL_INPUT_3:
    {
      if(pressedKey == SW_LEFT)
      {
        displayLabelInput2();
        curr_state = DISPLAY_LABEL_INPUT_2;
      }
      else if(pressedKey == SW_RIGHT)
      {
        displayLabelInput4();
        curr_state = DISPLAY_LABEL_INPUT_4;
      }
      else if(pressedKey == SW_UP)
      {
        updateTallyChar(nameOfTally_ch3, INC);
        displayLabelInput3();
      }
      else if(pressedKey == SW_DOWN)
      {
        updateTallyChar(nameOfTally_ch3, DEC);
        displayLabelInput3();
      }
      else if(pressedKey == SW_ENTER)
      {
        displayConfirmLabelOk();
        curr_state = DISPLAY_CONFIRM_LABEL_OK;
      }
    }break;
    case DISPLAY_LABEL_INPUT_4:
    {
      if(pressedKey == SW_LEFT)
      {
        displayLabelInput3();
        curr_state = DISPLAY_LABEL_INPUT_3;
      }
      else if(pressedKey == SW_UP)
      {
        updateTallyNum(nameOfTally_ch4, INC);
        displayLabelInput4();
      }
      else if(pressedKey == SW_DOWN)
      {
        updateTallyNum(nameOfTally_ch4, DEC);
        displayLabelInput4();
      }
      else if(pressedKey == SW_ENTER)
      {
        displayConfirmLabelOk();
        curr_state = DISPLAY_CONFIRM_LABEL_OK;
      }
    }break;
    case DISPLAY_CONFIRM_LABEL_OK:
    {
      if(pressedKey == SW_LEFT)
      {
        displayConfirmLabelCancel();
        curr_state = DISPLAY_CONFIRM_LABEL_CANCEL;
      }
      else if(pressedKey == SW_ENTER)
      {
        displayOffsetInput1();
        curr_state = DISPLAY_OFFSET_INPUT_1;
      }
    }break;
    case DISPLAY_CONFIRM_LABEL_CANCEL:
    {
      if(pressedKey == SW_RIGHT)
      {
        displayConfirmLabelOk();
        curr_state = DISPLAY_CONFIRM_LABEL_OK;
      }
      else if(pressedKey == SW_ENTER)
      {
        displayLabelInput1();
        curr_state = DISPLAY_LABEL_INPUT_1;
      }
    }break;
    case DISPLAY_OFFSET_INPUT_1:
    {
      if(pressedKey == SW_RIGHT)
      {
        displayOffsetInput2();
        curr_state = DISPLAY_OFFSET_INPUT_2;
      }
      else if(pressedKey == SW_UP)
      {
        updateTallyNum(offsetInMM_ch1, INC);
        displayOffsetInput1();
      }
      else if(pressedKey == SW_DOWN)
      {
        updateTallyNum(offsetInMM_ch1, DEC);
        displayOffsetInput1();
      }
      else if(pressedKey == SW_ENTER)
      {
        displayConfirmOffsetOk();
        curr_state = DISPLAY_CONFIRM_OFFSET_OK;
      }
    }break;
    case DISPLAY_OFFSET_INPUT_2:
    {
      //yet to implement - displayOffsetInput2(nameOfTally);
    }break;
    case DISPLAY_OFFSET_INPUT_3:
    {
      //yet to implement - displayOffsetInput3(nameOfTally);
    }break;
    case DISPLAY_CONFIRM_OFFSET_OK:
    {
      //yet to implement - displayConfirmOffsetOk();
    }break;
    case DISPLAY_CONFIRM_OFFSET_CANCEL:
    {
     //yet to implement -  displayConfirmOffsetCancel();
    }break;
    default:
    break;
  }
}
void iniSDcard(void)
{
  do
  {
    // Display intro Screen
    displayIntro();
    delay(3000);
    if (!sd.begin(SD_CS_PIN, SD_SCK_MHZ(50))) //(SD card not present)
    {
      displayErrorSdCard();  
    }
    else
    {
      break;
    }
    while(1)
    {
      if(digitalRead(SW_ENTER) == PRESSED)
      {
        delay(100);
        if(digitalRead(SW_ENTER) == PRESSED)
        {
          break;
        }
      }
    }
  }while(1); 
}

// ******************************* PRIVATE function **********************************//
void updateTallyChar(char *arr, int8_t update_inst)
{
  char temp = arr[0];

  if((temp < 0x41) || (temp > 0x5A)){  temp = 0x41;} // set default to A, if out of range
  
  if(update_inst == INC)
  {
    temp++;
    if(temp > 0x5A){ temp = 0x41;}
  }
  else if(update_inst == DEC)
  {
    temp--;
    if(temp < 0x41){ temp = 0x5A;}
  }
  arr[0] = temp;
}
void updateTallyNum(char *arr, int8_t update_inst)
{
  char temp = arr[0];

  if((temp < 0x30) || (temp > 0x39)){  temp = 0x30;}   // set default to 0, if out of range

  if(update_inst == INC)
  {
    temp++;
    if(temp > 0x39){ temp = 0x30;}
  }
  else if(update_inst == DEC)
  {
    temp--;
    if(temp < 0x30){ temp = 0x39;}
  }
  arr[0] = temp;
}

