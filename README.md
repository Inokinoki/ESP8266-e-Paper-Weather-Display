# ESP8266 / ESP32 e-Paper Weather Display

ESP8266 / ESP32 电子墨水屏天气站，从 [OpenWeatherMap](https://openweathermap.org/) 拉取当前天气和预报并显示。

本仓库是 [G6EJD/ESP32-e-Paper-Weather-Display](https://github.com/G6EJD/ESP32-e-Paper-Weather-Display) 的 fork，目标是让 **4.2" 屏在 ESP8266 上可用**，同时保留 ESP32 示例。

An ESP8266 or ESP32 plus an e-paper panel that reads OpenWeatherMap and displays the weather.

---

## 平台支持 / Platform support

| 示例 | 推荐 MCU | 说明 |
| --- | --- | --- |
| `Waveshare_4_2` | **ESP8266 或 ESP32** | 本 fork 的主示例。ESP8266 默认按 Wemos D1 mini / NodeMCU 接线 |
| `Waveshare_1_54` / `2.9` / `2.7` / `2.13_T5` | ESP32 | 未改成 ESP8266 |
| `Waveshare_7_5` / `7_5_T7` / `9_7` | ESP32 | 帧缓冲太大，ESP8266 内存不够 |
| `M5_CoreInk` | ESP32 | M5 专用 |

---

## 重要变化 / Breaking changes

1. **必须使用经纬度**  
   OpenWeatherMap 已弃用城市名查询。请在 `owm_credentials.h` 中填写 `LAT` 和 `LON`（城市名仅用于屏幕标题）。  
   Current Weather 2.5 与 5-day / 3-hour Forecast 2.5 仍可走免费额度。One Call 3.0 需要付费订阅，本项目不使用。

2. **ESP8266 4.2" 引脚**  
   ESP8266 没有 GPIO 17/18/19/23。4.2" 示例默认改为：

   | 墨水屏 | Wemos D1 mini |
   | --- | --- |
   | BUSY | D2 (GPIO4) |
   | RST | D4 (GPIO2) |
   | DC | D3 (GPIO0) |
   | CS | D8 (GPIO15) |
   | CLK | D5 (GPIO14) |
   | DIN | D7 (GPIO13) |
   | GND | GND |
   | 3.3V | **3.3V（不要接 5V）** |

   Waveshare ESP8266 Driver Board 可用：BUSY=16, RST=5, DC=4, CS=15, CLK=14, DIN=13。

---

## 安装 / Setup

1. 用 Arduino IDE：Sketch → Include Library → Add .ZIP Library，加入本仓库。
2. 依赖库：
   - [GxEPD2](https://github.com/ZinggJM/GxEPD2)（需要 [Adafruit_GFX](https://github.com/adafruit/Adafruit-GFX-Library)）
   - [U8g2_for_Adafruit_GFX](https://github.com/olikraus/U8g2_for_Adafruit_GFX)
   - [ArduinoJson](https://github.com/bblanchon/ArduinoJson) v6 或 v7
3. File → Examples → `ESP8266-e-Paper-Weather-Display` → 选择你的屏幕尺寸。
4. 申请免费 [OWM API key](https://openweathermap.org/appid)。
5. 编辑示例里的 `owm_credentials.h`：WiFi、API key、**LAT/LON**、语言、公制/英制、时区。
6. 编译上传。

经纬度可在 [OpenStreetMap](https://www.openstreetmap.org/) 或 [latlong.net](https://www.latlong.net/) 查询。如果屏幕上全是空白数值，通常是坐标或 API key 无效。

旧版 Waveshare HAT 请在 `InitialiseDisplay()` 里改用 `display.init();`。

---

## ESP8266 注意 / ESP8266 notes

- **内存**：`common.h` 对 ESP8266 使用过滤后的 JSON 和 12KB 文档，避免 35KB 缓冲分配失败。
- **时间**：ESP8266 没有 `getLocalTime()`。4.2" 示例用 NTP + `localtime_r()`。
- **电池**：ESP8266 默认不画电池。若 A0 接了分压，在编译时定义 `HAS_BATTERY_MONITOR`。
- **深度睡眠**：4.2" 示例调用 `ESP.deepSleep()`；GPIO16 需接到 RST 才能自动唤醒。
- **调试输出**：在包含 `common.h` 之前 `#define WX_DEBUG 0` 可关掉天气字段打印，节省时间和电量。

---

## 分区域更新 / Partial refresh (4.2")

`Waveshare_4_2` 默认用 GxEPD2 `displayWindow()` 按区域刷新，避免每次整屏闪白：

| 区域 | 范围 | 内容 |
| --- | --- | --- |
| 顶栏 | 400×16 | 时间、日期、城市、电量 |
| 左侧 | 232×172 | 风向、图标、温度、描述 |
| 右侧 | 168×172 | 3 小时预报、降水、月相 |
| 底部 | 400×112 | 气压 / 温度 / 降水曲线 |

每 `FULL_REFRESH_EVERY` 次天气更新（默认 8 次，约 4 小时）仍会整屏全刷，用来清残影。深度睡眠时请保持面板 3.3V，否则局部刷新会花屏。

在 `Waveshare_4_2.ino` 里可改：

```cpp
#define USE_PARTIAL_UPDATE 1     // 0 = 始终全屏刷新
#define FULL_REFRESH_EVERY 8
#define CLOCK_PARTIAL_MINUTES 0  // 改成 1 则每分钟只刷新顶栏时间（更费电）
```

部分 4.2" 新面板局部刷新较差，若出现残影或花屏，把 `USE_PARTIAL_UPDATE` 设为 `0`。

---

## 语言 / Languages

在 sketch 里改 `#include "lang.h"` 为其它文件：`lang_cz.h`、`lang_es.h`、`lang_fr.h`、`lang_gr.h`、`lang_it.h`、`lang_nl.h`、`lang_no.h`、`lang_pl.h`、`lang_pt.h`、**`lang_zh.h`（简体中文）**。

Helvetica 字库不含汉字。要用 `lang_zh.h`，请把 u8g2 字体换成例如 `u8g2_font_wqy12_t_gb2312`。OWM 的 `Language = "ZH_CN"` 只影响接口返回的天气描述。

带音标的语言可把 `u8g2_font_helvB08_tf` 换成 `u8g2_font_helvB08_te`（`tf` → `te`）。

---

## 接线总图 / Wiring

所有 Waveshare 墨水屏的信号线定义相同（7.5" 与 4.2" / 2.9" / 1.54" 一样）。TTGO T5 / T5S 已板载连好。

![Wiring schematic](/Schematic.JPG)

ESP32 电池监测默认按 Lolin D32（GPIO35，板载 100K+100K 分压）。其它板请改 `analogRead` 引脚并自行加分压。TTGO T5 / T5S 已有分压。

---

## 屏幕预览 / Layouts

7.5" 800x480

![7.5 new](/Waveshare_7_5_new.jpg)

7.5" 640x384

![7.5](/Waveshare_7_5.jpg)

4.2" 400x300

![4.2](/Waveshare_4_2.jpg)

2.7" 264x176

![2.7](/Waveshare_2_7.jpg)

2.13" 250x122

![2.13](/Waveshare_2_13.jpg)

1.54" 200x200

![1.54](/Waveshare_1_54.jpg)

最新 Waveshare HAT 需要较新的 GxEPD2，初始化为：

```cpp
display.init(115200, true, 2, false);
```
