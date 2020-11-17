# NTPClient

[![Build Status](https://travis-ci.org/arduino-libraries/NTPClient.svg?branch=master)](https://travis-ci.org/arduino-libraries/NTPClient)

Connect to a NTP server, here is how:

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
// additionaly you can specify the update interval (in milliseconds).
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

#### Constructor: NTPClient(UDP& udp, const char* poolServerName, long timeOffset, unsigned long updateInterval)
  - `udp` : WiFiUDP object.
  - `poolSeverName` (optional): NTP Server Name. Use the nearest server to your loaction. The server list can be found [here](https://www.ntppool.org/en/). The default is pool.ntp.org (global).
  - `timeOffset` (optional): Time offset with GMT for your location in seconds. The deafut is 0.
  - `updateInterval` (optional): Refresh Interval im milliseconds. Default is 60000 millisecond or 1 minute.

#### getEpochTime()
Returns the unix epoch, which are the seconds elapsed since 00:00:00 UTC on 1 January 1970 (leap seconds are ignored, every day is treated as having 86400 seconds). **Attention**: If you have set a time offset this time offset will be added to your epoch timestamp.

#### getFormattedTime()
Returns the formatted current time string in hh:mm:ss format.

#### getHours()
Returns the current "hour" in 24 hour format (0-23).

#### getMinutes()
Returns the current "minute" integer.

#### getSeconds()
Returns the current "second" integer.
