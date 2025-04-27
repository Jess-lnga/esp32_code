#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <driver/i2s.h>

// --- CONFIG Wi-Fi ---
const char* ssid = "Redmi";
const char* password = "Ljhki23132";

// --- CONFIG SERVER ---
AsyncWebServer server(80);

// --- CONFIG I2S ---
#define I2S_MIC_SERIAL_CLOCK   25  // BCLK
#define I2S_MIC_LEFT_RIGHT_CLOCK 22  // WS/LRC
#define I2S_MIC_SERIAL_DATA    34  // DO

#define I2S_SPEAKER_SERIAL_CLOCK  25  // BCLK (partagé)
#define I2S_SPEAKER_LEFT_RIGHT_CLOCK 22  // WS/LRC (partagé)
#define I2S_SPEAKER_SERIAL_DATA 26  // DIN

#define I2S_PORT I2S_NUM_0

// --- AUDIO BUFFER ---
#define SAMPLE_RATE     16000  // 16kHz
#define SAMPLE_BITS     16     // 16bits
#define RECORD_TIME_SEC 3      // <<< Réduit de 10s à 3s
#define BUFFER_SIZE (SAMPLE_RATE * RECORD_TIME_SEC)

int16_t* audioBuffer = nullptr;
volatile bool newRecordRequested = false;

// --- SETUP Wi-Fi ---
void setupWiFi() 
{
    Serial.print("Trying to connect to Wifi");
    WiFi.begin(ssid, password);
  
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.println(".");
    }

    Serial.print("Connected to Wifi!");
    Serial.print("IP ESP32 : ");
    Serial.println(WiFi.localIP());
}

// --- SETUP I2S ---
void setupI2SMic() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = (i2s_bits_per_sample_t)SAMPLE_BITS,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S, // <<< corrige deprecated
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_MIC_SERIAL_CLOCK,
    .ws_io_num = I2S_MIC_LEFT_RIGHT_CLOCK,
    .data_out_num = -1,
    .data_in_num = I2S_MIC_SERIAL_DATA
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_PORT, &pin_config);
}

void setupI2SSpeaker() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = (i2s_bits_per_sample_t)SAMPLE_BITS,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S, // <<< corrige deprecated
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SPEAKER_SERIAL_CLOCK,
    .ws_io_num = I2S_SPEAKER_LEFT_RIGHT_CLOCK,
    .data_out_num = I2S_SPEAKER_SERIAL_DATA,
    .data_in_num = -1
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_PORT, &pin_config);
}

// --- SERVER HANDLER ---
void setupServer() {
  server.on("/record", HTTP_GET, [](AsyncWebServerRequest *request){
    newRecordRequested = true;
    request->send(200, "text/plain", "Recording triggered!");
  });
  server.begin();
}

// --- RECORD AUDIO ---
void recordAudio() {
  Serial.println("Recording...");
  i2s_zero_dma_buffer(I2S_PORT);

  size_t bytesRead = 0;
  int32_t sample = 0;
  int16_t sample16 = 0;

  // Switch I2S mode to RX
  setupI2SMic();

  uint32_t totalSamples = SAMPLE_RATE * RECORD_TIME_SEC;
  for (uint32_t i = 0; i < totalSamples; i++) {
    i2s_read(I2S_PORT, &sample, sizeof(sample), &bytesRead, portMAX_DELAY);
    sample16 = sample >> 8;
    audioBuffer[i] = sample16;
  }
  Serial.println("Recording done!");

  // Switch I2S mode back to TX
  setupI2SSpeaker();
}

// --- PLAY AUDIO LOOP ---
void playAudioLoop() {
  while (!newRecordRequested) {
    Serial.println("Playing audio...");
    for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
      size_t bytesWritten;
      i2s_write(I2S_PORT, &audioBuffer[i], sizeof(audioBuffer[i]), &bytesWritten, portMAX_DELAY);
    }
    Serial.println("Waiting 2 seconds...");
    delay(2000);
  }
}

void setup() {
  Serial.begin(115200);

  // Allocation de buffer
  audioBuffer = (int16_t*)malloc(BUFFER_SIZE * sizeof(int16_t));
  if (audioBuffer == nullptr) {
    Serial.println("Erreur allocation mémoire !");
    while (true);
  }

  setupWiFi();
  setupServer();
  setupI2SMic();
}

void loop() {
  if (newRecordRequested) {
    newRecordRequested = false;
    recordAudio();
  }

  if (audioBuffer[0] != 0) {
    playAudioLoop();
  }

  delay(100);
}
