#ifndef WIFI_CD
#define WIFI_CD

#include <WiFi.h>
#include <WebServer.h>

void setup_comm();

void HandleClient();

bool get_request_update();
void request_treated();

#endif