#include <Arduino.h>
//#include <SPI.h>
#include "U8x8lib.h"
#include "U8g2lib.h"

// object - display library.
U8X8_SSD1322_NHD_128X64_4W_HW_SPI u8x8(10, 9);

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
#define DISPLAY_INTRO                   0
#define DISPLAY_ERROR_SD_CARD           1
#define DISPLAY_CREATE_TALLY            2
#define DISPLAY_LOAD_TALLY              3
#define DISPLAY_LABEL_INPUT_1           4
#define DISPLAY_LABEL_INPUT_2           5
#define DISPLAY_LABEL_INPUT_3           6
#define DISPLAY_LABEL_INPUT_4           7
#define DISPLAY_CONFIRM_LABEL_OK        8
#define DISPLAY_CONFIRM_LABEL_CANCEL    9
#define DISPLAY_OFFSET_INPUT_1          10
#define DISPLAY_OFFSET_INPUT_2          11
#define DISPLAY_OFFSET_INPUT_3          12
#define DISPLAY_CONFIRM_OFFSET_OK       13
#define DISPLAY_CONFIRM_OFFSET_CANCEL   14

uint8_t curr_state, last_state;

// User variable
char nameOfTally[4] = {'A', 'A', 'A', '1'};
char offsetInMM[3] = {'0', '0', '0'};

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

  // Switches - set pin as INPUT pin.
  pinMode(SW_POWER, INPUT);
  pinMode(SW_MENU, INPUT);
  pinMode(SW_UP, INPUT);
  pinMode(SW_DOWN, INPUT);
  pinMode(SW_LEFT, INPUT);
  pinMode(SW_RIGHT, INPUT);
  pinMode(SW_ENTER, INPUT);

  // Initialize display
  u8x8.begin();
  u8x8.clearDisplay();

  while(digitalRead(SW_POWER) == UNPRESSED);
  delay(100);

  do
  {
    // Display intro Screen
    displayIntro();
    delay(3000);
    // check if SD card is present (implement later)
    if(0) //(SD card not present)
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

  displayCreateTally();
  curr_state = last_state = DISPLAY_CREATE_TALLY;
}

void loop(void)
{
  keySwitchNavigation();
}
// **************************** Key navigation and state cycle functions *************************** //
/*
 * Key switch detection can be improved.
 * Power off key switch mechanism is not yet added once it turns ON display.
 * Label and offset display cursor position change mechanism can be optimized to eliminate full screen
 *      refresh time.
 * Menu display is not yet added in state cycle.
 */

void keySwitchNavigation(void)
{
  // To remove de-bouncing, 100 miliSecond waiting time is added after each key press detection.
  // If multiple keys are pressed, it will be served in sequence - MENU, UP, DOWN, LEFT, RIGHT, ENTER
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
  switch(curr_state)
  {
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

// ********************************** Display function ************************************ //
/*
 * Need to set font size and style (must be checked on actual display)
 * Need to optimize string passed in drawString function to reduce RAM usage.
 * Need to implement all remaining things mentioned in comment of individual function.
 */
void displayIntro(void)
{
  u8x8.clearDisplay();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(32,16,"titleee");
  // draw horizontal line 
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(40,42,"starting. . .");
}
void displayErrorSdCard(void)
{
  u8x8.clearDisplay();
  // draw square with error and OK box open
  u8x8.drawString(13,5,"error");
  u8x8.drawString(25,16,"Please insert an");
  u8x8.drawString(25,29,"sd card into the");
  u8x8.drawString(25,40,"machine. . .");
  // invert font color for 'OK' text or make inverted filled square box over 'OK' text.
  u8x8.drawString(97,51,"OK");
}
void displayCreateTally(void)
{
  u8x8.clearDisplay();
  u8x8.drawString(19,17,"Create new tally");
  // invert font color for 'new' text or make inverted filled square box over 'new' text.
  u8x8.drawString(30,40,"new");
  // draw square box around 'load' text
  u8x8.drawString(80,39,"load");
}
void displayLoadTally(void)
{
  u8x8.clearDisplay();
  u8x8.drawString(24,15,"Load old tally");
  // invert font color for 'new' text or make inverted filled square box over 'new' text.
  u8x8.drawString(30,40,"new");
  // draw square box around 'load' text
  u8x8.drawString(80,39,"load");
}
void displayLabelInput1(char *var)
{
  u8x8.clearDisplay();
  u8x8.drawString(17,24,"enter");
  u8x8.drawString(17,35,"label");
  // draw up and down arrow
  // solve issue : constant does not support in drawString
  // get name as argument and display it using drawString
  u8x8.drawString(55,26,"V");
  u8x8.drawString(71,26,"A");
  u8x8.drawString(90,26,"A");
  u8x8.drawString(108,26,"1");
}
void displayLabelInput2(char *var)
{
  u8x8.clearDisplay();
  u8x8.drawString(17,24,"enter");
  u8x8.drawString(17,35,"label");
  // draw up and down arrow
  // get name as argument and display it using drawString
  u8x8.drawString(55,26,"V");
  u8x8.drawString(71,26,"A");
  u8x8.drawString(90,26,"A");
  u8x8.drawString(108,26,"1");
}
void displayLabelInput3(char *var)
{
  u8x8.clearDisplay();
  u8x8.drawString(17,24,"enter");
  u8x8.drawString(17,35,"label");
  // draw up and down arrow
  // get name as argument and display it using drawString
  u8x8.drawString(55,26,"V");
  u8x8.drawString(71,26,"A");
  u8x8.drawString(90,26,"A");
  u8x8.drawString(108,26,"1");
}
void displayLabelInput4(char *var)
{
  u8x8.clearDisplay();
  u8x8.drawString(17,24,"enter");
  u8x8.drawString(17,35,"label");
  // draw up and down arrow
  // get name as argument and display it using drawString
  u8x8.drawString(55,26,"V");
  u8x8.drawString(71,26,"A");
  u8x8.drawString(90,26,"A");
  u8x8.drawString(108,26,"1");
}
void displayConfirmLabelOk(void)
{
    u8x8.clearDisplay();
  // draw square with confirm, CANCEL and OK box open
  u8x8.drawString(12,5,"confirm");
  u8x8.drawString(17,18,"Do you want to name");
  u8x8.drawString(17,32,"this tally VAR1?");
  // draw square box over 'CANCEL' text  
  u8x8.drawString(17,51,"CANCEL");
  // invert font color for 'OK' text or make inverted filled square box over 'OK' text.
  u8x8.drawString(97,51,"OK");
}
void displayConfirmLabelCancel(void)
{
    u8x8.clearDisplay();
  // draw square with confirm, CANCEL and OK box open
  u8x8.drawString(12,5,"confirm");
  u8x8.drawString(17,18,"Do you want to name");
  u8x8.drawString(17,32,"this tally VAR1?");
  // invert font color for 'CANCEL' text or make inverted filled square box over 'CANCEL' text.
  u8x8.drawString(17,51,"CANCEL");
  // draw square box over 'OK' text  
  u8x8.drawString(97,51,"OK");
}
void displayOffsetInput1(char *var)
{
  u8x8.clearDisplay();
  u8x8.drawString(16,19,"enter");
  u8x8.drawString(13,30,"offset");
  u8x8.drawString(18,38,"(mm)");
  u8x8.drawString(107,29,"mm");
  // draw up and down arrow
  // solve issue : constant does not support in drawString
  // get name as argument and display it using drawString
  // font : increase font size of number
  u8x8.drawString(55,26,"0");
  u8x8.drawString(71,26,"0");
  u8x8.drawString(90,26,"0");
}
void displayOffsetInput2(char *var)
{
  u8x8.clearDisplay();
  u8x8.drawString(16,19,"enter");
  u8x8.drawString(13,30,"offset");
  u8x8.drawString(18,38,"(mm)");
  u8x8.drawString(107,29,"mm");
  // draw up and down arrow
  // solve issue : constant does not support in drawString
  // get name as argument and display it using drawString
  // font : increase font size of number
  u8x8.drawString(55,26,"0");
  u8x8.drawString(71,26,"0");
  u8x8.drawString(90,26,"0");
}
void displayOffsetInput3(char *var)
{
  u8x8.clearDisplay();
  u8x8.drawString(16,19,"enter");
  u8x8.drawString(13,30,"offset");
  u8x8.drawString(18,38,"(mm)");
  u8x8.drawString(107,29,"mm");
  // draw up and down arrow
  // solve issue : constant does not support in drawString
  // get name as argument and display it using drawString
  // font : increase font size of number
  u8x8.drawString(55,26,"0");
  u8x8.drawString(71,26,"0");
  u8x8.drawString(90,26,"0");
}
void displayConfirmOffsetOk(void)
{
  u8x8.clearDisplay();
  // draw square with confirm, CANCEL and OK box open
  u8x8.drawString(12,5,"confirm");
  u8x8.drawString(27,14,"You entered an");
  u8x8.drawString(27,27,"offset of 0 mm");
  u8x8.drawString(28,36,"Is this correct?");
  // draw square box over 'CANCEL' text  
  u8x8.drawString(17,51,"CANCEL");
  // invert font color for 'OK' text or make inverted filled square box over 'OK' text.
  u8x8.drawString(97,51,"OK");
}
void displayConfirmOffsetCancel(void)
{
  u8x8.clearDisplay();
  // draw square with confirm, CANCEL and OK box open
  u8x8.drawString(12,5,"confirm");
  u8x8.drawString(27,14,"You entered an");
  u8x8.drawString(27,27,"offset of 0 mm");
  u8x8.drawString(28,36,"Is this correct?");
  // invert font color for 'CANCEL' text or make inverted filled square box over 'CANCEL' text.
  u8x8.drawString(17,51,"CANCEL");
  // draw square box over 'OK' text  
  u8x8.drawString(97,51,"OK");
}
void displayMenuHud(void)
{
  u8x8.clearDisplay();
  // draw horizontal line below 'menu' text
  u8x8.drawString(54,6,"menu");
  // invert font color for 'HUD' text or make inverted filled square box over 'HUD' text.
  u8x8.drawString(56,20,"HUD");
  u8x8.drawString(53,33,"EDIT");
  u8x8.drawString(47,46,"MEMORY");
}
void displayMenuEdit(void)
{
  u8x8.clearDisplay();
  // draw horizontal line below 'menu' text
  u8x8.drawString(54,6,"menu");
  // invert font color for 'EDIT' text or make inverted filled square box over 'EDIT' text.
  u8x8.drawString(56,20,"HUD");
  u8x8.drawString(53,33,"EDIT");
  u8x8.drawString(47,46,"MEMORY");
}
void displayMenuMemory(void)
{
  u8x8.clearDisplay();
  // draw horizontal line below 'menu' text
  u8x8.drawString(54,6,"menu");
  u8x8.drawString(53,20,"EDIT");
  // invert font color for 'MEMORY' text or make inverted filled square box over 'MEMORY' text.
  u8x8.drawString(47,33,"MEMORY");
  u8x8.drawString(53,46,"SEND");
}
void displayMenuSend(void)
{
  u8x8.clearDisplay();
  // draw horizontal line below 'menu' text
  u8x8.drawString(54,6,"menu");
  u8x8.drawString(47,20,"MEMORY");
  // invert font color for 'SEND' text or make inverted filled square box over 'SEND' text.
  u8x8.drawString(53,33,"SEND");
  u8x8.drawString(41,46,"SETTINGS");
}
void displayMenuSettings(void)
{
  u8x8.clearDisplay();
  // draw horizontal line below 'menu' text
  u8x8.drawString(54,6,"menu");
  u8x8.drawString(47,20,"MEMORY");
  u8x8.drawString(53,33,"SEND");
  // invert font color for 'SETTINGS' text or make inverted filled square box over 'SETTINGS' text.
  u8x8.drawString(41,46,"SETTINGS");
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

