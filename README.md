# PUMBA-Keyboard

PUMBA-Keyboard is a PS2 to USB macro board adapter created using the Arduino framework.

PUMBA stands for PS2 to USB Macro Board Adapter.

## Description

PUMBA-Keyboard is an Arduino project that converts a PS2 keyboard into a USB keyboard with additional features. It includes a built-in calculator and the ability to create and playback macros. The board also has a small display for showing different information.

![PUMBA-Keyboard](https://user-images.githubusercontent.com/102840190/175848182-f5005b3c-a151-4c44-8dc3-c826e4afac13.jpg)

### Features
Here is a list of features offered by PUMBA-Keyboard:
* Six different keyboard modes, allowing each key to have a different assigned macro.
* Easy on-the-fly macro creation without the need for PC software.
* All settings and macros are stored on the SD card, making modification and backup effortless.
* Calculator mode that turns a part or the whole keyboard into a calculator, sending results as keypresses.
* Numpad can be converted into a calculator in any of the six keyboard modes while leaving the rest of the keyboard untouched.
* Hex and decimal converter mode.
* Numlock is automatically enabled during startup and mode changes.
* Ability to rebind keys and have different key layouts for each keyboard mode (partially implemented).
* Key bindings can be changed without requiring the SD card to be inserted into a PC.

## Compatibility

PUMBA has been tested with SAMD21 and SAMD51 microcontroller boards. The project may work with most microcontrollers capable of emulating a HID device. However, it is not compatible with Arduino Leonardo or Uno boards due to limited available RAM. Most PS2 keyboards have been tested successfully, but it may not work with USB keyboards using a USB to PS2 adapter. The code was written using PlatformIO with the Arduino framework.

Tested Boards:
* Adafruit Feather M4 Express
* Adafruit QT Py SAMD21
* Seeeduino Xiao

## PCB and Cases

PCB and case designs are available for this project. However, they are not mandatory, as you can integrate everything inside a keyboard or use a breadboard. The repository includes KiCad project files, PDFs of the schematic, PCBs, and a bill of materials (BOM).

## Keyboard Modes

The PUMBA keyboard offers six different keyboard modes, each with its own layouts and macros. One mode is dedicated to functioning solely as a calculator, but all keyboard modes can activate the numpad as a calculator. Another mode is a hex and decimal converter, while one keyboard mode does not support macros.

When the PUMBA keyboard is plugged into a computer, it automatically enters "Keyboard 1" mode. You can switch between modes by pressing the "function key," which is set as the default to the menu button. After pressing the function key, the display shows available actions. To switch modes, press the corresponding key for the desired mode. Pressing the function key again takes you to the second-to-last mode.

### Key Assignments
Here is a list of key assignments for mode switching:
* 1: Keyboard 1
* 2: Keyboard 2
* 3: Keyboard 3
* 4: Keyboard 4
* 5: Keyboard 5
* 6: Keyboard 6
* 7: Not assigned
* 8: Hex and decimal converter
* 9: Calculator


* 0: Keyboard mode with no macros
* Shift + 1-6: Keyboard mode with calculator functionality on the numpad
* Ctrl + 1-6: Program macros/rebind keys for a key in the mode. The display provides instructions.

## Calculator Controls

The calculator mode on the PUMBA keyboard includes the following controls:
* Num Lock / Backspace: Delete the last character.
* Scroll Lock: Type out the answer using key strokes.
* Delete: Clear the current number, last number, and operator.
* =/Enter: Calculate and display the answer.
* /-+*. : Perform corresponding operations.
* x: Multiply.
* t p: Add.
* Page Up/Down: Change the number of decimal points shown.

## Macros and Key Rebinding

When creating macros, they are stored on the micro SD card inserted into the device. If no SD card is present, all keyboard modes will act as normal keyboards. If there are no settings on the SD card or it is not inserted, the device will revert to default settings.

The following Arduino libraries are used by this program:
1. PS2KeyAdvanced (located in the "lib" folder)
2. HID-Project
3. Adafruit_GFX and Adafruit_sm1306 (for OLED display support)
4. SD

To learn how to use the PS2KeyAdvanced library, refer to the documentation at [https://github.com/techpaul/PS2KeyAdvanced](https://github.com/techpaul/PS2KeyAdvanced).

This project utilizes Project-hid for USB keyboard functionality. Refer to the documentation at [https://github.com/NicoHood/HID](https://github.com/NicoHood/HID).

The PS2KeyAdvanced library in the "lib" folder has been modified to disable keypad switching keybindings and modify lock keys to prevent issues.

* Num Lock is now PS2_KEY_LANG1.
* Scroll Lock is now PS2_KEY_LANG2.
* Caps Lock is now PS2_KEY_LANG3.
* Sending data (Lock LED) to the PS2 keyboard does not work with SAMD boards.

## Changing Settings

On the PUMBA Keyboard, you can change settings such as the screen timeout duration, screen rotation, and the function key. Currently, there is no way to change these settings using the keyboard directly, but you can modify them in the code or by creating a settings file on the SD card. On startup, PUMBA looks for a config file named "CONFIG.TXT" located in the "/SETTING" folder on the SD card to apply the settings.

To change settings using the SD card, follow these steps:
1. Create a file named "CONFIG.TXT" in a folder named "SETTING" on the SD card.
2. In the config file, enter the name of the setting followed by an "=" sign. Each setting should be on a new line.
   * sleep=SETTING
     * SETTING: The duration in minutes before the screen turns off.
   * fnkey=SETTING
     * SETTING: The PS2lib key binding in decimal for the function key.
   * rotation=SETTING
     * SETTING: The screen rotation value. 0 is default, 2 is rotated 180 degrees.
3. Save the changes to the file.
4. Insert the SD card into PUMBA and plug it in.

## Key Rebinding

PUMBA now supports the ability to rebind keys for each keyboard mode. To rebind keys, create a file named "LAYOUT.TXT" and place it in the mode for which you want to rebind the keys. Each line in the file corresponds to a PS2lib key name and the corresponding HID scancode in

 decimal. In the code files, you can find a file called "default layout.txt" which contains the default key bindings. You can make a copy of this file, rename it, and modify it according to your needs. Additionally, there is a file named "layout cheatsheet.txt" that provides a helpful reference.

In any of the 6 modes, you can set macros or rebind almost any key on the keyboard. However, please note that some keys such as Scroll Lock, Num Lock, and Pause Break may not be compatible with macros or key rebindings.
