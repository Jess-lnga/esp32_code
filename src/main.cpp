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

#include "BluetoothSerial.h"

// Créer un objet Bluetooth
BluetoothSerial SerialBT;

void setup() {
  // Démarrer le port série classique pour voir les logs dans le moniteur série
  Serial.begin(115200);
  Serial.println("Démarrage Bluetooth...");

  // Démarrer Bluetooth avec un nom visible
  if (!SerialBT.begin("ESP32_BT_Test")) { // Tu peux changer "ESP32_BT_Test"
    Serial.println("Erreur de démarrage Bluetooth");
    while (true); // Bloque ici si erreur
  }
  Serial.println("Bluetooth prêt. Connecte-toi avec ton téléphone !");
}

void loop() {
  // Si on reçoit quelque chose via Bluetooth
  if (SerialBT.available()) {
    String incoming = SerialBT.readStringUntil('\n'); // Lecture jusqu'au retour à la ligne
    Serial.print("Reçu via Bluetooth : ");
    Serial.println(incoming);

    // Echo : on renvoie ce qu'on reçoit
    SerialBT.println("J'ai bien reçu ton message");
  }

  // Tu peux aussi envoyer des trucs périodiquement
  // SerialBT.println("Ping depuis ESP32 !");
  
  delay(100);
}










