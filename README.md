# PUMBA-Keyboard
PUMBA has-Keyboard has:
is a PS2 to USB macro board adapter 

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
  * WILL ADD SOON the ablity to change settings without puting the sd card into a PC
  * WILL ADD SOON the abilty to rebind keys and have differnt key layouts per keyboard mode (half way done)

PUMBA has only been test with SAMD21 and SAMD51 micro controll board. The project COULD work on most microcontroller that can emulate a HID device. It will not work on a Arduino leardo or Uno because the program takes to much ram. It has worked with almost all PS2 keyboards i have try. It will not work with most USB keyboard using a USB to PS2 adperter. the code was writen useing plateformio with the arduino framwork.

Tested Boards:
* Adafruit Feather M4 Express
* Adafruit QT Py SAMD21
* Seeeduino Xiao

There are PCB and cases for this project. You dont have to use the PCB, you can embend everything inside a keyboard or use a bread board. included is KiCad project files, pdf of the schmatic, pdf of the PCBs, and BOM.

The PUMBA keyboard has 6 differnt keyboard modes wither there own layouts and macros on it. There are is one mode decacated just for the keyboard being just a calulator, but all Keyboard modes have the ablity for the numpad to become a calculator. There is also a hex and dec converter mode and another keyboard mode that cant use macros.

When you plug in the PUMBA keyboard into a computer it will go into "Keyboard 1" mode. You switch modes by pressing the "function key" witch is defualt set to the menu button. after you press the function key the display will pop up things that you can do. To switch modes you press the corispnding key for the mode. if you press the function key again it will take you to the 2nd to last mode you are in.
list of key and what they do:
1: Keyboard 1
2: Keyboard 2
3: Keyboard 3
4: Keyboard 4
5: Keyboard 5
6: Keyboard 6
7: 
8: Hex and dec converter
9: calculator
0: keyboard mode with no macros
Shift + 1-6: keyboard mode but the numpad is a calculator
Ctrl + 1-6: program macros for a key in the mode

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
