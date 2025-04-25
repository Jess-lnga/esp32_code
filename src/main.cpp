#include "wifi_protocol.h"

int i = 0;
void setup() 
{
  Serial.begin(115200);
  connect_to_wifi();
  connect_to_bridge(); 
  send_msg_to_bridge("esp32 ready!", true);
}

void loop() 
{
  reconnect_to_wifi();
  reconnect_to_bridge();

  /// Code pour tester la communication du système {e-puck + bridge + esp32}
  if(i==0)
  {
    send_msg_to_epuck("avance");
    wait_msg_treated();
  }else if(i==1)
  {
    send_msg_to_epuck("tourne a gauche");
    wait_msg_treated();

  }else if(i==2)
  {
    send_msg_to_epuck("recule");
    wait_msg_treated();

  }else if(i==3)
  {
    send_msg_to_epuck("tourne a droite");
    wait_msg_treated();
    i = 0;
  }
  delay(1000);
  
  
  send_msg_to_epuck("stop");
  wait_msg_treated();
  ++i;

  delay(1000);
}
