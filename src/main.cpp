// JSON Example
#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <cstdio>

// wybrac dowolne
#define SSID "stud_wifi"
#define PASS ""

const char* address = "http://10.0.2.32:80/tcmed-platform-web/api/web/command/find/2";

HTTPClient http;

class Command
{
public:
    int mType;
    int mId;

    Command(int type, int id)
        : mType(type), mId(id) {}

};

// #################################################
// ################# DECLARATIONS ##################
// #################################################

bool wifiConnect();
Command deserialize(const char* payload);
void blinkLed();

// #################################################
// ############### SETUP AND MAIN LOOP #############
// #################################################


void setup() {
    Serial.begin(115200);
    pinMode(BUILTIN_LED, OUTPUT);

    if (wifiConnect()) {
        Serial.printf("Connected to %s, on pass %s\n", SSID, PASS);
    }
}

void loop()
{
    http.begin(address);
    auto httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)    {
        String payload = http.getString();

        auto myCommand = deserialize(payload.c_str());
        Serial.printf("Type is %d\n", myCommand.mType);
        Serial.printf("Id is %d\n", myCommand.mId);

        if(myCommand.mType == 1)        digitalWrite(LED_BUILTIN, LOW);
        else if(myCommand.mType == 0)   digitalWrite(LED_BUILTIN, HIGH);
    }
    else if (httpCode != HTTP_CODE_NOT_FOUND)    {
        Serial.printf("Code is %d\n", httpCode);
    }
    

    delay(1000);
}

// #################################################
// #################### DEFINITIONS ################
// #################################################

bool wifiConnect()
{
    unsigned int retries = 0;
    WiFi.begin(SSID, PASS);

    while(WiFi.status() != WL_CONNECTED){
        Serial.printf("Connect to %s, on pass %s\n", SSID, PASS);
        retries++;
        if(retries > 50){
            Serial.println("Failed to connect after 50 attempts");

            return false;
        }
    }
    
    return true;
}
Command deserialize(const char* payload)
{
    // Stream& input;
    StaticJsonDocument<500> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error)    {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return Command(0, 0);
    }

    int type = doc["type"]; 
    int id = doc["id"];             

    return Command(type, id);
}
// #################################################