#ifndef WIFI_PTL_H
#define WIFI_PTL_H

#include <WiFi.h>


void connect_to_wifi();
void connect_to_bridge();

void reconnect_to_wifi();
void reconnect_to_bridge();

void send_msg_to_bridge(String message, bool wait);
void send_msg_to_epuck(String message);
void wait_msg();
void wait_msg_treated();

#endif