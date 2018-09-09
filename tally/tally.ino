#include <Arduino.h>
//#include <SPI.h>
#include "U8x8lib.h"
#include "U8g2lib.h"

//#include <SdFat.h>
//#include "SpiRAM.h"


/*
 * Yet to decide SD_CS_PIN, all switch pin
 */
#define SD_CS_PIN   A4
//SdFat sd;

#define SS_PIN   A5
//SpiRAM SpiRam(0, SS_PIN);

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

uint8_t SW_arr[7] = {SW_POWER, SW_MENU, SW_UP, SW_DOWN, SW_LEFT, SW_RIGHT, SW_ENTER};

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
char nameOfTally[4] = {'A', 'A', 'A', '1'};
char offsetInMM[3] = {'0', '0', '0'};

void setup(void)
{
  // Switches - Enable internal pull-ups
//  digitalWrite(SW_POWER, HIGH);
//  digitalWrite(SW_MENU, HIGH);
//  digitalWrite(SW_UP, HIGH);
//  digitalWrite(SW_DOWN, HIGH);
//  digitalWrite(SW_LEFT, HIGH);
//  digitalWrite(SW_RIGHT, HIGH);
//  digitalWrite(SW_ENTER, HIGH);
//
//  // Switches - set pin as INPUT pin.
//  pinMode(SW_POWER, INPUT);
//  pinMode(SW_MENU, INPUT);
//  pinMode(SW_UP, INPUT);
//  pinMode(SW_DOWN, INPUT);
//  pinMode(SW_LEFT, INPUT);
//  pinMode(SW_RIGHT, INPUT);
//  pinMode(SW_ENTER, INPUT);

  Serial.begin(115200);
  Serial.println("starting");

  // Initialize display
  u8g2.begin();
  u8g2.clearDisplay();
  Serial.println("test1");
   
//  while(digitalRead(SW_POWER) == UNPRESSED);
  delay(100);

  //iniSDcard();
  displayLoadTally();

  //displayIntro();
  
  curr_state = last_state = DISPLAY_CREATE_TALLY;
  
  Serial.println("init display");
  while(1);
}

void loop(void)
{
  keySwitchNavigation();
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
        displayLabelInput1(nameOfTally);
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
        displayLabelInput2(nameOfTally);
        curr_state = DISPLAY_LABEL_INPUT_2;
      }
      else if(pressedKey == SW_UP)
      {
        updateTallyChar(nameOfTally, 1, INC);
        displayLabelInput1(nameOfTally);
      }
      else if(pressedKey == SW_DOWN)
      {
        updateTallyChar(nameOfTally, 1, DEC);
        displayLabelInput1(nameOfTally);
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
        displayLabelInput1(nameOfTally);
        curr_state = DISPLAY_LABEL_INPUT_1;
      }
      else if(pressedKey == SW_RIGHT)
      {
        displayLabelInput3(nameOfTally);
        curr_state = DISPLAY_LABEL_INPUT_3;
      }
      else if(pressedKey == SW_UP)
      {
        updateTallyChar(nameOfTally, 2, INC);
        displayLabelInput2(nameOfTally);
      }
      else if(pressedKey == SW_DOWN)
      {
        updateTallyChar(nameOfTally, 2, DEC);
        displayLabelInput2(nameOfTally);
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
        displayLabelInput2(nameOfTally);
        curr_state = DISPLAY_LABEL_INPUT_2;
      }
      else if(pressedKey == SW_RIGHT)
      {
        displayLabelInput4(nameOfTally);
        curr_state = DISPLAY_LABEL_INPUT_4;
      }
      else if(pressedKey == SW_UP)
      {
        updateTallyChar(nameOfTally, 3, INC);
        displayLabelInput3(nameOfTally);
      }
      else if(pressedKey == SW_DOWN)
      {
        updateTallyChar(nameOfTally, 3, DEC);
        displayLabelInput3(nameOfTally);
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
        displayLabelInput3(nameOfTally);
        curr_state = DISPLAY_LABEL_INPUT_3;
      }
      else if(pressedKey == SW_UP)
      {
        updateTallyNum(nameOfTally, 4, INC);
        displayLabelInput4(nameOfTally);
      }
      else if(pressedKey == SW_DOWN)
      {
        updateTallyNum(nameOfTally, 4, DEC);
        displayLabelInput4(nameOfTally);
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
        displayOffsetInput1(offsetInMM);
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
        displayLabelInput1(nameOfTally);
        curr_state = DISPLAY_LABEL_INPUT_1;
      }
    }break;
    case DISPLAY_OFFSET_INPUT_1:
    {
      if(pressedKey == SW_RIGHT)
      {
        displayOffsetInput2(offsetInMM);
        curr_state = DISPLAY_OFFSET_INPUT_2;
      }
      else if(pressedKey == SW_UP)
      {
        updateTallyNum(offsetInMM, 1, INC);
        displayOffsetInput1(offsetInMM);
      }
      else if(pressedKey == SW_DOWN)
      {
        updateTallyNum(offsetInMM, 1, DEC);
        displayOffsetInput1(offsetInMM);
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
//  do
//  {
//    // Display intro Screen
//    displayIntro();
//    delay(3000);
//    if (!sd.begin(SD_CS_PIN, SD_SCK_MHZ(50))) //(SD card not present)
//    {
//      displayErrorSdCard();  
//    }
//    else
//    {
//      break;
//    }
//    while(1)
//    {
//      if(digitalRead(SW_ENTER) == PRESSED)
//      {
//        delay(100);
//        if(digitalRead(SW_ENTER) == PRESSED)
//        {
//          break;
//        }
//      }
//    }
//  }while(1); 
}

// ********************************** Display function ************************************ //
/*
 * Need to set font size and style (must be checked on actual display)
 * Need to optimize string passed in drawStr function to reduce RAM usage.
 * Need to implement all remaining things mentioned in comment of individual function.
 */
void displayIntro(void)
{
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB14_tf);
  u8g2.drawStr(32,26,"titleee");
  u8g2.drawHLine(11,31,105);
  u8g2.setFont(u8g2_font_ncenB08_tf);
  u8g2.drawStr(38,43,"starting ...");
  u8g2.sendBuffer();
}
void displayErrorSdCard(void)
{
//  u8g2.clearDisplay();
//  // draw square with error and OK box open
////  u8g2.drawStr(13,5,"error");
////  u8g2.drawStr(25,16,"Please insert an");
////  u8g2.drawStr(25,29,"sd card into the");
//  u8g2.drawStr(25,40,"machine. . .");
//  // invert font color for 'OK' text or make inverted filled square box over 'OK' text.
//  u8g2.drawStr(97,51,"OK");
}
void displayCreateTally(void)
{
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tf);
  u8g2.drawStr(19,24,"Create new tally");
  u8g2.drawFrame(75,36,32,13);
  u8g2.drawStr(80,46,"load");
  // drawing text on filled square is not working yet.
  //u8g2.drawBox(22,36,32,13);
  //u8g2.setFontMode(0);
  u8g2.drawStr(30,45,"new");
  u8g2.sendBuffer();
}
void displayLoadTally(void)
{
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tf);
  u8g2.drawStr(24,22,"Load old tally");
  u8g2.drawFrame(22,36,36,13);
  u8g2.drawStr(30,45,"new");
  // drawing text on filled square is not working yet.
  //u8g2.drawBox(75,36,32,13);
  //u8g2.setFontMode(0);
  u8g2.drawStr(80,46,"load");
  u8g2.sendBuffer();
}
void displayLabelInput1(char *var)
{
  u8g2.clearBuffer();
  u8g2.drawStr(17,24,"enter");
  u8g2.drawStr(17,35,"label");
  // draw up and down arrow
  // solve issue : constant does not support in drawStr
  // get name as argument and display it using drawStr
  u8g2.drawStr(55,26,"V");
  u8g2.drawStr(71,26,"A");
  u8g2.drawStr(90,26,"A");
  u8g2.drawStr(108,26,"1");
  u8g2.sendBuffer();
}
void displayLabelInput2(char *var)
{
//  u8g2.clearDisplay();
////  u8g2.drawStr(17,24,"enter");
////  u8g2.drawStr(17,35,"label");
//  // draw up and down arrow
//  // get name as argument and display it using drawStr
//  u8g2.drawStr(55,26,"V");
//  u8g2.drawStr(71,26,"A");
//  u8g2.drawStr(90,26,"A");
//  u8g2.drawStr(108,26,"1");
}
void displayLabelInput3(char *var)
{
//  u8g2.clearDisplay();
////  u8g2.drawStr(17,24,"enter");
////  u8g2.drawStr(17,35,"label");
//  // draw up and down arrow
//  // get name as argument and display it using drawStr
//  u8g2.drawStr(55,26,"V");
//  u8g2.drawStr(71,26,"A");
//  u8g2.drawStr(90,26,"A");
//  u8g2.drawStr(108,26,"1");
}
void displayLabelInput4(char *var)
{
//  u8g2.clearDisplay();
//  u8g2.drawStr(17,24,"enter");
//  u8g2.drawStr(17,35,"label");
//  // draw up and down arrow
//  // get name as argument and display it using drawStr
//  u8g2.drawStr(55,26,"V");
//  u8g2.drawStr(71,26,"A");
//  u8g2.drawStr(90,26,"A");
//  u8g2.drawStr(108,26,"1");
}
void displayConfirmLabelOk(void)
{
//    u8g2.clearDisplay();
//  // draw square with confirm, CANCEL and OK box open
//  u8g2.drawStr(12,5,"confirm");
//  u8g2.drawStr(17,18,"Do you want to name");
//  u8g2.drawStr(17,32,"this tally VAR1?");
//  // draw square box over 'CANCEL' text  
//  u8g2.drawStr(17,51,"CANCEL");
//  // invert font color for 'OK' text or make inverted filled square box over 'OK' text.
//  u8g2.drawStr(97,51,"OK");
}
void displayConfirmLabelCancel(void)
{
//    u8g2.clearDisplay();
//  // draw square with confirm, CANCEL and OK box open
//  u8g2.drawStr(12,5,"confirm");
//  u8g2.drawStr(17,18,"Do you want to name");
//  u8g2.drawStr(17,32,"this tally VAR1?");
//  // invert font color for 'CANCEL' text or make inverted filled square box over 'CANCEL' text.
//  u8g2.drawStr(17,51,"CANCEL");
//  // draw square box over 'OK' text  
//  u8g2.drawStr(97,51,"OK");
}
void displayOffsetInput1(char *var)
{
//  u8g2.clearDisplay();
//  u8g2.drawStr(16,19,"enter");
//  u8g2.drawStr(13,30,"offset");
//  u8g2.drawStr(18,38,"(mm)");
//  u8g2.drawStr(107,29,"mm");
//  // draw up and down arrow
//  // solve issue : constant does not support in drawStr
//  // get name as argument and display it using drawStr
//  // font : increase font size of number
//  u8g2.drawStr(55,26,"0");
//  u8g2.drawStr(71,26,"0");
//  u8g2.drawStr(90,26,"0");
}
void displayOffsetInput2(char *var)
{
//  u8g2.clearDisplay();
//  u8g2.drawStr(16,19,"enter");
//  u8g2.drawStr(13,30,"offset");
//  u8g2.drawStr(18,38,"(mm)");
//  u8g2.drawStr(107,29,"mm");
//  // draw up and down arrow
//  // solve issue : constant does not support in drawStr
//  // get name as argument and display it using drawStr
//  // font : increase font size of number
//  u8g2.drawStr(55,26,"0");
//  u8g2.drawStr(71,26,"0");
//  u8g2.drawStr(90,26,"0");
}
void displayOffsetInput3(char *var)
{
//  u8g2.clearDisplay();
//  u8g2.drawStr(16,19,"enter");
//  u8g2.drawStr(13,30,"offset");
//  u8g2.drawStr(18,38,"(mm)");
//  u8g2.drawStr(107,29,"mm");
//  // draw up and down arrow
//  // solve issue : constant does not support in drawStr
//  // get name as argument and display it using drawStr
//  // font : increase font size of number
//  u8g2.drawStr(55,26,"0");
//  u8g2.drawStr(71,26,"0");
//  u8g2.drawStr(90,26,"0");
}
void displayConfirmOffsetOk(void)
{
//  u8g2.clearDisplay();
//  // draw square with confirm, CANCEL and OK box open
//  u8g2.drawStr(12,5,"confirm");
//  u8g2.drawStr(27,14,"You entered an");
//  u8g2.drawStr(27,27,"offset of 0 mm");
//  u8g2.drawStr(28,36,"Is this correct?");
//  // draw square box over 'CANCEL' text  
//  u8g2.drawStr(17,51,"CANCEL");
//  // invert font color for 'OK' text or make inverted filled square box over 'OK' text.
//  u8g2.drawStr(97,51,"OK");
}
void displayConfirmOffsetCancel(void)
{
//  u8g2.clearDisplay();
//  // draw square with confirm, CANCEL and OK box open
//  u8g2.drawStr(12,5,"confirm");
//  u8g2.drawStr(27,14,"You entered an");
//  u8g2.drawStr(27,27,"offset of 0 mm");
//  u8g2.drawStr(28,36,"Is this correct?");
//  // invert font color for 'CANCEL' text or make inverted filled square box over 'CANCEL' text.
//  u8g2.drawStr(17,51,"CANCEL");
//  // draw square box over 'OK' text  
//  u8g2.drawStr(97,51,"OK");
}
void displayMenuHud(void)
{
//  u8g2.clearDisplay();
//  // draw horizontal line below 'menu' text
//  u8g2.drawStr(54,6,"menu");
//  // invert font color for 'HUD' text or make inverted filled square box over 'HUD' text.
//  u8g2.drawStr(56,20,"HUD");
//  u8g2.drawStr(53,33,"EDIT");
//  u8g2.drawStr(47,46,"MEMORY");
}
void displayMenuEdit(void)
{
//  u8g2.clearDisplay();
//  // draw horizontal line below 'menu' text
//  u8g2.drawStr(54,6,"menu");
//  // invert font color for 'EDIT' text or make inverted filled square box over 'EDIT' text.
//  u8g2.drawStr(56,20,"HUD");
//  u8g2.drawStr(53,33,"EDIT");
//  u8g2.drawStr(47,46,"MEMORY");
}
void displayMenuMemory(void)
{
//  u8g2.clearDisplay();
//  // draw horizontal line below 'menu' text
//
////  SpiRam.write_stream(address, "menu", 4);
//  
//  u8g2.drawStr(54,6,"menu");
//  u8g2.drawStr(53,20,"EDIT");
//  // invert font color for 'MEMORY' text or make inverted filled square box over 'MEMORY' text.
//  u8g2.drawStr(47,33,"MEMORY");
//  u8g2.drawStr(53,46,"SEND");
}
void displayMenuSend(void)
{
//  u8g2.clearDisplay();
//  // draw horizontal line below 'menu' text
//  u8g2.drawStr(54,6,"menu");
//  u8g2.drawStr(47,20,"MEMORY");
//  // invert font color for 'SEND' text or make inverted filled square box over 'SEND' text.
//  u8g2.drawStr(53,33,"SEND");
//  u8g2.drawStr(41,46,"SETTINGS");
}
void displayMenuSettings(void)
{
//  u8g2.clearDisplay();
//  // draw horizontal line below 'menu' text
//  u8g2.drawStr(54,6,"menu");
//  u8g2.drawStr(47,20,"MEMORY");
//  u8g2.drawStr(53,33,"SEND");
//  // invert font color for 'SETTINGS' text or make inverted filled square box over 'SETTINGS' text.
//  u8g2.setCursor(41,46);
//  u8g2.print("SETTINGS");
}


// ******************************* PRIVATE function **********************************//
void updateTallyChar(char *arr, uint8_t position, int8_t update_inst)
{
  char temp = arr[position - 1];

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
  arr[position + 1] = temp;
}
void updateTallyNum(char *arr, uint8_t position, int8_t update_inst)
{
  char temp = arr[position - 1];

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
  arr[position + 1] = temp;
}

