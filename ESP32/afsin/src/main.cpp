#include <Arduino.h>

#include <esp_now.h>
#include <WiFi.h>

#define LED_PIN 4  // ESP32-CAM üzerinde LED flaş genellikle GPIO 4

uint8_t masterAddress[] = {0xAA, 0xBB, 0xCC, 0x11, 0x22, 0x33}; // MASTER'ın MAC adresi BURAYA yazılacak!

void setup() {
  pinMode(LED_PIN, OUTPUT);
  WiFi.mode(WIFI_STA);
  
  if (esp_now_init() != ESP_OK) {
    // Hata durumu
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, masterAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  esp_now_add_peer(&peerInfo);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  const char *msg = "LED_ON";
  esp_now_send(masterAddress, (uint8_t *)msg, strlen(msg));
  delay(3000);

  digitalWrite(LED_PIN, LOW);
  msg = "LED_OFF";
  esp_now_send(masterAddress, (uint8_t *)msg, strlen(msg));
  delay(3000);
}
