#include "BluetoothSerial.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"

BluetoothSerial SerialBT;

// Adresse MAC cible (celle de ton e-puck)
String targetAddress = "24:0A:C4:82:5F:9A";
bool connected = false;

void btCallback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param) {
  if (event == ESP_BT_GAP_DISC_RES_EVT) {
    // Construction de l'adresse MAC trouvée
    char bda_str[18];
    snprintf(bda_str, sizeof(bda_str),
             "%02X:%02X:%02X:%02X:%02X:%02X",
             param->disc_res.bda[0], param->disc_res.bda[1], param->disc_res.bda[2],
             param->disc_res.bda[3], param->disc_res.bda[4], param->disc_res.bda[5]);
    
    Serial.print("Appareil trouvé : ");
    Serial.println(bda_str);

    String foundAddress(bda_str);

    // Si c'est l'adresse du e-puck
    if (foundAddress.equalsIgnoreCase(targetAddress) && !connected) {
      Serial.println("e-puck trouvé, tentative de connexion...");
      
      esp_bt_gap_cancel_discovery();  // Arrêter le scan
      delay(1000); // Petit délai pour laisser l'annulation du scan se faire
      
      if (SerialBT.connect(targetAddress)) {
        Serial.println("Connexion réussie au e-puck !");
        connected = true;
      } else {
        Serial.println("Échec de connexion, scan relancé.");
        esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 10, 0);
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Bluetooth Scanner - Connexion automatique au e-puck...");

  if (!SerialBT.begin("ESP32_Scanner", true)) { // true = client
    Serial.println("Erreur de démarrage du Bluetooth");
    while (1);
  }
  
  esp_bt_gap_register_callback(btCallback);

  Serial.println("Démarrage du scan Bluetooth...");
  esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 10, 0);
}

void loop() {
  if (connected) {
    // Ici tu peux envoyer/recevoir des données avec l'e-puck !
    if (Serial.available()) {
      String data = Serial.readStringUntil('\n');
      SerialBT.println(data); // Envoi au e-puck
    }

    if (SerialBT.available()) {
      String dataFromEPuck = SerialBT.readStringUntil('\n');
      Serial.print("Réponse e-puck : ");
      Serial.println(dataFromEPuck);
    }
  }

  delay(20);
}
