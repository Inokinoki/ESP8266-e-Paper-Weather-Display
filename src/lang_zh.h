#define FONT(x) x##_tf

// Simplified Chinese UI strings.
// Helvetica fonts used by the examples do not contain CJK glyphs.
// To actually render these strings, switch u8g2 fonts to a Chinese set, e.g.:
//   u8g2_font_wqy12_t_gb2312
//   u8g2_font_wqy14_t_gb2312
// OpenWeatherMap descriptions can use Language = "ZH_CN" independently of this file.

//Temperature - Humidity - Forecast
const String TXT_FORECAST_VALUES  = "三日预报";
const String TXT_CONDITIONS       = "天气";
const String TXT_DAYS             = "(天)";
const String TXT_TEMPERATURES     = "温度";
const String TXT_TEMPERATURE_C    = "温度 (*C)";
const String TXT_TEMPERATURE_F    = "温度 (*F)";
const String TXT_HUMIDITY_PERCENT = "湿度 (%)";

// Pressure
const String TXT_PRESSURE         = "气压";
const String TXT_PRESSURE_HPA     = "气压 (hPa)";
const String TXT_PRESSURE_IN      = "气压 (in)";
const String TXT_PRESSURE_STEADY  = "稳定";
const String TXT_PRESSURE_RISING  = "上升";
const String TXT_PRESSURE_FALLING = "下降";

//RainFall / SnowFall
const String TXT_RAINFALL_MM = "降雨 (mm)";
const String TXT_RAINFALL_IN = "降雨 (in)";
const String TXT_SNOWFALL_MM = "降雪 (mm)";
const String TXT_SNOWFALL_IN = "降雪 (in)";
const String TXT_PRECIPITATION_SOON = "降水";


//Sun
const String TXT_SUNRISE  = "日出";
const String TXT_SUNSET   = "日落";

//Moon
const String TXT_MOON_NEW             = "新月";
const String TXT_MOON_WAXING_CRESCENT = "峨眉月";
const String TXT_MOON_FIRST_QUARTER   = "上弦月";
const String TXT_MOON_WAXING_GIBBOUS  = "盈凸月";
const String TXT_MOON_FULL            = "满月";
const String TXT_MOON_WANING_GIBBOUS  = "亏凸月";
const String TXT_MOON_THIRD_QUARTER   = "下弦月";
const String TXT_MOON_WANING_CRESCENT = "残月";

//Power / WiFi
const String TXT_POWER  = "电量";
const String TXT_WIFI   = "WiFi";
const char* TXT_UPDATED = "更新:";


//Wind
const String TXT_WIND_SPEED_DIRECTION = "风速/风向";
const String TXT_N   = "北";
const String TXT_NNE = "北东北";
const String TXT_NE  = "东北";
const String TXT_ENE = "东东北";
const String TXT_E   = "东";
const String TXT_ESE = "东东南";
const String TXT_SE  = "东南";
const String TXT_SSE = "南东南";
const String TXT_S   = "南";
const String TXT_SSW = "南西南";
const String TXT_SW  = "西南";
const String TXT_WSW = "西西南";
const String TXT_W   = "西";
const String TXT_WNW = "西西北";
const String TXT_NW  = "西北";
const String TXT_NNW = "北西北";

//Day of the week
const char* weekday_D[] = { "日", "一", "二", "三", "四", "五", "六" };

//Month
const char* month_M[] = { "1月", "2月", "3月", "4月", "5月", "6月", "7月", "8月", "9月", "10月", "11月", "12月" };
