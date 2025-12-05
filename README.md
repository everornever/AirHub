# 🪴 AIR HUB

This project monitors critical indoor air data (temperature, humidity, air quality, particulate matter, and air pressure) and visualizes it in real-time on an ST7735 TFT display.

---

## ⚙️ HARDWARE

This project is built around the Seeed Studio **Arduino Lotus** (Arduino R2) and utilizes the **Grove system** for easy sensor integration.

| Component | Link |
| :--- | :--- |
| Arduino Lotus | [Seeed Studio Arduino Lotus](https://wiki.seeedstudio.com/Seeeduino_Lotus/) |
| 1.8" TFT Display (ST7735) | [AZ-Delivery 1.77" TFT Display](https://www.az-delivery.de/products/1-77-zoll-spi-tft-display) |
| Grove Air Quality Sensor | [Grove Air Quality Sensor v1.3](https://wiki.seeedstudio.com/Grove-Air_Quality_Sensor_v1.3/) |
| Grove Barometer Sensor (BMP280) | [Grove Barometer Sensor (BMP280)](https://wiki.seeedstudio.com/Grove-Barometer_Sensor-BMP280/) |
| Grove Temperature and Humidity Sensor (DHT11) | [Grove Temperature and Humidity Sensor (DHT11)](https://wiki.seeedstudio.com/Grove-Temperature_and_Humidity_Sensor/) |
| Grove Dust Sensor | [Grove Dust Sensor](https://wiki.seeedstudio.com/Grove-Dust_Sensor/) |
| Grove LED | [Grove LED](https://wiki.seeedstudio.com/Grove-Red_LED/) |
---

## 💾 SOFTWARE & DEPENDENCIES

To compile and run this project, you will need the Arduino IDE and the following libraries.

### 1. **Development Environment Setup**

To ensure compatibility with the **Seeeduino Lotus** board, you must add the board support package to the Arduino IDE.

* Open the Arduino IDE and go to `File > Preferences`.
* Under *Additional Boards Manager URLs*, add the following link:
    ```
    [https://raw.githubusercontent.com/Seeed-Studio/Seeed_Platform/master/package_legacy_seeeduino_boards_index.json](https://raw.githubusercontent.com/Seeed-Studio/Seeed_Platform/master/package_legacy_seeeduino_boards_index.json)
    ```
* Go to `Tools > Board > Boards Manager...` and search for **"Seeed"** to install the platform support.

### 2. **Required Libraries**

The following libraries must be installed via the Arduino Library Manager (`Sketch > Include Library > Manage Libraries...`):

| Library Name | Version | Link / Used For |
| :--- | :--- | :--- |
| **Arduino Core Libraries** | N/A | Base functionality |
| **Wire** | N/A | I²C communication |
| **SPI** | N/A | SPI communication |
| **Adafruit GFX Library** | 1.12.4 | [GitHub](https://github.com/olikraus/U8g2_for_Adafruit_GFX) |
| **Adafruit ST7735 Library** | 1.11.0 | [GitHub](https://github.com/adafruit/Adafruit-ST7735-Library) |
| **Seeed_BMP280** | 1.0.1 | [GitHub](https://github.com/Seeed-Studio/Grove_BMP280) |
| **Grove DHT** | 2.0.2 | [GitHub](https://github.com/Seeed-Studio/Grove_Temperature_And_Humidity_Sensor) |
| **Air Quality Sensor** | 1.0.2 | [GitHub](https://github.com/Seeed-Studio/Grove_Air_quality_Sensor) |

---

## ✨ CORE FUNCTIONALITY

The terminal provides a quick, visual overview of the environment using real-time data and a clear status system.

### A. DATA LOGGING

The system continuously reads the environment and displays the primary readings:

* **DHT11 Sensor:** Measures **Temperature** (C°) and **Humidity** (%)
* **Dust Sensor:** Measures **Dust Concentration** (ug/m^3)
* **Air Quality Sensor:** Provides the **Air Quality** status (Raw Value / Slope Status).
* **BMP280 Sensor:** Measures **Barometric Pressure** ($\text{hPa}$).
    > **Note:** The BMP280 also measures temperature and altitude. These values are processed internally but are currently only available in the serial logs, not on the main display.

### B. STATUS LOGIC (Traffic Light System)

A colored circle next to each value indicates the current status based on predefined thresholds:

| Status | Color | Meaning |
| :--- | :--- | :--- |
| **🟢 GREEN** | Optimal/Fresh | Conditions are ideal (e.g., Temp 19-23°C, Hum 30-60%) |
| **🟠 ORANGE** | Warning | Threshold reached (e.g., Temp slightly too high/low, mild pollution) |
| **🔴 RED** | Critical | High pollution, extreme temperature, or critical dust levels |
| **🟣 PINK** | System Error | Extreme State or AQS `FORCE_SIGNAL` (Maximum Overload) |

### C. HEADER & UPTIME

The header area displays the project title (`AIR HUB`) and the system's **UPTIME** (hours and minutes), tracking the operational time since the last reset.
