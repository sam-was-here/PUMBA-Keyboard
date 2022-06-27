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
  * WILL ADD SOON the ablity to change settings without puting the sd card into a PC
  * WILL ADD SOON the abilty to rebind keys and have differnt key layouts per keyboard mode (half way done)

PUMBA has only been test with SAMD21 and SAMD51 micro controll board. The project COULD work on most microcontroller that can emulate a HID device. It will not work on a Arduino leardo or Uno because the program takes to much ram. It has worked with almost all PS2 keyboards i have try. It will probly not work with most USB keyboard.
Tested Boards:
* Adafruit Feather M4 Express
* Adafruit QT Py SAMD21
* Seeeduino Xiao

There are PCB and cases for this project. You dont have to use the PCB, you can embend everything inside a keyboard or use a bread board. included is KiCad project files, pdf of the schmatic, pdf of the PCBs, and BOM.

