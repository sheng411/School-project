//a
#include <WiFi.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include "ecc.h"
#define LED_PIN LED_BUILTIN
int ecc_key[16];
using namespace std;

//local
const char* ssid = "kkESP32";
const char* password = "1234567890";
WiFiServer server(3559);

//connect
const char* c_SSID = "shESP32";
const char* c_Password = "1234567890";
const int c_ServerPort = 3559;
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

String ecc_receive_msg(){
    //a to b
    Serial.println("Waiting for message from B...");
    while (!server.hasClient()) {
        delay(100);   // 等待B建立連接
    }
    String message;
    WiFiClient client = server.available();
    if (client) {
        while (client.connected()) {
            if (client.available()) {
                message = client.readStringUntil('\n');
                Serial.print("[ECC] ");
                Serial.print(WiFi.SSID());
                Serial.print(" : ");
                Serial.println(message);
                break;
            }
        }
        client.stop();
    }
    message.remove(message.length()-1);   //Delete "\n"
    return message;
}

void ecc_send_msg(String message){
    //a to b
    message.trim();
    if (!message.isEmpty()) {
        Serial.println("[ECC] Sending to B:" + message);
        if (client.connect(WiFi.gatewayIP(), c_ServerPort)) {
            client.println(message);
            client.stop();
        } else {
            Serial.println("Connection to B failed");
        }
    }
}

void ecc_cut_key(String hex_string,int ecc_key[]){
    String hex_string1;
    Serial.print("hex_str-->");
    Serial.println(hex_string);
    for(int i=0;i<16;i++){
        hex_string1 = hex_string.substring(hex_string.length()-2,hex_string.length());
        Serial.print("hex_str1-->");
        Serial.println(hex_string1);
        int hex_value = strtoul(hex_string1.c_str(), NULL, 16);  // 將字串轉為16進位整數
        Serial.print("hex_value-->");
        Serial.println(hex_value);
        ecc_key[i]=hex_value;
        hex_string.remove(hex_string.length()-2);

        Serial.print("ecc_key[");
        Serial.print(i);
        Serial.print("] = ");
        Serial.println(ecc_key[i],HEX);
    }
}

void ecc_connect(){
    points  G,Q,D;
    G.x={0xDE4E6D5E5C94EEE8,0x7BBC11ACAA07D793,0X2FE13C053};
    G.y={0x0536D538CCDAA3D9,0x5D38FF58321F2E80,0X289070FB0};
    mbits K;
    for(int i=0;i<3;i++){
        uint64_t rand;
        rand=random(0x7fffffff);
        K.a[i]=rand;
        Serial.print("K[");
        Serial.print(i);
        Serial.print("] = ");
        Serial.println(K.a[i]);
    }
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
    
    ecc_send_msg(str);
    String re_str2;     //接收到的Key
    re_str2=ecc_receive_msg();
    Serial.print("re_str2-->");
    Serial.println(re_str2);
    ecc_cut_key(re_str2,ecc_key);
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
            if (client.connect(WiFi.gatewayIP(), c_ServerPort)) {
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
    WiFi.begin(c_SSID, c_Password);

    pinMode(LED_PIN, OUTPUT);
    led_off();

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to B WiFi...");
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
