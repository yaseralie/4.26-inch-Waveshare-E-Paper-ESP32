
// base class GxEPD2_GFX can be used to pass references or pointers to the display instance as parameter, uses ~1.2k more code
// enable or disable GxEPD2_GFX base class
/* Wiring:
  E-Paper ke ESP32:
  CS 	  --> G5
  SCL 	--> G18
  SDA 	--> G23
  BUSY	--> G15
  RST	  --> G2
  DC	  --> G0

  RFID Reader RC522 ke ESP32:
  VCC 	--> VCC 3v
  GND 	--> GND
  RST	  --> G4
  MISO	--> G19
  MOSI	--> G23
  SCK	  --> G18
  SS	  --> G21
*/
#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold36pt7b.h>
#include <Fonts/FreeMonoBold42pt7b.h>

// For RFID
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 21  //D4
#define RST_PIN 4 //D3

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
unsigned long lastUID = 0;         // Stores the last UID read
bool cardDetected = false;         // Status whether there is a card in range
unsigned long uidDec = 0;

// ESP32 CS(SS)=5,SCL(SCK)=18,SDA(MOSI)=23,BUSY=15,RES(RST)=2,DC=0

// 4.26'' EPD Module
GxEPD2_BW<GxEPD2_426_GDEQ0426T82, GxEPD2_426_GDEQ0426T82::HEIGHT> display(GxEPD2_426_GDEQ0426T82(/*CS=5*/ 5, /*DC=*/ 0, /*RES=*/ 2, /*BUSY=*/ 15)); // DEPG0213BN 122x250, SSD1680


void setup()
{
  //For E-Paper
  display.init(115200, true, 50, false);
  header_text();
  delay(1000);

  //For RFID
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();          // Initiate SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  delay(500);
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void header_text()
{
  display.setRotation(0);
  display.setFont(&FreeMonoBold36pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(0, 80);
    display.print("RFID UID:");
  }
  while (display.nextPage());
}

void showRFID()
{
  delay(1000);
  // ukuran box
  uint16_t box_w = 800;
  uint16_t box_h = 80;

  // center position
  uint16_t box_x = (display.width() - box_w) / 2;
  uint16_t box_y = ((display.height() - box_h) / 2) + 40;

  uint16_t incr = display.epd2.hasFastPartialUpdate ? 1 : 3;
  display.setFont(&FreeMonoBold42pt7b);
  if (display.epd2.WIDTH < 104) display.setFont(0);
  display.setTextColor(GxEPD_BLACK);

  display.setRotation(0);
  display.setPartialWindow(box_x, box_y, box_w, box_h);
  display.firstPage();
  do
  {
    display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
    // Calculate text size
    char r_str[20];
    sprintf(r_str, "%lu", uidDec);
    int16_t tbx, tby;
    uint16_t tbw, tbh;
    display.getTextBounds(r_str, 0, 0, &tbx, &tby, &tbw, &tbh);

    // Calculate the center position of the box
    int16_t x = box_x + (box_w - tbw) / 2;
    int16_t y = box_y + (box_h - tbh) / 2 + tbh;

    display.setCursor(x, y);
    display.print(uidDec);
  }
  while (display.nextPage());
  delay(500);

}

void loop() {
  // Detect whether there is a new card or  cannot read the card serial
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
  {
    lastUID = 0;
    return; // No new card or can't read serial, continue to loop
  }

  // Convert UID to decimal
  for (int i = mfrc522.uid.size - 1; i >= 0; i--) // Reverse byte order
  {
    uidDec = (uidDec << 8) | mfrc522.uid.uidByte[i];
  }

  // If UID is different, display it
  if (uidDec != lastUID)
  {
    lastUID = uidDec; // Save the latest UID

    // Show UID in Hex format
    Serial.print("UID (Hex): ");
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      if (i < mfrc522.uid.size - 1) Serial.print(" ");
    }
    Serial.println();

    // Display UID in Decimal format
    Serial.print("UID (USB Dec): ");
    Serial.println(uidDec);
    showRFID();
  }

  // Wait for the card to be removed, make sure no cards are detected before continuing.
  while (mfrc522.PICC_IsNewCardPresent())
  {
    delay(50); // Wait a moment to make sure the card is completely removed.
  }
}
