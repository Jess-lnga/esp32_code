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
#include <ESP32Ping.h>

const char* ssid = "Redmi";  
const char* password = "Ljhki23132"; 

WebServer server(80);

// Définir les pins pour UART2 (pour communiquer avec la STM32)
#define RXD2 16  // Pin 16 pour RX (Serial2)
#define TXD2 17  // Pin 17 pour TX (Serial2)

void handlePost() {
  if (server.hasArg("message")) {
    String message = server.arg("message");

    Serial.println("Message reçu : " + message);

    Serial2.print(message);  // Envoi du message sur l'UART2
    Serial2.println();       // Optionnel: ajouter un saut de ligne après le message

    // Affichage dans le moniteur série de l'ESP32
    Serial.println("Message envoyé à la STM32 via UART2");

    server.send(200, "text/plain", "ESP32: message reçu et envoyé à la STM32");
  } else {
    server.send(400, "text/plain", "Pas de message reçu");
  }
}

void setup() {

  Serial.begin(115200);  
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2); // UART2 avec GPIO16 (RX) et GPIO17 (TX)

  while (!Serial2);  // Attente de la connexion de l'UART2

  Serial.println("Initialisation de l'ESP32...");

  // Connexion au réseau Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Tentative de connexion Wi-Fi...");
  }

  Serial.println("Connecté au Wi-Fi !");
  Serial.print("Adresse IP: ");
  Serial.println(WiFi.localIP());

  // Test de connexion avec un ping
  Serial.println("Ping de 8.8.8.8...");
  if (Ping.ping("8.8.8.8")) {
    Serial.println("Ping réussi !");
  } else {
    Serial.println("Échec du ping !");
  }

  // Définir la route HTTP pour recevoir les messages
  server.on("/send", HTTP_POST, handlePost);
  server.begin();

  Serial.println("Serveur HTTP démarré.");
}

void loop() {
  server.handleClient();  // Gérer les requêtes HTTP reçues
}





