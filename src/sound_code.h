#ifndef SND_CD
#define SND_CD

#include <driver/i2s.h>

void setupI2SMic();
void setupI2SSpeaker();
void setup_sound();

void recordAudio();
void playAudio();

#endif