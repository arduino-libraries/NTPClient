#pragma once

#include "Arduino.h"

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define SEVENZYYEARS 2208988800UL
#define NTP_PACKET_SIZE 48

class NTPClient {
  private:
    WiFiUDP       _udp;

    const char*   _poolServerName = "time.nist.gov"; // Default time server
    int           _port           = 1337;
    int           _timeOffset;

    unsigned int  _updateInterval = 60000;  // In ms

    unsigned long _currentEpoc;             // In s
    unsigned long _lastUpdate     = 0;      // In ms

    byte          _packetBuffer[NTP_PACKET_SIZE];

    void          sendNTPPacket(IPAddress _timeServerIP);

  public:
    NTPClient();
    NTPClient(int timeOffset);
    NTPClient(const char* poolServerName);
    NTPClient(const char* poolServerName, int timeOffset);
    NTPClient(const char* poolServerName, int timeOffset, int updateInterval);

    /**
     * This should be called in the main loop of your application. By default an update from the NTP Server is only
     * made every 60 seconds. This can be configured in the NTPClient constructor.
     */
    void update();

    /**
     * This will force the update from the NTP Server.
     */
    void forceUpdate();

    String getHours();
    String getMinutes();
    String getSeconds();

    /**
     * @return time formatted like `hh:mm:ss`
     */
    String getFormattedTime();

    /**
     * @return time as raw seconds
     */
    unsigned long getRawTime();
};
