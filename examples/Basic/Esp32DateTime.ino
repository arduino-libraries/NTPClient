//#include <WiFi.h> // for WiFi shield Or Esp32
//#include <WiFi101.h> // for WiFi 101 shield or MKR1000
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void setup() {
  Serial.begin(115200);
  WiFi.begin("yourSSID", "yourPassword");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  timeClient.begin();
  timeClient.update();
}

void loop() {
  timeClient.update();

  if (timeClient.isTimeSet()) {
    unsigned long epochTime = timeClient.getEpochTime();
    struct tm *timeinfo;
    time_t time = (time_t)epochTime;
    timeinfo = gmtime(&time);

    int year = timeinfo->tm_year + 1900;
    int month = timeinfo->tm_mon + 1;
    int day = timeinfo->tm_mday;
    int hour = timeinfo->tm_hour;
    int minute = timeinfo->tm_min;
    int second = timeinfo->tm_sec;

    Serial.print("Current date and time: ");
    Serial.print(year);
    Serial.print("-");
    Serial.print(month);
    Serial.print("-");
    Serial.print(day);
    Serial.print(" ");
    Serial.print(hour);
    Serial.print(":");
    Serial.print(minute);
    Serial.print(":");
    Serial.println(second);
  }

  delay(1000);
}
