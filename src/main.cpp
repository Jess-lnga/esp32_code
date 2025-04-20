#include "BluetoothSerial.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"

// Création de l'objet Bluetooth
BluetoothSerial SerialBT;

// Fonction appelée à chaque événement Bluetooth trouvé
void btCallback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param) {
  if (event == ESP_BT_GAP_DISC_RES_EVT) {
    char bda_str[18];
    snprintf(bda_str, sizeof(bda_str),
             "%02x:%02x:%02x:%02x:%02x:%02x",
             param->disc_res.bda[0], param->disc_res.bda[1], param->disc_res.bda[2],
             param->disc_res.bda[3], param->disc_res.bda[4], param->disc_res.bda[5]);
    
    Serial.print("Appareil trouvé : ");
    Serial.println(bda_str);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Bluetooth Scanner - Recherche des périphériques...");

  if (!SerialBT.begin("ESP32_Scanner", true)) {  // true = mode client
    Serial.println("Erreur de démarrage du Bluetooth");
    while (1);
  }
  
  // Initialiser l'événement de scan
  esp_bt_gap_register_callback(btCallback);

  // Commencer la découverte
  esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 10, 0);
  // 10 secondes de scan, mode "General Inquiry"
}

void loop() {
  // On ne fait rien dans la boucle, tout se passe dans la fonction callback
}
