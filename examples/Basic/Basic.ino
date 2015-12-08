#include <NTPClient.h>
#include <ESP8266WiFi.h>

const char *ssid     = "<SSID>";
const char *password = "<PASSWORD>";



NTPClient timeClient();

void setup(){
  Serial.begin(115200);

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
