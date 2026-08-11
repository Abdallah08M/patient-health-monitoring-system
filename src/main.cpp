#include <DHT.h>

#define DHT_PIN 4
#define DHT_TYPE DHT22
#define HEART_RATE_PIN 34
#define SPO2_PIN 35

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(115200);

  dht.begin();

  Serial.println("Patient Health Monitoring System");
  Serial.println("DHT22 temperature sensor initialized");
  Serial.println("Heart rate simulation initialized");
  Serial.println("SpO2 simulation initialized");
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Read heart-rate potentiometer
  int sensorValue = analogRead(HEART_RATE_PIN);

  // Convert potentiometer position to simulated heart rate
  int heartRate = map(sensorValue, 0, 4095, 50, 130);

  // Determine heart-rate status
  String heartRateStatus;

  if (heartRate < 60) {
    heartRateStatus = "LOW HEART RATE";
  } else if (heartRate <= 100) {
    heartRateStatus = "NORMAL";
  } else {
    heartRateStatus = "HIGH HEART RATE";
  }

  // Read SpO2 potentiometer
  int spo2SensorValue = analogRead(SPO2_PIN);

  // Convert potentiometer position to simulated SpO2
  int spo2 = map(spo2SensorValue, 0, 4095, 85, 100);

  // Determine SpO2 status
  String spo2Status;

  if (spo2 < 90) {
    spo2Status = "LOW SpO2";
  } else if (spo2 < 95) {
    spo2Status = "BORDERLINE SpO2";
  } else {
    spo2Status = "NORMAL SpO2";
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
    Serial.print(heartRateStatus);
    Serial.print(" | SpO2: ");
    Serial.print(spo2);
    Serial.print(" % | Status: ");
    Serial.println(spo2Status);
  }

  delay(2000);
}