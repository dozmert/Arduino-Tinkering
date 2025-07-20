
## Description
Read / Write using an RFID/NFC module

## Build
### Hardware
#### Components
- 1x Arduino Uno
-  1x RFID-RC522 Module
![](RFID-RC522/img/rfid_1.jpg)
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

![](RFID-RC522/img/rfid_2.jpg)
### Software
#### Libraries
First we must install the libraries associated with this module.
```
1. Open Arduino IDE
2. Go to Sketch > Include Library > Manage Libraries
3. Search for: **MFRC522**
4. Install: MFRC522 by GithubCommunity
```
#### Script
Using the following code we can monitor the serial connection for RFID tags and print out the following
- Card UID
- Card Type
- Sector headers 
- Block Data | Hex + ASCII view

```C++
#include <SPI.h>         // SPI communication for RC522
#include <MFRC522.h>     // RFID library for MFRC522

#define RST_PIN 9        // Reset pin (RST) connected to digital pin 9
#define SS_PIN 10        // Slave Select pin (SDA) connected to digital pin 10

MFRC522 rfid(SS_PIN, RST_PIN);  // Create MFRC522 instance
MFRC522::MIFARE_Key key;        // Create a MIFARE key structure

void setup() {
  Serial.begin(9600);     // Initialize Serial Monitor
  SPI.begin();            // Initialize SPI bus
  rfid.PCD_Init();        // Initialize the RC522 reader
  Serial.println("Ready to scan a card...");

  // Set default key (factory default for most MIFARE Classic cards)
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
}

void loop() {
  // Wait for a new card to be present
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return; // No card detected
  }

  // Print the card UID (Unique Identifier)
  Serial.print("\nCard UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) Serial.print("0");
    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  // Print card type
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.print("Card Type: ");
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Loop through all 64 blocks of the card
  for (byte block = 0; block < 64; block++) {

    // Print sector header every 4 blocks
    if (block % 4 == 0) {
      Serial.print("\n=== Sector ");
      Serial.print(block / 4);
      Serial.println(" ===");
    }

    byte buffer[18];
    byte bufferSize = sizeof(buffer);

    // Authenticate the block with Key A
    MFRC522::StatusCode status = rfid.PCD_Authenticate(
      MFRC522::PICC_CMD_MF_AUTH_KEY_A,
      block,
      &key,
      &(rfid.uid)
    );

    if (status != MFRC522::STATUS_OK) {
      Serial.print("Block ");
      Serial.print(block);
      Serial.print(" auth failed: ");
      Serial.println(rfid.GetStatusCodeName(status));
      continue;
    }

    // Try to read the block data
    status = rfid.MIFARE_Read(block, buffer, &bufferSize);
    if (status != MFRC522::STATUS_OK) {
      Serial.print("Block ");
      Serial.print(block);
      Serial.print(" read failed: ");
      Serial.println(rfid.GetStatusCodeName(status));
      continue;
    }

    // Mark trailer block clearly
    bool isTrailer = (block + 1) % 4 == 0;

    Serial.print("Block ");
    if (block < 10) Serial.print("0");
    Serial.print(block);
    Serial.print(isTrailer ? " (Trailer): " : "           : ");

    // Print raw hex values
    for (byte i = 0; i < 16; i++) {
      if (buffer[i] < 0x10) Serial.print("0");
      Serial.print(buffer[i], HEX);
      Serial.print(" ");
    }

    // Print ASCII representation (skip for trailer)
    if (!isTrailer) {
      Serial.print("| ASCII: ");
      for (byte i = 0; i < 16; i++) {
        char c = buffer[i];
        Serial.print(isPrintable(c) ? c : '.');
      }
    }

    Serial.println(); // New line per block
  }

  // End communication with card
  rfid.PICC_HaltA();         // Tell the card we're done
  rfid.PCD_StopCrypto1();    // Stop encryption on reader

  Serial.println("\n-------- END --------");
  delay(1000);               // Brief pause before reading next card
}
```

Lets prepare the module to now write very basic information to blank cards.
For example, I'll want to record an employee number and name onto the card so I can have a record of who's gained access or learn who the card belongs to if it's lost.

This code has been created to allow the user to not only read cards but also overwrite blocks. The code validates input data within the write options to prevent the user from making any changes to data that could otherwise render the card inoperable as this is only for early stage testing.

```C++
#include <SPI.h>            // Include SPI library for communication
#include <MFRC522.h>        // Include MFRC522 library for RFID functions

#define SS_PIN 10           // Slave Select pin for RC522 module
#define RST_PIN 9           // Reset pin for RC522 module

MFRC522 rfid(SS_PIN, RST_PIN);      // Create MFRC522 instance
MFRC522::MIFARE_Key key;            // Key used for card authentication

// --- Function Declarations ---
String getUserInput(int maxLength = 32);          // Read user input up to maxLength
String getValidatedInput(String validOptions);    // Ensure input matches allowed choices
bool isSensitiveBlock(int block);                 // Check if block is a trailer block
int getCardMaxBlock(byte type);                   // Get max writable block based on card type
void wipeCard(int maxBlock);                      // Wipe card excluding sensitive blocks
void readCardMenu();                              // Read card data menu
void writeCardMenu();                             // Write card data menu
void overwriteBlock(int maxBlock);                // Overwrite a specific block on the card

void setup() {
  Serial.begin(9600);            // Start serial communication
  while (!Serial);               // Wait for Serial monitor
  SPI.begin();                   // Start SPI bus
  rfid.PCD_Init();               // Initialize RFID reader

  // Set key to default factory key: 6 bytes of 0xFF
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  Serial.println("NFC Terminal Ready.");
}

void loop() {
  // Display main menu
  Serial.println("\nMain Menu:");
  Serial.println("1 - Read Card");
  Serial.println("2 - Write Card");

  String selection = getValidatedInput("12");     // Get user choice

  if (selection == "1") readCardMenu();           // If choice is 1, read card
  else if (selection == "2") writeCardMenu();     // If choice is 2, write to card
}
void readCardMenu() {
  Serial.println(F("Scan a card to read..."));
  while (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    delay(50);
  }

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.print(F("Card UID: "));
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();

  Serial.print(F("Card Type: "));
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Handle MIFARE Ultralight / NTAG / TNP3XXX
  if (piccType == MFRC522::PICC_TYPE_MIFARE_UL) {
    Serial.println(F("Reading MIFARE Ultralight/NTAG pages..."));

    for (byte page = 4; page <= 44; page++) {
      byte buffer[18];
      byte size = sizeof(buffer);

      MFRC522::StatusCode status = rfid.MIFARE_Read(page, buffer, &size);
      if (status != MFRC522::STATUS_OK) {
        Serial.print(F("Failed to read page "));
        Serial.print(page);
        Serial.print(F(": "));
        Serial.println(rfid.GetStatusCodeName(status));
        continue;
      }

      Serial.print(F("Page "));
      Serial.print(page);
      Serial.print(F(": "));
      for (byte i = 0; i < 4; i++) {
        if (buffer[i] < 0x10) Serial.print("0");
        Serial.print(buffer[i], HEX);
        Serial.print(" ");
      }
      Serial.print(F(" | ASCII: "));
      for (byte i = 0; i < 4; i++) {
        char c = buffer[i];
        Serial.print(isPrintable(c) ? c : '.');
      }
      Serial.println();
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    return;
  }

  // Handle MIFARE Classic 1K / 4K
  if (piccType == MFRC522::PICC_TYPE_MIFARE_1K || piccType == MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Reading MIFARE Classic sectors..."));

    for (byte sector = 0; sector < 16; sector++) {
      if (!rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, sector * 4, &key, &(rfid.uid))) {
        Serial.print(F("Authentication failed for sector "));
        Serial.print(sector);
        Serial.println(F(". Skipping..."));
        continue;
      }

      Serial.print(F("Sector "));
      Serial.print(sector);
      Serial.println(F(":"));

      for (byte block = 0; block < 4; block++) {
        byte blockAddr = sector * 4 + block;
        byte buffer[18];
        byte size = sizeof(buffer);

        if (rfid.MIFARE_Read(blockAddr, buffer, &size) != MFRC522::STATUS_OK) {
          Serial.print(F("  Block "));
          Serial.print(blockAddr);
          Serial.println(F(": Read failed"));
          continue;
        }

        Serial.print(F("  Block "));
        Serial.print(blockAddr);
        Serial.print(F(": "));
        for (byte i = 0; i < 16; i++) {
          if (buffer[i] < 0x10) Serial.print("0");
          Serial.print(buffer[i], HEX);
          Serial.print(" ");
        }

        Serial.print(F(" | ASCII: "));
        for (byte i = 0; i < 16; i++) {
          char c = buffer[i];
          Serial.print(isPrintable(c) ? c : '.');
        }
        Serial.println();
      }
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    return;
  }

  // Unknown or unsupported card type
  Serial.println(F("This card type is currently not supported."));
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

void writeCardMenu() {
  Serial.println("\nPresent card to write...");
  while (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {}

  // Display UID and card type
  Serial.print("Card UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) Serial.print("0");
    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.print("Card Type: ");
  Serial.println(rfid.PICC_GetTypeName(piccType));

  int maxBlock = getCardMaxBlock(piccType);
  if (maxBlock == -1) {
    Serial.println("Unsupported card type. Returning to main menu.");
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    return;
  }

  // Ask user what kind of write they want
  Serial.println("\nWrite Options:");
  Serial.println("1 - Overwrite Specific Block");
  Serial.println("2 - Wipe Entire Card (except trailer blocks)");
  String choice = getValidatedInput("12");

  if (choice == "1") overwriteBlock(maxBlock);
  else if (choice == "2") wipeCard(maxBlock);
}

void overwriteBlock(int maxBlock) {
  while (true) {
    int block = -1;
    String input;

    // Ask user for valid block number to overwrite
    while (true) {
      Serial.print("Enter block number to overwrite (0 to ");
      Serial.print(maxBlock);
      Serial.println("): ");
      input = getUserInput();

      if (input.length() == 0) {
        Serial.println("No input entered. Please try again.");
        continue;
      }

      block = input.toInt();
      if (block < 0 || block > maxBlock) {
        Serial.println("Invalid block number.");
        continue;
      }

      if (isSensitiveBlock(block)) {
        Serial.println("Cannot overwrite trailer blocks.");
        continue;
      }

      break;  // Valid input
    }

    // Authenticate and read current block
    MFRC522::StatusCode status = rfid.PCD_Authenticate(
      MFRC522::PICC_CMD_MF_AUTH_KEY_A,
      block,
      &key,
      &(rfid.uid)
    );

    if (status != MFRC522::STATUS_OK) {
      Serial.print("Authentication failed: ");
      Serial.println(rfid.GetStatusCodeName(status));
      return;
    }

    byte oldBuffer[18];
    byte size = sizeof(oldBuffer);
    status = rfid.MIFARE_Read(block, oldBuffer, &size);

    // Show existing data
    Serial.println("\nExisting Block data");
    Serial.print("--- Sector "); Serial.print(block / 4); Serial.println(" ---");
    Serial.print("Block "); Serial.print(block); Serial.print(": ");
    for (int i = 0; i < 16; i++) {
      if (oldBuffer[i] < 0x10) Serial.print("0");
      Serial.print(oldBuffer[i], HEX); Serial.print(" ");
    }
    Serial.print("| ASCII: ");
    for (int i = 0; i < 16; i++) Serial.print(isPrintable(oldBuffer[i]) ? (char)oldBuffer[i] : '.');
    Serial.println();

    // Get new data to write
    Serial.println("\nEnter new payload (max 16 characters): ");
    String payload = getUserInput(16);
    byte newBuffer[16] = {0};
    for (int i = 0; i < payload.length(); i++) newBuffer[i] = payload[i];

    // Show new data
    Serial.println("\nNew block data");
    Serial.print("Block "); Serial.print(block); Serial.print(": ");
    for (int i = 0; i < 16; i++) {
      if (newBuffer[i] < 0x10) Serial.print("0");
      Serial.print(newBuffer[i], HEX); Serial.print(" ");
    }
    Serial.print("| ASCII: ");
    for (int i = 0; i < 16; i++) Serial.print(isPrintable(newBuffer[i]) ? (char)newBuffer[i] : '.');
    Serial.println();

    // Confirm write
    Serial.print("Confirm write? (Y/N): ");
    String confirm = getValidatedInput("YyNn");
    if (confirm != "Y" && confirm != "y") {
      Serial.println("Write cancelled.");
    } else {
      status = rfid.MIFARE_Write(block, newBuffer, 16);
      if (status == MFRC522::STATUS_OK) Serial.println("Block written successfully.");
      else {
        Serial.print("Write failed: ");
        Serial.println(rfid.GetStatusCodeName(status));
      }
    }

    // Next action
    Serial.println("\nOptions:");
    Serial.println("1 - Overwrite another block");
    Serial.println("2 - Return to main menu");
    String next = getValidatedInput("12");

    if (next == "2") {
      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();
      return;
    }
  }
}

void wipeCard(int maxBlock) {
  Serial.print("\nThis will wipe all writable blocks on the card (excluding trailer blocks).\n");
  Serial.print("Are you sure? (Y/N): ");
  String confirm = getValidatedInput("YyNn");
  if (confirm != "Y" && confirm != "y") {
    Serial.println("Wipe cancelled.");
    return;
  }

  // Clear data buffer
  byte nullBuffer[16];
  memset(nullBuffer, 0x00, sizeof(nullBuffer));

  // Overwrite all writable blocks
  for (int block = 0; block <= maxBlock; block++) {
    if (isSensitiveBlock(block)) continue;

    MFRC522::StatusCode status = rfid.PCD_Authenticate(
      MFRC522::PICC_CMD_MF_AUTH_KEY_A,
      block,
      &key,
      &(rfid.uid)
    );

    if (status != MFRC522::STATUS_OK) {
      Serial.print("Auth failed for block ");
      Serial.print(block);
      Serial.print(": ");
      Serial.println(rfid.GetStatusCodeName(status));
      continue;
    }

    status = rfid.MIFARE_Write(block, nullBuffer, 16);
    if (status == MFRC522::STATUS_OK) {
      Serial.print("Wiped block ");
      Serial.println(block);
    } else {
      Serial.print("Failed to wipe block ");
      Serial.print(block);
      Serial.print(": ");
      Serial.println(rfid.GetStatusCodeName(status));
    }
  }

  Serial.println("Wipe operation complete.");
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

// Check if a block is a trailer block (used for access keys)
bool isSensitiveBlock(int block) {
  return block % 4 == 3;
}

// Get the highest valid block number for given card type
int getCardMaxBlock(byte type) {
  switch (type) {
    case MFRC522::PICC_TYPE_MIFARE_1K: return 63;
    case MFRC522::PICC_TYPE_MIFARE_4K: return 255;
    case MFRC522::PICC_TYPE_MIFARE_MINI: return 19;
    default: return -1; // Unknown/unsupported type
  }
}

// Get user input from Serial with optional max length
String getUserInput(int maxLength) {
  String input = "";
  while (true) {
    if (Serial.available()) {
      char c = Serial.read();
      if (c == '\n' || c == '\r') break;
      if (isPrintable(c) && input.length() < maxLength) input += c;
    }
  }
  return input;
}

// Wait until user enters one of the allowed characters
String getValidatedInput(String validOptions) {
  while (true) {
    while (!Serial.available()) {}
    char c = Serial.read();
    if (validOptions.indexOf(c) >= 0) {
      Serial.println(c);
      return String(c);
    }
  }
}
```
## Notes & Troubleshooting
- There is still so much that can be done with this module. It's been very difficult to build a functional menu within the limitations of the IDE.



