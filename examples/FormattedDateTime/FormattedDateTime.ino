#include <NTPClient.h>
// change next line to use with another board/shield
#include <ESP8266WiFi.h>
//#include <WiFi.h> // for WiFi shield
//#include <WiFi101.h> // for WiFi 101 shield or MKR1000
#include <WiFiUdp.h>

const char *ssid     = "<SSID>";
const char *password = "<PASSWORD>";

WiFiUDP ntpUDP;

// You can specify the time server pool and the offset (in seconds, can be
// changed later with setTimeOffset() ). Additionally you can specify the
// update interval (in milliseconds, can be changed using setUpdateInterval() ).
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

void setup(){
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  timeClient.begin();
  timeClient.setDateLanguage("pt"); // Available languages: "pt", "es" and "en" (default)
}

void loop() {
  timeClient.update();
  Serial.print("%Y: ");
  Serial.println(timeClient.getFormattedDateTime("%Y")); // Full year (e.g., 2023)
  Serial.print("%y: ");
  Serial.println(timeClient.getFormattedDateTime("%y")); // Last two digits of the year (e.g., 23 for 2023)
  Serial.print("%m: ");
  Serial.println(timeClient.getFormattedDateTime("%m")); // Month as a zero-padded decimal number (01 to 12)
  Serial.print("%d: ");
  Serial.println(timeClient.getFormattedDateTime("%d")); // Day of the month as a zero-padded decimal number (01 to 31)
  Serial.print("%H: ");
  Serial.println(timeClient.getFormattedDateTime("%H")); // Hour (00 to 23) as a zero-padded decimal number
  Serial.print("%M: ");
  Serial.println(timeClient.getFormattedDateTime("%M")); // Minute as a zero-padded decimal number (00 to 59)
  Serial.print("%S: ");
  Serial.println(timeClient.getFormattedDateTime("%S")); // Second as a zero-padded decimal number (00 to 59)
  Serial.print("%a: ");
  Serial.println(timeClient.getFormattedDateTime("%a")); // Abbreviated weekday name according to the current locale
  Serial.print("%A: ");
  Serial.println(timeClient.getFormattedDateTime("%A")); // Full weekday name according to the current locale
  Serial.print("%w: ");
  Serial.println(timeClient.getFormattedDateTime("%w")); // Weekday as a decimal number (0 for Sunday through 6 for Saturday)
  Serial.print("%b: ");
  Serial.println(timeClient.getFormattedDateTime("%b")); // Abbreviated month name according to the current locale
  Serial.print("%B: ");
  Serial.println(timeClient.getFormattedDateTime("%B")); // Full month name according to the current locale
  Serial.print("%p: ");
  Serial.println(timeClient.getFormattedDateTime("%p")); // "AM" or "PM" based on the hour (Note: This is locale-sensitive and might not be applicable in all languages)
  Serial.println("-------------------");
  delay(1000);
}
