#include <Arduino.h>

void setup() {
    Serial.begin(115200);
    Serial.println("Patient Health Monitoring System");
    Serial.println("ESP32 is running!");
}

void loop() {
    delay(1000);
    Serial.println("System monitoring...");
}