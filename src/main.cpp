#include <WiFi.h>

// Remplacer par le nom et le mot de passe de ton Wi-Fi
const char* ssid = "Redmi";
const char* password = "Ljhki23132";

// Adresse IP de ton ordinateur (trouvée avec ipconfig)
const char* server_ip = "192.168.112.57";
const uint16_t server_port = 2345;  // Port choisi côté bridge.py

WiFiClient client;

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

void connectToServer() {
  while (!client.connected()) {
    Serial.print("Tentative de connexion au serveur... ");
    if (client.connect(server_ip, server_port)) {
      Serial.println("Connecté au serveur !");
    } else {
      Serial.println("Échec, nouvelle tentative dans 1s...");
      delay(1000);
    }
  }
}

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
