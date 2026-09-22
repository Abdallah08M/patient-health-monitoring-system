#include <Arduino.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#define DHT_PIN 4
#define DHT_TYPE DHT22
#define HEART_RATE_PIN 34
#define SPO2_PIN 35
#define BUZZER_PIN 25

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

// ===============================
// Wi-Fi settings for Wokwi
// ===============================
const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";

// ===============================
// ThingSpeak
// ===============================
const char* THINGSPEAK_API_KEY = "YOUR_WRITE_API_KEY";

const unsigned long THINGSPEAK_INTERVAL = 20000;
unsigned long lastThingSpeakUpdate = 0;

DHT dht(DHT_PIN, DHT_TYPE);

Adafruit_SSD1306 display(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &Wire,
    OLED_RESET
);

// ===============================
// Connect to Wi-Fi
// ===============================
void connectWiFi() {
    Serial.println("Connecting to Wokwi Wi-Fi...");

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int attempts = 0;

    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Wi-Fi connected");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("Wi-Fi connection failed");
    }
}

// ===============================
// Send data to ThingSpeak
// ===============================
void sendToThingSpeak(
    float temperature,
    float humidity,
    int heartRate,
    int spo2,
    bool warning
) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Wi-Fi not connected");
        return;
    }

    WiFiClientSecure client;
    client.setInsecure();   // Wokwi TLS

    HTTPClient http;

    String url = "https://api.thingspeak.com/update?api_key=";
    url += THINGSPEAK_API_KEY;
    url += "&field1=" + String(temperature, 1);
    url += "&field2=" + String(humidity, 1);
    url += "&field3=" + String(heartRate);
    url += "&field4=" + String(spo2);
    url += "&field5=" + String(warning ? 0 : 1);

    Serial.println("Sending data to ThingSpeak...");

    http.begin(client, url);

    int httpCode = http.GET();

    Serial.print("HTTP Code: ");
    Serial.println(httpCode);

    if (httpCode == 200) {
        String payload = http.getString();

        Serial.print("ThingSpeak Entry ID: ");
        Serial.println(payload);
    } else {
        Serial.print("HTTP Error: ");
        Serial.println(httpCode);
    }

    http.end();
}

void setup() {
    Serial.begin(115200);

    dht.begin();

    // Buzzer
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    // I2C: SDA = GPIO 21, SCL = GPIO 22
    Wire.begin(21, 22);

    // OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
        Serial.println("OLED initialization failed");

        while (true) {
            delay(100);
        }
    }

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    display.setTextSize(1);
    display.setCursor(20, 0);
    display.println("PATIENT MONITOR");

    display.setCursor(20, 20);
    display.println("Connecting WiFi...");

    display.display();

    Serial.println("Patient Health Monitoring System");
    Serial.println("DHT22 temperature sensor initialized");
    Serial.println("Heart rate simulation initialized");
    Serial.println("SpO2 simulation initialized");
    Serial.println("OLED display initialized");
    Serial.println("Buzzer alarm initialized");

    // Connect to Wi-Fi
    connectWiFi();

    display.clearDisplay();

    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("PATIENT MONITOR");

    display.setCursor(0, 20);

    if (WiFi.status() == WL_CONNECTED) {
        display.println("WiFi Connected");
        display.setCursor(0, 35);
        display.println("Cloud Ready");
    } else {
        display.println("WiFi Failed");
    }

    display.display();

    delay(2000);
}

void loop() {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // ===============================
    // Heart-rate simulation
    // ===============================
    int sensorValue = analogRead(HEART_RATE_PIN);
    int heartRate = map(sensorValue, 0, 4095, 50, 130);

    String heartRateStatus;

    if (heartRate < 60) {
        heartRateStatus = "LOW HEART RATE";
    } else if (heartRate <= 100) {
        heartRateStatus = "NORMAL";
    } else {
        heartRateStatus = "HIGH HEART RATE";
    }

    // ===============================
    // SpO2 simulation
    // ===============================
    int spo2SensorValue = analogRead(SPO2_PIN);
    int spo2 = map(spo2SensorValue, 0, 4095, 85, 100);

    String spo2Status;

    if (spo2 < 90) {
        spo2Status = "LOW SpO2";
    } else if (spo2 < 95) {
        spo2Status = "BORDERLINE SpO2";
    } else {
        spo2Status = "NORMAL SpO2";
    }

    // ===============================
    // Determine warning
    // ===============================
    bool warning = false;
    String warningReason = "";

    if (heartRate < 60) {
        warning = true;
        warningReason = "LOW HR";
    } else if (heartRate > 100) {
        warning = true;
        warningReason = "HIGH HR";
    } else if (spo2 < 90) {
        warning = true;
        warningReason = "LOW SpO2";
    }

    // ===============================
    // Buzzer alarm
    // ===============================
    if (warning) {
        tone(BUZZER_PIN, 1000);
    } else {
        noTone(BUZZER_PIN);
    }

    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Failed to read from DHT22");
    } else {

        // ===============================
        // Serial Monitor
        // ===============================
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.print(" °C | Humidity: ");
        Serial.print(humidity);
        Serial.print(" % | Heart Rate: ");
        Serial.print(heartRate);
        Serial.print(" BPM | Status: ");
        Serial.print(heartRateStatus);
        Serial.print(" | SpO2: ");
        Serial.print(spo2);
        Serial.print(" % | Status: ");
        Serial.print(spo2Status);

        if (warning) {
            Serial.print(" | WARNING: ");
            Serial.println(warningReason);
        } else {
            Serial.println(" | PATIENT NORMAL");
        }

        // ===============================
        // OLED
        // ===============================
        display.clearDisplay();

        display.setTextSize(1);

        display.setCursor(0, 0);
        display.println("PATIENT MONITOR");

        display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

        // Temperature
        display.setCursor(0, 14);
        display.print("Temp: ");
        display.print(temperature, 1);
        display.println(" C");

        // Heart rate
        display.setCursor(0, 26);
        display.print("Heart: ");
        display.print(heartRate);
        display.println(" BPM");

        // SpO2
        display.setCursor(0, 38);
        display.print("SpO2: ");
        display.print(spo2);
        display.println(" %");

        // Overall status
        display.setCursor(0, 50);

        if (warning) {
            display.print("WARNING: ");
            display.println(warningReason);
        } else {
            display.println("STATUS: NORMAL");
        }

        display.display();

        // ===============================
        // ThingSpeak upload
        // ===============================
        if (millis() - lastThingSpeakUpdate >= THINGSPEAK_INTERVAL) {

            sendToThingSpeak(
                temperature,
                humidity,
                heartRate,
                spo2,
                warning
            );

            lastThingSpeakUpdate = millis();
        }
    }

    delay(2000);
}