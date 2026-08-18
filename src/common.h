#ifndef COMMON_H_
#define COMMON_H_

#include <Arduino.h>
#include <ArduinoJson.h>

#include "forecast_record.h"
#include "common_functions.h"
#include "platform.h"

#ifndef WX_DEBUG
#define WX_DEBUG 1
#endif

#if WX_DEBUG
  #define WX_PRINT(...)   Serial.print(__VA_ARGS__)
  #define WX_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
  #define WX_PRINT(...)
  #define WX_PRINTLN(...)
#endif

// ESP8266 has ~80KB heap; a 35KB JSON document plus the 4.2" frame buffer
// will often fail to allocate. Filter the payload and use a smaller buffer.
#ifdef ESP8266
  #define WX_JSON_BUFFER_SIZE (12 * 1024)
#else
  #define WX_JSON_BUFFER_SIZE (35 * 1024)
#endif

//#########################################################################################
void Convert_Readings_to_Imperial() {
  WxConditions[0].Pressure = hPa_to_inHg(WxConditions[0].Pressure);
  WxForecast[1].Rainfall   = mm_to_inches(WxForecast[1].Rainfall);
  WxForecast[1].Snowfall   = mm_to_inches(WxForecast[1].Snowfall);
}

//#########################################################################################
template <typename TDoc>
static void BuildWxFilter(TDoc& filter, const String& Type) {
  if (Type == "weather") {
    filter["coord"]["lon"] = true;
    filter["coord"]["lat"] = true;
    filter["weather"][0]["main"] = true;
    filter["weather"][0]["description"] = true;
    filter["weather"][0]["icon"] = true;
    filter["weather"][1]["description"] = true;
    filter["weather"][2]["description"] = true;
    filter["main"]["temp"] = true;
    filter["main"]["feels_like"] = true;
    filter["main"]["pressure"] = true;
    filter["main"]["humidity"] = true;
    filter["main"]["temp_min"] = true;
    filter["main"]["temp_max"] = true;
    filter["wind"]["speed"] = true;
    filter["wind"]["deg"] = true;
    filter["clouds"]["all"] = true;
    filter["visibility"] = true;
    filter["rain"]["1h"] = true;
    filter["snow"]["1h"] = true;
    filter["sys"]["country"] = true;
    filter["sys"]["sunrise"] = true;
    filter["sys"]["sunset"] = true;
    filter["timezone"] = true;
  } else {
    // In a filter document, list[0] applies to every array element.
    filter["list"][0]["dt"] = true;
    filter["list"][0]["main"]["temp"] = true;
    filter["list"][0]["main"]["temp_min"] = true;
    filter["list"][0]["main"]["temp_max"] = true;
    filter["list"][0]["main"]["pressure"] = true;
    filter["list"][0]["main"]["humidity"] = true;
    filter["list"][0]["weather"][0]["main"] = true;
    filter["list"][0]["weather"][0]["description"] = true;
    filter["list"][0]["weather"][0]["icon"] = true;
    filter["list"][0]["weather"][1]["main"] = true;
    filter["list"][0]["weather"][2]["main"] = true;
    filter["list"][0]["clouds"]["all"] = true;
    filter["list"][0]["wind"]["speed"] = true;
    filter["list"][0]["wind"]["deg"] = true;
    filter["list"][0]["rain"]["3h"] = true;
    filter["list"][0]["snow"]["3h"] = true;
    filter["list"][0]["pop"] = true;
    filter["list"][0]["dt_txt"] = true;
  }
}

//#########################################################################################
// Problems with structuring JSON decodes, see here: https://arduinojson.org/assistant/
bool DecodeWeather(WiFiClient& json, String Type) {
  WX_PRINT(F("\nCreating object...and "));
#if ARDUINOJSON_VERSION_MAJOR >= 7
  JsonDocument filter;
  JsonDocument doc;
#else
  StaticJsonDocument<1536> filter;
  DynamicJsonDocument doc(WX_JSON_BUFFER_SIZE);
#endif
  BuildWxFilter(filter, Type);

  DeserializationError error = deserializeJson(doc, json, DeserializationOption::Filter(filter));
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return false;
  }

  JsonObject root = doc.as<JsonObject>();
  WX_PRINTLN(" Decoding " + Type + " data");
  if (Type == "weather") {
    WxConditions[0].lon         = root["coord"]["lon"].as<float>();                    WX_PRINTLN(" Lon: "+String(WxConditions[0].lon));
    WxConditions[0].lat         = root["coord"]["lat"].as<float>();                    WX_PRINTLN(" Lat: "+String(WxConditions[0].lat));
    WxConditions[0].Main0       = root["weather"][0]["main"].as<const char*>();        WX_PRINTLN("Main: "+String(WxConditions[0].Main0));
    WxConditions[0].Forecast0   = root["weather"][0]["description"].as<const char*>(); WX_PRINTLN("For0: "+String(WxConditions[0].Forecast0));
    WxConditions[0].Forecast1   = root["weather"][1]["description"].as<const char*>(); WX_PRINTLN("For1: "+String(WxConditions[0].Forecast1));
    WxConditions[0].Forecast2   = root["weather"][2]["description"].as<const char*>(); WX_PRINTLN("For2: "+String(WxConditions[0].Forecast2));
    WxConditions[0].Icon        = root["weather"][0]["icon"].as<const char*>();        WX_PRINTLN("Icon: "+String(WxConditions[0].Icon));
    WxConditions[0].Temperature = root["main"]["temp"].as<float>();                    WX_PRINTLN("Temp: "+String(WxConditions[0].Temperature));
    WxConditions[0].Feelslike   = root["main"]["feels_like"].as<float>();              WX_PRINTLN("Feel: "+String(WxConditions[0].Feelslike));
    WxConditions[0].Pressure    = root["main"]["pressure"].as<float>();                WX_PRINTLN("Pres: "+String(WxConditions[0].Pressure));
    WxConditions[0].Humidity    = root["main"]["humidity"].as<float>();                WX_PRINTLN("Humi: "+String(WxConditions[0].Humidity));
    WxConditions[0].Low         = root["main"]["temp_min"].as<float>();                WX_PRINTLN("TLow: "+String(WxConditions[0].Low));
    WxConditions[0].High        = root["main"]["temp_max"].as<float>();                WX_PRINTLN("THig: "+String(WxConditions[0].High));
    WxConditions[0].Windspeed   = root["wind"]["speed"].as<float>();                   WX_PRINTLN("WSpd: "+String(WxConditions[0].Windspeed));
    WxConditions[0].Winddir     = root["wind"]["deg"].as<float>();                     WX_PRINTLN("WDir: "+String(WxConditions[0].Winddir));
    WxConditions[0].Cloudcover  = root["clouds"]["all"].as<int>();                     WX_PRINTLN("CCov: "+String(WxConditions[0].Cloudcover));
    WxConditions[0].Visibility  = root["visibility"].as<int>();                        WX_PRINTLN("Visi: "+String(WxConditions[0].Visibility));
    WxConditions[0].Rainfall    = root["rain"]["1h"].as<float>();                      WX_PRINTLN("Rain: "+String(WxConditions[0].Rainfall));
    WxConditions[0].Snowfall    = root["snow"]["1h"].as<float>();                      WX_PRINTLN("Snow: "+String(WxConditions[0].Snowfall));
    WxConditions[0].Country     = root["sys"]["country"].as<const char*>();            WX_PRINTLN("Ctry: "+String(WxConditions[0].Country));
    WxConditions[0].Sunrise     = root["sys"]["sunrise"].as<int>();                    WX_PRINTLN("SRis: "+String(WxConditions[0].Sunrise));
    WxConditions[0].Sunset      = root["sys"]["sunset"].as<int>();                     WX_PRINTLN("SSet: "+String(WxConditions[0].Sunset));
    WxConditions[0].Timezone    = root["timezone"].as<int>();                          WX_PRINTLN("TZon: "+String(WxConditions[0].Timezone));
  }
  if (Type == "forecast") {
    WX_PRINT(F("\nReceiving Forecast period - "));
    JsonArray list = root["list"];
    for (byte r = 0; r < max_readings; r++) {
      WX_PRINTLN("\nPeriod-" + String(r) + "--------------");
      WxForecast[r].Dt                = list[r]["dt"].as<int>();                                WX_PRINTLN("DTim: "+String(WxForecast[r].Dt));
      WxForecast[r].Temperature       = list[r]["main"]["temp"].as<float>();                    WX_PRINTLN("Temp: "+String(WxForecast[r].Temperature));
      WxForecast[r].Low               = list[r]["main"]["temp_min"].as<float>();                WX_PRINTLN("TLow: "+String(WxForecast[r].Low));
      WxForecast[r].High              = list[r]["main"]["temp_max"].as<float>();                WX_PRINTLN("THig: "+String(WxForecast[r].High));
      WxForecast[r].Pressure          = list[r]["main"]["pressure"].as<float>();                WX_PRINTLN("Pres: "+String(WxForecast[r].Pressure));
      WxForecast[r].Humidity          = list[r]["main"]["humidity"].as<float>();                WX_PRINTLN("Humi: "+String(WxForecast[r].Humidity));
      WxForecast[r].Forecast0         = list[r]["weather"][0]["main"].as<const char*>();        WX_PRINTLN("For0: "+String(WxForecast[r].Forecast0));
      WxForecast[r].Forecast1         = list[r]["weather"][1]["main"].as<const char*>();        WX_PRINTLN("For1: "+String(WxForecast[r].Forecast1));
      WxForecast[r].Forecast2         = list[r]["weather"][2]["main"].as<const char*>();        WX_PRINTLN("For2: "+String(WxForecast[r].Forecast2));
      WxForecast[r].Icon              = list[r]["weather"][0]["icon"].as<const char*>();        WX_PRINTLN("Icon: "+String(WxForecast[r].Icon));
      WxForecast[r].Description       = list[r]["weather"][0]["description"].as<const char*>(); WX_PRINTLN("Desc: "+String(WxForecast[r].Description));
      WxForecast[r].Cloudcover        = list[r]["clouds"]["all"].as<int>();                     WX_PRINTLN("CCov: "+String(WxForecast[r].Cloudcover));
      WxForecast[r].Windspeed         = list[r]["wind"]["speed"].as<float>();                   WX_PRINTLN("WSpd: "+String(WxForecast[r].Windspeed));
      WxForecast[r].Winddir           = list[r]["wind"]["deg"].as<float>();                     WX_PRINTLN("WDir: "+String(WxForecast[r].Winddir));
      WxForecast[r].Rainfall          = list[r]["rain"]["3h"].as<float>();                      WX_PRINTLN("Rain: "+String(WxForecast[r].Rainfall));
      WxForecast[r].Snowfall          = list[r]["snow"]["3h"].as<float>();                      WX_PRINTLN("Snow: "+String(WxForecast[r].Snowfall));
      WxForecast[r].Pop               = list[r]["pop"].as<float>();                             WX_PRINTLN("Pop:  "+String(WxForecast[r].Pop));
      WxForecast[r].Period            = list[r]["dt_txt"].as<const char*>();                    WX_PRINTLN("Peri: "+String(WxForecast[r].Period));
    }
    float pressure_trend = WxForecast[0].Pressure - WxForecast[2].Pressure;
    pressure_trend = ((int)(pressure_trend * 10)) / 10.0;
    WxConditions[0].Trend = "0";
    if (pressure_trend > 0)  WxConditions[0].Trend = "+";
    if (pressure_trend < 0)  WxConditions[0].Trend = "-";
    if (pressure_trend == 0) WxConditions[0].Trend = "0";

    if (Units == "I") Convert_Readings_to_Imperial();
  }
  return true;
}
//#########################################################################################
String ConvertUnixTime(int unix_time) {
  // Returns either '21:12  ' or ' 09:12pm' depending on Units mode
  time_t tm = unix_time;
  struct tm *now_tm = localtime(&tm);
  char output[40];
  if (Units == "M") {
    strftime(output, sizeof(output), "%H:%M %d/%m/%y", now_tm);
  }
  else {
    strftime(output, sizeof(output), "%I:%M%P %m/%d/%y", now_tm);
  }
  return output;
}
//#########################################################################################
bool obtain_wx_data(WiFiClient& client, const String& RequestType) {
  const String units = (Units == "M" ? "metric" : "imperial");
  client.stop();
  HTTPClient http;
  // OpenWeatherMap deprecated city-name lookup. Prefer lat/lon (Current Weather
  // and 5-day / 3-hour Forecast 2.5 APIs remain available on the free tier).
  String uri = "/data/2.5/" + RequestType + "?appid=" + apikey + "&mode=json&units=" + units + "&lang=" + Language;
  if (LAT.length() > 0 && LON.length() > 0) {
    uri += "&lat=" + LAT + "&lon=" + LON;
  } else {
    uri += "&q=" + City + "," + Country;
  }
  if (RequestType != "weather") {
    uri += "&cnt=" + String(max_readings);
  }
  http.begin(client, server, 80, uri);
  http.setTimeout(15000);
  http.useHTTP10(true); // avoid chunked encoding, which ArduinoJson cannot parse from the raw stream
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    if (!DecodeWeather(http.getStream(), RequestType)) {
      client.stop();
      http.end();
      return false;
    }
    client.stop();
    http.end();
    return true;
  }
  Serial.printf("connection failed, error: %s", http.errorToString(httpCode).c_str());
  client.stop();
  http.end();
  return false;
}
#endif /* ifndef COMMON_H_ */
