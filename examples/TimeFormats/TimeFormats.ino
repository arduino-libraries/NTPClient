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
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

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

  Serial.println("=============================");
  
  // 24-hour format
  Serial.print("24-hour format: ");
  Serial.println(timeClient.getFormattedTime());
  
  Serial.print("Hours (24h): ");
  Serial.println(timeClient.getHours());
  
  Serial.println("-----------------------------");
  
  // 12-hour format
  Serial.print("12-hour format: ");
  Serial.println(timeClient.getFormattedTime12());
  
  Serial.print("Hours (12h): ");
  Serial.println(timeClient.getHours12());
  
  Serial.print("AM/PM: ");
  Serial.println(timeClient.isPM() ? "PM" : "AM");
  
  Serial.println("=============================");
  Serial.println();

  delay(5000);
}
