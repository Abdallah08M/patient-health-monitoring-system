#include <Arduino.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define DHT_PIN 4
#define DHT_TYPE DHT22
#define HEART_RATE_PIN 34
#define SPO2_PIN 35
#define BUZZER_PIN 25

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

DHT dht(DHT_PIN, DHT_TYPE);

Adafruit_SSD1306 display(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &Wire,
    OLED_RESET
);

void setup() {
    Serial.begin(115200);

    dht.begin();

    // Buzzer
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    // I2C: SDA = GPIO 21, SCL = GPIO 22
    Wire.begin(21, 22);

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
    display.println("System Ready");

    display.display();

    Serial.println("Patient Health Monitoring System");
    Serial.println("DHT22 temperature sensor initialized");
    Serial.println("Heart rate simulation initialized");
    Serial.println("SpO2 simulation initialized");
    Serial.println("OLED display initialized");
    Serial.println("Buzzer alarm initialized");

    delay(2000);
}

void loop() {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Heart-rate simulation
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

    // SpO2 simulation
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

    // Overall patient status
    bool warning = false;

    if (heartRate < 60 || heartRate > 100 || spo2 < 90) {
        warning = true;
    }

    // Alarm condition
    if (warning) {
        tone(BUZZER_PIN, 1000);
    } else {
        noTone(BUZZER_PIN);
    }

    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Failed to read from DHT22");
    } else {

        // Serial Monitor
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
            Serial.println(" | PATIENT WARNING");
        } else {
            Serial.println(" | PATIENT NORMAL");
        }

        // OLED
        display.clearDisplay();

        display.setTextSize(1);

        display.setCursor(0, 0);
        display.println("PATIENT MONITOR");

        display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

        display.setCursor(0, 14);
        display.print("Temp: ");
        display.print(temperature, 1);
        display.println(" C");

        display.setCursor(0, 26);
        display.print("Heart: ");
        display.print(heartRate);
        display.println(" BPM");

        display.setCursor(0, 38);
        display.print("SpO2: ");
        display.print(spo2);
        display.println(" %");

        display.setCursor(0, 50);

        if (warning) {
            display.println("!! WARNING !!");
        } else {
            display.println("STATUS: NORMAL");
        }

        display.display();
    }

    delay(2000);
}