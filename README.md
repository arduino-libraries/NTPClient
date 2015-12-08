# NTPClient 

Connect to a NTP server, here is how:

```cpp
#include <NTPClient.h>
#include <ESP8266WiFi.h>

const char *ssid     = "<SSID>";
const char *password = "<PASSWORD>";

// By default 'time.nist.gov' is used.
NTPClient timeClient();

// You can specify the time server pool and the offset, (in seconds)
// additionaly you can specify the update interval (in milliseconds).
// NTPClient timeClient("europe.pool.ntp.org", 3600, 60000);

void setup(){
  Serial.begin(11520);
  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  // Start the NTPClient after an WiFi connection is established
  timeClient.begin();

}

void loop() {
  timeClient.update();
  
  Serial.println(timeClient.getFormattedTime());
  
  delay(1000);
}
```