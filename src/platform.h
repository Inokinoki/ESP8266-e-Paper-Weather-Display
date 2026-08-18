#ifndef PLATFORM_COMPAT_H_
#define PLATFORM_COMPAT_H_

#include <Arduino.h>
#include <time.h>

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

#endif /* ifndef PLATFORM_COMPAT_H_ */
