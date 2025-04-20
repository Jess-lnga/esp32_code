#include "BluetoothSerial.h"

// Création de l'objet Bluetooth Serial
BluetoothSerial SerialBT;

String ePuckAddress = "24:0A:C4:82:5F:9A"; // Adresse MAC de ton e-puck

void connectToEPuck() 
{
  Serial.println("Tentative de connexion à l'e-puck...");

  while (!SerialBT.connect(ePuckAddress)) 
  {
    Serial.println("Connexion échouée, nouvel essai dans 1 seconde...");
    delay(1000);
  }

  Serial.println("Connexion à l'e-puck réussie !");
}

void setup() 
{
  Serial.begin(115200);
  Serial.println("ESP32 Bluetooth Client - Démarrage...");

  connectToEPuck();
}

void loop() {

  if (!SerialBT.connected()) 
  {
    Serial.println("Connexion perdue !");
    SerialBT.disconnect(); 
    connectToEPuck();
  }

  // Si l'utilisateur écrit quelque chose dans le Serial Monitor, l'envoyer à l'e-puck
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    SerialBT.println(input);
    Serial.print("Envoyé au e-puck : ");
    Serial.println(input);
  }

  // Si le e-puck envoie un message, l'afficher sur le Serial Monitor
  if (SerialBT.available()) {
    String incoming = SerialBT.readStringUntil('\n');
    Serial.print("Reçu du e-puck : ");
    Serial.println(incoming);
  }

  delay(10); // Petit delay pour éviter de saturer le processeur
}


