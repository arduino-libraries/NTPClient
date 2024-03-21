/* Advanced NTPClient With RTC example 
 *  Copyright (c) 2021 by Anthony F. Dela Paz
 *  
 * Featuring:
 *  + an RTC (RTCLib can use hardware RTC by simply changing 'RTC_Millis' with appropriate declaration).
 *  + DateTime facility of the RTCLib to facilitate epochtime conversion
 *  + separately setting the ntp server, timezone offset, and update interval
 *  + syncing the internal software RTC of the NTPClient with the RTC time
 *  
 * This example code demonstrates NTPClient's new capability of syncing with NTP time server
 * down to millisecond accuracy.  It's best to upload this code to at least two ESP-12E modules
 * to see how both their blue LED blinks simultaneously upon synchronization with NTP server.
 *  
 * The MIT License (MIT) 
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "RTClib.h"

const char *ssid     = "<SSID>"; // change the SSID and PASSWORD
const char *password = "<PASSWORD>";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

RTC_Millis rtc; // this should be a hardware RTC but that won't work as a quick example

const byte ledBlue = 2;  // Blue LED on the ESP-12E module
unsigned long oldTick, blinkMillis;

void setup() {

  pinMode(ledBlue, OUTPUT);

  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  // following line sets the RTC to the date & time this sketch was compiled
  rtc.begin(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit local date & time, for example to set
  // June 9, 2021 at 12:52:36 PM, you would call:
  // rtc.adjust(DateTime(2021, 6, 9, 12, 52, 30));

  timeClient.begin();
  timeClient.setTimeOffset((-4) * 3600); // in seconds, Time zone in New York, NY, USA (GMT-4)
  timeClient.setPoolServerName("europe.pool.ntp.org");
  timeClient.setUpdateInterval(3600); // seconds
  timeClient.setEpochTime(rtc.now().unixtime()); // set the timeClient's internal software rtc time.
}

void loop() {
  timeClient.update();

  // Synchronize the blinking LED with transition of seconds.
  if (timeClient.getEpochTime() != oldTick) {
    oldTick = timeClient.getEpochTime();
    blinkMillis = millis();
    Serial.println(timeClient.getFormattedTime());
  }

  // blink the LED once per second with 500ms ON-time
  if ((millis() - blinkMillis) % 1000 > 500) {
    digitalWrite(ledBlue, HIGH);
  } else {
    digitalWrite(ledBlue, LOW);
  }
}
