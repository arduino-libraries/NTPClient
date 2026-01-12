/**
 * The MIT License (MIT)
 * Copyright (c) 2015 by Fabrice Weinberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "NTPClient.h"

static const unsigned long NTP_DEFAULT_TIMEOUT_MS = 1000;

NTPClient::NTPClient(UDP& udp) {
  this->_udp            = &udp;
  this->_transport      = Transport::SyncUDP;
}

NTPClient::NTPClient(UDP& udp, long timeOffset) {
  this->_udp            = &udp;
  this->_timeOffset     = timeOffset;
  this->_transport      = Transport::SyncUDP;
}

NTPClient::NTPClient(UDP& udp, const char* poolServerName) {
  this->_udp            = &udp;
  this->_poolServerName = poolServerName;
  this->_transport      = Transport::SyncUDP;
}

NTPClient::NTPClient(UDP& udp, IPAddress poolServerIP) {
  this->_udp            = &udp;
  this->_poolServerIP   = poolServerIP;
  this->_poolServerName = NULL;
  this->_transport      = Transport::SyncUDP;
}

NTPClient::NTPClient(UDP& udp, const char* poolServerName, long timeOffset) {
  this->_udp            = &udp;
  this->_timeOffset     = timeOffset;
  this->_poolServerName = poolServerName;
  this->_transport      = Transport::SyncUDP;
}

NTPClient::NTPClient(UDP& udp, IPAddress poolServerIP, long timeOffset){
  this->_udp            = &udp;
  this->_timeOffset     = timeOffset;
  this->_poolServerIP   = poolServerIP;
  this->_poolServerName = NULL;
  this->_transport      = Transport::SyncUDP;
}

NTPClient::NTPClient(UDP& udp, const char* poolServerName, long timeOffset, unsigned long updateInterval) {
  this->_udp            = &udp;
  this->_timeOffset     = timeOffset;
  this->_poolServerName = poolServerName;
  this->_updateInterval = updateInterval;
  this->_transport      = Transport::SyncUDP;
}

NTPClient::NTPClient(UDP& udp, IPAddress poolServerIP, long timeOffset, unsigned long updateInterval) {
  this->_udp            = &udp;
  this->_timeOffset     = timeOffset;
  this->_poolServerIP   = poolServerIP;
  this->_poolServerName = NULL;
  this->_updateInterval = updateInterval;
  this->_transport      = Transport::SyncUDP;
}

#if NTPCLIENT_HAS_ASYNCUDP
NTPClient::NTPClient(AsyncUDP& udp) {
  this->_transport = Transport::AsyncUDP;
  this->_asyncUdp  = &udp;
}

NTPClient::NTPClient(AsyncUDP& udp, long timeOffset) {
  this->_transport  = Transport::AsyncUDP;
  this->_asyncUdp   = &udp;
  this->_timeOffset = timeOffset;
}

NTPClient::NTPClient(AsyncUDP& udp, const char* poolServerName) {
  this->_transport      = Transport::AsyncUDP;
  this->_asyncUdp       = &udp;
  this->_poolServerName = poolServerName;
}

NTPClient::NTPClient(AsyncUDP& udp, IPAddress poolServerIP) {
  this->_transport      = Transport::AsyncUDP;
  this->_asyncUdp       = &udp;
  this->_poolServerIP   = poolServerIP;
  this->_poolServerName = NULL;
}

NTPClient::NTPClient(AsyncUDP& udp, const char* poolServerName, long timeOffset) {
  this->_transport      = Transport::AsyncUDP;
  this->_asyncUdp       = &udp;
  this->_timeOffset     = timeOffset;
  this->_poolServerName = poolServerName;
}

NTPClient::NTPClient(AsyncUDP& udp, IPAddress poolServerIP, long timeOffset){
  this->_transport      = Transport::AsyncUDP;
  this->_asyncUdp       = &udp;
  this->_timeOffset     = timeOffset;
  this->_poolServerIP   = poolServerIP;
  this->_poolServerName = NULL;
}

NTPClient::NTPClient(AsyncUDP& udp, const char* poolServerName, long timeOffset, unsigned long updateInterval) {
  this->_transport      = Transport::AsyncUDP;
  this->_asyncUdp       = &udp;
  this->_timeOffset     = timeOffset;
  this->_poolServerName = poolServerName;
  this->_updateInterval = updateInterval;
}

NTPClient::NTPClient(AsyncUDP& udp, IPAddress poolServerIP, long timeOffset, unsigned long updateInterval) {
  this->_transport      = Transport::AsyncUDP;
  this->_asyncUdp       = &udp;
  this->_timeOffset     = timeOffset;
  this->_poolServerIP   = poolServerIP;
  this->_poolServerName = NULL;
  this->_updateInterval = updateInterval;
}
#endif

void NTPClient::begin() {
  this->begin(NTP_DEFAULT_LOCAL_PORT);
}

void NTPClient::begin(unsigned int port) {
  this->_port = port;

  this->_lastError = LastError::None;

#if NTPCLIENT_HAS_ASYNCUDP
  if (this->_transport == Transport::AsyncUDP) {
    if (this->_asyncUdp == nullptr) {
      this->_lastError = LastError::NotBegun;
      return;
    }

    // Bind to local port once and register callback.
    if (!this->_asyncListening) {
      if (!this->_asyncUdp->listen(this->_port)) {
        this->_lastError = LastError::NotBegun;
        return;
      }
      this->_asyncUdp->onPacket([this](AsyncUDPPacket packet) {
        if (packet.length() < NTP_PACKET_SIZE) {
          this->_lastError = LastError::ShortPacket;
          return;
        }
        portENTER_CRITICAL(&this->_asyncMux);
        memcpy(this->_packetBuffer, packet.data(), NTP_PACKET_SIZE);
        this->_asyncResponseAt = millis();
        this->_asyncPacketReady = true;
        this->_asyncRequestInFlight = false;
        portEXIT_CRITICAL(&this->_asyncMux);
      });
      this->_asyncListening = true;
    }

    this->_udpSetup = true;
    return;
  }
#endif

  this->_udp->begin(this->_port);
  this->_udpSetup = true;
}

bool NTPClient::forceUpdate() {
  #ifdef DEBUG_NTPClient
    Serial.println("Update from NTP Server");
  #endif

  this->_lastError = LastError::None;

#if NTPCLIENT_HAS_ASYNCUDP
  if (this->_transport == Transport::AsyncUDP) {
    if (!this->_udpSetup) {
      this->begin(this->_port);
      if (!this->_udpSetup) return false;
    }

    // Consume ready packet (if any)
    if (this->_asyncPacketReady) {
      unsigned long receivedAt = 0;
      portENTER_CRITICAL(&this->_asyncMux);
      receivedAt = this->_asyncResponseAt;
      this->_asyncPacketReady = false;
      portEXIT_CRITICAL(&this->_asyncMux);
      return this->processNTPPacket(receivedAt);
    }

    // Timeout check
    if (this->_asyncRequestInFlight && (millis() - this->_asyncRequestSentAt > NTP_DEFAULT_TIMEOUT_MS)) {
      this->_asyncRequestInFlight = false;
      this->_lastError = LastError::Timeout;
      return false;
    }

    // Trigger request if none in-flight
    if (!this->_asyncRequestInFlight) {
      if (!this->sendNTPPacket()) return false;
    }

    // Non-blocking: response will be handled on a future call.
    return false;
  }
#endif

  // flush any existing packets
  while(this->_udp->parsePacket() != 0)
    this->_udp->flush();

  if (!this->sendNTPPacket()) return false;

  // Wait till data is there or timeout...
  byte timeout = 0;
  int cb = 0;
  do {
    delay ( 10 );
    cb = this->_udp->parsePacket();
    if (timeout > 100) return false; // timeout after 1000 ms
    timeout++;
  } while (cb == 0);

  this->_lastUpdate = millis() - (10 * (timeout + 1)); // Account for delay in reading the time

  this->_udp->read(this->_packetBuffer, NTP_PACKET_SIZE);

  return this->processNTPPacket(this->_lastUpdate);
}

bool NTPClient::update() {
#if NTPCLIENT_HAS_ASYNCUDP
  if (this->_transport == Transport::AsyncUDP) {
    // Always allow consuming a received packet, even if we're not "due".
    if (this->_asyncPacketReady) {
      return this->forceUpdate();
    }

    // Trigger a new request only when due.
    if ((millis() - this->_lastUpdate >= this->_updateInterval) || this->_lastUpdate == 0) {
      if (!this->_udpSetup) this->begin(this->_port);
      // Non-blocking forceUpdate() will send and return.
      (void)this->forceUpdate();
    }
    return false;
  }
#endif

  if ((millis() - this->_lastUpdate >= this->_updateInterval) || this->_lastUpdate == 0) {
    if (!this->_udpSetup || this->_port != NTP_DEFAULT_LOCAL_PORT) this->begin(this->_port);
    return this->forceUpdate();
  }
  return false;
}

bool NTPClient::isTimeSet() const {
  return (this->_lastUpdate != 0); // returns true if the time has been set, else false
}

unsigned long NTPClient::getEpochTime() const {
  return this->_timeOffset + // User offset
         this->_currentEpoc + // Epoch returned by the NTP server
         ((millis() - this->_lastUpdate) / 1000); // Time since last update
}

int NTPClient::getDay() const {
  return (((this->getEpochTime()  / 86400L) + 4 ) % 7); //0 is Sunday
}
int NTPClient::getHours() const {
  return ((this->getEpochTime()  % 86400L) / 3600);
}
int NTPClient::getMinutes() const {
  return ((this->getEpochTime() % 3600) / 60);
}
int NTPClient::getSeconds() const {
  return (this->getEpochTime() % 60);
}

String NTPClient::getFormattedTime() const {
  unsigned long rawTime = this->getEpochTime();
  unsigned long hours = (rawTime % 86400L) / 3600;
  String hoursStr = hours < 10 ? "0" + String(hours) : String(hours);

  unsigned long minutes = (rawTime % 3600) / 60;
  String minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);

  unsigned long seconds = rawTime % 60;
  String secondStr = seconds < 10 ? "0" + String(seconds) : String(seconds);

  return hoursStr + ":" + minuteStr + ":" + secondStr;
}

void NTPClient::end() {
#if NTPCLIENT_HAS_ASYNCUDP
  if (this->_transport == Transport::AsyncUDP) {
    if (this->_asyncUdp != nullptr) {
      this->_asyncUdp->close();
    }
    this->_asyncListening = false;
    this->_asyncRequestInFlight = false;
    this->_asyncPacketReady = false;
    this->_udpSetup = false;
    return;
  }
#endif

  this->_udp->stop();
  this->_udpSetup = false;
}

void NTPClient::setTimeOffset(int timeOffset) {
  this->_timeOffset     = timeOffset;
}

void NTPClient::setUpdateInterval(unsigned long updateInterval) {
  this->_updateInterval = updateInterval;
}

void NTPClient::setPoolServerName(const char* poolServerName) {
    this->_poolServerName = poolServerName;
}

bool NTPClient::sendNTPPacket() {
  // set all bytes in the buffer to 0
  memset(this->_packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  this->_packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  this->_packetBuffer[1] = 0;     // Stratum, or type of clock
  this->_packetBuffer[2] = 6;     // Polling Interval
  this->_packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  this->_packetBuffer[12]  = 49;
  this->_packetBuffer[13]  = 0x4E;
  this->_packetBuffer[14]  = 49;
  this->_packetBuffer[15]  = 52;

#if NTPCLIENT_HAS_ASYNCUDP
  if (this->_transport == Transport::AsyncUDP) {
    if (this->_asyncUdp == nullptr) {
      this->_lastError = LastError::NotBegun;
      return false;
    }

    IPAddress target;
    if (this->_poolServerName) {
      if (this->_poolServerIP == IPAddress()) {
        if (!WiFi.hostByName(this->_poolServerName, this->_poolServerIP)) {
          this->_lastError = LastError::DnsFailed;
          return false;
        }
      }
      target = this->_poolServerIP;
    } else {
      target = this->_poolServerIP;
    }

    portENTER_CRITICAL(&this->_asyncMux);
    this->_asyncPacketReady = false;
    this->_asyncRequestInFlight = true;
    this->_asyncRequestSentAt = millis();
    portEXIT_CRITICAL(&this->_asyncMux);

    size_t written = this->_asyncUdp->writeTo(this->_packetBuffer, NTP_PACKET_SIZE, target, 123);
    if (written != NTP_PACKET_SIZE) {
      this->_asyncRequestInFlight = false;
      this->_lastError = LastError::SendFailed;
      return false;
    }
    return true;
  }
#endif

  // Sync UDP transport
  int ok = 0;
  if (this->_poolServerName) {
    ok = this->_udp->beginPacket(this->_poolServerName, 123);
  } else {
    ok = this->_udp->beginPacket(this->_poolServerIP, 123);
  }
  if (ok == 0) {
    this->_lastError = LastError::SendFailed;
    return false;
  }

  size_t written = this->_udp->write(this->_packetBuffer, NTP_PACKET_SIZE);
  if (written != NTP_PACKET_SIZE) {
    this->_lastError = LastError::SendFailed;
    (void)this->_udp->endPacket();
    return false;
  }

  ok = this->_udp->endPacket();
  if (ok == 0) {
    this->_lastError = LastError::SendFailed;
    return false;
  }

  return true;
}

void NTPClient::setRandomPort(unsigned int minValue, unsigned int maxValue) {
  randomSeed(analogRead(0));
  this->_port = random(minValue, maxValue);
}

bool NTPClient::processNTPPacket(unsigned long receivedAtMillis) {
  unsigned long highWord = word(this->_packetBuffer[40], this->_packetBuffer[41]);
  unsigned long lowWord = word(this->_packetBuffer[42], this->_packetBuffer[43]);
  unsigned long secsSince1900 = (highWord << 16) | lowWord;
  this->_currentEpoc = secsSince1900 - SEVENZYYEARS;
  this->_lastUpdate = receivedAtMillis;
  return true;
}

uint8_t NTPClient::getLastError() const {
  return static_cast<uint8_t>(this->_lastError);
}
