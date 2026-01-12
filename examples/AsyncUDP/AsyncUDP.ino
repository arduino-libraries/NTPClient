/*
 * NTPClient - AsyncUDP example (ESP32)
 *
 * This example shows how to use NTPClient with ESP32's AsyncUDP for
 * non-blocking NTP synchronization.
 */

#include <NTPClient.h>

#if !defined(ARDUINO_ARCH_ESP32)
  #error "This example is intended to run on ESP32."
#endif

#include <WiFi.h>
#include <AsyncUDP.h>

const char* ssid     = "<SSID>";
const char* password = "<PASSWORD>";

AsyncUDP ntpUDP;

// By default 'pool.ntp.org' is used with 60 seconds update interval and no offset.
// You can also pass an IPAddress or custom pool name.
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60 * 1000);

static uint8_t lastErr = 0;

void setup() {
  Serial.begin(115200);
  delay(200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print('.');
  }
  Serial.println();
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // Bind local UDP port + register packet callback.
  timeClient.begin();

  // Kick off the first request immediately.
  // For AsyncUDP this is non-blocking; the reply is processed during loop().
  (void)timeClient.forceUpdate();
}

void loop() {
  // Call frequently: AsyncUDP replies are processed non-blocking.
  if (timeClient.update()) {
    Serial.print("NTP updated: ");
    Serial.println(timeClient.getFormattedTime());
  }

  // Optional: print any errors (e.g., DNS failure/timeout) when they occur.
  uint8_t err = timeClient.getLastError();
  if (err != 0 && err != lastErr) {
    lastErr = err;
    Serial.print("NTP error code: ");
    Serial.println(err);
  }

  // Show current time once per second after first sync.
  static unsigned long lastPrint = 0;
  if (timeClient.isTimeSet() && (millis() - lastPrint) >= 1000) {
    lastPrint = millis();
    Serial.println(timeClient.getFormattedTime());
  }

  delay(5);
}
