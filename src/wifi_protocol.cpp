#include "wifi_protocol.h"

const char* ssid = "Redmi";
const char* password = "Ljhki23132";

const char* server_ip = "192.168.112.57";
const uint16_t server_port = 2345;  // Port choisi côté bridge.py

const int COOLING_TIME = 250;

WiFiClient client;

String message_from_epuck;

void connect_to_wifi() 
{
    Serial.print("Trying to connect to Wifi");
    WiFi.begin(ssid, password);
  
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.println(".");
    }

    Serial.print("Connected to Wifi!");
    Serial.print("IP ESP32 : ");
    Serial.println(WiFi.localIP());
}
  
void connect_to_bridge()
{
    while (!client.connected()) 
    {
        Serial.print("Trying to connect to bridge... ");
        if (client.connect(server_ip, server_port)) 
        {
            Serial.println("Connected to bridge!");
        } else {
            Serial.println("Failed, new try in 1s...");
            delay(1000);
        }
    }
}

void reconnect_to_wifi()
{
    if(WiFi.status() != WL_CONNECTED)
    {
        Serial.println("-------Wifi connexion lost - Trying to reconnect-------");
        connect_to_wifi();
    }
}

void reconnect_to_bridge()
{
    if(!client.connected())
    {
        Serial.println("-------Bridge connexion lost - Trying to reconnect-------");
        connect_to_bridge();
    }
}

void wait_ack(String formated_message)
{
    bool ack_received(false);

    while(!ack_received)
    {
        reconnect_to_bridge();
        client.println(formated_message);

        delay(COOLING_TIME/10);

        if(client.available())
        {
            String response = client.readStringUntil('\n');
            response.trim();
            if (response == "ACK")
            {
                Serial.println("ACK received!");
                ack_received = true;
            }
        }
        delay(COOLING_TIME);
    } 
}

void send_msg_to_bridge(String message, bool wait)
{
    message.trim();
    reconnect_to_bridge();
    wait_ack("[br-type]"+ message);
}

void send_msg_to_epuck(String message)
{
    reconnect_to_bridge();
    wait_ack("[c-type]"+ message);
}



void send_ack()
{
    reconnect_to_bridge(); 
    
    client.println("[br-type]ACK");
    delay(COOLING_TIME);
}

void wait_msg()
{
    bool msg_received = false;
    while(!msg_received)
    {
        reconnect_to_bridge();

        if(client.available())
        {
            String response = client.readStringUntil('\n');
            response.trim();
            if (response!="")
            {
                Serial.println("Message received: "+ response);
                message_from_epuck = response;
                
                send_ack();
                msg_received = true;
            }
        }
        //delay(COOLING_TIME);
    }
}

void wait_msg_treated()
{
    bool msg_treated = false;

    while(!msg_treated)
    {
        reconnect_to_bridge();

        if(client.available())
        {
            String response = client.readStringUntil('\n');
            response.trim();
            if (response == "TRT")
            {
                Serial.println("Message Treated by epuck!");
                
                delay(COOLING_TIME);
                send_ack();
                msg_treated = true;
            }
        }
    }
}