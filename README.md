# PUMBA-Keyboard
PUMBA-Keyboard is a PS2 to USB macro board adapter made with the arduino framework.

PUMBA stands for: PS2 to USB Macro Board Adpter

WARNING: code mostly done, but a lot of work is need on documention. There are a lot of spelling/grammer mistakes and still need to add stuff to the repo.

This is an arduino project that converts a PS2 keyboard into an USB keyaboard with addital fetures. It has has a built in calacultor and the abilty to make/playback macros. It had a small display on it to display differnt things. 

![IMG_20220626_191421](https://user-images.githubusercontent.com/102840190/175848182-f5005b3c-a151-4c44-8dc3-c826e4afac13.jpg)

Here is a list of fetures the PUMBA-Keyboard has:
  * 6 differnt keyboards modes in which nearly all keys can have a differnt macro asign to it.
  * The ablity to easly make macros on the fly without any PC software
  * All setting/macros are stored on the SD card so you can easyly modifly/backup them.
  * Calculater mode that turns part or the whole keyboard in to calculate and send results as keypresses
  * In any of the 6 keyboard modes you can convert the numpad into a calculter and leave the rest of the keyboard alown 
  * Hex and Dec converter mode
  * Forces numlock on so you dont always have to turn it on (do on startup and when modes are being changed)
  * Some other things but cant remenber
  * the abilty to rebind keys and have differnt key layouts per keyboard mode (half way done)
  * WILL ADD SOON the ablity to change settings/key bindings without puting the sd card into a PC 


PUMBA has only been test with SAMD21 and SAMD51 micro controll board. The project COULD work on most microcontroller that can emulate a HID device. It will not work on a Arduino leardo or Uno because the program takes to much ram. It has worked with almost all PS2 keyboards i have try. It will not work with most USB keyboard using a USB to PS2 adperter. the code was writen useing plateformio with the arduino framwork.

Tested Boards:
* Adafruit Feather M4 Express
* Adafruit QT Py SAMD21
* Seeeduino Xiao

There are PCB and cases for this project. You dont have to use the PCB, you can embend everything inside a keyboard or use a bread board. included is KiCad project files, pdf of the schmatic, pdf of the PCBs, and BOM.

The PUMBA keyboard has 6 differnt keyboard modes wither there own layouts and macros on it. There are is one mode decacated just for the keyboard being just a calulator, but all Keyboard modes have the ablity for the numpad to become a calculator. There is also a hex and dec converter mode and another keyboard mode that cant use macros.WILL ADD SOON 

When you plug in the PUMBA keyboard into a computer it will go into "Keyboard 1" mode. You switch modes by pressing the "function key" witch is defualt set to the menu button. after you press the function key the display will pop up things that you can do. To switch modes you press the corispnding key for the mode. if you press the function key again it will take you to the 2nd to last mode you are in.

list of key and what they do:
* 1: Keyboard 1 
* 2: Keyboard 2
* 3: Keyboard 3
* 4: Keyboard 4
* 5: Keyboard 5
* 6: Keyboard 6
* 7: 
* 8: Hex and dec converter
* 9: calculator
* 0: keyboard mode with no macros
* Shift + 1-6: keyboard mode but the numpad is a calculator
* Ctrl + 1-6: program macros for a key in the mode

controls for the calcultor:
* Num Lock / Backspace: delates lest charitor
* Scroll Lock: types the answer out in keystrocks
* Delete: clears the current number, last number, and operator
* =/enter: give the answer
* /-+*. : does what they are
* x: multiplys
* t p: add
* Pageup/down: change how many decsial points are shown


when makeing macros they are all stored on the mirco SD card plugged into the device. if there is no sd card all the keyboard mode will act like normal keyboard. if there is no setting on sd card or its not plug in it will go back to defualt settings

   Arduino libartys that this programs uses
   1. PS2KeyAdvanced (in lib folder)
   2. HID-Project
   3. Adafruit_GFX and Adafruit_sm1306 (if useing oled display)
   4. SD

  Doc on how to use the PS2KEYADVANCE Libarty.
  https://github.com/techpaul/PS2KeyAdvanced
  You should go through it to made sure how to use it.
  
  this project uses Project-hid for all the usb keyboard stuff. here is the doc for it: https://github.com/NicoHood/HID

  PS2KeyAdvanced lib is in the lib folder. I modifly the lib to disable keypad switching keybindings and change lock keys to not break everything
  
*   Num Lock is now PS2_KEY_LANG1
*   Scoll Lock is now PS2_KEY_LANG2
*   Cap Lock is now PS2_KEY_LANG3
*   Sending data (Lock led) to the ps2 keyboard dont work using a SAMD board.

On the PUMBA Keyboard you can change settings like how long it takes for the screen to turn off, the rotation of the screen, and what the function key is. currently there is no way to change these settings using the keyboard but you can change them in the code or put the setting in a file on the sd card. the pumba on start up looks for a config file located /SETTING/CONFIG.TXT on sd card to change the settings. 

To change setting using the SD card. there is a exsample in the code folder
 1. make a file on sd card called CONFIG.TXT in a folder named SETTING
 2. in the config file put the name of the settings folloed by a "=". each setting needs to be on a newline
   - sleep=SETTING
     - SETTING the time in min that the screen withh turn off
   - fnkey=SETTING
     - SETTING is the ps2lib key binding in dec, the key that is used as function key
   - rotation=SETTING
     - SETTING set the rotation of the screen, 0 is defualt, 2 is rotated 180 degress

 3. Save changes to the file
 4. put sd card in to the PUMBA and then plug it in

The Pumba now has the ablity to rebind keys per keyboard mode. to rebind the key you have to make a file called LAYOUT.TXT and put it in the mode you want to rebind the keys. the line number is the ps2lib key name and the number on the line is the hid scancode in decimal. in the code files the is a file called "default layout.txt" with is the default bind. you can copy the file and rename it and then modifly it to your needs. there is also the file called "layout cheatsheet.txt" that can help you too
