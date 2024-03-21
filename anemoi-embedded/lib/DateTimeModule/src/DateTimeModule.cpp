#include "DateTimeModule.h"

RTC_DS3231 rtc;

void initDateTimeModule() {
  if (!rtc.begin()) {
    Serial.println("RTC module is NOT found");
    Serial.flush();
    while (true) continue;
  }

  // Uncomment to set date (comment again)
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void serialPrintDateTime () {
    DateTime now = rtc.now();

    Serial.print("RTC Date Time: ");
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print('-');
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.println(now.second(), DEC);
}