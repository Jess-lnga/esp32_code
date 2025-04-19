#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// Définir la durée du scan (en secondes)
#define SCAN_TIME 5

BLEScan* pBLEScan;

// Callback pour chaque périphérique détecté
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("Nom : ");
    Serial.print(advertisedDevice.getName().c_str());
    Serial.print(" - Adresse MAC : ");
    Serial.println(advertisedDevice.getAddress().toString().c_str());
  }
};

void setup() {
  // Initialisation de la connexion série
  Serial.begin(115200);
  Serial.println("Démarrage du scan BLE...");

  // Initialisation du périphérique BLE
  BLEDevice::init("");

  // Créer une instance de BLEScan
  pBLEScan = BLEDevice::getScan(); // Créer un scanner BLE

  // Paramétrer le scan
  pBLEScan->setActiveScan(true);  // Active le scan actif (reçoit plus de données)
  pBLEScan->setInterval(100);     // Intervalle entre deux scans
  pBLEScan->setWindow(99);        // Fenêtre de scan

  // Enregistrer la classe callback
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
}

void loop() {
  // Démarrer le scan pendant une période définie
  Serial.println("Scan des périphériques Bluetooth...");
  BLEScanResults foundDevices = pBLEScan->start(SCAN_TIME, false);

  // Afficher le nombre de périphériques trouvés
  Serial.print("Périphériques trouvés : ");
  Serial.println(foundDevices.getCount());

  // Redémarrer le scan après une courte pause
  delay(2000);  // Attendre 2 secondes avant de lancer un nouveau scan
}
