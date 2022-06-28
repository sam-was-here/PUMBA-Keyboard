// <this program converts a PS2 keyboard to a USB keyboard and add extra functionality>
// Copyright (C) <2022>  <Sam Kim>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "Arduino.h"
#include "Ps2tohid.h"
#include <PS2KeyAdvanced.h>
PS2KeyAdvanced keyboard1;
#include <HID-Project.h>
#include <HID-Settings.h>
#include <SPI.h>
#include <SD.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// need to get from this info from begin funtion
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void _macroFinder();
int keyboardCheck();
bool sdStatus();
byte displayRotation();
void setLayout();

// int _solveHexDec(String currentHexDec);

// set a couple of peramiters. will mod later and can get info from SD card
// these are defualts and can change depeinging on setting on SD card
//
// int interval       - the time in min that the screen withh turn off
// int funcKey        - the key that is used as function key
// bool flipScreen    - set the rotation of the screen, true means normal
Ps2tohid::Ps2tohid(int interval, int funcKey = PS2_KEY_MENU, byte flipScreen = 0)
{
  _interval = _interval * interval;
  functionKey = funcKey;
  _displayRotation = flipScreen;
}

// intilaizeses the everything for the keyboard
//
// int dataPin1   - pin with keyboard data connected to
// int clkPin2    - pin with keyboard clock connected to
// int chipSelect - pin with SD select connected to
void Ps2tohid::begin(int dataPin1, int clkPin2, int chipSelect, int i2cAdrs)
{
  keyboard1.begin(dataPin1, clkPin2);
  // Serial.begin(115200);
  BootKeyboard.begin();
  _sdStatus = SD.begin(chipSelect);
  display1.begin(SSD1306_SWITCHCAPVCC, i2cAdrs, false, true);

  // getting setting from sd card
  // if (_sdStatus)
  // {
    setSetting();
    setLayout();
  // }
}

// return true if SD card was plug in when the keyboard was powered up
bool Ps2tohid::sdStatus()
{
  return _sdStatus;
}

byte Ps2tohid::displayRotation()
{
  return _displayRotation;
}

void Ps2tohid::setSetting()
{
  if (!_sdStatus) // breaks if no sd card in
    return;
  if ((SD.exists("setting/config.TXT")))
  {

    // change the setting
    String buffer;
    File dataFile = SD.open("setting/config.TXT");
    if (dataFile)
    {
      while (dataFile.available())
      {
        // Write one line to buffer
        buffer = dataFile.readStringUntil('\n');
        // test to see if line don't have #, # is a comment
        if (!buffer.startsWith("#"))
        {
          // Serial.println("should run 3 times");
          //  splits buffer it to the 2 parts. name and data parts, splits by =
          int breakIdx = buffer.indexOf('=');                  // the point the data is splited
          String settingName = buffer.substring(0, breakIdx);  // the name of the setting
          String settingData = buffer.substring(breakIdx + 1); // value of the setting
          // Serial.println(settingName);
          // Serial.println(settingData);

          if (settingName.equalsIgnoreCase("sleep")) // sleep timer
          {
            _interval = settingData.toInt();
            _interval = _interval * interval;
          }
          else if (settingName.equalsIgnoreCase("fnkey")) // function key
          {
            functionKey = settingData.toInt();
          }
          else if (settingName.equalsIgnoreCase("rotation")) // display flip
          {
            _displayRotation = settingData.toInt();
            display1.setRotation(_displayRotation);
          }
          else if (settingName.equalsIgnoreCase("setting4="))
          {
          }
          else if (settingName.equalsIgnoreCase("setting5="))
          {
          }
          else if (settingName.equalsIgnoreCase("setting6="))
          {
          }
          else if (settingName.equalsIgnoreCase("setting7="))
          {
          }
          else if (settingName.equalsIgnoreCase("setting8="))
          {
          }
          else if (settingName.equalsIgnoreCase("setting9="))
          {
          }
        }
      }
      dataFile.close();
    }
  }
  // update macro list
  // set all list to false
  for (int i = 0; i <= 254; i++)
  {
    _marcoKeyList1[i] = false;
    _marcoKeyList2[i] = false;
    _marcoKeyList3[i] = false;
    _marcoKeyList4[i] = false;
    _marcoKeyList5[i] = false;
    _marcoKeyList6[i] = false;
  }

  // set keys with macros to true for each mode
  for (int i = 1; i <= 6; i++)
  {
    File dir = SD.open("/" + String(i) + "/");

    while (true)
    {

      File entry = dir.openNextFile();

      if (!entry)
      {
        // no more files
        break;
      }

      String fileName = entry.name(); // turn the name of the file into a string
      if (fileName.endsWith(".TXT"))  // check to see if file or folder
      {
        switch (i) // put the macro to the mode it is for
        {
        case 1:
          _marcoKeyList1[fileName.toInt()] = true;
          break;
        case 2:
          _marcoKeyList2[fileName.toInt()] = true;
          break;
        case 3:
          _marcoKeyList3[fileName.toInt()] = true;
          break;
        case 4:
          _marcoKeyList4[fileName.toInt()] = true;
          break;
        case 5:
          _marcoKeyList5[fileName.toInt()] = true;
          break;
        case 6:
          _marcoKeyList6[fileName.toInt()] = true;
          break;
        }
      }
      entry.close();
    }
  }
  delay(100);
}
void Ps2tohid::updateSetting()
{
  if (!_sdStatus) // breaks if no sd card in
    return;
}

void Ps2tohid::setLayout()
{
  bool _modesWLayout[7] = {0, 0, 0, 0, 0, 0, 0}; // need to be 7 and not 6 because indexing of arrey starts at 0 and the array index is the folder the file is in
  // when no sd card all layouts are set to the default (layout0)

  if (_sdStatus) // if there is a sd card, it will check each mode to see if there is a layout file
  {
    _modesWLayout[1] = SD.exists("1/layout.TXT");
    _modesWLayout[2] = SD.exists("2/layout.TXT");
    _modesWLayout[3] = SD.exists("3/layout.TXT");
    _modesWLayout[4] = SD.exists("4/layout.TXT");
    _modesWLayout[5] = SD.exists("5/layout.TXT");
    _modesWLayout[6] = SD.exists("6/layout.TXT");
  }
  // set keys with macros to true for each mode
  for (int i = 1; i <= 6; i++)
  {
    if (_modesWLayout[i])
    {
      String buffer;
      File dataFile = SD.open(String(i) + "/layout.TXT");

      // If the file is available, read it
      if (dataFile)
      {
        for (int n = 0; n <= 160; n++)    // runs only 161 times
        {
          if (!dataFile.available()) // if the file was not complet it will not put random stuff as keybindings, it will unbind the rest of the keys
          {
            // make binding 0
          }
          else
          {
            buffer = dataFile.readStringUntil('\n');
            int rawDateInt = buffer.toInt();
            switch (i)
            {
            case 1:
                layout1[n] = rawDateInt;
              break;
            case 2:
                layout2[n] = rawDateInt;
              break;
            case 3:
                layout3[n] = rawDateInt;
              break;
            case 4:
                layout4[n] = rawDateInt;
              break;
            case 5:
                layout5[n] = rawDateInt;
              break;
            case 6:
                layout6[n] = rawDateInt;
              break;
            }
          }
        }
        dataFile.close();
      }
    }
    else // if there is no sd card or the mode dont have a layout this will make it the defualt layout
    {
      switch (i)
      {
      case 1:
        for (int k = 0; k <= 160; k++)
        {
          layout1[k] = layout0[k];
        }
        break;
      case 2:
        for (int k = 0; k <= 160; k++)
        {
          layout2[k] = layout0[k];
        }
        break;
      case 3:
        for (int k = 0; k <= 160; k++)
        {
          layout3[k] = layout0[k];
        }
        break;
      case 4:
        for (int k = 0; k <= 160; k++)
        {
          layout4[k] = layout0[k];
        }
        break;
      case 5:
        for (int k = 0; k <= 160; k++)
        {
          layout5[k] = layout0[k];
        }
        break;
      case 6:
        for (int k = 0; k <= 160; k++)
        {
          layout6[k] = layout0[k];
        }
        break;
      }
    }
  }
}

/*
  function checks to see if a PS2 scancode was sent and then reuturn the scancode.
  return 0 if there was no scancodes,
  return -1 is function key was press,
  return -2 if sleep timeout
*/
int Ps2tohid::keyboardCheck()
{
  _currentMillis = millis();
  if (keyboard1.available())
  {
    // read the next key
    _code = keyboard1.read();
    if ((_code & 0xFF) > 0)
    {
      _keyDelay = _previousMillis - _currentMillis;
      _previousMillis = _currentMillis; // update timer to turn off display

      // switch modes when press menu button
      if ((_code & 0xFF) == functionKey)
      {
        if (!(_code & PS2_BREAK))
          return -1; // return -1 if which modes

        return 0;
      }

      return _code;
    }
    return 0; // return 0 if nothing is press
  }
  // turn off diisplay is no keypress
  if (_currentMillis - _previousMillis >= _interval)
  {
    return -2; // returns -2 if sleep
  }
  return 0; // return 0 if nothing is press
}

// Return the amount of time sence the last scancode was sent
long Ps2tohid::keyPressDelay()
{
  return _keyDelay;
}

// // SolveNum calcuales the 2 numbers the user put in and uses the operators (op) the user gave
// // var for calc mode
// // String lastNum;
// // double eq;                     // the valve
// // int nextOp = 0;                // When you press a opertor when there already one, if will put it on after it solve the problem
// // int op = 0;                    // op selces what operators the user inputes in cal mode
// // const char opStr[6] = " +-*/"; // converts op to a string that can be displayed
// // int _decPoint = 6;              // max number of decmal point that are show in answer

void Ps2tohid::solveNum()
{
  switch (_op)
  {
  // Add
  case 1:
    _eq = _lastNum.toDouble() + _currentNum.toDouble();
    break;
  // Minus
  case 2:
    _eq = _lastNum.toDouble() - _currentNum.toDouble();
    break;
  // Times
  case 3:
    _eq = _lastNum.toDouble() * _currentNum.toDouble();
    break;
  // Div
  case 4:
    _eq = _lastNum.toDouble() / _currentNum.toDouble();
    break;
  }

  //  display.print("Aswer is: ");
  //  display.println(eq, 6);
  _lastNum = "";
  // change the number to change how many extra zero there are
  _currentNum = String(_eq, _decPoint);

  // remove extra zero from answer
  for (int i = 0; i < _decPoint; i++)
  {
    if (_currentNum.endsWith("0"))
      _currentNum.remove(_currentNum.length() - 1, 1); // deletes last 0 in currnt num
  }
  if (_currentNum.endsWith("."))
    _currentNum.remove(_currentNum.length() - 1, 1); // deletes "." in currnt num

  if (_nextOp != 0)
  {
    _op = _nextOp;
    _nextOp = 0;
    _lastNum = String(_currentNum);
    _currentNum = "";
  }
  else
  {
    _op = 0;
  }
}

/* Current num is the number you are are typing
   ths function sets currentnum to lastnum
   when you press a op. if you pres a op when you
   had allready it will solve the equation.
*/
void Ps2tohid::numSwitch()
{
  if (_lastNum == 0)
  {
    _lastNum = String(_currentNum);
    _currentNum = "";
  }
  else
  {
    solveNum();
  }
}

void Ps2tohid::_calcDelete()
{

  if (_currentNum.length() == 0)
  {                         // see if currrent num is emptue
    _op = 0;                // when current num is empty set op to zere
    _currentNum = _lastNum; // and switch to current num to last num
    _lastNum = "";
  }
  else
  {
    _currentNum.remove(_currentNum.length() - 1, 1); // deletes last char in currnt num
  }
}

// Turns keyboard in to a calculator. give it the PS2 scancode and the mode name
void Ps2tohid::calcMode(int code, String modeName)
{
  // clear screen, show header, and make text smaller
  display1.clearDisplay();
  display1.setTextSize(2);              // make text smaller
  display1.setTextColor(SSD1306_WHITE); // make white text
  display1.setCursor(0, 0);
  display1.print(modeName);
  display1.setTextSize(1);

  /*this see what key has been press
     when a key is press it  see if it is a number or a op
     if it is a num it add the num to _currentNum
     op it change op to what is press*/

  // delet if code is 0, a hot fiix for a bug, num lock not working
  // if (code == 1)
  // {
  //   if (_currentNum.length() == 0)
  //   {                         // see if currrent num is emptue
  //     _op = 0;                // when current num is empty set op to zere
  //     _currentNum = _lastNum; // and switch to current num to last num
  //     _lastNum = "";
  //   }
  // }

  if (!(code & PS2_BREAK))
    switch (code & 0xFF)
    {
    case PS2_KEY_ENTER:
    case PS2_KEY_KP_ENTER:
      if (_op > 0)
      {
        solveNum();
      }
      break;
    case PS2_KEY_EQUAL:
    case PS2_KEY_KP_PLUS:
    case PS2_KEY_T:
    case PS2_KEY_P:
      if (_lastNum == 0)
      {
        _op = 1;
      }
      else
      {
        _nextOp = 1;
      }
      numSwitch();
      break;
    case PS2_KEY_MINUS:
    case PS2_KEY_KP_MINUS:
      if (_currentNum.length() == 0)
      {
        _currentNum = String(_currentNum + "-");
      }
      else
      {
        if (_lastNum == 0)
        {
          _op = 2;
        }
        else
        {
          _nextOp = 2;
        }
        numSwitch();
      }
      break;
    case PS2_KEY_KP_TIMES:
    case PS2_KEY_X:
      if (_lastNum == 0)
      {
        _op = 3;
      }
      else
      {
        _nextOp = 3;
      }
      numSwitch();
      break;
    case PS2_KEY_KP_DIV:
    case PS2_KEY_DIV:
      if (_lastNum == 0)
      {
        _op = 4;
      }
      else
      {
        _nextOp = 4;
      }
      numSwitch();
      break;

    /* when you press a number key, it appends the num to current num */
    case PS2_KEY_KP0:
    case PS2_KEY_0:
      _currentNum = String(_currentNum + "0");
      break;
    case PS2_KEY_KP1:
    case PS2_KEY_1:
      _currentNum = String(_currentNum + "1");

      break;
    case PS2_KEY_KP2:
    case PS2_KEY_2:
      _currentNum = String(_currentNum + "2");

      break;
    case PS2_KEY_KP3:
    case PS2_KEY_3:
      _currentNum = String(_currentNum + "3");

      break;
    case PS2_KEY_KP4:
    case PS2_KEY_4:
      _currentNum = String(_currentNum + "4");

      break;
    case PS2_KEY_KP5:
    case PS2_KEY_5:
      _currentNum = String(_currentNum + "5");
      break;
    case PS2_KEY_KP6:
    case PS2_KEY_6:
      _currentNum = String(_currentNum + "6");
      break;
    case PS2_KEY_KP7:
    case PS2_KEY_7:
      _currentNum = String(_currentNum + "7");
      break;
    case PS2_KEY_KP8:
    case PS2_KEY_8:
      _currentNum = String(_currentNum + "8");
      break;
    case PS2_KEY_KP9:
    case PS2_KEY_9:
      _currentNum = String(_currentNum + "9");
      break;
    case PS2_KEY_KP_DOT:
    case PS2_KEY_DOT:
      if (_currentNum.indexOf('.') == -1) // this is so you cant press dot more than 1 time
        _currentNum = String(_currentNum + ".");
      break;
    // case PS2_KEY_NUM:
    case PS2_KEY_LANG1: // num lock key print number to your computer
    case PS2_KEY_BS:    // delete button
      if (_currentNum.length() == 0)
      {                         // see if currrent num is emptue
        _op = 0;                // when current num is empty set op to zere
        _currentNum = _lastNum; // and switch to current num to last num
        _lastNum = "";
      }
      else
      {
        _currentNum.remove(_currentNum.length() - 1, 1); // deletes last char in currnt num
      }
      break;
    case PS2_KEY_DELETE: // clear button
      _currentNum = "";
      _lastNum = "";
      _op = 0;
      _nextOp = 0;
      break;
    case PS2_KEY_LANG2: // scroll lock key print number to your computer
      BootKeyboard.print(_currentNum);
      break;
    case PS2_KEY_INSERT:
      // currentHexDec = _currentNum;
      break;

    // can change the amount of decail places shown with page up and down
    case PS2_KEY_PGDN:
      _decPoint--;
      _decPoint--;
    case PS2_KEY_PGUP:
      _decPoint++;
      if (_decPoint < 0)
        _decPoint = 0;
      display1.setCursor(0, 24);
      display1.print("Decials Shown: ");
      display1.print(_decPoint);
      break;
    }

  // update screen with all the calculator stuff
  display1.setCursor(0, 16);
  if (_op != 0)
  {                                // see if there is a operator
    display1.println(_lastNum);    // show last num first
    display1.println(_opStr[_op]); // show op next
  }
  display1.println(_currentNum); // always print current num last
  display1.display();            // update display
}

// returns what is in the calculater as a string
String Ps2tohid::calcNum()
{
  return _currentNum;
}

// should change to a array so (maybe) faster and easyer for changing

// This function convernts the PS2 scan codes to usb HID scan codes
// later change it to arrays and then have perma for differnt layouts. differnt array can be stored on sd card
int Ps2tohid::usbCodes(int code, int layout)
{
  int usbCode = 0;

  switch (layout)
  {
  case 1:
    usbCode = layout1[code & 0xFF];
    break;
  case 2:
    usbCode = layout2[code & 0xFF];
    break;
  case 3:
    usbCode = layout3[code & 0xFF];
    break;
  case 4:
    usbCode = layout4[code & 0xFF];
    break;
  case 5:
    usbCode = layout5[code & 0xFF];
    break;
  case 6:
    usbCode = layout6[code & 0xFF];
    break;

  default:
    usbCode = layout0[code & 0xFF];
    break;
  }
  return usbCode;
  // int usbCode = 0;
  // switch (code & 0xFF)
  // {
  // case PS2_KEY_PAUSE:
  //   usbCode = 0x48;
  //   break;
  // case PS2_KEY_LANG1: // num lock
  //   usbCode = 0x53;
  //   break;
  // case PS2_KEY_LANG2: // scroll lock
  //   usbCode = 0x47;
  //   break;
  // case PS2_KEY_LANG3: // cap lock
  //   usbCode = 0x39;
  //   break;
  // case PS2_KEY_L_SHIFT:
  //   usbCode = 0xE1;
  //   break;
  // case PS2_KEY_R_SHIFT:
  //   usbCode = 0xE5;
  //   break;
  // case PS2_KEY_L_CTRL:
  //   usbCode = 0xE0;
  //   break;
  // case PS2_KEY_R_CTRL:
  //   usbCode = 0xE4;
  //   break;
  // case PS2_KEY_L_ALT:
  //   usbCode = 0xE2;
  //   break;
  // case PS2_KEY_R_ALT:
  //   usbCode = 0xE6;
  //   break;
  // case PS2_KEY_A:
  //   usbCode = 4;
  //   break;
  // case PS2_KEY_B:
  //   usbCode = 5;
  //   break;
  // case PS2_KEY_C:
  //   usbCode = 6;
  //   break;
  // case PS2_KEY_D:
  //   usbCode = 7;
  //   break;
  // case PS2_KEY_E:
  //   usbCode = 8;
  //   break;
  // case PS2_KEY_F:
  //   usbCode = 9;
  //   break;
  // case PS2_KEY_G:
  //   usbCode = 10;
  //   break;
  // case PS2_KEY_H:
  //   usbCode = 11;
  //   break;
  // case PS2_KEY_I:
  //   usbCode = 12;
  //   break;
  // case PS2_KEY_J:
  //   usbCode = 13;
  //   break;
  // case PS2_KEY_K:
  //   usbCode = 14;
  //   break;
  // case PS2_KEY_L:
  //   usbCode = 15;
  //   break;
  // case PS2_KEY_M:
  //   usbCode = 16;
  //   break;
  // case PS2_KEY_N:
  //   usbCode = 17;
  //   break;
  // case PS2_KEY_O:
  //   usbCode = 18;
  //   break;
  // case PS2_KEY_P:
  //   usbCode = 19;
  //   break;
  // case PS2_KEY_Q:
  //   usbCode = 20;
  //   break;
  // case PS2_KEY_R:
  //   usbCode = 21;
  //   break;
  // case PS2_KEY_S:
  //   usbCode = 22;
  //   break;
  // case PS2_KEY_T:
  //   usbCode = 23;
  //   break;
  // case PS2_KEY_U:
  //   usbCode = 24;
  //   break;
  // case PS2_KEY_V:
  //   usbCode = 25;
  //   break;
  // case PS2_KEY_W:
  //   usbCode = 26;
  //   break;
  // case PS2_KEY_X:
  //   usbCode = 27;
  //   break;
  // case PS2_KEY_Y:
  //   usbCode = 28;
  //   break;
  // case PS2_KEY_Z:
  //   usbCode = 29;
  //   break;
  // case PS2_KEY_KP0:
  //   usbCode = 0x62;
  //   break;
  // case PS2_KEY_0:
  //   usbCode = 39;
  //   break;
  // case PS2_KEY_KP1:
  //   usbCode = 0x59;
  //   break;
  // case PS2_KEY_1:
  //   usbCode = 30;
  //   break;
  // case PS2_KEY_KP2:
  //   usbCode = 0x5A;
  //   break;
  // case PS2_KEY_2:
  //   usbCode = 31;
  //   break;
  // case PS2_KEY_KP3:
  //   usbCode = 0x5B;
  //   break;
  // case PS2_KEY_3:
  //   usbCode = 32;
  //   break;
  // case PS2_KEY_KP4:
  //   usbCode = 0x5C;
  //   break;
  // case PS2_KEY_4:
  //   usbCode = 33;
  //   break;
  // case PS2_KEY_KP5:
  //   usbCode = 0x5D;
  //   break;
  // case PS2_KEY_5:
  //   usbCode = 34;
  //   break;
  // case PS2_KEY_KP6:
  //   usbCode = 0x5E;
  //   break;
  // case PS2_KEY_6:
  //   usbCode = 35;
  //   break;
  // case PS2_KEY_KP7:
  //   usbCode = 0x5F;
  //   break;
  // case PS2_KEY_7:
  //   usbCode = 36;
  //   break;
  // case PS2_KEY_KP8:
  //   usbCode = 0x60;
  //   break;
  // case PS2_KEY_8:
  //   usbCode = 37;
  //   break;
  // case PS2_KEY_KP9:
  //   usbCode = 0x61;
  //   break;
  // case PS2_KEY_9:
  //   usbCode = 38;
  //   break;
  // case PS2_KEY_TAB:
  //   usbCode = 43;
  //   break;
  // case PS2_KEY_INSERT:
  //   usbCode = 0x49;
  //   break;
  // case PS2_KEY_BS:
  //   usbCode = 42;
  //   break;
  // case PS2_KEY_KP_ENTER:
  //   usbCode = 0x58;
  //   break;
  // case PS2_KEY_ENTER:
  //   usbCode = 40;
  //   break;
  // case PS2_KEY_PGDN:
  //   usbCode = 0x4E;
  //   break;
  // case PS2_KEY_PGUP:
  //   usbCode = 0x4B;
  //   break;
  // case PS2_KEY_L_ARROW:
  //   usbCode = 0x50;
  //   break;
  // case PS2_KEY_R_ARROW:
  //   usbCode = 0x4F;
  //   break;
  // case PS2_KEY_UP_ARROW:
  //   usbCode = 0x52;
  //   break;
  // case PS2_KEY_DN_ARROW:
  //   usbCode = 0x51;
  //   break;
  // case PS2_KEY_DELETE:
  //   usbCode = 0x4C;
  //   break;
  // case PS2_KEY_HOME:
  //   usbCode = 0x4A;
  //   break;
  // case PS2_KEY_END:
  //   usbCode = 0x4D;
  //   break;
  // case PS2_KEY_R_GUI:
  //   usbCode = 0xE7;
  //   break;
  // case PS2_KEY_L_GUI:
  //   usbCode = 0xE3;
  //   break;
  // case PS2_KEY_F1:
  //   usbCode = 0x3A;
  //   break;
  // case PS2_KEY_F2:
  //   usbCode = 0x3B;
  //   break;
  // case PS2_KEY_F3:
  //   usbCode = 0x3C;
  //   break;
  // case PS2_KEY_F4:
  //   usbCode = 0x3D;
  //   break;
  // case PS2_KEY_F5:
  //   usbCode = 0x3E;
  //   break;
  // case PS2_KEY_F6:
  //   usbCode = 0x3F;
  //   break;
  // case PS2_KEY_F7:
  //   usbCode = 0x40;
  //   break;
  // case PS2_KEY_F8:
  //   usbCode = 0x41;
  //   break;
  // case PS2_KEY_F9:
  //   usbCode = 0x42;
  //   break;
  // case PS2_KEY_F10:
  //   usbCode = 0x43;
  //   break;
  // case PS2_KEY_F11:
  //   usbCode = 0x44;
  //   break;
  // case PS2_KEY_F12:
  //   usbCode = 0x45;
  //   break;
  // case PS2_KEY_PRTSCR:
  //   usbCode = 0x46;
  //   break;
  // case PS2_KEY_ESC:
  //   usbCode = 41;
  //   break;
  // case PS2_KEY_SPACE:
  //   usbCode = 44;
  //   break;
  // case PS2_KEY_EQUAL:
  //   usbCode = 46;
  //   break;
  // case PS2_KEY_SEMI:
  //   usbCode = 51;
  //   break;
  // case PS2_KEY_COMMA:
  //   usbCode = 54;
  //   break;
  // case PS2_KEY_KP_MINUS:
  //   usbCode = 0x56;
  //   break;
  // case PS2_KEY_MINUS:
  //   usbCode = 45;
  //   break;
  // case PS2_KEY_KP_DOT:
  //   usbCode = 0x63;
  //   break;
  // case PS2_KEY_DOT:
  //   usbCode = 55;
  //   break;
  // case PS2_KEY_KP_DIV:
  //   usbCode = 0x54;
  //   break;
  // case PS2_KEY_DIV:
  //   usbCode = 56;
  //   break;
  // case PS2_KEY_KP_PLUS:
  //   usbCode = 0x57;
  //   break;
  // case PS2_KEY_KP_TIMES:
  //   usbCode = 0x55;
  //   break;
  // case PS2_KEY_BACK:
  //   usbCode = 49;
  //   break;
  // case PS2_KEY_OPEN_SQ:
  //   usbCode = 0x2F;
  //   break;
  // case PS2_KEY_CLOSE_SQ:
  //   usbCode = 0x30;
  //   break;
  // case PS2_KEY_APOS: // single quaouts
  //   usbCode = 52;
  //   break;
  // case PS2_KEY_SINGLE: // tilde
  //   usbCode = 53;
  //   break;
  // case PS2_KEY_MENU:
  //   usbCode = 0x65;
  // }
  // return usbCode;
}

/* This functions is used for recoading, make, or deleting macros

  int ScanCode - scancode (HID) of the key the macro is for

  String modeNum - the mode number of the mode the macro is for
    the macor is located on SD card at modeNum folder in ScanCode file

  bool sendKeyCodes - if true then the keyboard will sent scancodes to connected device

  bool deleteMacro - delects the macro files so there is no more macors on the key.

  int speedChange - speed multiplyer. didvides the delay between scancodes, so you can play back macros faster.
        set speedChange to 1 for no change in delays
*/
void Ps2tohid::recordMacros(int ScanCode, String modeNum, bool sendKeyCodes, bool deleteMacro, int speedChange = 1)
{
  if (speedChange <= 0)
    speedChange = 1;
  // filename is the dir + file name of the macro
  String fileName = String(modeNum + '/' + String(ScanCode) + ".TXT");
  // if there is already a macro, it will delete it
  if (SD.exists(fileName))
  {
    SD.remove(fileName);
    if (deleteMacro)
    {
      setSetting();
      return; // return if user just wants to delet the macro
    }
  }

  if (deleteMacro)
  {
    setSetting();
    return;
  }
  // if there is no dir, it will make one
  if (!(SD.exists(modeNum + '/')))
  {
    SD.mkdir(modeNum + '/');
  }

  bool removeDelay = true;
  // open file
  File dataFile = SD.open(fileName, FILE_WRITE);

  _code = keyboardCheck();
  // wait for keypress before recording macros
  // this is for when you play back a macro, there is no delay after you press the key
  while (_code == 0)
  {
    _code = keyboardCheck();
  }

  while (_code > -1)
  {
    String keyString = "";
    String delayString = "";
    _code = keyboardCheck();
    if (_code > 0)
    {

      // if the file is available, write to it:

      delayString += String(_keyDelay / speedChange);
      int usbScanCodes = usbCodes(_code, modeNum.toInt());

      if (!(_code & PS2_BREAK))
      {
        keyString += String(usbScanCodes + 256);
        if (sendKeyCodes)
        {
          BootKeyboard.press(KeyboardKeycode(usbScanCodes)); // press the key
        }
      }
      if ((_code & PS2_BREAK))
      {
        keyString += String(usbScanCodes);
        if (sendKeyCodes)
        {
          BootKeyboard.release(KeyboardKeycode(usbScanCodes)); // release the key
        }
      }

      if (dataFile)
      {
        if ((removeDelay))
        {
          dataFile.println("-1"); // if i recalled right -1 means release all keys
          removeDelay = false;
        }
        else
        {
          dataFile.println(delayString);
        }
        dataFile.println(keyString);
        // dataFile.close();
      }
    }
  }
  dataFile.close();
  setSetting(); // update setting after adding macro
}

// test to see if there is a macro for a key that is presses
// testes to see if the file is on SD card
bool Ps2tohid::playMacros(int ScanCode, String modeNum, bool sendCodes)
{

  if (_marcoKeyList[ScanCode] == false)
  { // check to see if the key has a macro, if not then return function
    return false;
  }
  else if (!(sendCodes)) // ?
  {                      // if you use function toto see if
    return true;
  }

  // redunt may remove
  String fileName = String(modeNum + '/' + String(ScanCode));
  if (!(SD.exists(modeNum + "/" + String(ScanCode) + ".TXT")))
  {
    return false;
  }
  else if (!(sendCodes)) // ?
  {                      // if you use function toto see if
    return true;
  }

  String buffer;
  File dataFile = SD.open(modeNum + "/" + String(ScanCode) + ".TXT");

  // If the file is available, read it
  if (dataFile)
  {
    while (dataFile.available())
    {
      // Write one line to buffer
      buffer = dataFile.readStringUntil('\n');
      // Print to serial monitor
      // Serial.println(buffer);

      int rawDateInt = buffer.toInt();
      if (rawDateInt < 0)
      {
        rawDateInt = rawDateInt * -1;
        delay(rawDateInt);
      }
      else if (rawDateInt > 256)
      {
        rawDateInt = rawDateInt - 256;
        BootKeyboard.press(KeyboardKeycode(rawDateInt));
      }
      else
      {
        BootKeyboard.release(KeyboardKeycode(rawDateInt));
      }
      // break out of macro loop if you press function button, and release all the keys
      if (keyboardCheck() == -1)
      {
        BootKeyboard.releaseAll();
        break;
      }
    }
    dataFile.close();
  }
  return true;
}

/* This function is for differnt keyboard modes with differnt macros

String modeName - The mode number the keyboard is in.
 it corrispons with the that modes macros

bool calcFuncMode - if true then the num pad will be a calculator

returns true if function key is press
returns false if keyboard has reach screen timeout
*/
bool Ps2tohid::keyMacroMode(String modeName, bool calcFuncMode)
{
  bool switchModes = true;
  int usbPress;
  int keyboardCode = 0;
  if (calcFuncMode)
  {
    calcMode(keyboardCode, "Calc+Key " + modeName);
  }

  switch (modeName.toInt())
  {
  case 1:
    for (int i = 0; i <= 254; i++)
    {
      _marcoKeyList[i] = _marcoKeyList1[i];
    }
    break;
  case 2:
    for (int i = 0; i <= 254; i++)
    {
      _marcoKeyList[i] = _marcoKeyList2[i];
    }
    break;
  case 3:
    for (int i = 0; i <= 254; i++)
    {
      _marcoKeyList[i] = _marcoKeyList3[i];
    }
    break;
  case 4:
    for (int i = 0; i <= 254; i++)
    {
      _marcoKeyList[i] = _marcoKeyList4[i];
    }
    break;
  case 5:
    for (int i = 0; i <= 254; i++)
    {
      _marcoKeyList[i] = _marcoKeyList5[i];
    }
    break;
  case 6:
    for (int i = 0; i <= 254; i++)
    {
      _marcoKeyList[i] = _marcoKeyList6[i];
    }
    break;
  }

  while (keyboardCode > -1)
  {
    keyboardCode = keyboardCheck();

    /*
    if keyboard is in cal mode, it will first check if there are any num pad kays
    if there is a num pad key press it runs the calc functions
    when done with calc functions, it will change keyboardCode to 0
    */

    // so dont also sent scan codes to the usb device
    if (calcFuncMode)
    {
      if (!(keyboardCode & PS2_BREAK))
      {
        switch (keyboardCode)
        {
        case PS2_KEY_LANG1: // num lock key, acts like bs key in this case
                            // case PS2_KEY_NUM: // num lock key, acts like bs key in this case
        case PS2_KEY_KP0:
        case PS2_KEY_KP1:
        case PS2_KEY_KP2:
        case PS2_KEY_KP3:
        case PS2_KEY_KP4:
        case PS2_KEY_KP5:
        case PS2_KEY_KP6:
        case PS2_KEY_KP7:
        case PS2_KEY_KP8:
        case PS2_KEY_KP9:
        case PS2_KEY_KP_DIV:
        case PS2_KEY_KP_DOT:
        case PS2_KEY_KP_TIMES:
        case PS2_KEY_KP_PLUS:
        case PS2_KEY_KP_MINUS:
        case PS2_KEY_KP_EQUAL:
        case PS2_KEY_KP_ENTER:
          calcMode(keyboardCode, "Calc+Key " + modeName);
          keyboardCode = 0;
          break;
        }
      }
      else if ((keyboardCode & PS2_BREAK))
      {
        switch (keyboardCode)
        {
        case PS2_KEY_LANG1: // num lock key, acts like bs key in this case
        case PS2_KEY_KP0:
        case PS2_KEY_KP1:
        case PS2_KEY_KP2:
        case PS2_KEY_KP3:
        case PS2_KEY_KP4:
        case PS2_KEY_KP5:
        case PS2_KEY_KP6:
        case PS2_KEY_KP7:
        case PS2_KEY_KP8:
        case PS2_KEY_KP9:
        case PS2_KEY_KP_DIV:
        case PS2_KEY_KP_DOT:
        case PS2_KEY_KP_TIMES:
        case PS2_KEY_KP_PLUS:
        case PS2_KEY_KP_MINUS:
        case PS2_KEY_KP_EQUAL:
        case PS2_KEY_KP_ENTER:
          keyboardCode = 0;
          break;
        }
      }
    }

    if (keyboardCode > 0)
    {
      usbPress = usbCodes(keyboardCode, modeName.toInt());
      if (!(keyboardCode & PS2_BREAK))
      { // see if key is press

        if (!(playMacros(usbPress, modeName, true))) // see if there is a macro on sd card for a key that was just pressed
        {
          switch (usbPress)
          {
          case 0x53: // num lock, use to print what is in calculater
            // i dont know why but this allways run (when num lock press) even if its not support to
            if (calcFuncMode)
            {
              calcMode(0x96, "Calc+Key " + modeName);
              //_calcDelete();
            }
            break;
          case 0x39: // cap lock, do nothing
            break;
          case 0x47: // scroll lock, use to print what is in calculater
            BootKeyboard.print(_currentNum);
            break;
          default:
            BootKeyboard.press(KeyboardKeycode(usbPress)); // press the key
          }
        }
      }
      else
      { // see if key is not press it will release key
        if (!(playMacros(usbPress, modeName, false)))
        {
          switch (usbPress)
          {
          case 0x53: // num lock, use to print what iis in calculater
            // Keyboard.print(_currentNum);
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
  }
  BootKeyboard.releaseAll(); // release all key when switching modes
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
