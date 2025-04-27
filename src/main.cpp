
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
  HandleClient();

  if (get_request_update()) 
  {
    Serial.println("Nouvel enregistrement demandé!");
    request_treated();
    Serial.println("Enregistrement en cours");
    recordAudio();

    Serial.println("Enregistrement terminé");
  }

  playAudio();
  delay(100);  
}
