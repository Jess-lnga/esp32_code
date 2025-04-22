#include <WiFi.h>
#include "wifi_protocol.h"

void setup() {
  Serial.begin(115200);
  connectToWiFi();
}

void loop() {
  if (!client.connected()) {
    connectToServer();
    if (client.connected()) {
      client.println("Hello from ESP32");
      Serial.println("Message envoyé : Hello from ESP32");
    }
  }
  
  // Ici tu pourrais ajouter d'autres traitements si besoin

  delay(2000); // Juste pour éviter de saturer le CPU
}
