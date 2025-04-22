#include <WiFi.h>

// Remplacer par le nom et le mot de passe de ton Wi-Fi
const char* ssid = "Redmi";
const char* password = "Ljhki23132";

const char* server_ip = "192.168.112.57";
const uint16_t server_port = 2345;  // Port choisi côté bridge.py

WiFiClient client;

void connectToWifi();
void connectToServer();
