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
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)
//#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// initalized functions befor they are used
void _macroFinder();
int keyboardCheck();
bool sdStatus();
byte displayRotation();
void setLayout();

// int _solveHexDec(String currentHexDec);

/*!
    @brief  This constuter is used to set the defualt settings for the keyboard and gets things setted up. note all the setting can be change with the SD card.
    @param  interval
            The time in minuise that it takes for the display on the keyboard to turn
            off to revent screen burn in.
    @param  funcKey
            This sets what key is used as the function key. The function key is used to switch modes, hult playing macros, and stop recording macros.
    @param  flipScreen
            This rotates the screen. 0 is normal and 2 is 180 degs
    @note   Should call this befor other things in the lib and before setup. I should shang it to a function too.
*/
Ps2tohid::Ps2tohid(int interval, int funcKey = PS2_KEY_MENU, byte flipScreen = 0)
{
  _interval = _interval * interval;
  functionKey = funcKey;
  _displayRotation = flipScreen;
}

/*!
    @brief  This function initallizes everything and should only run 1 time in the setup.
    @param  dataPin1
            The pin the data line of the PS2 keyboard is connected.
    @param  clkPin2
            The pin the clock line of the PS2 keyboard is connected.
    @param  chipSelect
            The pin the SD card chipselect is connected to.
    @param  i2cAdrs
            The I2C address of the diaplay. Default is 0x3C
    @note
*/
void Ps2tohid::begin(int dataPin1, int clkPin2, int chipSelect, int i2cAdrs = 0x3C)
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
  // }.
  BootKeyboard.releaseAll(); // release all keys when plug in keyboard, sometimes some keys are press  when they are not
}

// return true if SD card was plug in when the keyboard was powered up
bool Ps2tohid::sdStatus()
{
  return _sdStatus;
}

// returns the rotaction of the display
byte Ps2tohid::displayRotation()
{
  return _displayRotation;
}

/* This function sets all the libary settings to the settings that are stored on the SD card.
If a setting is not stored on the SD card, it will use the defualt settings.
*/
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
          //  splits buffer it to the 2 parts. name and data parts, splits by =
          int breakIdx = buffer.indexOf('=');                  // the point the data is splited
          String settingName = buffer.substring(0, breakIdx);  // the name of the setting
          String settingData = buffer.substring(breakIdx + 1); // value of the setting

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

/* Updates the key layout on the micro to the layout stored on the SD card.
  If it cant find a layout file on SD card, it will use the default layout.
*/
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
        for (int n = 0; n <= 160; n++) // runs only 161 times
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
    else // if there is no sd card or the mode dont have a layout, this will make it the defualt layout
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

/*!
    @brief  This function is used to rebind a mod key. This is store on a file on the SD card.
    @param  ScanCode
            HID scancode you want to bind the PS2 scancode
    @param  keyCode
            The PS2 scancode of the key that you want to change to translation to the HID scancode
    @param  layout
            Name of the mode the keyboard is in, and is the layout that this will effect.
    @param  updateSD
            When true, will update layout on SD card. When false only update the array for the layout
    @note
*/
void Ps2tohid::keyRebinder(int ScanCode, int keyCode, int layout, bool updateSD)
{
  if (!_sdStatus) // breaks if no sd card in
  {
    return;
  }
  // setLayout(); // update the arrays of the layouts from SD card

  String fileName = String(String(layout) + '/' + "LAYOUT.TXT");

  // check to see if the layout file exists, if does it will remove it
  if (SD.exists(fileName))
  {
    SD.remove(fileName);
  }

  // see if dir exist, if not will make it
  if (!(SD.exists(String(layout) + '/')))
  {
    SD.mkdir(String(layout) + '/');
  }

  // make and open new layout file
  File dataFile = SD.open(fileName, FILE_WRITE);

  switch (layout) //  make the changes to the layout arrays that is wanted to change
  {
  case 1:
    layout1[keyCode & 0xFF] = ScanCode; // update the key binding
    if (updateSD)                       // only update the sd card when flag is true
    {
      for (int i = 0; i <= 160; i++) // loop through the array and prints it out on the SD card
      {
        dataFile.println(String(layout1[i]));
      }
    }
    break;
  case 2:
    layout2[keyCode & 0xFF] = ScanCode;
    if (updateSD) // only update the sd card when flag is true
    {
      for (int i = 0; i <= 160; i++)
      {
        dataFile.println(String(layout2[i]));
      }
    }
    break;
  case 3:
    layout3[keyCode & 0xFF] = ScanCode;
    if (updateSD) // only update the sd card when flag is true
    {
      for (int i = 0; i <= 160; i++)
      {
        dataFile.println(String(layout3[i]));
      }
    }
    break;
  case 4:
    layout4[keyCode & 0xFF] = ScanCode;
    if (updateSD) // only update the sd card when flag is true
    {
      for (int i = 0; i <= 160; i++)
      {
        dataFile.println(String(layout4[i]));
      }
    }
    break;
  case 5:
    layout5[keyCode & 0xFF] = ScanCode;
    if (updateSD) // only update the sd card when flag is true
    {
      for (int i = 0; i <= 160; i++)
      {
        dataFile.println(String(layout5[i]));
      }
    }
    break;
  case 6:
    layout6[keyCode & 0xFF] = ScanCode;
    if (updateSD) // only update the sd card when flag is true
    {
      for (int i = 0; i <= 160; i++)
      {
        dataFile.println(String(layout6[i]));
      }
    }
    break;
  }

  dataFile.close(); // close the file on SD card
}

/*!
    @brief  This function is used to check if there has been a keypress from the PS2 keyboard.
    @return
            Positive Number - the scancode of the key that was press/release.
            0  - No keypresses.
            -1 - Function key was press.
            -2 - If it has reach the sleep timeout.
    @note
*/
int Ps2tohid::keyboardCheck()
{
  _currentMillis = millis();
  if (keyboard1.available())
  {
    // read the next key
    int _code = keyboard1.read(); // could change _code to uint16_t
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

// // used to delete the last thing in the cal, this is not used
// void Ps2tohid::_calcDelete()
// {

//   if (_currentNum.length() == 0)
//   {                         // see if currrent num is emptue
//     _op = 0;                // when current num is empty set op to zere
//     _currentNum = _lastNum; // and switch to current num to last num
//     _lastNum = "";
//   }
//   else
//   {
//     _currentNum.remove(_currentNum.length() - 1, 1); // deletes last char in currnt num
//   }
// }

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

/*!
    @brief  This function converts the PS2 scancodes into the HID scancodes
    @param  code
            The PS2 scancode that is converted to a HID scancode.
    @param  layout
            What layout that is used in the convertion, default is 0
    @return HID scancode
    @note
*/
int Ps2tohid::usbCodes(int code, int layout = 0)
{
  int usbCode = 0;

  switch (layout) // switch between differnt layouts
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
}

/*!
  @brief This functions is used for recoading, make, or deleting macros. The function key is used to end the recording.

  @param ScanCode
          scancode (HID) of the key the macro is for

  @param modeNum
  The mode number of the mode the macro is for
  the macro is located on SD card at modeNum folder in ScanCode file. This is a STRING.

  @param sendKeyCodes If true then the keyboard will sent scancodes to connected device.

  @param deleteMacro Delects the macro files so there is no more macors on the key for the given mode.

  @param speedChange Speed multiplyer. Didvides the delay between scancodes, so you can play back macros faster.
        set speedChange to 1 for no change in delays. Defualt is 1.
  @note
*/
void Ps2tohid::recordMacros(int ScanCode, int modeNum, bool sendKeyCodes, bool deleteMacro, int speedChange = 1)
{
  String modeName = String(modeNum);
  if (speedChange <= 0)
  {
    speedChange = 1;
  }
  // filename is the dir + file name of the macro
  String fileName = String(modeName + '/' + String(ScanCode) + ".TXT");
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
  if (!(SD.exists(modeName + '/')))
  {
    SD.mkdir(modeName + '/');
  }

  bool removeDelay = true;
  // open file_calcDelete
  File dataFile = SD.open(fileName, FILE_WRITE);

  int _code = keyboardCheck();
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
      int usbScanCodes = usbCodes(_code, modeNum);

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

/*!
  @brief This function plays back a macro if the key that was press has one, only press not release. it checks the SD card for the macros.

  @param ScanCode The HID scancode that is used to check if there is a macros with the same name

  @param modeNum Name of the mode the keyboard is in.

  @param sendCodes if true it will play the macro back if the key has one. false will not play the macro if there is one

  @return true if there is a macro for the key, false if there are not macros for the key

  @note
*/
bool Ps2tohid::playMacros(int ScanCode, int modeNum, bool sendCodes)
{

  if (_marcoKeyList[ScanCode] == false)
  { // check to see if the key has a macro, if not then return function
    return false;
  }
  else if (!(sendCodes)) //
  {
    return true; // if a key has macro and sendCode false, reuturn true, usfull for key releases
  }
  else if (!(SD.exists(String(modeNum) + "/" + String(ScanCode) + ".TXT"))) // double check to see if it is still there or something
  {
    return false;
  }

  String buffer;
  File dataFile = SD.open(String(modeNum) + "/" + String(ScanCode) + ".TXT");

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

/*!
  @brief This function runs most of the diffent modes on the keyboard. It acts like a normal usb keyboard, unless a key has a macro on, then it plays the macro binded to the key.

  @param modeName Name of the mode the keyboard is in. 1-6

  @param calcFuncMode Name of the mode the keyboard is in. False maen numpad is normal, true means numpad is calc.

  @return True when switch modes. False to got to sleep.

  @note
*/
bool Ps2tohid::keyMacroMode(int modeName, bool calcFuncMode = false)
{
  bool switchModes = true;
  int usbPress;
  int keyboardCode = 0;
  if (calcFuncMode)
  {
    calcMode(keyboardCode, "Calc+Key " + String(modeName));
  }

  switch (modeName)
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
          calcMode(keyboardCode, "Calc+Key " + String(modeName));
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
      usbPress = usbCodes(keyboardCode, modeName);
      if (!(keyboardCode & PS2_BREAK))
      { // see if key is press

        if (!(playMacros(usbPress, modeName, true))) // see if there is a macro on sd card for a key that was just pressed
        {
          if (usbPress == 0x47 || usbPress == 0x53)
          { // check to see if scroll/num lock are press
            if (usbPress == 0x53)
            { // if num lock press
              if (!calcFuncMode)
              {
                BootKeyboard.press(KeyboardKeycode(usbPress)); // release the key when not in calc mode
              }
              else
              {
                calcMode(0x96, "Calc+Key " + String(modeName));
              }
            }
            else
            {
              BootKeyboard.print(_currentNum);
            }
          }
          else
          {                                                // normall key are press as normal
            BootKeyboard.press(KeyboardKeycode(usbPress)); // press the key
          }
        }
      }
      else
      {                                               // see if key is not press it will release key
        if (!(playMacros(usbPress, modeName, false))) // if there is a macro for the key that was release, it will not sent a key release scancode
        {
          if (usbPress == 0x47 || usbPress == 0x53)
          { // check to see if scroll/num lock are press
            if (usbPress == 0x53 && !calcFuncMode)
            {
              BootKeyboard.release(KeyboardKeycode(usbPress)); // release the key when not in calc mode
            }
            // not press any keys
          }
          else
          {
            BootKeyboard.release(KeyboardKeycode(usbPress)); // press the key
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