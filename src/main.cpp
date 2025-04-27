
#include "wifi_code.h"
#include "sound_code.h"

void setup() 
{
  Serial.begin(115200);
  
  setup_sound();
  setup_comm();
}

void loop() 
{
/* 
  // joue 1000 Hz pendant 1 seconde
  playFreq(1000, 1000);
  delay(500);  

  playFreq(500, 1000); // 500 Hz pendant 1 s
  delay(500);
*/


  HandleClient();

  if (get_request_update()) 
  {
    Serial.println("Nouvel enregistrement demandé!");
    request_treated();
    recordAudio();
    Serial.println("Enregistrement terminé");
  }
  

  playAudio();
  delay(100);  
  
}
