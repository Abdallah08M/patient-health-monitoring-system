#include <Arduino.h>
#include <DHT.h>

#define DHT_PIN 4
#define DHT_TYPE DHT22
#define HEART_RATE_PIN 34

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
    Serial.begin(115200);

    dht.begin();

    Serial.println("Patient Health Monitoring System");
    Serial.println("DHT22 temperature sensor initialized");
    Serial.println("Heart rate simulation initialized");
}

void loop() {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    int sensorValue = analogRead(HEART_RATE_PIN);

    // Convert potentiometer position to simulated heart rate
    int heartRate = map(sensorValue, 0, 4095, 50, 130);

    String heartRateStatus;

    if (heartRate < 60) {
        heartRateStatus = "LOW HEART RATE";
    } else if (heartRate <= 100) {
        heartRateStatus = "NORMAL";
    } else {
        heartRateStatus = "HIGH HEART RATE";
    }

    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Failed to read from DHT22");
    } else {
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.print(" °C | Humidity: ");
        Serial.print(humidity);
        Serial.print(" % | Heart Rate: ");
        Serial.print(heartRate);
        Serial.print(" BPM | Status: ");
        Serial.println(heartRateStatus);
    }

    delay(2000);
}