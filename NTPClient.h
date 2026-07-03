#pragma once

#include "Arduino.h"

#include <Udp.h>

// Optional ESP32 AsyncUDP support.
// This keeps the existing UDP-based API intact and adds new constructors
// when AsyncUDP is available.
#if defined(ARDUINO_ARCH_ESP32)
  #include <WiFi.h>
  #include <AsyncUDP.h>
  #include "freertos/FreeRTOS.h"
  #include "freertos/portmacro.h"
  #define NTPCLIENT_HAS_ASYNCUDP 1
#else
  #define NTPCLIENT_HAS_ASYNCUDP 0
#endif

#define SEVENZYYEARS 2208988800UL
#define NTP_PACKET_SIZE 48
#define NTP_DEFAULT_LOCAL_PORT 1337

class NTPClient {
  private:
    enum class Transport : uint8_t {
      SyncUDP,
      AsyncUDP,
    };

    enum class LastError : uint8_t {
      None,
      NotBegun,
      SendFailed,
      DnsFailed,
      Timeout,
      ShortPacket,
    };

    Transport     _transport     = Transport::SyncUDP;
    UDP*          _udp;
    bool          _udpSetup       = false;

#if NTPCLIENT_HAS_ASYNCUDP
    AsyncUDP*     _asyncUdp       = nullptr;
    bool          _asyncListening  = false;
    volatile bool _asyncRequestInFlight = false;
    volatile bool _asyncPacketReady     = false;
    unsigned long _asyncRequestSentAt   = 0;
    unsigned long _asyncResponseAt      = 0;
    portMUX_TYPE  _asyncMux = portMUX_INITIALIZER_UNLOCKED;
#endif

    LastError     _lastError      = LastError::None;

    const char*   _poolServerName = "pool.ntp.org"; // Default time server
    IPAddress     _poolServerIP;
    unsigned int  _port           = NTP_DEFAULT_LOCAL_PORT;
    long          _timeOffset     = 0;

    unsigned long _updateInterval = 60000;  // In ms

    unsigned long _currentEpoc    = 0;      // In s
    unsigned long _lastUpdate     = 0;      // In ms

    byte          _packetBuffer[NTP_PACKET_SIZE];

    bool          sendNTPPacket();
    bool          processNTPPacket(unsigned long receivedAtMillis);

  public:
    NTPClient(UDP& udp);
    NTPClient(UDP& udp, long timeOffset);
    NTPClient(UDP& udp, const char* poolServerName);
    NTPClient(UDP& udp, const char* poolServerName, long timeOffset);
    NTPClient(UDP& udp, const char* poolServerName, long timeOffset, unsigned long updateInterval);
    NTPClient(UDP& udp, IPAddress poolServerIP);
    NTPClient(UDP& udp, IPAddress poolServerIP, long timeOffset);
    NTPClient(UDP& udp, IPAddress poolServerIP, long timeOffset, unsigned long updateInterval);

  #if NTPCLIENT_HAS_ASYNCUDP
    NTPClient(AsyncUDP& udp);
    NTPClient(AsyncUDP& udp, long timeOffset);
    NTPClient(AsyncUDP& udp, const char* poolServerName);
    NTPClient(AsyncUDP& udp, const char* poolServerName, long timeOffset);
    NTPClient(AsyncUDP& udp, const char* poolServerName, long timeOffset, unsigned long updateInterval);
    NTPClient(AsyncUDP& udp, IPAddress poolServerIP);
    NTPClient(AsyncUDP& udp, IPAddress poolServerIP, long timeOffset);
    NTPClient(AsyncUDP& udp, IPAddress poolServerIP, long timeOffset, unsigned long updateInterval);
  #endif

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
     * @return true on success, false on failure
     */
    bool update();

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

    /**
     * Returns the last internal error state (0 means none).
     * Primarily useful for AsyncUDP integrations and debugging.
     */
    uint8_t getLastError() const;

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
     * Stops the underlying UDP client
     */
    void end();
};
