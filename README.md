# Grocery_product_detection_using_ESP32_CAM
ESP32-CAM Auto-Bill A low-cost, edge-AI produce-counter that detects fruit/veg with Edge-Impulse FOMO, weighs it with an HX711 load-cell, and displays the total price on an OLED – all on a non-PSRAM ESP32-CAM. Grab the code, flash, and turn your kitchen scale into an instant cash register.


# ESP32-CAM Auto-Bill 🥕💰

Edge-AI fruit & veg counter that **detects**, **weighs** and **prices** items in real-time.

## 🚀 Features
- **96×96 RGB** image classifier (Edge-Impulse FOMO)
- **HX711 load-cell** for weight
- **SSD1306 OLED** for live price
- **non-PSRAM** ESP32-CAM compatible
- **no cloud / no Wi-Fi** (optional stream)

## 🧰 Hardware
| Part | Purpose |
|---|---|
| ESP32-CAM (AI-Thinker) | Camera & MCU |
| HX711 + 5 kg load-cell | Weight sensor |
| SSD1306 OLED 128×64 | Display |
| 5 V PSU | Stable power |

## 🔧 Quick start
1. **Clone repo**
   ```bash
   git clone https://github.com/<your-user>/esp32-cam-autobill.git
