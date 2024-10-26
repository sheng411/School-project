//b
#include <WiFi.h>
#include "ecc.h"
#define LED_PIN LED_BUILTIN

//local
const char* ssid = "shESP32";
const char* password = "1234567890";
WiFiServer server(80);

//connect
const char* c_SSID = "kkESP32";
const char* c_Password = "1234567890";
const int c_ServerPort = 80;
WiFiClient client;

void show_wifi_info(){
    Serial.print("connect IP->");
    Serial.println(WiFi.localIP());
    Serial.print("connect ssid->");
    Serial.println(WiFi.SSID());
    Serial.print("My IP->");
    Serial.println(WiFi.softAPIP());
}

void check_connect(){
    int ck=1;
    if(WiFi.status() != WL_CONNECTED){
        ck=0;
        led_toggle();
        Serial.println("Wifi disconnected,reconnect...");
        WiFi.begin(c_SSID, c_Password);
        delay(100);
    }
    else{
        led_on();
    }

    if(ck==0 && WiFi.status() == WL_CONNECTED){
        Serial.println("\nReconnect OK\n");
        show_wifi_info();
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
    Serial.println("Waiting for message from A...");
    while (!server.hasClient()) {
        delay(100);   // 等待A建立連接
    }
    String message;
    WiFiClient client = server.available();
    if (client) {
        //Serial.println("Client connected");
        while (client.connected()) {
            if (client.available()) {
                String message = client.readStringUntil('\n');
                Serial.print("[ECC] ");
                Serial.print(WiFi.SSID());
                Serial.print(" : ");
                Serial.println(message);
                break;
                //client.println("Message received on A device: " + message);
            }
        }
        client.stop();
        //Serial.println("Client disconnected");
    }
}

void ecc_send_msg(String message){
    //b to a
    message.trim();
    if (!message.isEmpty()) {
        Serial.println("[ECC] Sending to A:" + message);
        if (client.connect(WiFi.gatewayIP(), c_ServerPort)) {
            client.println(message);
            client.stop();
        } else {
            Serial.println("Connection to A failed");
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

    ecc_receive_msg();

    ecc_send_msg(str);

    Serial.println("ECC over-B");
}

void receive_msg(){
    //a to b
    WiFiClient client = server.available();
    if (client) {
        //Serial.println("Client connected");
        while (client.connected()) {
            if (client.available()) {
                String message = client.readStringUntil('\n');
                Serial.print(WiFi.SSID());
                Serial.print(" : ");
                Serial.println(message);
            }
        }
        client.stop();
        //Serial.println("Client disconnected");
    }
}

void send_msg(){
    //b to a
    if (Serial.available() > 0) {
        String message = Serial.readStringUntil('\n');
        message.trim();
        if (!message.isEmpty()) {
            Serial.println("Sending to A:" + message);
            if (client.connect(WiFi.gatewayIP(), c_ServerPort)) {
                client.println(message);
            } else {
                Serial.println("Connection to A failed");
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
    WiFi.begin(c_SSID, c_Password);

    pinMode(LED_PIN, OUTPUT);
    led_off();

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to A WiFi...");
    }

    show_wifi_info();
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
