#include "wifi_code.h"

bool newRecordRequested = false;

// --- CONFIG Wi-Fi --- //
const char* ssid = "Redmi";
const char* password = "Ljhki23132";

// --- CONFIG SERVER --- //
WebServer server(80);

// --- SETUP Wi-Fi --- //
void setupWiFi() 
{
    Serial.print("Trying to connect to Wifi");
    WiFi.begin(ssid, password);
  
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.println("...");
    }

    Serial.print("Connected to Wifi!");
    Serial.print("IP ESP32 : ");
    Serial.println(WiFi.localIP());
}

// --- SERVER HANDLER --- //
void handlePost() 
{
  if (server.hasArg("message")) 
  {
    String message = server.arg("message");

    Serial.println("Message reçu : " + message);

    if(message == "Record")
    {
      newRecordRequested = true;
    }

    server.send(200, "text/plain", "ESP32: message reçu et envoyé à la STM32");
  } else {
    server.send(400, "text/plain", "Pas de message reçu");
  }
}

// --- SETUP SERVER --- //
void setupServer() 
{
  server.on("/send", HTTP_POST, handlePost);
  server.begin();
}

/// --- SETUP COMMUNICATION --- ///
void setup_comm()
{
    setupWiFi();
    setupServer();
}

 
void HandleClient()
{
    server.handleClient();
}

bool get_request_update()
{
    return newRecordRequested;
}

void request_treated()
{
    newRecordRequested = false;
}