// enable or disable GxEPD2_GFX base class
#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeMonoBold42pt7b.h>
#include <Fonts/FreeMonoBold54pt7b.h>

#include <WiFi.h>
#include "time.h"
#include "sntp.h"

const char* ssid = "SSID";
const char* password = "PASSWORD";

const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const char* time_zone = "WIB-7";  // TimeZone rule for GMT+7 (WIB)

// ESP32 CS(SS)=5,SCL(SCK)=18,SDA(MOSI)=23,BUSY=15,RES(RST)=2,DC=0
// 4.26'' EPD Module
GxEPD2_BW<GxEPD2_426_GDEQ0426T82, GxEPD2_426_GDEQ0426T82::HEIGHT> display(GxEPD2_426_GDEQ0426T82(/*CS=*/5, /*DC=*/0, /*RES=*/2, /*BUSY=*/15));  // DEPG0213BN 122x250, SSD1680

String formatted_date = "2000-01-01";
String previous_date = "2000-01-01";
String formatted_time = "00:00:00";

// Callback function (gets called when time adjusts via NTP)
void timeavailable(struct timeval* t) {
  Serial.println("Got time adjustment from NTP!");
}

void setup() {
  Serial.begin(115200);

  // Set notification callback function
  sntp_set_time_sync_notification_cb(timeavailable);

  // Use configTzTime to set timezone with rules
  configTzTime(time_zone, ntpServer1, ntpServer2);

  // Connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");
  delay(1000);
  display.init(115200, true, 200, false);
  header_text();
  delay(1000);
}

void header_text() {
  int16_t tbx, tby;
  uint16_t tbw, tbh;

  display.setRotation(0);
  display.setFont(&FreeMonoBold24pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(0, 30);
    display.print("Date:");
    display.setCursor(0, 235);
    display.print("Time:");

    display.setFont(&FreeMonoBold42pt7b);
    display.getTextBounds(formatted_date, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t x = ((display.width() - tbw) / 2) - tbx;
    display.setCursor(x, 140);
    display.print(formatted_date);
  } while (display.nextPage());
}

void showTime() {
  uint16_t box_w = 800;
  uint16_t box_h = 80;

  uint16_t box_x = (display.width() - box_w) / 2;
  uint16_t box_y = 300;  //((display.height() - box_h) / 2) + 120;

  uint16_t incr = display.epd2.hasFastPartialUpdate ? 1 : 3;
  display.setFont(&FreeMonoBold54pt7b);
  if (display.epd2.WIDTH < 104) display.setFont(0);
  display.setTextColor(GxEPD_BLACK);

  display.setRotation(0);
  display.setPartialWindow(box_x, box_y, box_w, box_h);
  display.firstPage();
  do {
    display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
    int16_t tbx, tby;
    uint16_t tbw, tbh;
    display.getTextBounds(formatted_time, 0, 0, &tbx, &tby, &tbw, &tbh);

    int16_t x = box_x + (box_w - tbw) / 2;
    int16_t y = box_y + (box_h - tbh) / 2 + tbh;

    display.setCursor(x, y);
    display.print(formatted_time);
  } while (display.nextPage());
}

void loop() {
  delay(500);
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("No time available (yet)");
    return;
  }
  //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  char timeBuffer[9];   // Buffer for HH:MM:SS
  char dateBuffer[11];  // Buffer for YYYY-MM-DD

  strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", &timeinfo);
  strftime(dateBuffer, sizeof(dateBuffer), "%Y-%m-%d", &timeinfo);

  formatted_time = String(timeBuffer);
  formatted_date = String(dateBuffer);

  showTime();
  Serial.println(&timeinfo, "%H:%M:%S");
  if (formatted_date != previous_date) {
    Serial.println(dateBuffer);
    header_text();
    previous_date = formatted_date;  // Update previous_date
    delay(1000);
  }
}
