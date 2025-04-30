#include "sound_code.h"

#include <math.h>
#include <stdlib.h>

#include <stdint.h>

// Filtre passe-bande simple (RC basique)

class SimpleBandpassFilter {
private:
    float lowPass;
    float highPass;
    float alphaLP;
    float alphaHP;

public:
    SimpleBandpassFilter(float fLow, float fHigh, uint32_t sampleRate) {
        float dt = 1.0f / sampleRate;
        alphaLP = dt / (dt + 1.0f / (2.0f * M_PI * fHigh));
        alphaHP = 1.0f / (1.0f + (1.0f / (2.0f * M_PI * fLow * dt)));
        lowPass = 0;
        highPass = 0;
    }

    int16_t process(int16_t input) {
        // Passe-haut
        float hp = alphaHP * (highPass + input - lowPass);
        highPass = input;
        // Passe-bas
        lowPass += alphaLP * (hp - lowPass);
        return (int16_t)lowPass;
    }
};


// --- CONFIG I2S ---
#define I2S_MIC_SERIAL_CLOCK   25  // BCLK
#define I2S_MIC_LEFT_RIGHT_CLOCK 22  // WS/LRC
#define I2S_MIC_SERIAL_DATA    34  // DO

#define I2S_SPEAKER_SERIAL_CLOCK  32  // BCLK (différente)
#define I2S_SPEAKER_LEFT_RIGHT_CLOCK 33  // WS/LRC (différente)
#define I2S_SPEAKER_SERIAL_DATA 26  // DIN

#define I2S_PORT_MIC I2S_NUM_0
#define I2S_PORT_SPEAKER I2S_NUM_1

// --- AUDIO BUFFER ---
#define SAMPLE_RATE     16000  // 16kHz
#define SAMPLE_BITS     16     // 16 bits réels utilisés
#define RECORD_TIME_SEC 3      
#define BUFFER_SIZE (SAMPLE_RATE * RECORD_TIME_SEC)

int16_t* audioBuffer = nullptr;

// --- SETUP I2S ---
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

    i2s_driver_install(I2S_PORT_MIC, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_PORT_MIC, &pin_config);
}

void setupI2SSpeaker() {
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

    i2s_driver_install(I2S_PORT_SPEAKER, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_PORT_SPEAKER, &pin_config);
}

// --- RECORD AUDIO ---
void recordAudio() {
    Serial.println("Enregistrement en cours...");
    i2s_zero_dma_buffer(I2S_PORT_MIC);

    size_t bytesRead = 0;
    int32_t sample = 0;
    int16_t sample16 = 0;

    uint32_t totalSamples = SAMPLE_RATE * RECORD_TIME_SEC;

    for (uint32_t i = 0; i < totalSamples; i++) {
        i2s_read(I2S_PORT_MIC, &sample, sizeof(sample), &bytesRead, portMAX_DELAY);

        // Conversion propre : de 32 bits (24 bits utiles) -> 16 bits
        sample16 = (int16_t)(sample >> 11); // Shifter de 11 bits ≈ gain correct pour éviter saturation

        // Filtrage simple pour enlever une partie du bruit (filtre passe-haut très basique)
        if (abs(sample16) < 150) {
            sample16 = 0; // Supprime les très faibles bruits parasites
        }

        audioBuffer[i] = sample16;
    }

    Serial.println("Enregistrement terminé !");
}


void recordAudio2() {
    Serial.println("Enregistrement avec filtrage voix...");
    i2s_zero_dma_buffer(I2S_PORT_MIC);

    size_t bytesRead = 0;
    int32_t sample = 0;
    int16_t sample16 = 0;

    uint32_t totalSamples = SAMPLE_RATE * RECORD_TIME_SEC;

    // On initialise un filtre pour la voix humaine
    SimpleBandpassFilter filter(300, 3400, SAMPLE_RATE);

    for (uint32_t i = 0; i < totalSamples; i++) {
        i2s_read(I2S_PORT_MIC, &sample, sizeof(sample), &bytesRead, portMAX_DELAY);

        // Conversion propre : mieux que juste un shift
        sample16 = (int16_t)(sample >> 8); // Pas d'overboost
        sample16 = constrain(sample16, -32768, 32767); 

        // Appliquer filtre passe-bande simple
        sample16 = filter.process(sample16);

        audioBuffer[i] = sample16;
    }

    Serial.println("Enregistrement terminé !");
}

void recordAudio3() {
  Serial.println("Enregistrement RAW...");

  i2s_zero_dma_buffer(I2S_PORT_MIC);

  size_t bytesRead = 0;
  uint8_t i2sData[4]; // 32 bits (4 octets) par échantillon
  int32_t sample = 0;
  int16_t sample16 = 0;

  uint32_t totalSamples = SAMPLE_RATE * RECORD_TIME_SEC;

  for (uint32_t i = 0; i < totalSamples; i++) {
      i2s_read(I2S_PORT_MIC, &i2sData, sizeof(i2sData), &bytesRead, portMAX_DELAY);

      // Reconstruction du sample
      sample = ((int32_t)i2sData[0]) | ((int32_t)i2sData[1] << 8) | ((int32_t)i2sData[2] << 16);

      // Important: pas de simple >>8 qui détruit la dynamique
      sample16 = (int16_t)(sample >> 8); 

      audioBuffer[i] = sample16;
  }

  Serial.println("Enregistrement terminé !");
}


// --- PLAY AUDIO ---
void playAudio() {
    Serial.println("Lecture audio...");
    for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
        size_t bytesWritten;
        int16_t sampleToPlay = audioBuffer[i];

        // Option : amplifier un peu si besoin (décommenter si trop faible)
        /*
        sampleToPlay *= 2;
        if (sampleToPlay > 32767) sampleToPlay = 32767;
        if (sampleToPlay < -32768) sampleToPlay = -32768;
        */

        i2s_write(I2S_PORT_SPEAKER, &sampleToPlay, sizeof(sampleToPlay), &bytesWritten, portMAX_DELAY);
    }
    delay(2000); // Petite pause pour éviter de relancer direct
    Serial.println("Lecture terminée !");
}

void playAudio2() {
  Serial.println("Lecture RAW...");

  for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
      size_t bytesWritten;
      i2s_write(I2S_PORT_SPEAKER, &audioBuffer[i], sizeof(int16_t), &bytesWritten, portMAX_DELAY);
  }

  delay(500); // Petite pause après la lecture
}


void setup_sound() {
    audioBuffer = (int16_t*)malloc(BUFFER_SIZE * sizeof(int16_t));
    if (audioBuffer == nullptr) {
        Serial.println("Erreur allocation mémoire !");
        while (true);
    }

    setupI2SMic();
    setupI2SSpeaker();
}

void playFreq(uint16_t freq, uint32_t durationMs) {
    const uint32_t totalSamples = (SAMPLE_RATE * durationMs) / 1000;
    const uint16_t periodSamples = SAMPLE_RATE / freq;
    int16_t* toneBuf = (int16_t*)malloc(periodSamples * sizeof(int16_t));
    if (!toneBuf) return;

    const int16_t amplitude = 2000;

    for (uint16_t i = 0; i < periodSamples; i++) {
        float angle = 2.0f * M_PI * i / float(periodSamples);
        toneBuf[i] = int16_t(amplitude * sinf(angle));
    }

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
