#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include <driver/i2s.h>

const char* ssid = "TON_SSID_WIFI";
const char* password = "TON_MOT_DE_PASSE";

WebServer server(80);

#define I2S_NUM         I2S_NUM_0  // Utiliser l'I2S numéro 0
#define I2S_BCK_PIN     25         // Bit Clock -> BCLK
#define I2S_WS_PIN      22         // Word Select -> LRC
#define I2S_DATA_PIN    26         // Data -> DIN

void initI2S() 
{
  // Configuration de l'I2S en mode Master TX
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = 16000,  // 16kHz adapté au fichier (à ajuster si besoin)
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = true
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCK_PIN,
    .ws_io_num = I2S_WS_PIN,
    .data_out_num = I2S_DATA_PIN,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM, &pin_config);
}

void handleUploadAudio() 
{
  HTTPUpload& upload = server.upload();
  static File file;

  if (upload.status == UPLOAD_FILE_START) {
    Serial.printf("Upload start: %s\n", upload.filename.c_str());
    String path = "/audio_received.wav";
    file = SPIFFS.open(path, FILE_WRITE);
    if (!file) {
      Serial.println("Erreur ouverture fichier !");
      return;
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (file) {
      file.write(upload.buf, upload.currentSize);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (file) {
      file.close();
      Serial.println("Upload terminé.");
      server.send(200, "text/plain", "Upload réussi !");
    } else {
      server.send(500, "text/plain", "Erreur serveur.");
    }
  }
}

void setup() 
{
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connexion au WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnecté!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  if (!SPIFFS.begin(true)) {
    Serial.println("Erreur SPIFFS...");
    return;
  }

  server.on("/upload", HTTP_POST, []() { server.send(200); }, handleUploadAudio);
  server.begin();
  Serial.println("Serveur lancé !");

  initI2S(); // Initialisation I2S
}

void loop() {
  server.handleClient();
  playAudioLoop();
}

void playAudioLoop() 
{
  static unsigned long lastPlayed = 0;
  unsigned long currentMillis = millis();

  if (currentMillis - lastPlayed >= 5000) { // Attente de 5 secondes
    lastPlayed = currentMillis;
    File audioFile = SPIFFS.open("/audio_received.wav", "r");
    
    if (!audioFile) {
      Serial.println("Pas de fichier audio à jouer !");
      return;
    }

    Serial.println("Lecture du fichier audio...");
    uint8_t buffer[512];
    size_t bytesRead;
    size_t bytesWritten;

    // Sauter l'entête WAV (44 octets typiques)
    audioFile.seek(44, SeekSet);

    while ((bytesRead = audioFile.read(buffer, sizeof(buffer))) > 0) {
      i2s_write(I2S_NUM, buffer, bytesRead, &bytesWritten, portMAX_DELAY);
    }

    audioFile.close();
    Serial.println("Lecture terminée.");
  }
}
