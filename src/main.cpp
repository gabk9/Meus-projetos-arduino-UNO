#include <Arduino.h>

void setup(void) {
    Serial.begin(9600);
}

void loop(void) {
    Serial.println("Hello, World");
    delay(1000);
}