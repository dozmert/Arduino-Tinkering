### Description
Connectivity for an RFID-RC522 module
#### Usage
- Software 1 - Allows the user to Read/Write using the module .

### Build
#### Hardware
- 1x Arduino Uno
-  1x RFID-RC522 Module

![](/img/rfid_1.jpg)
#### Pinout
| Module Pin | Arduino Uno Pin       |
| ---------- | --------------------- |
| **SDA**    | **Pin 10**            |
| **SCK**    | **Pin 13**            |
| **MOSI**   | **Pin 11**            |
| **MISO**   | **Pin 12**            |
| **IRQ**    | (not connected)       |
| **GND**    | **GND**               |
| **RST**    | **Pin 9**             |
| **3.3V**   | **3.3V** (⚠️ not 5V!) |
**Important:** RC522 is a 3.3V device. **Do NOT connect it to 5V** — it may damage the module.

![](/img/rfid_2.jpg)
#### Libraries
First we must install the libraries associated with this module.
```
1. Open Arduino IDE
2. Go to `Sketch > Include Library > Manage Libraries`
3. Search for: **MFRC522**
4. Install: **MFRC522 by GithubCommunity**
```

### Software 1
Using the following code we can monitor the serial connection for RFID tags and print out the following
- Card UID
- Card Type
- Sector headers 
- Block Data | Hex + ASCII view
This .ide code can be found [here](https://github.com/dozmert/Arduino-tinkering/blob/master/Example%20scripts/InformationDump_1).
### Software 2
Lets prepare the module to now write very basic information to blank cards.
For example, I'll want to record an employee number and name onto the card so I can have a record of who's gained access or learn who the card belongs to if it's lost.

This code has been created to allow the user to not only read cards but also overwrite blocks. The code validates input data within the write options to prevent the user from making any changes to data that could otherwise render the card inoperable as this is only for early stage testing.
This .ide code can be found [here](https://github.com/dozmert/Arduino-tinkering/blob/master/Example%20scripts/ReadWriter_1).
### Notes & Troubleshooting

There is still so much that can be done with this module. It's been very difficult to build a functional menu within the limitations of the IDE.

