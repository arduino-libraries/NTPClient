# NTPClient

[![Check Arduino status](https://github.com/arduino-libraries/NTPClient/actions/workflows/check-arduino.yml/badge.svg)](https://github.com/arduino-libraries/NTPClient/actions/workflows/check-arduino.yml)
[![Compile Examples status](https://github.com/arduino-libraries/NTPClient/actions/workflows/compile-examples.yml/badge.svg)](https://github.com/arduino-libraries/NTPClient/actions/workflows/compile-examples.yml)
[![Spell Check status](https://github.com/arduino-libraries/NTPClient/actions/workflows/spell-check.yml/badge.svg)](https://github.com/arduino-libraries/NTPClient/actions/workflows/spell-check.yml)

Connect to an NTP server, here is how:

```cpp
#include <NTPClient.h>
// change next line to use with another board/shield
#include <ESP8266WiFi.h>
//#include <WiFi.h> // for WiFi shield
//#include <WiFi101.h> // for WiFi 101 shield or MKR1000
#include <WiFiUdp.h>

const char *ssid     = "<SSID>";
const char *password = "<PASSWORD>";

WiFiUDP ntpUDP;

// By default 'pool.ntp.org' is used with 60 seconds update interval and
// no offset
NTPClient timeClient(ntpUDP);

// You can specify the time server pool and the offset, (in seconds)
// additionally you can specify the update interval (in milliseconds).
// NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

void setup(){
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  timeClient.begin();
}

void loop() {
  timeClient.update();

  Serial.println(timeClient.getFormattedTime());

  delay(1000);
}
```

## Function documentation
`getEpochTime` returns the Unix epoch, which are the seconds elapsed since 00:00:00 UTC on 1 January 1970 (leap seconds are ignored, every day is treated as having 86400 seconds). **Attention**: If you have set a time offset this time offset will be added to your epoch timestamp.

`setEpochTime` sets the Unix epoch internally maintained by the NTPClient library.  The function parameter is the local time converted to number of seconds since 00:00:00 on 1 January 1970 (local time).  Useful when you have a hardware RTC (eg., DS3231) running on local time but uses the NTPClient library for timekeeping.  At start-up, you set the NTPClient time offset and then set the internal NTPClient epoch timestamp using the time you fetch from the hardware RTC.  The `setEpochTime` uses the time offset that you specified to compute the epoch timestamp.
