//a
#include <WiFi.h>
#define LED_PIN LED_BUILTIN

//local
const char* ssid = "kkESP32";
const char* password = "987654321";
WiFiServer server(80);

//connect
const char* bSSID = "shESP32";
const char* bPassword = "987654321";
const int bServerPort = 80;
WiFiClient client;

void check_connect(){
    int ck=1;
    if(WiFi.status() != WL_CONNECTED){
        ck=0;
        led_toggle();
        Serial.println("Wifi disconnected,reconnect...");
        WiFi.begin(aSSID, aPassword);
        delay(100);
    }
    else if(ck==0){
        Serial.print("connect IP->");
        Serial.println(WiFi.localIP());
        Serial.print("connect ssid->");
        Serial.println(WiFi.SSID());
        Serial.print("My IP->");
        Serial.println(WiFi.softAPIP());
        ck=1;
    }
    else{
        led_on();
    }
}

void led_on(){
    digitalWrite(LED_PIN, HIGH);
}

void led_off(){
    digitalWrite(LED_PIN, LOW);
}

void led_toggle() {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
}

void setup() {
    Serial.begin(115200); 
    WiFi.softAP(ssid, password);
    WiFi.begin(bSSID, bPassword);

    pinMode(LED_PIN, OUTPUT);
    led_off();

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to B WiFi...");
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

    //b to a
    WiFiClient client = server.available();
    if (client) {
        //Serial.println("Client connected");
        while (client.connected()) {
            if (client.available()) {
                String message = client.readStringUntil('\n');
                Serial.print(WiFi.SSID());
                Serial.print(" : ");
                Serial.println(message);
                //client.println("Message received on A device: " + message);
            }
        }
        client.stop();
        //Serial.println("Client disconnected");
    }
    delay(10);

    //a to b
    if (Serial.available() > 0) {
        String message = Serial.readStringUntil('\n');
        message.trim();
        if (!message.isEmpty()) {
            Serial.println("Sending to B:" + message);
            if (client.connect(WiFi.gatewayIP(), bServerPort)) {
                client.println(message);
            } else {
                Serial.println("Connection to B failed");
            }
        }
    }
    delay(10);
}
