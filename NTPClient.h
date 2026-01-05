#pragma once

#include "Arduino.h"

#include <Udp.h>

#define SEVENZYYEARS 2208988800UL
#define NTP_PACKET_SIZE 48
#define NTP_DEFAULT_LOCAL_PORT 1337

class NTPClient {
  private:
    UDP*          _udp;
    String        _dateLanguage   = "en"; // Default language
    bool          _udpSetup       = false;

    const char*   _poolServerName = "pool.ntp.org"; // Default time server
    IPAddress     _poolServerIP;
    unsigned int  _port           = NTP_DEFAULT_LOCAL_PORT;
    long          _timeOffset     = 0;

    unsigned long _updateInterval = 60000;  // In ms

    unsigned long _currentEpoc    = 0;      // In s
    unsigned long _lastUpdate     = 0;      // In ms

    byte          _packetBuffer[NTP_PACKET_SIZE];

    void          sendNTPPacket();

    struct DateLanguageData {
        const char* shortWeekDays[7];
        const char* longWeekDays[7];
        const char* shortMonths[12];
        const char* longMonths[12];
    };

    struct FullDateComponents {
        int year;
        int month;
        int day;
    };

    // Language map
    struct LanguageMap {
        const char* code;
        const DateLanguageData* data;
    };

    static const DateLanguageData EnglishData;
    static const DateLanguageData SpanishData;
    static const DateLanguageData PortugueseData;

    const LanguageMap languageMap[3] = {
        {"en", &EnglishData},
        {"es", &SpanishData},
        {"pt", &PortugueseData}
    };

    const int languageMapSize = sizeof(languageMap) / sizeof(languageMap[0]);
    const DateLanguageData* findLanguageData(const String& code) const;
    FullDateComponents calculateFullDateComponents() const;

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

    int getDayOfWeek() const;
    int getDay() const;
    int getMonth() const;
    int getYear() const;
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
     * @return Date Time string formatted. The available format codes are:
      %Y: Full year (e.g., 2023)
      %y: Last two digits of the year (e.g., 23 for 2023)
      %m: Month as a zero-padded decimal number (01 to 12)
      %d: Day of the month as a zero-padded decimal number (01 to 31)
      %H: Hour (00 to 23) as a zero-padded decimal number
      %M: Minute as a zero-padded decimal number (00 to 59)
      %S: Second as a zero-padded decimal number (00 to 59)
      %a: Abbreviated weekday name according to the current locale
      %A: Full weekday name according to the current locale
      %w: Weekday as a decimal number (0 for Sunday through 6 for Saturday)
      %b: Abbreviated month name according to the current locale
      %B: Full month name according to the current locale
      %p: "AM" or "PM" based on the hour (Note: This is locale-sensitive and might not be applicable in all languages)
     */
    String getFormattedDateTime(const String &format);

     /**
     * Set language for displaying date. Available languages are 'pt', 'es' and 'en' (default)
     * @param dateLanguage
     */
    void setDateLanguage(const String &dateLanguage);

    /**
     * @return time in seconds since Jan. 1, 1970
     */
    unsigned long getEpochTime() const;

    /**
     * Stops the underlying UDP client
     */
    void end();
};
