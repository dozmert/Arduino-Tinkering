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
