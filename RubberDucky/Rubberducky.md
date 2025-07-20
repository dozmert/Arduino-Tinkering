## Description
A project based on the work of [Seytonic's Duckduino-microSD](https://github.com/Seytonic/Duckduino-microSD).  Videos of his project can be found [here](https://www.youtube.com/watch?v=ksvo1WDYQ7s) and [here](https://www.youtube.com/watch?v=QrwqeI99I8E). I wanted to tackle this project myself as both a learning experience and a utility tool for my craft. 

## Build
### Software
The following table contains Arduino .ino files that I've put together based on my own testing and examples found online.
#### Arduino Scripts

| Script Name                                                                                                                                   | Script Description                                                                                                                                                                                                           | Features                                                                                                                                                                                                                                                                                                                    | Status   |
| --------------------------------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | -------- |
| [Arduino-RubberDucky-Part1](https://github.com/dozmert/Arduino-Tinkering/tree/main/RubberDucky/resources/Arduino%20scripts/RubberDucky-Part1) | This script allows the user to execute scripts loaded onto a MicroSD card module connected to the Arduino. When the Arduino is plugged in via USB, the script on the memory card is executed.                                | Checks if a button is pressed when connected via USB.<br>If not pressed when connected, reads and executes a script on the storage card.<br>If pressed when connected, nothing is run and onboard LEDs strobe.                                                                                                              | Complete |
| [Arduino-RubberDucky-Part2](https://github.com/dozmert/Arduino-Tinkering/tree/main/RubberDucky/resources/Arduino%20scripts/RubberDucky-Part2) | This script allows the user to load multiple scripts onto a MicroSD card module connected to the Arduino. Using a DIP switch combination, the user can select what script to execute when the Arduino is plugged in via USB. | Checks if a button is pressed when connected via USB.<br>If not pressed when connected, checks DIP switch states and executes a script on the storage card based on the DIP state.<br>If pressed when connected, nothing is run and onboard LEDs strobe.                                                                    | Complete |
| [Arduino-RubberDucky-Part3](https://github.com/dozmert/Arduino-Tinkering/tree/main/RubberDucky/resources/Arduino%20scripts/RubberDucky-Part3) | This script aims to introduce Bluetooth remote interactivity to allow the user to load and execute scripts remotely, as well as send and receive keystrokes made on the PC the device is connected into via USB.             | Checks if a button is pressed when connected via USB.<br>If not pressed when connected, checks DIP switch states and executes a script on the storage card based on the DIP state.<br>If pressed when connected, nothing is run and onboard LEDs strobe.<br>Bluetooth mode available to listen for keystrokes sent over BLE | Hiatus   |
#### Payload Scripts

|                                                              Payload script                                                              | Description                                                                                                                                                        |
| :--------------------------------------------------------------------------------------------------------------------------------------: | ------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| [Part1-payload](https://github.com/dozmert/Arduino-Tinkering/blob/main/RubberDucky/resources/Payload%20scripts/Part1-payload/script.txt) | A simple payload which runs cmd.exe and echos "Hello world"                                                                                                        |
|      [Part2-payload](https://github.com/dozmert/Arduino-Tinkering/tree/main/RubberDucky/resources/Payload%20scripts/Part2-payload)       | 64 payloads based on a 6-way DIP switch's binary output. In addition is a txt file with all 64 filenames listed that can be used to index/record your script usage |
|                                                              Part3-payload                                                               | N/A                                                                                                                                                                |
### Hardware

#### Components - Part 1 
- 1x Arduino controller
>TZT Pro Micro ATmega32U4 5V 16MHz Original Chip

![](https://github.com/dozmert/Arduino-Tinkering/blob/main/RubberDucky/img/build_1.jpg)

- 1x MicroSD card reader module
>Micro SD Storage Expansion Board Micro SD TF Card Memory Shield Module SPI For Arduino
> Formatted as FAT32

![](https://github.com/dozmert/Arduino-Tinkering/blob/main/RubberDucky/img/build_2.jpg)

- 1x Tactile switch
- 1x Resistor (220 Ohm)

![](https://github.com/dozmert/Arduino-Tinkering/blob/main/RubberDucky/img/build_3.jpg)

#### Components - Part 2
- 1x DIP Switch 
> 6 way DIP switch, standard PCB IC mounting. Can use various sizes based on Arduino pinout.

![](https://github.com/dozmert/Arduino-Tinkering/blob/main/RubberDucky/img/build_4.jpg)

#### Components - Part 3
- 1x Bluetooth Module
> Generic Board

![](https://github.com/dozmert/Arduino-Tinkering/blob/main/RubberDucky/img/build_8.jpg)

#### Pinout - Part 1,2 & 3
Data below based on breadboard layout.

| Build Part | Module Pin | - | Module Pin | - | Module Pin |
| :---: | :---: | :---: | :---: | :---: | :---: |
| Part 1 | Arduino VCC | -> | microSD VCC |  |  |
| Part 1 | Arduino GND | -> | microSD GND |  |  |
| Part 1 | Arduino D14 | -> | microSD MISO |  |  |
| Part 1 | Arduino D16 | -> | microSD MOSI |  |  |
| Part 1 | Arduino D15 | -> | microSD SCK |  |  |
| Part 1 | Arduino D4 | -> | microSD CS |  |  |
| Part 1 | Arduino VCC | -> | TactileButton-1 |  |  |
| Part 1 | Arduino GND | -> | Resistor | -> | TactileButton-2-1
| Part 1 | Arduino D10 | -> | TactileButton-2-2 |  |  |
| Part 2 | Arduino D3 | -> | DIPSwitch-1 | -> | Arduino GND |
| Part 2 | Arduino D5 | -> | DIPSwitch-2 | -> | Arduino GND |
| Part 2 | Arduino D6 | -> | DIPSwitch-3 | -> | Arduino GND |
| Part 2 | Arduino D7 | -> | DIPSwitch-4 | -> | Arduino GND |
| Part 2 | Arduino D8 | -> | DIPSwitch-5 | -> | Arduino GND |
| Part 2 | Arduino D9 | -> | DIPSwitch-6 | -> | Arduino GND |
| Part 3 | Arduino VCC | -> | BTModule-VCC |  |  |
| Part 3 | Arduino GND | -> | BTModule-GND |  |  |
| Part 3 | Arduino D0 (TX0) | -> | BTModule-TX |  |  |
| Part 3 | Arduino D1 (RX1) | -> | BTModule-RX |  |  |

### Testing
##### Part 1
1. Wiring MicroSD reader to Arduino board

![](https://github.com/dozmert/Arduino-Tinkering/blob/main/RubberDucky/img/build_5.jpg)

2. Wiring tactile button to board.

![](https://github.com/dozmert/Arduino-Tinkering/blob/main/RubberDucky/img/build_6.jpg)
##### Part 2
4. Wiring DIP switch to board.

![](https://github.com/dozmert/Arduino-Tinkering/blob/main/RubberDucky/img/build_7.jpg)

##### Part 3
N/A

## Notes & Troubleshooting
- I originally had a lot of trouble getting this working which ended up being a result of my pin contacts failing to carry signals. If you're using pins, ensure they're soldered.
- Long ducky-like script strings may crash Arduino due to RAM consumption. String lines over 300 characters should be split into separate lines. This will not affect how the script runs but will help reduce RAM consumption.
> STRING abcdefghijklmnopqrstu
> STRING vwxyzmnopqrstuvwxyz
- Hyphens must not be used for simultaneous key presses due to current scripting. 
>"CTRL ALT DELETE" instead of "CTRL-ALT-DELETE"
- Features not implemented: DEFAULT_DELAY, REPLAY. This project uses Arduino's inbuilt <a href="https://github.com/arduino-libraries/Keyboard/blob/master/src/Keyboard.h">keyboard.h</a> library, any keys not implemented in that will not work with this. eg: PRINTSCREEN.

### To do
- Confirm resistor used in hardware for documentation.
- Find a better/efficient wiring method for the tactile button. I'm not too knowledgable on wiring.
- Install bluetooth module for remote interactivity.
- Implement more keys that aren't used in [keyboard.h library](https://github.com/arduino-libraries/Keyboard/blob/master/src/Keyboard.h).
- 3D print a small form factor housing.
- Expand example payloads based on official Hak5 rubberducky payloads - [Github](https://github.com/hak5/usbrubberducky-payloads)
- Emulate official Hak5 rubberducky attackmodes - [documentation](https://docs.hak5.org/hak5-usb-rubber-ducky/attack-modes-constants-and-variables/attack-modes)
