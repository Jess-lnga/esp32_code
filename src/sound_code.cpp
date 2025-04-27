#include "sound_code.h"

#include <math.h>    
#include <stdlib.h>  

// --- CONFIG I2S ---
// Microphone I2S pins
#define I2S_MIC_SERIAL_CLOCK   25  // BCLK
#define I2S_MIC_LEFT_RIGHT_CLOCK 22  // WS/LRC
#define I2S_MIC_SERIAL_DATA    34  // DO

// Speaker I2S pins (utilise des broches différentes)
#define I2S_SPEAKER_SERIAL_CLOCK  32  // BCLK (différente)
#define I2S_SPEAKER_LEFT_RIGHT_CLOCK 33  // WS/LRC (différente)
#define I2S_SPEAKER_SERIAL_DATA 26  // DIN

#define I2S_PORT_MIC I2S_NUM_0
#define I2S_PORT_SPEAKER I2S_NUM_1

// --- AUDIO BUFFER ---
#define SAMPLE_RATE     16000  // 16kHz
#define SAMPLE_BITS     16     // 16bits
#define RECORD_TIME_SEC 3      // Réduit de 10s à 3s
#define BUFFER_SIZE (SAMPLE_RATE * RECORD_TIME_SEC)

int16_t* audioBuffer = nullptr;


// --- SETUP I2S ---
// Configuration du microphone
void setupI2SMic() {
    i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = (i2s_bits_per_sample_t)SAMPLE_BITS,
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
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
  
    // Installation du driver pour I2S_MIC
    i2s_driver_install(I2S_PORT_MIC, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_PORT_MIC, &pin_config);
}


// Configuration du haut-parleur
void setupI2SSpeaker() 
{
    i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = (i2s_bits_per_sample_t)SAMPLE_BITS,
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
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
  
    // Installation du driver pour I2S_SPEAKER
    i2s_driver_install(I2S_PORT_SPEAKER, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_PORT_SPEAKER, &pin_config);
}


// --- RECORD AUDIO ---
void recordAudio() 
{
  Serial.println("Enregistrement en cours");
  i2s_zero_dma_buffer(I2S_PORT_MIC);

  size_t bytesRead = 0;
  int32_t sample = 0;
  int16_t sample16 = 0;

  uint32_t totalSamples = SAMPLE_RATE * RECORD_TIME_SEC;
  for (uint32_t i = 0; i < totalSamples; i++) 
  {
    
    i2s_read(I2S_PORT_MIC, &sample, sizeof(sample), &bytesRead, portMAX_DELAY);
    sample16 = sample >> 8;
    //Serial.println(sample16);
    audioBuffer[i] = sample16;
  }
}

// --- PLAY AUDIO ---
void playAudio() 
{
  for (uint32_t i = 0; i < BUFFER_SIZE; i++) 
  {
    size_t bytesWritten;
    i2s_write(I2S_PORT_SPEAKER, &audioBuffer[i], sizeof(audioBuffer[i]), &bytesWritten, portMAX_DELAY);
  }

  delay(2000);
}

void setup_sound()
{
  audioBuffer = (int16_t*)malloc(BUFFER_SIZE * sizeof(int16_t));
  if (audioBuffer == nullptr) 
  {
    Serial.println("Erreur allocation mémoire !");
    while (true);
  }

  setupI2SMic();
  setupI2SSpeaker();
}

void playFreq(uint16_t freq, uint32_t durationMs) 
{
  // Nombre total d'échantillons à jouer
  const uint32_t totalSamples = (SAMPLE_RATE * durationMs) / 1000;
  // Taille d'un cycle complet de la sinusoïde
  const uint16_t periodSamples = SAMPLE_RATE / freq;
  // Tampon pour une période
  int16_t* toneBuf = (int16_t*) malloc(periodSamples * sizeof(int16_t));
  if (!toneBuf) return;

  // Amplitude max (à ajuster si nécessaire)
  const int16_t amplitude = 2000;

  // Génération d'une période de sinusoïde
  for (uint16_t i = 0; i < periodSamples; i++) {
    float angle = 2.0f * M_PI * i / float(periodSamples);
    toneBuf[i] = int16_t(amplitude * sinf(angle));
  }

  // Écriture répétée jusqu'à couvrir totalSamples
  uint32_t played = 0;
  while (played < totalSamples) {
    uint16_t chunk = (periodSamples < (totalSamples - played)) ? periodSamples : (totalSamples - played);
    size_t bytesWritten = 0;
    i2s_write(I2S_PORT_SPEAKER,
              toneBuf,
              chunk * sizeof(int16_t),
              &bytesWritten,
              portMAX_DELAY);
    played += chunk;
  }

  free(toneBuf);
}
