#ifndef SND_CD
#define SND_CD

#include <Arduino.h>
#include <driver/i2s.h>

void setupI2SMic();
void setupI2SSpeaker();
void setup_sound();

void recordAudio();
void recordAudio2();
void recordAudio3();

void playAudio();
void playAudio2();
void playFreq(uint16_t freq, uint32_t durationMs);

#endif