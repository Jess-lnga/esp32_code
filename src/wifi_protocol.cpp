#include "wifi_protocol.h"


void connectToWiFi() {
    Serial.println("Connexion au WiFi...");
    WiFi.begin(ssid, password);
  
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nConnecté au WiFi !");
    Serial.print("IP ESP32 : ");
    Serial.println(WiFi.localIP());
  }
  
void connectToServer()
{
    while (!client.connected()) 
    {
        Serial.print("Tentative de connexion au serveur... ");
        if (client.connect(server_ip, server_port)) 
        {
            Serial.println("Connecté au serveur !");
        } else {
            Serial.println("Échec, nouvelle tentative dans 1s...");
            delay(1000);
        }
    }
}