/*
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

void setup() {
  Serial.begin(115200);

  Serial.println("Point d'accès démarré");
  Serial.print("Adresse IP : ");

  Serial.println("Serveur HTTP démarré");
}

void loop() 
{

  Serial.println("Hello from ESP32!");
}
*/
/*
#include <WiFi.h>  // Inclure la bibliothèque WiFi de l'ESP32

// Remplacer par TON SSID et TON mot de passe !
const char* ssid = "Redmi";
const char* password = "Ljhki23132";

void setup() {
  Serial.begin(115200); // Démarrer la communication série à 115200 bauds
  delay(1000);

  Serial.println();
  Serial.println("Connexion au Wi-Fi...");
  WiFi.disconnect(true);  // Déconnecte avant de commencer
  delay(1000);
  WiFi.begin(ssid, password);

  //WiFi.begin(ssid, password); // Démarrer la connexion

  // Tant que l'ESP32 n'est pas connectée, afficher des points
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    Serial.println(WiFi.status()); // <-- pour afficher le code d'erreur
  }

  // Une fois connecté
  Serial.println("");
  Serial.println("✅ Connecté au Wi-Fi !");
  Serial.print("Adresse IP : ");
  Serial.println(WiFi.localIP()); // Afficher l'adresse IP obtenue
}

void loop() {
  // Ici tu peux ajouter ce que tu veux (par ex : communication avec ton app mobile)
}
*/

#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Ping.h>  // <--- Ajoute cette librairie

const char* ssid = "Redmi";  
const char* password = "Ljhki23132"; 

WebServer server(80);

void handlePost() {
  if (server.hasArg("message")) {
    String message = server.arg("message");
    Serial.println("Message reçu: " + message);
    server.send(200, "text/plain", "ESP32: message reçu");
  } else {
    server.send(400, "text/plain", "Pas de message reçu");
  }
}

void setup() {
  Serial.begin(115200);

  while (!Serial) 
  {
    delay(100);
  }

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);

  Serial.println("Scan des réseaux Wi-Fi...");
  
  int n = WiFi.scanNetworks(); // Scanner les réseaux disponibles
  
  if (n == 0) {
    Serial.println("Aucun réseau trouvé.");
  } else {
    Serial.print(n);
    Serial.println(" réseaux trouvés :");
    
    // Afficher les SSID des réseaux trouvés
    for (int i = 0; i < n; ++i) 
    {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));  // Affiche le SSID du réseau
      Serial.print(" - Signal: ");
      Serial.print(WiFi.RSSI(i));  // Affiche la puissance du signal
      Serial.println(" dBm");
      delay(10);
    }
  }
  WiFi.disconnect();

  Serial.println("Connexion au réseau Wi-Fi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Tentative de connexion...");
  }

  Serial.println("Connecté !");
  Serial.print("Adresse IP: ");
  Serial.println(WiFi.localIP());

  // TEST PING ici :
  Serial.println("Ping de 8.8.8.8...");
  if (Ping.ping("8.8.8.8")) {
    Serial.println("Ping réussi !");
  } else {
    Serial.println("Échec du ping !");
  }

  server.on("/send", HTTP_POST, handlePost);
  server.begin();
}

void loop() {
  server.handleClient(); 
}



