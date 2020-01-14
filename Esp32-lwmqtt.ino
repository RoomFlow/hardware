/******************************************************************************
 * Copyright 2018 Google
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
#include "esp32-mqtt.h"

#define PIR_PIN 23
#define LED_DETECTED 22
#define LED_READY 21

// variable for Motion Detected
bool motionDetected = false;
// variable to store value from PIR
int pirValue;

unsigned long nextMotionMillis = millis() + 60000;
bool start = true;

unsigned long cur = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  setupCloudIoT();

  // initialize PIR sensor as an input
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_READY, OUTPUT);
  pinMode(LED_DETECTED, OUTPUT);

  digitalWrite(LED_READY, LOW);
  digitalWrite(LED_DETECTED, LOW);

  Serial.println("Acclimatizing motion sensor.");
}

unsigned long lastMillis = 0;
void loop() {
  mqttClient->loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!mqttClient->connected()) {
    connect();
  }

  cur = millis();

  if (cur >= nextMotionMillis) {
    if (start) {
      start = false;
      Serial.println("Motion sensor ready.");
      digitalWrite(LED_READY, HIGH);
    }

    if (motionDetected) {
      digitalWrite(LED_DETECTED, LOW);
      digitalWrite(LED_READY, HIGH);
      motionDetected = false;
    }

    pirValue = digitalRead(PIR_PIN);   // read PIR sensor value

    // If motion is detected
    if (pirValue == HIGH) {
      digitalWrite(LED_DETECTED, HIGH);
      digitalWrite(LED_READY, LOW);

      motionDetected = true;

      publishTelemetry("{\"room\":\"" + String(room) + "\",\"content\":{\"motion\":true,\"timestamp\":" + time(nullptr) + "}}");

      nextMotionMillis = cur + 6000;
 
      Serial.println("Motion detected");
    } else {
      digitalWrite(LED_DETECTED, LOW);
    }
  }
}
