#ifndef PLATFORM_COMPAT_H_
#define PLATFORM_COMPAT_H_

#include <Arduino.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

// ESP8266 and ESP32 expose the same WiFi / HTTPClient class names, but the
// headers live in different libraries.
#ifdef ESP8266
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
#else
  #include <WiFi.h>
  #include <HTTPClient.h>
  #include <esp_sleep.h>
#endif

// Wait until NTP has set the system clock, then fill *info with local time.
// ESP32 provides getLocalTime(); ESP8266 does not.
inline bool PlatformGetLocalTime(struct tm *info, uint32_t timeout_ms = 10000) {
#ifdef ESP8266
  time_t now = 0;
  uint32_t start = millis();
  while (millis() - start < timeout_ms) {
    time(&now);
    if (now > 8 * 3600 * 2) { // anything past 1970-01-01 means NTP has answered
      localtime_r(&now, info);
      return true;
    }
    delay(200);
  }
  return false;
#else
  return getLocalTime(info, timeout_ms);
#endif
}

inline void PlatformDeepSleepSeconds(uint64_t seconds) {
#ifdef ESP8266
  ESP.deepSleep(seconds * 1000000ULL);
#else
  esp_sleep_enable_timer_wakeup(seconds * 1000000ULL);
  esp_deep_sleep_start();
#endif
}

inline bool PlatformSetUnixTime(time_t unix_time) {
  struct timeval tv;
  tv.tv_sec = unix_time;
  tv.tv_usec = 0;
  return settimeofday(&tv, NULL) == 0;
}

// Survives deep sleep (not a power-on reset). Used for partial refresh cadence
// and reconstructing the clock without NTP.
struct PlatformRtcState {
  uint32_t magic;
  uint32_t bootCount;
  uint32_t weatherCount;
  uint32_t unixAtSleep;
  uint32_t lastWeatherUnix;
  uint32_t lastSleepSecs;
};

static const uint32_t kPlatformRtcMagic = 0xE42A0002;

#ifdef ESP32
static RTC_DATA_ATTR PlatformRtcState g_platformRtcState;
#endif

inline bool PlatformRtcRead(PlatformRtcState* state) {
#ifdef ESP8266
  return ESP.rtcUserMemoryRead(0, reinterpret_cast<uint32_t*>(state), sizeof(PlatformRtcState));
#else
  *state = g_platformRtcState;
  return true;
#endif
}

inline void PlatformRtcWrite(const PlatformRtcState* state) {
#ifdef ESP8266
  ESP.rtcUserMemoryWrite(0, const_cast<uint32_t*>(reinterpret_cast<const uint32_t*>(state)), sizeof(PlatformRtcState));
#else
  g_platformRtcState = *state;
#endif
}

inline bool PlatformRtcLoad(PlatformRtcState* state) {
  if (!PlatformRtcRead(state) || state->magic != kPlatformRtcMagic) {
    memset(state, 0, sizeof(*state));
    state->magic = kPlatformRtcMagic;
    return false;
  }
  return true;
}

#endif /* ifndef PLATFORM_COMPAT_H_ */
