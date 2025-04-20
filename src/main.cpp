#include "BluetoothSerial.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"

BluetoothSerial SerialBT;

// Adresse MAC du e-puck (à personnaliser si besoin)
uint8_t epuckAddress[6] = { 0x24, 0x0A, 0xC4, 0x82, 0x5F, 0x9A };

bool isConnecting = false;
bool isConnected = false;

// Fonction appelée lors d'une demande de PIN
void bt_gap_callback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param) 
{
    if (event == ESP_BT_GAP_PIN_REQ_EVT) 
    {
        Serial.println("[Bluetooth] Demande de PIN reçue, réponse automatique...");
        esp_bt_pin_code_t pin_code;
        strcpy((char *)pin_code, "1234");  // PIN automatique
        esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
    }
}

void setup() 
{
    Serial.begin(115200);
    delay(1000);

    Serial.println("=== Initialisation Bluetooth ===");

    // Enregistre la fonction de callback pour gérer les PIN
    esp_bt_gap_register_callback(bt_gap_callback);

    if (!SerialBT.begin("ESP32_Client", true)) // true -> mode client
    { 
        Serial.println("[Erreur] Impossible de démarrer le Bluetooth !");
        while (true);
    }
    Serial.println("[OK] Bluetooth démarré en mode client");

    delay(1000);

    Serial.print("[Info] Tentative de connexion au e-puck : ");
    for (int i = 0; i < 6; i++) 
    {
        Serial.printf("%02X", epuckAddress[i]);
        if (i < 5) Serial.print(":");
    }
    Serial.println();

    // Lancement de la connexion
    isConnecting = true;
    if (SerialBT.connect(epuckAddress)) 
    {
        Serial.println("[Succès] Connecté au e-puck !");
        isConnected = true;

    }else{
        Serial.println("[Erreur] Échec de connexion au e-puck !");
        isConnected = false;
    }
}

void loop() 
{
    if (isConnected) 
    {
        Serial.println("[Info] Envoi d'un message test au e-puck...");
        SerialBT.println("Hello e-puck !");
        delay(2000);

    }else if (isConnecting){
        Serial.println("[Warning] Pas encore connecté, tentative de reconnexion...");
        delay(5000);
        if (SerialBT.connect(epuckAddress)) 
        {
            Serial.println("[Succès] Reconnecté !");
            isConnected = true;
        }else{
            Serial.println("[Erreur] Toujours pas connecté...");
        }
    }else{
        delay(1000);
    }
}
