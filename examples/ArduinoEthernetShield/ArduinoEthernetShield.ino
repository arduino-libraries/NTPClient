#include <EthernetUdp.h>
#include <NTPClient.h>

#define ethernetShieldPin 10  // Most Arduino shields
// #define ethernetShieldPin 5   // MKR ETH shield
// #define ethernetShieldPin 0   // Teensy 2.0
// #define ethernetShieldPin 20  // Teensy++ 2.0
// #define ethernetShieldPin 15  // ESP8266 with Adafruit Featherwing Ethernet
// #define ethernetShieldPin 33  // ESP32 with Adafruit Featherwing Ethernet

byte localMac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
unsigned int localUdpPort = 8888;
EthernetUDP udp;

NTPClient timeClient(udp);

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  Ethernet.init(ethernetShieldPin);
  Serial.println("Initialize Ethernet with DHCP");
  if (Ethernet.begin(localMac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.");
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    while (true) {
      delay(1);
    }
  }
  udp.begin(localUdpPort);
}

void loop() {
  if(timeClient.update()) {
    Serial.print("Updated current time: ");
    Serial.println(timeClient.getFormattedTime());
  }
}
