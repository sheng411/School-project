//a
#include <WiFi.h>
#include "ecc.h"
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
        WiFi.begin(bSSID, bPassword);
        delay(100);
    }
    else{
        led_on();
    }

    if(ck==0 && WiFi.status() == WL_CONNECTED){
        Serial.println("\nReconnect OK\n");
        Serial.print("connect IP->");
        Serial.println(WiFi.localIP());
        Serial.print("connect ssid->");
        Serial.println(WiFi.SSID());
        Serial.print("My IP->");
        Serial.println(WiFi.softAPIP());
        ck=1;
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

void ecc_receive_msg(){
    //a to b
    String message;
    WiFiClient client = server.available();
    if (client) {
        //Serial.println("Client connected");
        while (client.connected()) {
            if (client.available()) {
                Serial.print(WiFi.SSID());
                Serial.print(" : ");
                Serial.println(message);
                //client.println("Message received on B device: " + message);
            }
        }
        client.stop();
        //Serial.println("Client disconnected");
    }
}

void ecc_send_msg(String message){
    //b to a
    if (Serial.available() > 0) {
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
}

void ecc_connect(){
    points  G,Q,D;
    G.x={0xDE4E6D5E5C94EEE8,0x7BBC11ACAA07D793,0X2FE13C053};
    G.y={0x0536D538CCDAA3D9,0x5D38FF58321F2E80,0X289070FB0};
    mbits K={0Xffffffffffffffff,0Xffffffffffffffff,0X7fffffffff};
    D=scalarmA2(K,G);
    showword(D.x);
    showword(D.y);

    int h;
    String str1[]={"0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F"};                   
    String str="";
    for(int i=wsize-1;i>=0;i--){
        for(int j=(63-3);j>=0;j=j-4)
        str=str+str1[(D.x.a[i]>>j)&0x0f];
    }
    
    for(int i=0;i<3;i++){
        ecc_send_msg(str);
    }
    
    for(int i=0;i<3;i++){
        ecc_receive_msg();
    }
    Serial.println("ECC over-A");
}

void receive_msg(){
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
                //client.println("Message received on B device: " + message);
            }
        }
        client.stop();
        //Serial.println("Client disconnected");
    }
}

void send_msg(){
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
}

void setup() {
    Serial.begin(115200); 
    //ECC maketable
    makepowt(1,anT1)    ;
    makepowt(2,anT2)    ;
    makepowt(5,anT5)    ;
    makepowt(10,anT10)  ;
    makepowt(20,anT20)  ;
    makepowt(40,anT40)  ;
    
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
    ecc_connect();
}

void loop() {
    check_connect();
    receive_msg();
    delay(10);
    send_msg();    
    delay(10);
}
