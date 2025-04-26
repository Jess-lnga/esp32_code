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

  if(i==0)
  {
    send_msg_to_epuck("avance");
    wait_msg_treated();
  }else if(i==1)
  {
    send_msg_to_epuck("gauche");
    wait_msg_treated();

  }else if(i==2)
  {
    send_msg_to_epuck("recule");
    wait_msg_treated();

  }else if(i==3)
  {
    send_msg_to_epuck("droite");
    wait_msg_treated();
  }

  delay(1000);
  
  send_msg_to_epuck("stop");
  wait_msg_treated();

  if(i==3)
  {
    i=0;
  }else{
    ++i;
  }
  delay(1000);
}
