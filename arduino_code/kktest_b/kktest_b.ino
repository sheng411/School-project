//b
#include <WiFi.h>
#define LED_PIN LED_BUILTIN

//local
const char* ssid = "shESP32";
const char* password = "987654321";
WiFiServer server(80);

//connect
const char* aSSID = "kkESP32";
const char* aPassword = "987654321";
const int aServerPort = 80;
WiFiClient client;

void check_connect(){
    if(WiFi.status() != WL_CONNECTED){
        digitalWrite(LED_BUILTIN,LOW);
        Serial.println("Wifi disconnected,reconnect...");
        WiFi.begin(aSSID, aPassword);
        delay(100);
    }
    else{
        digitalWrite(LED_BUILTIN,HIGH);
    }
}

void setup() {
    Serial.begin(115200);
    WiFi.softAP(ssid, password);
    WiFi.begin(aSSID, aPassword);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to A WiFi...");
    }

    Serial.print("connect IP->");
    Serial.println(WiFi.localIP());
    Serial.print("connect ssid->");
    Serial.println(WiFi.SSID());
    Serial.print("My IP->");
    Serial.println(WiFi.softAPIP());

    server.begin();
}

void loop() {
    check_connect();

    //a to b
    WiFiClient client = server.available();
    if (client) {
        //Serial.println("Client connected");
        while (client.connected()) {
            if (client.available()) {
                String message = client.readStringUntil('\n');
                Serial.print("Message: ");
                Serial.println(message);
                //client.println("Message received on A device: " + message);
            }
        }
        client.stop();
        //Serial.println("Client disconnected");
    }
    delay(10);

    //b to a
    if (Serial.available() > 0) {
        String message = Serial.readStringUntil('\n');
        message.trim();
        if (!message.isEmpty()) {
            Serial.println("Sending to A:" + message);
            if (client.connect(WiFi.gatewayIP(), aServerPort)) {
                client.println(message);
            } else {
                Serial.println("Connection to A failed");
            }
        }
    }
    delay(100);
}
