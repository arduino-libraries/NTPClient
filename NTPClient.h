#pragma once

#include "Arduino.h"

#include <Udp.h>

#define SEVENZYYEARS 2208988800UL
#define NTP_PACKET_SIZE 48
#define NTP_DEFAULT_LOCAL_PORT 1337
#define DEBUG_NTPClient

class NTPClient {
  private:
    UDP*          _udp;
    bool          _udpSetup       = false;

    const char*   _poolServerName = "pool.ntp.org"; // Default time server
    IPAddress     _poolServerIP;
    unsigned int  _port           = NTP_DEFAULT_LOCAL_PORT;
    long          _timeOffset     = 0;

    unsigned long _updateInterval = 60000;  // In ms

    unsigned long _currentEpoc    = 0;      // In s
    float         _current_epoc_dec = 0;    // In s, decimal part of current epoc
    uint32_t      _lastUpdate     = 0xff000000;// In ms

    uint16_t      _ntp_timeout    = 1000;    // In ms
    uint32_t      _last_fail      = 0xffff0000; // In ms

    byte          _packetBuffer[NTP_PACKET_SIZE];

    void          sendNTPPacket();

  public:
    NTPClient(UDP& udp);
    NTPClient(UDP& udp, long timeOffset);
    NTPClient(UDP& udp, const char* poolServerName);
    NTPClient(UDP& udp, const char* poolServerName, long timeOffset);
    NTPClient(UDP& udp, const char* poolServerName, long timeOffset, unsigned long updateInterval);
    NTPClient(UDP& udp, IPAddress poolServerIP);
    NTPClient(UDP& udp, IPAddress poolServerIP, long timeOffset);
    NTPClient(UDP& udp, IPAddress poolServerIP, long timeOffset, unsigned long updateInterval);

    /**
     * Set time server name
     *
     * @param poolServerName
     */
    void setPoolServerName(const char* poolServerName);

     /**
     * Set random local port
     */
    void setRandomPort(unsigned int minValue = 49152, unsigned int maxValue = 65535);

    /**
     * clear time server and set ip
     *
     * @param ServerIP
     */
    void setPoolServerIP(IPAddress server_ip);

    /**
     * Set ntp timeout (recommendation < 1000ms)
     *
     * @param t_ms
     */
    void setTimeout(uint16_t t_ms);

    /**
     * Starts the underlying UDP client with the default local port
     */
    void begin();

    /**
     * Starts the underlying UDP client with the specified local port
     */
    void begin(unsigned int port);

    /**
     * This should be called in the main loop of your application. By default an update from the NTP Server is only
     * made every 60 seconds. This can be configured in the NTPClient constructor.
     *
     * @return 1(true) on updated and success
     *         0(false) on updated and failure
     *         2 on not time to update
     *         3 on it's time to update but last failed was just happen, so it decided to wait more
     */
    int8_t update();

    /**
     * This will force the update from the NTP Server.
     *
     * @return true on success, false on failure
     */
    bool forceUpdate();

    /**
     * This allows to check if the NTPClient successfully received a NTP packet and set the time.
     *
     * @return true if time has been set, else false
     */
    bool isTimeSet() const;

    int getDay() const;
    int getHours() const;
    int getMinutes() const;
    int getSeconds() const;

    /**
     * Changes the time offset. Useful for changing timezones dynamically
     */
    void setTimeOffset(int timeOffset);

    /**
     * Set the update interval to another frequency. E.g. useful when the
     * timeOffset should not be set in the constructor
     */
    void setUpdateInterval(unsigned long updateInterval);

    /**
     * @return time formatted like `hh:mm:ss`
     */
    String getFormattedTime() const;

    /**
     * @return time in seconds since Jan. 1, 1970
     */
    unsigned long getEpochTime() const;

    /**
     * @return ms of this second, in ms
     */
    float get_millis() const;

    /**
     * Stops the underlying UDP client
     */
    void end();
};
