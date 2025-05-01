#include <esp_now.h>
#include <WiFi.h>
#include <Arduino.h>

void onReceive(const uint8_t * mac, const uint8_t *incomingData, int len) {
  //Serial.print("Slave'dan veri geldi: ");
  char msg[len + 1];
  memcpy(msg, incomingData, len);
  msg[len] = '\0';
  Serial.println(msg);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW başlatılamadı");
    return;
  }

  esp_now_register_recv_cb(onReceive);
}

void loop() {
  // Bu cihaz sadece veri alır, loop boş
}
