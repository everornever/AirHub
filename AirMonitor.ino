// ###############################################################
//  PROJEKT: AIR HUB - CYBERPUNK SENSOR DISPLAY
//  Von Leon Kling
// ###############################################################

// --------------------------------------------------------------
//  Bibliotheken
// --------------------------------------------------------------

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include "Seeed_BMP280.h"
#include "Grove_Temperature_And_Humidity_Sensor.h"
#include "Air_Quality_Sensor.h"

// --------------------------------------------------------------
//  Definitionen
// --------------------------------------------------------------

// --- Display Pins ---
#define TFT_CS   10
#define TFT_DC    9
#define TFT_RST   8

// --- Sensor Pins ---
#define LED_PIN          2
#define DUST_PIN         3
#define TEMP_PIN         4    
#define AQS_PIN  A0

// --- Sensor Typen ---
#define DHTTYPE DHT11

// --- Farben ---
#define BLACK      ST77XX_BLACK
#define GREEN      0x07E0
#define PINK       0xF81F
#define BLUE       0x07FF
#define WHITE      0xFFFF
#define GRAY       0x4A49
#define YELLOW     0xFD20
#define RED        0xF800

// --------------------------------------------------------------
//  Objekte initialisieren
// --------------------------------------------------------------

// Display
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Sensoren
AirQualitySensor AQS(AQS_PIN);
BMP280 bmp280;
DHT dht(TEMP_PIN, DHTTYPE);

// --------------------------------------------------------------
//  Globale Variablen
// --------------------------------------------------------------

// Timing & Serial Monitor
unsigned long startTime;
unsigned long lastMeasureTime = 0;
const unsigned long measureInterval = 30000;              // 30 Sekunden Intervall

// Animation
int currentPulseRing = 4;
unsigned long lastPulseTime = 0;
const int PULSE_SPEED = 250;
const int RING_COUNT = 4;
const int cx = 110; 
const int cy = 25; 
const int radiusMax = 10;
const int radiusStep = 4;

// Sensor Werte
float DHT_Temp = 0.0;
float DHT_Hum = 0.0;

float BMP_Pressure = 0.0;
float BMP_Temp = 0.0;
float BMP_altitude = 0.0;

int   AQS_Value = 0;
int   AQS_Quality = 0;

float DUST_Consentration = 0.0;

// Staub-Sensor Variablen
unsigned long sampletime_ms = 30000;

// --------------------------------------------------------------
//  Funktionen
// --------------------------------------------------------------

// ----------------- Main Layout -----------------
void drawMainInterface() {
  tft.fillScreen(BLACK);
  drawRadarKreis();
  drawFrame();
  drawHeader();
  drawValueFrame();
}

// ----------------- Radar Kreis -----------------
void drawRadarKreis() {
  for (int i = 0; i <= 5; i++) {
    tft.drawCircle(cx, cy, radiusMax - i * radiusStep, BLUE);
  }
}

// ----------------- Rahmen -----------------
void drawFrame() {

  const int edgeOffset = 8; 
  const int maxX = 127; 
  const int maxY = 159; 

  // --- HORIZONTALE LINIEN ---
  tft.drawLine(edgeOffset, 0, maxX - edgeOffset, 0, BLUE); 
  tft.drawLine(edgeOffset, maxY, maxX - edgeOffset, maxY, BLUE); 

  // --- VERTIKALE LINIEN ---
  tft.drawLine(0, edgeOffset, 0, maxY - edgeOffset, BLUE);
  tft.drawLine(maxX, edgeOffset, maxX, maxY - edgeOffset, BLUE);

  // --- DIAGONALE ECKEN ---
  tft.drawLine(0, edgeOffset, edgeOffset, 0, BLUE);    
  tft.drawLine(maxX - edgeOffset, 0, maxX, edgeOffset, BLUE);   
  tft.drawLine(0, maxY - edgeOffset, edgeOffset, maxY, BLUE); 
  tft.drawLine(maxX - edgeOffset, maxY, maxX, maxY - edgeOffset, BLUE);
}

// ----------------- Header -----------------
void drawHeader() {

  const int startY = 12;
  tft.setTextSize(1);
  
  // Dekorstriche
  tft.setTextColor(WHITE);
  tft.setCursor(8, startY); 
  tft.print("////////"); 
  
  // Uptime Platzhalter 
  tft.setTextColor(PINK);
  tft.setCursor(59, startY);
  tft.print("00:00"); 
  
  // System Titel
  tft.setTextColor(WHITE); 
  tft.setTextSize(2);
  tft.setCursor(8, startY + 12);
  tft.print("AIR HUB");
}

// ----------------- Value Frame -----------------
void drawValueFrame() {

  const int edgeOffset = 4; 
  const int frameStartX = 8;
  const int frameStartY = 45;
  const int frameEndX = 120;
  const int frameEndY = 152;
  const uint16_t frameColor = GRAY;
  
  // --- Horizontale Linien ---
  tft.drawLine(frameStartX + edgeOffset, frameStartY, frameEndX - edgeOffset, frameStartY, frameColor); 
  tft.drawLine(frameStartX + edgeOffset, frameEndY, frameEndX - edgeOffset, frameEndY, frameColor); 
  
  // --- Vertikale Linien ---
  tft.drawLine(frameStartX, frameStartY + edgeOffset, frameStartX, frameEndY - edgeOffset, frameColor); 
  tft.drawLine(frameEndX, frameStartY + edgeOffset, frameEndX, frameEndY - edgeOffset, frameColor); 
  
  // --- Diagonale Ecken ---
  tft.drawLine(frameStartX, frameStartY + edgeOffset, frameStartX + edgeOffset, frameStartY, frameColor); 
  tft.drawLine(frameEndX - edgeOffset, frameStartY, frameEndX, frameStartY + edgeOffset, frameColor);
  tft.drawLine(frameStartX, frameEndY - edgeOffset, frameStartX + edgeOffset, frameEndY, frameColor);
  tft.drawLine(frameEndX - edgeOffset, frameEndY, frameEndX, frameEndY - edgeOffset, frameColor);
}

// ----------------- Kreis Animation -----------------
void updateRadarPulseWave() {

  if (millis() - lastPulseTime < PULSE_SPEED) {
    return;
  }
  lastPulseTime = millis();

  if (currentPulseRing < 5) {
    int r_prev = radiusMax - (currentPulseRing + 1) * radiusStep;
    tft.drawCircle(cx, cy, r_prev, YELLOW);
  } else {
    int r_last = radiusMax - 0 * radiusStep;
    tft.drawCircle(cx, cy, r_last, YELLOW);
  }
  
  int r_current = radiusMax - currentPulseRing * radiusStep;
  tft.drawCircle(cx, cy, r_current, GRAY);
  
  currentPulseRing--;
  if (currentPulseRing < 0) {
    currentPulseRing = RING_COUNT;
  }

}

// ----------------- Uptime -----------------
void updateUptime() {

  unsigned long now = millis();
  unsigned long runSeconds = (now - startTime) / 1000;
  int runH = runSeconds / 3600;
  int runM = (runSeconds / 60) % 60;

  const int uptimeX = 59;
  const int uptimeY = 12;

  // Bereich löschen (Hintergrundfarbe)
  tft.fillRect(uptimeX, uptimeY, 40, 8, BLACK);

  if (runH <= 99) { 
    tft.setTextSize(1);
    tft.setTextColor(PINK);
    tft.setCursor(uptimeX, uptimeY);
    
    if (runH < 10) tft.print("0");
    tft.print(runH);
    tft.print(":");
    if (runM < 10) tft.print("0");
    tft.print(runM);

    Serial.println("Time updated");

  } else {

    tft.setTextColor(RED);
    tft.setCursor(uptimeX, uptimeY);
    tft.print("ERROR!");

  }
}

// ----------------- Sensor Display -----------------
void updateSensorDisplay() {
    
    // --- Layout Konfiguration ---
    const int startY = 58;      // Start Y-Position
    const int stepY = 20;       // Abstand zwischen den Zeilen (passt perfekt für 5 Werte)
    const int xCircle = 18;     // Position der Ampel
    const int xLineStart = 26;  // Start der Verbindungslinie
    const int xLineEnd = 38;    // Ende der Verbindungslinie
    const int xValue = 44;      // Start des Zahlenwerts
    const int radius = 3;       // Radius der Ampel
    
    tft.setTextSize(1);
    tft.setTextWrap(false);

    // ==========================================
    // 1. TEMPERATUR (°C)
    // ==========================================

    int yPos = startY;
    uint16_t tempColor;
    
    // Ampel-Logik
    if (DHT_Temp < 16) tempColor = RED;
    else if (DHT_Temp < 19) tempColor = YELLOW;
    else if (DHT_Temp <= 23) tempColor = GREEN;
    else if (DHT_Temp <= 26) tempColor = YELLOW;
    else tempColor = RED;
    
    // Löschen
    tft.fillRect(xValue, yPos - 4, 75, 8, BLACK);
    
    // Kreis und Value
    tft.fillCircle(xCircle, yPos, radius, tempColor);
    tft.drawLine(xLineStart, yPos, xLineEnd, yPos, GRAY);
    
    // Einheit
    tft.setCursor(xValue, yPos - 3);
    tft.setTextColor(WHITE);
    tft.print(DHT_Temp);
    tft.print(" C");

    // ==========================================
    // 2. FEUCHTIGKEIT (%)
    // ==========================================

    yPos += stepY;
    uint16_t humColor;
    
    if (DHT_Hum < 20) humColor = RED;
    else if (DHT_Hum <= 30) humColor = YELLOW;
    else if (DHT_Hum <= 60) humColor = GREEN;
    else if (DHT_Hum < 70) humColor = YELLOW;
    else humColor = RED;
    
    tft.fillRect(xValue, yPos - 4, 75, 8, BLACK);
  
    tft.fillCircle(xCircle, yPos, radius, humColor);
    tft.drawLine(xLineStart, yPos, xLineEnd, yPos, GRAY);
    
    tft.setCursor(xValue, yPos - 3);
    tft.setTextColor(WHITE);
    tft.print(DHT_Hum);
    tft.print(" %");

    // ==========================================
    // 3. LUFTQUALITÄT (AQS)
    // ==========================================

    yPos += stepY;
    uint16_t aqsColor;
    
    if (AQS_Quality == AirQualitySensor::FRESH_AIR) {
        aqsColor = GREEN;
    } else if (AQS_Quality == AirQualitySensor::LOW_POLLUTION) {
        aqsColor = YELLOW;
    } else if (AQS_Quality == AirQualitySensor::HIGH_POLLUTION) {
        aqsColor = RED;    
    } else {
        aqsColor = PINK; 
    }
    
    tft.fillRect(xValue, yPos - 4, 75, 8, BLACK);
    
    tft.fillCircle(xCircle, yPos, radius, aqsColor);
    tft.drawLine(xLineStart, yPos, xLineEnd, yPos, GRAY);
    
    tft.setCursor(xValue, yPos - 3);
    tft.setTextColor(WHITE);
    tft.print(AQS_Value);
    tft.print(" AQS");

    // ==========================================
    // 4. STAUBBELASTUNG (µg/m³)
    // ==========================================

    yPos += stepY;
    uint16_t dustColor;
    
    if (DUST_Consentration <= 12) dustColor = GREEN;
    else if (DUST_Consentration <= 55) dustColor = YELLOW; 
    else if (DUST_Consentration <= 150) dustColor = RED;
    else dustColor = PINK; 
    
    tft.fillRect(xValue, yPos - 4, 75, 8, BLACK);
    
    tft.fillCircle(xCircle, yPos, radius, dustColor);
    tft.drawLine(xLineStart, yPos, xLineEnd, yPos, GRAY);
    
    tft.setCursor(xValue, yPos - 3);
    tft.setTextColor(WHITE);
    tft.print(DUST_Consentration);
    tft.print(" ug/m3");

    // ==========================================
    // 5. LUFTDRUCK (hPa)
    // ==========================================

    yPos += stepY;
    uint16_t pressColor;
    
    if (BMP_Pressure < 980) pressColor = RED;
    else if (BMP_Pressure < 1001) pressColor = YELLOW;
    else if (BMP_Pressure <= 1020) pressColor = GREEN;
    else if (BMP_Pressure <= 1030) pressColor = YELLOW;
    else pressColor = RED;
    
    tft.fillRect(xValue, yPos - 4, 75, 8, BLACK);
    
    tft.fillCircle(xCircle, yPos, radius, pressColor);
    tft.drawLine(xLineStart, yPos, xLineEnd, yPos, GRAY);
    
    tft.setCursor(xValue, yPos - 3);
    tft.setTextColor(WHITE);
    tft.print(BMP_Pressure); 
    tft.print(" hPa");
}

// ----------------- Dust Sensor -----------------
void readDustSensor() {

  // Pulslänge direkt auslesen
  unsigned long duration = pulseIn(DUST_PIN, LOW);

  // Konzentration berechnen
  float ratio = duration / (sampletime_ms * 10.0); // sampletime_ms = 30000 ms
  DUST_Consentration = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62;

  // Emoji je nach Belastung
  if (DUST_Consentration <= 12) Serial.print("🟢 ");
  else if (DUST_Consentration <= 35) Serial.print("🟡 ");
  else if (DUST_Consentration <= 55) Serial.print("🟠 ");
  else if (DUST_Consentration <= 150) Serial.print("🔴 ");
  else Serial.print("⚫ ");

  // Ausgabe Staubbelastung + Wert
  Serial.print("Staubbelastung: ");
  if (DUST_Consentration <= 12) Serial.print("Sehr gering");
  else if (DUST_Consentration <= 35) Serial.print("Gering");
  else if (DUST_Consentration <= 55) Serial.print("Mäßig");
  else if (DUST_Consentration <= 150) Serial.print("Hoch");
  else Serial.print("Sehr hoch");

  Serial.print(" | Konzentration: ");
  Serial.print(DUST_Consentration);
  Serial.println(" µg/m³");
    
}

// ----------------- Air Quality Sensor ----------
void readAirQuality() {

    AQS_Quality = AQS.slope();
    AQS_Value = AQS.getValue();

    // Emoji je nach Wert
    if (AQS_Quality == AirQualitySensor::FRESH_AIR) Serial.print("🟢 ");
    else if (AQS_Quality == AirQualitySensor::LOW_POLLUTION) Serial.print("🟡 ");
    else if (AQS_Quality == AirQualitySensor::HIGH_POLLUTION) Serial.print("🔴 ");
    else if (AQS_Quality == AirQualitySensor::FORCE_SIGNAL) Serial.print("⚫ ");
    else Serial.print("❓ ");

    // Ausgabe Luftqualität + Wert
    Serial.print("Luftqualität: ");
    if (AQS_Quality == AirQualitySensor::FORCE_SIGNAL) Serial.print("Stark verschmutzt (Force Signal aktiv)");
    else if (AQS_Quality == AirQualitySensor::HIGH_POLLUTION) Serial.print("Stark verschmutzt");
    else if (AQS_Quality == AirQualitySensor::LOW_POLLUTION) Serial.print("Leicht verschmutzt");
    else if (AQS_Quality == AirQualitySensor::FRESH_AIR) Serial.print("Sauber");
    else Serial.print("Unbekannt");

    Serial.print(" | Rohwert: ");
    Serial.println(AQS_Value);
}

// ----------------- Barometer ----------
void readBMP280() {

    BMP_Pressure = bmp280.getPressure() / 100.0; // Pa → hPa
    BMP_Temp     = bmp280.getTemperature();
    BMP_altitude = bmp280.calcAltitude(BMP_Pressure * 100); // hPa → Pa für Höhe

    // ---- Druck einordnen ----
    const char* pressureState;
    if (BMP_Pressure < 980) {
        pressureState = "Sehr niedrig";
        Serial.print("🔴 ");
    } else if (BMP_Pressure < 1001) {
        pressureState = "Niedrig";
        Serial.print("🟡 ");
    } else if (BMP_Pressure <= 1020) {
        pressureState = "Normal";
        Serial.print("🟢 ");
    } else if (BMP_Pressure <= 1030) {
        pressureState = "Hoch";
        Serial.print("🟡 ");
    } else {
        pressureState = "Sehr hoch";
        Serial.print("🔴 ");
    }

    Serial.print("Luftdruck: ");
    Serial.print(pressureState);
    Serial.print(" | Wert: ");
    Serial.print(BMP_Pressure);
    Serial.print(" hPa");

    Serial.print(" | Höhe: ");
    Serial.print(BMP_altitude);
    Serial.print(" m");

    Serial.print(" | Temp: ");
    Serial.print(BMP_Temp);
    Serial.println("°C");
}

// ----------------- DHT11 Sensor -----------------
void readDHT11() {

  // values speichern
  float values[2] = {0};
  dht.readTempAndHumidity(values);
  DHT_Temp = values[1];
  DHT_Hum = values[0];

  // ---- Feuchtigkeit ----
  const char* humState;

  if (DHT_Hum < 20) {
    humState = "Sehr trocken";
    Serial.print("🔴 ");
  } else if (DHT_Hum <= 30) {
    humState = "Zu trocken";
    Serial.print("🟡 ");
  } else if (DHT_Hum <= 60) {
    humState = "Ideal";
    Serial.print("🟢 ");
  } else if (DHT_Hum < 70) {
    humState = "Zu feucht";
    Serial.print("🟡 ");
  } else {
    humState = "Sehr feucht";
    Serial.print("🔴 ");
  }

  Serial.print("Feuchtigkeit: ");
  Serial.print(humState);
  Serial.print(" | Wert: ");
  Serial.print(DHT_Hum);
  Serial.println("%");


  // ---- Temperatur ----
  const char* tempState;

  if (DHT_Temp < 16) {
      tempState = "Sehr kalt";
      Serial.print("🔴 ");
  } else if (DHT_Temp < 19) {
      tempState = "Zu kalt";
      Serial.print("🟡 ");
  } else if (DHT_Temp <= 23) {
      tempState = "Optimal";
      Serial.print("🟢 ");
  } else if (DHT_Temp <= 26) {
      tempState = "Zu warm";
      Serial.print("🟡 ");
  } else {
      tempState = "Sehr heiß";
      Serial.print("🔴 ");
  }

  Serial.print("Temperatur: ");
  Serial.print(tempState);
  Serial.print(" | Wert: ");
  Serial.print(DHT_Temp);
  Serial.println("°C");

}

// --------------------------------------------------------------
//  Main
// --------------------------------------------------------------

// ########################### - SETUP - ##########################
void setup() {
  
  // Serielle Kommunikation starten
  delay(1000);
  Serial.begin(9600);
  Serial.println("#################### - Setup - ######################");
  Serial.println("Alle Systeme werden initialisiert...");

  // Pins definieren
  pinMode(LED_PIN, OUTPUT);
  pinMode(DUST_PIN, INPUT);

  // Display & LED für Setup markieren
  digitalWrite(LED_PIN, HIGH);
  tft.initR(INITR_BLACKTAB); 
  tft.setRotation(2); 
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(1, 1);
  tft.println("Setup...");

  // Aufwärmphase Dust Sensor
  Serial.println("⏳ Staub Sensor heizt auf...");
  unsigned long warmupStart = millis();
  while (millis() - warmupStart < 30000) {  }
  Serial.println("✅ Staub Sensor bereit.");
  
  // Air Quality Sensor initialisieren
  Serial.println("⏳ Initialisiere Air Quality Sensor...");
  while (!AQS.init()) { }
  Serial.println("✅ Air Quality Sensor bereit.");

  // BMP280 initialisieren
  Serial.println("⏳ Initialisiere BMP280...");
  while (!bmp280.init()) { }
  Serial.println("✅ BMP280 bereit.");

  // DHT11 init
  Serial.println("⏳ Initialisiere DHT11...");
  dht.begin();
  Serial.println("✅ DHT11 bereit.");

  // Ende Setup
  Serial.println("✅ Setup abgeschlossen.");
  digitalWrite(LED_PIN, LOW);  

  // Startzeit für Uptime
  startTime = millis(); 
  
  // Interface aufbauen
  drawMainInterface();

}

// ########################### - LOOP - ##########################
void loop() {
  
  // Radar Animation
  updateRadarPulseWave();

  // Sensor Intervall (alle 30 Sekunden)
  unsigned long currentTime = millis();
  if (currentTime - lastMeasureTime >= measureInterval) {
    Serial.println("#################### - Neue Messwerte - ######################");
    digitalWrite(LED_PIN, HIGH);

    readDustSensor();
    readAirQuality();
    updateSensorDisplay();
    readBMP280();
    readDHT11();

    updateUptime();
    
    delay(500);
    digitalWrite(LED_PIN, LOW);
    lastMeasureTime = currentTime;
  }

}
