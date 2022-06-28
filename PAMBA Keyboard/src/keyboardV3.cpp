/*
    <this program converts a PS2 keyboard to a USB keyboard and add extra functionality>
    Copyright (C) <2022>  <Sam Kim>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    need to add email




  In advances, sorry for all the bad spelling in the comments or other things. 
  Im not the best at spelling and I dont care enghouf to spell check everything.

  This project does is converts a PS2 keyboard into a USB keyboard with additional fertuers.
  The project add these feturs to a PS2 Keyboard:
  * 6 differnt keyboards modes in which nearly all keys can have a differnt macro asign to it.
  * The ablity to easly make macros on the fly without any PC software
  * All setting/macros are stored on the SD card so you can easyly modifly/backup them.
  * Calculater mode that turns part or the whole keyboard in to calculate and send results as keypresses
  * In any of the 6 keyboard modes you can convert the numpad into a calculter and leave the rest of the keyboard alown 
  * Hex and Dec converter mode
  * Forces numlock on so you dont always have to turn it on (do on startup and when modes are being changed)
  * 
  * WILL ADD SOON the ablity to change settings without puting the sd card into a PC
  * WILL ADD SOON the abilty to rebind keys and have differnt key layouts per keyboard mode (half way done)

  All the mode on the keyboard:
   Mode 1-6 is keyboard mode and most keys can have macros on them through SD card .
   Mode 7 is macro mode, you can run scripts that is binded to the key press. scripts are made in the code
   Mode 8 hex and dec covernter
   Mode 9 is calcultor mode
   Mode 10 keyboard mode without the ablity to make macros
   Mode 11-16 are the same as modes 1-6 but numpad is a calculator


   Arduino libartys that this programs uses
   1. PS2KeyAdvanced (in lib folder)
   2. HID-Project
   3. Adafruit_GFX and Adafruit_sm1306 (if useing oled display)
   4. SD

  Doc on how to use the PS2KEYADVANCE Libarty.
  https://github.com/techpaul/PS2KeyAdvanced
  You should go through it to made sure how to use it.

  PS2KeyAdvanced lib is in the lib folder. I modifly the lib to disable keypad switching keybindings and change lock keys to not break everything
   Num Lock is now PS2_KEY_LANG1
   Scoll Lock is now PS2_KEY_LANG2
   Cap Lock is now PS2_KEY_LANG3
   Sending data (Lock led) to the ps2 keyboard dont work using a SAMD board ().

*/

// try to get most of these things done before the end of break,
// them publish to github and maybe write up something intstution website thing which i forgot the name of.

/*TO DO LIST
 * - add stuff to Ps2tohid libary .
 * Add differnt layouts for each keyboard mode and can be change on the fly and on the sd card
 * clean up all unused vars and change int's to other form of int to save space. hole can run on avr boards but dout
 * add golble defines in libary.
 * add mode starting permaiters to labary so less configing in libary.
 * remove all the .h files i used VERY wrongly and make a differnt libary or something else.
 * make better doc on how to add, remove, and modifly the modes.
 * make mode function return a bool so you can switch modes eszey in differnt ways, and make mode switch local to void loop
 * aloud user to change modes with differnt buttons. lib peram.
 * test on other boards like esp32.
 * bind all ps2 scancode to a hid scancode. (even if wrong, so people can use them for macros)
 */

#include "Arduino.h"
#include <HID-Project.h>
#include <HID-Settings.h>

// is all ready in Ps2tohid libary and dont need to add it, but you
// should have it for the key scancode define
#include <PS2KeyAdvanced.h>

/* Keyboard constants  Change to suit your Arduino
   define pins used for data and clock from keyboard. clock needs to intrup pins
   Keyboard need sd card chip select*/
////My Keyboard
/* set up pins per board
    this is so when compling for differnt boards, its auto change pins
*/
// PCB/QT Py pin
#ifdef BOARD_1
const int chipSelect = 1;
#define DATAPIN 3
#define IRQPIN 2
#endif
// My Keyboard/Feather M4 pins
#ifdef BOARD_2
const int chipSelect = 19;
#define DATAPIN 9
#define IRQPIN 12
#endif
// Mom's Keyboard/Seeed Xiao  pins
#ifdef BOARD_3
const int chipSelect = 3;
#define DATAPIN 0
#define IRQPIN 1

#endif

// For OLED screen
//#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// uint16_t code;
int usbPress;

/* Mode 1-6 is keyboard mode and most keys can have macros on them through SD card .
   Mode 7 is macro mode, you can run scripts that is binded to the key press. scripts are made in the code
   Mode 8 hex and dec covernter
   Mode 9 is calcultor mode
   Mode 11-16 are the same as modes 1-6 but numpad is a calculator */
int mode = 1;
// last modes are for switching between the last 2 mode that the keyboared was in quickly
int lastMode = 1;
// when press function key in switching mode, the mode will be switch to lastMode2
int lastMode2 = 1;

// the text that shows up on display when the keyboard is in that mode. 0 inx
// const char *modeStr[] = {"Macro Mode", "Keyboard", "Calc Mode", "TestMode", "TestMode 2", "HEX + DEC", "SD MACRO", "SD MACRO 2", "SD MACRO 3", "SD MACRO 4"}; // old
const char *modeStr[] = {"Keyboard", "Keyboard 1", "Keyboard 2", "Keyboard 3", "Keyboard 4", "Keyboard 5", "Keyboard 6", "Macro Mode", "HEX + DEC", "Calc Mode"};

// set to true when wake from sleep. when true it go into mode switcher but fousre it into a mode
bool sleepWake = true;  // its true so automaticly after initaliation keyboaurd go into a mode
bool modeSwitch = true; // set globle var to see to switch modes or sleep when break to loop.

#include "Ps2tohid.h"
const byte funcKey = PS2_KEY_MENU;
Ps2tohid Ps2tohidLib(30, funcKey, 0);

void setup()
{
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    for (;;)
      ; // Don't proceed, loop forever
  }
  // Serial.println("display started");
  Ps2tohidLib.begin(DATAPIN, IRQPIN, chipSelect);
  display.setRotation(Ps2tohidLib.displayRotation());
  display.display();

  BootKeyboard.begin();
  delay(1000); // Pause for 1 seconds

  // Clear the buffer and go in keyboard mode
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.print(modeStr[mode]);
  display.display();
}

// functions that run in all modes
// inported files with code to make look neeter.
//#include "keyPressMap.h"
//#include "keyMacro.h"
//#include "keyCalc.h"
//#include "hexDecBin.h"

/* to make new modes you add this
  void keyMode#() {
  int keyboardCode = 0;
  (put other var you want to intishalises here)


  while (keyboardCode != -1) {    // this is a loop that runs untill you press menu key
    keyboardCode = Ps2tohidLib.keyboardCheck();     // this check to see if you have pressed a key
    if (keyboardCode > 0) {           // see if you pressed a key or pressed menu key or pressed nothing

    (put all your code in here that you want to run when a key is pressed or relesed)

    }
    //not needed
    (this code run, even if key is not pressed or relesased.)
    (add a  else statment  if you want this code to run only if no keys are pressed)
  }
  if (keyboardCode == -2) {
    modeSwitch = false;
  } else {
    modeSwitch = true;
  }
  return;
  }
*/

// mode has no on the fly macros, yet. acts like a normal keyboard
bool defualtKeyboard()
{
  int keyboardCode = 0;
  bool switchModes = true;
  while (keyboardCode > -1)
  {
    keyboardCode = Ps2tohidLib.keyboardCheck();
    if (keyboardCode > 0)
    {
      usbPress = Ps2tohidLib.usbCodes(keyboardCode, 0);
      if (!(keyboardCode & PS2_BREAK))
      { // see if key is press
        switch (usbPress)
        {
        case 0x53: // num lock, nothing
          // BootKeyboard.print(currentNum);
          break;
        case 0x39: // cap lock, do nothing
          break;
        case 0x47: // scroll lock, use to print what is in calculater
          BootKeyboard.print(Ps2tohidLib.calcNum());
          //   Ps2tohidLib.playMacros(0x47, "TEST", bool sendCodes);
          break;
        default:
          BootKeyboard.press(KeyboardKeycode(usbPress)); // press the key
        }
      }
      else
      { // see if key is not press it will release key
        switch (usbPress)
        {
        case 0x53: // num lock, use to print what iis in calculater
          // Keyboard.print(currentNum);
          break;
        case 0x39: // cap lock, do nothing
          break;
        case 0x47: // scroll lock, do nothing
          break;
        default:
          BootKeyboard.release(KeyboardKeycode(usbPress)); // release the key
        }
      }
    }
  }
  if (keyboardCode == -2)
  {
    switchModes = false;
  }
  else
  {
    switchModes = true;
  }
  return switchModes;
}

void keyMode1()
{
  int keyboardCode = 0;

  while (keyboardCode > -1)
  {
    keyboardCode = Ps2tohidLib.keyboardCheck();
    if (keyboardCode > 0)
    {

      // if (!(keyboardCode & PS2_BREAK))
      //   macroMode(keyboardCode);
    }
  }
  if (keyboardCode == -2)
  {
    modeSwitch = false;
  }
  else
  {
    modeSwitch = true;
  }
  return;
}

void keyMode2()
{
  int keyboardCode = 0;
  // calcMode(1); // when switching mode, this runs so all the stuff shows uo
  Ps2tohidLib.calcMode(1, "Calc Mode");
  while (keyboardCode > -1)
  {
    keyboardCode = Ps2tohidLib.keyboardCheck();
    if (keyboardCode > 0)
    {
      if (!(keyboardCode & PS2_BREAK))
      {
        // calcMode(keyboardCode);
        //  if (keyboardCode == 0x96){
        //    keyboardCode = -8;
        //  }
        Ps2tohidLib.calcMode(keyboardCode, "Calc Mode");
      }
    }
  }
  if (keyboardCode == -2)
  {
    modeSwitch = false;
  }
  else
  {
    modeSwitch = true;
  }
  return;
}

// void keyMode3()
// {
//   int keyboardCode = 0;
//   while (keyboardCode > -1)
//   {
//     keyboardCode = Ps2tohidLib.keyboardCheck();
//     if (keyboardCode > 0)
//     {
//       /*KeyboardMode func converts all ps2 key inputs to
//         usb key presses using the "Keyboard" libary.
//         This is so the ps2 keybaord can be a usb keyboard.
//         The keyboard had */
//       Serial.print("Delay: ");
//       Serial.println(Ps2tohidLib.keyPressDelay());

//       if (!(keyboardCode & PS2_BREAK))
//       {
//         Serial.print("Press: ");
//         Serial.println(Ps2tohidLib.usbCodes(keyboardCode) + 256, HEX);
//       }
//       if ((keyboardCode & PS2_BREAK))
//       {
//         Serial.print("Release: ");
//         Serial.println(Ps2tohidLib.usbCodes(keyboardCode), HEX);
//       }
//     }
//   }
//   if (keyboardCode == -2)
//   {
//     modeSwitch = false;
//   }
//   else
//   {
//     modeSwitch = true;
//   }
//   return;
// }

// void keyMode4()
// {
//   int keyboardCode = 0;

//   while (keyboardCode > -1)
//   {
//     keyboardCode = Ps2tohidLib.keyboardCheck();
//     if (keyboardCode > 0)
//     {
//       Serial.println(Ps2tohidLib.keyPressDelay());
//       usbPress = Ps2tohidLib.usbCodes(keyboardCode);
//       if (!(keyboardCode & PS2_BREAK))
//       { // see if key is press
//         switch (usbPress)
//         {
//         case 0x53: // num lock, use to print what is in calculater
//           BootKeyboard.print(currentNum);
//           break;
//         case 0x39: // cap lock, do nothing
//           break;
//         case 0x47: // scroll lock, do nothing
//           break;
//         default:
//           BootKeyboard.press(KeyboardKeycode(usbPress)); // press the key
//           Serial.println(usbPress + 256, HEX);
//         }
//       }
//       else
//       { //see if key is not press it will release key
//         switch (usbPress)
//         {
//         case 0x53: // num lock, use to print what iis in calculater
//           //Keyboard.print(currentNum);
//           break;
//         case 0x39: // cap lock, do nothing
//           break;
//         case 0x47: // scroll lock, do nothing
//           break;
//         default:
//           BootKeyboard.release(KeyboardKeycode(usbPress)); //release the key
//           Serial.println(usbPress, HEX);
//         }
//       }
//     }
//   }
//   if (keyboardCode == -2)
//   {
//     modeSwitch = false;
//   }
//   else
//   {
//     modeSwitch = true;
//   }
//   return;
// }

int _solveHexDec(String currentHexDec)
{
  int returnNum = 0;
  if (currentHexDec.startsWith("0x"))
  { // Hex to Dec
    currentHexDec.remove(0, 2);
    int count = currentHexDec.length();
    int hexNum = 0;
    while (count != 0)
    {
      count = currentHexDec.length() - 1;
      switch (currentHexDec.charAt(0))
      {
      case '0':
        hexNum = (0 * pow(16, count)) + hexNum;
        break;
      case '1':
        hexNum = (1 * pow(16, count)) + hexNum;
        break;
      case '2':
        hexNum = (2 * pow(16, count)) + hexNum;
        break;
      case '3':
        hexNum = (3 * pow(16, count)) + hexNum;
        break;
      case '4':
        hexNum = (4 * pow(16, count)) + hexNum;
        break;
      case '5':
        hexNum = (5 * pow(16, count)) + hexNum;
        break;
      case '6':
        hexNum = (6 * pow(16, count)) + hexNum;
        break;
      case '7':
        hexNum = (7 * pow(16, count)) + hexNum;
        break;
      case '8':
        hexNum = (8 * pow(16, count)) + hexNum;
        break;
      case '9':
        hexNum = (9 * pow(16, count)) + hexNum;
        break;
      case 'A':
        hexNum = (10 * pow(16, count)) + hexNum;
        break;
      case 'B':
        hexNum = (11 * pow(16, count)) + hexNum;
        break;
      case 'C':
        hexNum = (12 * pow(16, count)) + hexNum;
        break;
      case 'D':
        hexNum = (13 * pow(16, count)) + hexNum;
        break;
      case 'E':
        hexNum = (14 * pow(16, count)) + hexNum;
        break;
      case 'F':
        hexNum = (15 * pow(16, count)) + hexNum;
        break;
      }
      // hexNum = hexNum + hexNum;
      currentHexDec.remove(0, 1);
    }
    returnNum = hexNum;
  }
  else
  { // Dec to Hex
    returnNum = currentHexDec.toInt();
  }
  //  currentHexDec = "";
  return returnNum;
}

bool hexDecMode()
{
  bool switchModes = true;
  String hexNumDec;
  bool showSolve = false;
  int keyboardCode = 0;
  int hexDecAnswer = 0;
  String currentHexDec;

  while (keyboardCode > -1)
  {
    keyboardCode = Ps2tohidLib.keyboardCheck();
    if (keyboardCode > 0)
    {
      display.clearDisplay();
      display.setTextSize(2); // make text smaller
      // display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.print("HEX + DEC");
      display.setTextSize(1);
      if (!(keyboardCode & PS2_BREAK))
      {
        switch (keyboardCode & 0xFF)
        {
        case PS2_KEY_ENTER:
        case PS2_KEY_KP_ENTER:
          hexDecAnswer = _solveHexDec(currentHexDec);
          showSolve = true;
          break;
        /* when you press a number key, it appends the num to current num */
        case PS2_KEY_KP0:
        case PS2_KEY_0:
          showSolve = false;
          currentHexDec = String(currentHexDec + "0");
          break;
        case PS2_KEY_KP1:
        case PS2_KEY_1:
          showSolve = false;
          currentHexDec = String(currentHexDec + "1");

          break;
        case PS2_KEY_KP2:
        case PS2_KEY_2:
          showSolve = false;
          currentHexDec = String(currentHexDec + "2");

          break;
        case PS2_KEY_KP3:
        case PS2_KEY_3:
          showSolve = false;
          currentHexDec = String(currentHexDec + "3");

          break;
        case PS2_KEY_KP4:
        case PS2_KEY_4:
          showSolve = false;
          currentHexDec = String(currentHexDec + "4");

          break;
        case PS2_KEY_KP5:
        case PS2_KEY_5:
          showSolve = false;
          currentHexDec = String(currentHexDec + "5");
          break;
        case PS2_KEY_KP6:
        case PS2_KEY_6:
          showSolve = false;
          currentHexDec = String(currentHexDec + "6");
          break;
        case PS2_KEY_KP7:
        case PS2_KEY_7:
          showSolve = false;
          currentHexDec = String(currentHexDec + "7");
          break;
        case PS2_KEY_KP8:
        case PS2_KEY_8:
          showSolve = false;
          currentHexDec = String(currentHexDec + "8");
          break;
        case PS2_KEY_KP9:
        case PS2_KEY_9:
          showSolve = false;
          currentHexDec = String(currentHexDec + "9");
          break;
        case PS2_KEY_A:
          showSolve = false;
          currentHexDec = String(currentHexDec + "A");
          break;
        case PS2_KEY_B:
          showSolve = false;
          currentHexDec = String(currentHexDec + "B");
          break;
        case PS2_KEY_C:
          showSolve = false;
          currentHexDec = String(currentHexDec + "C");
          break;
        case PS2_KEY_D:
          showSolve = false;
          currentHexDec = String(currentHexDec + "D");
          break;
        case PS2_KEY_E:
          showSolve = false;
          currentHexDec = String(currentHexDec + "E");
          break;
        case PS2_KEY_F:
          showSolve = false;
          currentHexDec = String(currentHexDec + "F");
          break;
        case PS2_KEY_X:
          if (currentHexDec.length() == 1)
            currentHexDec = String(currentHexDec + "x");
          break;
        case PS2_KEY_BS:                                       // delete button
          currentHexDec.remove(currentHexDec.length() - 1, 1); // deletes last char in currnt num
          break;
        case PS2_KEY_DELETE: // clear button
          currentHexDec = "";
          hexDecAnswer = 0;
          showSolve = false;
          break;
        case PS2_KEY_INSERT:
          // currentNum = String(hexDecAnswer);
          break;
        case PS2_KEY_LANG1: // num lock key print number to your computer
          BootKeyboard.print(hexDecAnswer, HEX);
          break;
        case PS2_KEY_LANG2: // num lock key print number to your computer
          BootKeyboard.print(hexDecAnswer);
          break;
        }
      }
      // update screen with all the calculator stuff
      display.setCursor(0, 16);
      if (showSolve)
      {
        display.print("Hex: 0x");
        display.println(hexDecAnswer, HEX);
        display.print("Dec: ");
        display.println(hexDecAnswer, DEC);
        display.print("Bin: ");
        display.println(hexDecAnswer, BIN);
      }
      else
      {
        display.println(currentHexDec);
      }
      display.display(); // update display
    }
  }

  if (keyboardCode == -2)
  {
    switchModes = false;
  }
  else
  {
    switchModes = true;
  }
  return switchModes;
}

/* void loop is used for 2 differnt things
   1 - switch betweeen differnt keyboard modes
   2 - to have the keyboard go to sleep (turn off the screen)
*/

// updates the display to show what mode the keyboard is in
void modeChangeDisplay(int mode, int textSize, int newTextSize = 2)
{
  display.clearDisplay();              // clear screen
  display.setTextSize(textSize);       // make text big
  display.setTextColor(SSD1306_WHITE); // make white text
  display.setCursor(0, 0);
  display.print(modeStr[mode]);
  display.setTextSize(newTextSize); // make text big
  display.display();                // puts all changes to screen
}

void macroMaker(int mode)
{
  if (Ps2tohidLib.sdStatus())
  { // check if sd card is plug in. softlock if ran and no sd card
    display.setCursor(0, 0);
    display.clearDisplay();
    display.setTextSize(1);
    display.println("press key you want to program a macro for.");
    display.println();
    display.println("press fn button to break");
    display.display();

    // seclect what key you want to reprogram
    int keyboardCode = 0;
    int playBackSpeed = 0;
    while (keyboardCode == 0)
    {
      keyboardCode = Ps2tohidLib.keyboardCheck();
      if (keyboardCode < 0)
      {
        break;
      }
      if (keyboardCode & PS2_BREAK)
      {
        keyboardCode = 0;
      }
    }
    if (keyboardCode < 0)
    {
      return;
    }

    // what scan code you want to set macro for
    int scanCode = Ps2tohidLib.usbCodes(keyboardCode, 0);

    // for selecting if you only want to delet a macro
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Do you want to program or remove a macro?");
    display.println();
    display.println("press 'y' to program, 'n' to only delete");
    display.display();
    keyboardCode = 0;

    while (keyboardCode == 0)
    {
      keyboardCode = Ps2tohidLib.keyboardCheck();
      if (keyboardCode < 0)
      {
        break;
      }
      if (keyboardCode & PS2_BREAK)
      {
        keyboardCode = 0;
      }
    }
    if (keyboardCode == PS2_KEY_Y)
    {
    }
    else if (keyboardCode == PS2_KEY_N)
    {
      Ps2tohidLib.recordMacros(scanCode, String(mode), false, true, 1);
      return;
    }
    else
    {
      return;
    }

    // choose play back speed
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("What playback speed do you want");
    display.println();

    display.println("a number key to choose speed, 1 is 1x speed");
    display.display();
    keyboardCode = 0;

    while (keyboardCode == 0)
    {
      keyboardCode = Ps2tohidLib.keyboardCheck();
      if (keyboardCode < 0)
      {
        break;
      }
      if (keyboardCode & PS2_BREAK)
      {
        keyboardCode = 0;
      }
    }

    switch (keyboardCode)
    {
    case PS2_KEY_1:
    case PS2_KEY_KP1:
      playBackSpeed = 1;
      break;
    case PS2_KEY_2:
    case PS2_KEY_KP2:
      playBackSpeed = 2;
      break;
    case PS2_KEY_3:
    case PS2_KEY_KP3:
      playBackSpeed = 3;
      break;
    case PS2_KEY_4:
    case PS2_KEY_KP4:
      playBackSpeed = 4;
      break;
    case PS2_KEY_5:
    case PS2_KEY_KP5:
      playBackSpeed = 5;
      break;
    case PS2_KEY_6:
    case PS2_KEY_KP6:
      playBackSpeed = 6;
      break;
    case PS2_KEY_7:
    case PS2_KEY_KP7:
      playBackSpeed = 7;
      break;
    case PS2_KEY_8:
    case PS2_KEY_KP8:
      playBackSpeed = 8;
      break;
    case PS2_KEY_9:
    case PS2_KEY_KP9:
      playBackSpeed = 9;
      break;
    default:
      return;
    }

    // if you want to send scan codes to your pc whan making the macro
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("do you want key presses to send to pc when making macro?");
    display.println();

    display.println("press 'y' for yes, 'n' for no");
    display.display();
    keyboardCode = 0;

    while (keyboardCode == 0)
    {
      keyboardCode = Ps2tohidLib.keyboardCheck();
      if (keyboardCode < 0)
      {
        break;
      }
      if (keyboardCode & PS2_BREAK)
      {
        keyboardCode = 0;
      }
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Macros are being recoded now.");
    display.println();
    display.println("press fn key to stop recording");

    display.display();

    if (keyboardCode == PS2_KEY_Y)
    {
      Ps2tohidLib.recordMacros(scanCode, String(mode), true, false, playBackSpeed);
    }
    else if (keyboardCode == PS2_KEY_N)
    {
      Ps2tohidLib.recordMacros(scanCode, String(mode), false, false, playBackSpeed);
    }
    return;
  }
  else
  {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println("NO SD CARD");
    display.setTextSize(1);
    display.println("plug sd card in and restart keyboard");
    display.display();
    delay(2000);
  }
}

void updateLastModes()
{
  lastMode2 = lastMode;
  lastMode = mode;
}

void loop()
{

  // BootKeyboard.releaseAll();
  int keyboardCode = 0;

  // goes in here to sleep screen
  if (!(modeSwitch))
  {
    // this is ran when the keyboard is sleeping, to prevent damage to the screen
    display.clearDisplay(); // clear screen
    display.display();

    int keyboardCode = 0;
    sleepWake = false;

    // keep checking to see if the keyboard have been updated
    while (keyboardCode == 0 || keyboardCode == -2)
    {
      keyboardCode = Ps2tohidLib.keyboardCheck();
    }

    sleepWake = true;
    modeSwitch = true; // so it will run the mode changer
  }
  // enable num lock when switch modes
  if (!(BootKeyboard.getLeds() & LED_NUM_LOCK))
    BootKeyboard.write(KEY_NUM_LOCK);

  // pervents menu key being from sent to pc when switching modes
  if (modeSwitch)
  {
    // check if the keyboerd has a update
    if (!(sleepWake))
    {
      mode = -1; // not a mode, so dont automaticly go into mode 0.
      keyboardCode = Ps2tohidLib.keyboardCheck();
      if (keyboardCode == -2)
        modeSwitch = false; // make so screen can sleep when you in menu
    }

    // when you press function key, you switch modes to previse
    if (keyboardCode == -1)
    {
      sleepWake = true;
      mode = lastMode2;
    }
    if (keyboardCode & PS2_SHIFT && keyboardCode & PS2_BREAK)
    { // when press shift and a key mode, on screen it will show a discrition of mode
      switch (keyboardCode & 0xFF)
      {
      case PS2_KEY_0:
      case PS2_KEY_KP0:
        break;
      case PS2_KEY_1:
      case PS2_KEY_KP1:
        mode = 11;
        modeSwitch = Ps2tohidLib.keyMacroMode("1", true);
        updateLastModes();
        break;
      case PS2_KEY_2:
      case PS2_KEY_KP2:
        mode = 12;
        modeSwitch = Ps2tohidLib.keyMacroMode("2", true);
        updateLastModes();
        break;
      case PS2_KEY_3:
      case PS2_KEY_KP3:
        mode = 13;
        modeSwitch = Ps2tohidLib.keyMacroMode("3", true);
        updateLastModes();
        break;
      case PS2_KEY_4:
      case PS2_KEY_KP4:
        mode = 14;
        modeSwitch = Ps2tohidLib.keyMacroMode("4", true);
        updateLastModes();
        break;
      case PS2_KEY_5:
      case PS2_KEY_KP5:
        mode = 15;
        modeSwitch = Ps2tohidLib.keyMacroMode("5", true);
        updateLastModes();
        break;
      case PS2_KEY_6:
      case PS2_KEY_KP6:
        mode = 16;
        modeSwitch = Ps2tohidLib.keyMacroMode("6", true);
        updateLastModes();
        break;
      case PS2_KEY_7:
      case PS2_KEY_KP7:
        // macroMaker(7);
        break;
      case PS2_KEY_8:
      case PS2_KEY_KP8:
        // macroMaker(8);
        break;
      case PS2_KEY_9:
      case PS2_KEY_KP9:
        //  macroMaker(9);
        break;
      }
      mode = lastMode2;
    }
    else if (keyboardCode & PS2_CTRL && keyboardCode & PS2_BREAK)
    { // when press ctrl and a key mode, on screen it will let you change settings for the mode.
      switch (keyboardCode & 0xFF)
      {
      case PS2_KEY_0:
      case PS2_KEY_KP0:
        break;
      case PS2_KEY_1:
      case PS2_KEY_KP1:
        macroMaker(1);
        break;
      case PS2_KEY_2:
      case PS2_KEY_KP2:
        macroMaker(2);
        break;
      case PS2_KEY_3:
      case PS2_KEY_KP3:
        macroMaker(3);
        break;
      case PS2_KEY_4:
      case PS2_KEY_KP4:
        macroMaker(4);
        break;
      case PS2_KEY_5:
      case PS2_KEY_KP5:
        macroMaker(5);
        break;
      case PS2_KEY_6:
      case PS2_KEY_KP6:
        macroMaker(6);
        break;
      case PS2_KEY_7:
      case PS2_KEY_KP7:
        // macroMaker(7);
        break;
      case PS2_KEY_8:
      case PS2_KEY_KP8:
        // macroMaker(8);
        break;
      case PS2_KEY_9:
      case PS2_KEY_KP9:
        //  macroMaker(9);
        break;
      }
    }
    else if ((keyboardCode & PS2_BREAK) || sleepWake || keyboardCode == -1)

    { // when you press a mode key it switch to that mode, or wake up from sleep

      if (!(sleepWake))
      {

        switch (keyboardCode & 0xFF)
        {
        case PS2_KEY_0:
        case PS2_KEY_KP0:
          mode = 0;
          break;
        case PS2_KEY_1:
        case PS2_KEY_KP1:
          mode = 1;
          break;
        case PS2_KEY_2:
        case PS2_KEY_KP2:
          mode = 2;
          break;
        case PS2_KEY_3:
        case PS2_KEY_KP3:
          mode = 3;
          break;
        case PS2_KEY_4:
        case PS2_KEY_KP4:
          mode = 4;
          break;
        case PS2_KEY_5:
        case PS2_KEY_KP5:
          mode = 5;
          break;
        case PS2_KEY_6:
        case PS2_KEY_KP6:
          mode = 6;
          break;
        case PS2_KEY_7:
        case PS2_KEY_KP7:
          mode = 7;
          break;
        case PS2_KEY_8:
        case PS2_KEY_KP8:
          mode = 8;
          break;
        case PS2_KEY_9:
        case PS2_KEY_KP9:
          mode = 9;
          break;
        }
      }
      sleepWake = false;
      switch (mode)
      {
        // keyboard with no changes
      case 0: // keyboard with no changes
        modeChangeDisplay(0, 2);
        modeSwitch = defualtKeyboard();
        updateLastModes();
        break;

        // keyboard modes with macros on SD card
      case 1:
        modeChangeDisplay(1, 2);
        modeSwitch = Ps2tohidLib.keyMacroMode("1", false);
        updateLastModes();
        // keyMode0();
        break;
      case 2:
        modeChangeDisplay(2, 2);
        modeSwitch = Ps2tohidLib.keyMacroMode("2", false);
        updateLastModes();
        // keyMode2();
        break;
      case 3:
        modeChangeDisplay(3, 2);
        modeSwitch = Ps2tohidLib.keyMacroMode("3", false);
        updateLastModes();
        // keyMode3();
        break;
      case 4:
        modeChangeDisplay(4, 2);
        modeSwitch = Ps2tohidLib.keyMacroMode("4", false);
        updateLastModes();
        // keyMode4();
        break;
      case 5:
        modeChangeDisplay(5, 2);
        modeSwitch = Ps2tohidLib.keyMacroMode("5", false);
        updateLastModes();
        // modeSwitch = hexDecMode();
        // keyMode5();
        break;
      case 6:
        modeChangeDisplay(6, 2);
        modeSwitch = Ps2tohidLib.keyMacroMode("6", false);
        updateLastModes();
        break;

        // macro mode, macros made in firemave, not sd card
      case 7:
        modeChangeDisplay(7, 2);
        keyMode1();
        updateLastModes();
        // modeSwitch = Ps2tohidLib.keyMacroMode("7", currentNum);
        break;

        // hex to dec converter, nice for programeing
      case 8:
        modeChangeDisplay(8, 2);
        modeSwitch = hexDecMode();
        updateLastModes();
        break;

        // calculater mode, make keyboard as calculter and lets you send it as key stroks
      case 9:
        modeChangeDisplay(9, 2);
        keyMode2();
        updateLastModes();
        // modeSwitch = Ps2tohidLib.keyMacroMode("9", currentNum);
        break;
      case 11:
        modeSwitch = Ps2tohidLib.keyMacroMode("1", true);
        updateLastModes();
        break;
      case 12:
        modeSwitch = Ps2tohidLib.keyMacroMode("2", true);
        updateLastModes();
        break;
      case 13:
        modeSwitch = Ps2tohidLib.keyMacroMode("3", true);
        updateLastModes();
        break;
      case 14:
        modeSwitch = Ps2tohidLib.keyMacroMode("4", true);
        updateLastModes();
        break;
      case 15:
        modeSwitch = Ps2tohidLib.keyMacroMode("5", true);
        updateLastModes();
        break;
      case 16:
        modeSwitch = Ps2tohidLib.keyMacroMode("6", true);
        updateLastModes();
        break;
      }
    }
    display.clearDisplay();              // clear screen
    display.setTextSize(2);              // make text big
    display.setTextColor(SSD1306_WHITE); // make white text
    display.setCursor(0, 0);
    display.println("ModeSelect");
    display.setTextSize(1);
    display.println("Num Key: Switch modes");
    display.println("SHIFT: Calc Mode");
    display.println("CTRL: Mode Settings");
    if (!Ps2tohidLib.sdStatus())
    {
      display.println();
      display.println("NO SD CARD");
      display.println("Macros Will not work");
    }
    display.display(); // puts all changes to screen
  }
}
