#ifndef PS2TOHID_h
#define PS2TOHID_h

#include "Arduino.h"
#include <PS2KeyAdvanced.h>
#include <HID-Project.h>
#include <HID-Settings.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class Ps2tohid
{
public:
  // Constructor
  Ps2tohid(int interval, int functionKey, byte flipScreen);

  const long interval = 60 * 1000;
  void setSetting();    // get setting from sd card
  void updateSetting(); // update current setting to sd card
  // Methods
  void begin(int dataPin1, int clkPin2, int chipSelect);
  int keyboardCheck();
  int functionKey;
  int usbCodes(int code, int layout);
  unsigned long _interval = 1000 * 60;
  bool _modeSwitch = false;
  // bool hexDecMode();
  // int _solveHexDec(String currentHexDec);
  long keyPressDelay();
  long _keyDelay;
  void recordMacros(int ScanCode, String modeNum, bool sendKeyCodes, bool deleteMacro, int speedChange);
  bool playMacros(int ScanCode, String modeNum, bool sendCodes);
  bool keyMacroMode(String modeName, bool calcFuncMode); // dafda
  bool sdStatus();
  String calcNum();
  void solveNum();
  void numSwitch();
  void calcMode(int code, String modeName);
  byte displayRotation();
  // all the layouts take about 1.2k of ram
  byte layout1[161];
  byte layout2[161];
  byte layout3[161];
  byte layout4[161];
  byte layout5[161];
  byte layout6[161];
  // default layout, store not on SD card. normal us layout
  byte layout0[161] = {
      0,    // not a scan code, all 0 mean not bindend to anything
      0,    // #define PS2_KEY_NUM         0X01	// was 0x01, change to unreal num so dont break
      0,    // #define PS2_KEY_SCROLL      0X02	// was 0x02
      0,    // #define PS2_KEY_CAPS        0X03	// was 0x03
      0x46, // #define PS2_KEY_PRTSCR      0x04
      0x48, // #define PS2_KEY_PAUSE       0x05
      0xE1, // #define PS2_KEY_L_SHIFT     0x06
      0xE5, // #define PS2_KEY_R_SHIFT     0x07
      0xE0, // #define PS2_KEY_L_CTRL      0X08
      0xE4, // #define PS2_KEY_R_CTRL      0X09
      0xE2, // #define PS2_KEY_L_ALT       0x0A
      0xE6, // #define PS2_KEY_R_ALT       0x0B
      0xE3, // #define PS2_KEY_L_GUI       0x0C
      0xE7, // #define PS2_KEY_R_GUI       0x0D
      0x65, // #define PS2_KEY_MENU        0x0E
      0,    // #define PS2_KEY_BREAK       0x0F   not set up yet also dont always work
      154,  // #define PS2_KEY_SYSRQ       0x10   // not sure if right
      0x4A, // #define PS2_KEY_HOME        0x11
      0x4D, // #define PS2_KEY_END         0x12
      0x4B, // #define PS2_KEY_PGUP        0x13
      0x4E, // #define PS2_KEY_PGDN        0x14
      0x50, // #define PS2_KEY_L_ARROW     0x15
      0x4F, // #define PS2_KEY_R_ARROW     0x16
      0x52, // #define PS2_KEY_UP_ARROW    0x17
      0x51, // #define PS2_KEY_DN_ARROW    0x18
      0x49, // #define PS2_KEY_INSERT      0x19
      0x4C, // #define PS2_KEY_DELETE      0x1A
      41,   // #define PS2_KEY_ESC         0x1B
      42,   // #define PS2_KEY_BS          0x1C
      43,   // #define PS2_KEY_TAB         0x1D
      0x58, // #define PS2_KEY_ENTER       0x1E
      44,   // #define PS2_KEY_SPACE       0x1F
      0x62, // #define PS2_KEY_KP0         0x20
      0x59, // #define PS2_KEY_KP1         0x21
      0x5A, // #define PS2_KEY_KP2         0x22
      0x5B, // #define PS2_KEY_KP3         0x23
      0x5C, // #define PS2_KEY_KP4         0x24
      0x5D, // #define PS2_KEY_KP5         0x25
      0x5E, // #define PS2_KEY_KP6         0x26
      0x5F, // #define PS2_KEY_KP7         0x27
      0x60, // #define PS2_KEY_KP8         0x28
      0x61, // #define PS2_KEY_KP9         0x29
      0x63, // #define PS2_KEY_KP_DOT      0x2A
      0x58, // #define PS2_KEY_KP_ENTER    0x2B
      0x57, // #define PS2_KEY_KP_PLUS     0x2C
      0x56, // #define PS2_KEY_KP_MINUS    0x2D
      0x55, // #define PS2_KEY_KP_TIMES    0x2E
      0x54, // #define PS2_KEY_KP_DIV      0x2F
      39,   // #define PS2_KEY_0           0X30
      30,   // #define PS2_KEY_1           0X31
      31,   // #define PS2_KEY_2           0X32
      32,   // #define PS2_KEY_3           0X33
      33,   // #define PS2_KEY_4           0X34
      34,   // #define PS2_KEY_5           0X35
      35,   // #define PS2_KEY_6           0X36
      36,   // #define PS2_KEY_7           0X37
      37,   // #define PS2_KEY_8           0X38
      38,   // #define PS2_KEY_9           0X39
      52,   // #define PS2_KEY_APOS        0X3A
      54,   // #define PS2_KEY_COMMA       0X3B
      45,   // #define PS2_KEY_MINUS       0X3C
      55,   // #define PS2_KEY_DOT         0X3D
      56,   // #define PS2_KEY_DIV         0X3E
      46,   // #define PS2_KEY_KP_EQUAL    0x3F
      53,   // #define PS2_KEY_SINGLE      0X40
      4,    // #define PS2_KEY_A           0X41
      5,    // #define PS2_KEY_B           0X42
      6,    // #define PS2_KEY_C           0X43
      7,    // #define PS2_KEY_D           0X44
      8,    // #define PS2_KEY_E           0X45
      9,    // #define PS2_KEY_F           0X46
      10,   // #define PS2_KEY_G           0X47
      11,   // #define PS2_KEY_H           0X48
      12,   // #define PS2_KEY_I           0X49
      13,   // #define PS2_KEY_J           0X4A
      14,   // #define PS2_KEY_K           0X4B
      15,   // #define PS2_KEY_L           0X4C
      16,   // #define PS2_KEY_M           0X4D
      17,   // #define PS2_KEY_N           0X4E
      18,   // #define PS2_KEY_O           0X4F
      19,   // #define PS2_KEY_P           0X50
      20,   // #define PS2_KEY_Q           0X51
      21,   // #define PS2_KEY_R           0X52
      22,   // #define PS2_KEY_S           0X53
      23,   // #define PS2_KEY_T           0X54
      24,   // #define PS2_KEY_U           0X55
      25,   // #define PS2_KEY_V           0X56
      26,   // #define PS2_KEY_W           0X57
      27,   // #define PS2_KEY_X           0X58
      28,   // #define PS2_KEY_Y           0X59
      29,   // #define PS2_KEY_Z           0X5A
      51,   // #define PS2_KEY_SEMI        0X5B
      49,   // #define PS2_KEY_BACK        0X5C
      0x2F, // #define PS2_KEY_OPEN_SQ     0X5D
      0x30, // #define PS2_KEY_CLOSE_SQ    0X5E
      46,   // #define PS2_KEY_EQUAL       0X5F
      133,  // #define PS2_KEY_KP_COMMA    0x60
      0x3A, // #define PS2_KEY_F1          0X61
      0x3B, // #define PS2_KEY_F2          0X62
      0x3C, // #define PS2_KEY_F3          0X63
      0x3D, // #define PS2_KEY_F4          0X64
      0x3E, // #define PS2_KEY_F5          0X65
      0x3F, // #define PS2_KEY_F6          0X66
      0x40, // #define PS2_KEY_F7          0X67
      0x41, // #define PS2_KEY_F8          0X68
      0x42, // #define PS2_KEY_F9          0X69
      0x43, // #define PS2_KEY_F10         0X6A
      0x44, // #define PS2_KEY_F11         0X6B
      0x45, // #define PS2_KEY_F12         0X6C
      104,  // #define PS2_KEY_F13         0X6D
      105,  // #define PS2_KEY_F14         0X6E
      106,  // #define PS2_KEY_F15         0X6F
      107,  // #define PS2_KEY_F16         0X70
      108,  // #define PS2_KEY_F17         0X71
      109,  // #define PS2_KEY_F18         0X72
      110,  // #define PS2_KEY_F19         0X73
      111,  // #define PS2_KEY_F20         0X74
      112,  // #define PS2_KEY_F21         0X75
      113,  // #define PS2_KEY_F22         0X76
      114,  // #define PS2_KEY_F23         0X77
      115,  // #define PS2_KEY_F24         0X78
      0,    // #define PS2_KEY_NEXT_TR     0X79   not set up yet
      0,    // #define PS2_KEY_PREV_TR     0X7A   not set up yet
      120,  // #define PS2_KEY_STOP        0X7B
      125,  // #define PS2_KEY_PLAY        0X7C
      127,  // #define PS2_KEY_MUTE        0X7D
      128,  // #define PS2_KEY_VOL_UP      0X7E
      129,  // #define PS2_KEY_VOL_DN      0X7F
      0,    // #define PS2_KEY_MEDIA       0X80   not set up yet
      0,    // #define PS2_KEY_EMAIL       0X81   not set up yet
      0,    // #define PS2_KEY_CALC        0X82   not set up yet
      0,    // #define PS2_KEY_COMPUTER    0X83   not set up yet
      0,    // #define PS2_KEY_WEB_SEARCH  0X84   not set up yet
      0,    // #define PS2_KEY_WEB_HOME    0X85   not set up yet
      0,    // #define PS2_KEY_WEB_BACK    0X86   not set up yet
      0,    // #define PS2_KEY_WEB_FORWARD 0X87   not set up yet
      0,    // #define PS2_KEY_WEB_STOP    0X88   not set up yet
      0,    // #define PS2_KEY_WEB_REFRESH 0X89   not set up yet
      0,    // #define PS2_KEY_WEB_FAVOR   0X8A   not set up yet
      0,    // #define PS2_KEY_EUROPE2     0X8B   not set up yet
      102,  // #define PS2_KEY_POWER       0X8C
      0,    // #define PS2_KEY_SLEEP       0X8D   not set up yet
      0,    // not a scan code
      0,    // not a scan code
      0,    // #define PS2_KEY_WAKE        0X90   not set up yet
      135,  // #define PS2_KEY_INTL1       0X91
      136,  // #define PS2_KEY_INTL2       0X92
      137,  // #define PS2_KEY_INTL3       0X93
      138,  // #define PS2_KEY_INTL4       0X94
      139,  // #define PS2_KEY_INTL5       0X95
      0x53, // #define PS2_KEY_LANG1       0X96  // now num lock
      0x47, // #define PS2_KEY_LANG2       0X97  // scroll lock
      0x39, // #define PS2_KEY_LANG3       0X98  // cap lock
      147,  // #define PS2_KEY_LANG4       0X99
      0,    // not a scan code
      0,    // not a scan code
      0,    // not a scan code
      0,    // not a scan code
      0,    // not a scan code
      0,    // not a scan code
      148,  // #define PS2_KEY_LANG5       0xA0

  };

private:
  unsigned long _previousMillis;
  unsigned long _previousKeypress;
  unsigned long _currentMillis;
  int _code;
  bool _sdStatus = true;
  String _lastNum;                // pereviens _currentNum that the op and _currentNum is used to get answer
  double _eq;                     // the valve that is the answer
  int _nextOp = 0;                // When you press a opertor when there already one, if will put it on after it solve the problem
  int _op = 0;                    // op selces what operators the user inputes in cal mode
  const char _opStr[6] = " +-*/"; // converts op to a string that can be displayed
  int _decPoint = 6;              // max number of decmal point that are show in answer
  String _currentNum;             // string that hold the number that is being modiffy
  byte _displayRotation = 0;      // setting to see if the screen need to be flipped, true is normal
  void _calcDelete();
  bool _marcoKeyList[255];        // the list of keys that have macros in the mode you are in. List is in HID scancodes
  bool _marcoKeyList1[255];        // the list of keys that have macros in the mode you are in. List is in HID scancodes
  bool _marcoKeyList2[255];        // the list of keys that have macros in the mode you are in. List is in HID scancodes
  bool _marcoKeyList3[255];        // the list of keys that have macros in the mode you are in. List is in HID scancodes
  bool _marcoKeyList4[255];        // the list of keys that have macros in the mode you are in. List is in HID scancodes
  bool _marcoKeyList5[255];        // the list of keys that have macros in the mode you are in. List is in HID scancodes
  bool _marcoKeyList6[255];        // the list of keys that have macros in the mode you are in. List is in HID scancodes
};
#endif
