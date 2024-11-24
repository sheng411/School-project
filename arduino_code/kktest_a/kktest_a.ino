//a
#include <WiFi.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include "ECC.h"
#include "AES.h"
#define LED_PIN LED_BUILTIN
uint8_t s_msg[16];
uint8_t r_msg[16];
// AES 參數設定
uint64_t s_box[256] = {0};
uint64_t invs_box[256] = {0};
uint64_t nk = 4;
uint64_t nr = 10;
uint64_t rc[10] = {0};
uint64_t w[(10 + 1) * 4] = {0};
uint64_t ecc_key[16];
int current_byte_count = 0;  // 當前區塊中的總字節數
using namespace std;

//local
const char* ssid = "kkESP32";
const char* password = "987654321";
WiFiServer server(3559);

//connect
const char* c_SSID = "shESP32";
const char* c_Password = "987654321";
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
        WiFi.disconnect();
        WiFi.begin(c_SSID, c_Password);
        delay(500);
    }
    else{
        led_on();
    }

    if(ck==0 && WiFi.status() == WL_CONNECTED){
        Serial.println("\nReconnect OK\n");
        show_wifi_info();
        ecc_connect();
        AES_setting();
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

//ECC area
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

void ecc_cut_key(String hex_string,uint64_t ecc_key[], mbits K){
    String hex_string1;
    int count=0;

    /* test message
    Serial.print("hex_str-->");
    Serial.println(hex_string);  //D*G
    */

    points DG;

    for(int i=0;i<3;i++){
        hex_string1 = hex_string.substring(hex_string.length()-16,hex_string.length());
        uint64_t num = strtoull(hex_string1.c_str(), NULL, 16);
        DG.y.a[i]=num;
        hex_string.remove(hex_string.length()-16);
    }

    for(int i=0;i<3;i++){
        hex_string1 = hex_string.substring(hex_string.length()-16,hex_string.length());
        uint64_t num = strtoull(hex_string1.c_str(), NULL, 16);
        DG.x.a[i]=num;
        hex_string.remove(hex_string.length()-16);
    }

    /* test message
    Serial.print("DG:");
    for(int i=0;i<3;i++)
        Serial.println(DG.x.a[i],HEX);
    */
    points KDG=scalarmA2(K,DG);
    //showword(KDG.x);
    //showword(KDG.y);

    for(int i=0;i<2;i++){
        count=0;
        for(int j=8*i;j<8*(i+1);j++){
        ecc_key[j]=((KDG.x.a[i]>>(8*count))&0xff);
        count++;
        }
    }
    /* test message(show ecc key)*/
    for(int i=0;i<16;i++){
        /*Serial.print("ecc_key[");
        Serial.print(i);
        Serial.print("] = ");*/
        Serial.print(ecc_key[i],HEX);
        Serial.print(",");
    }
    
}

uint64_t getRandom64(){
    uint64_t high = (uint64_t)random(0x7FFFFFFF) << 32;
    uint64_t low = (uint64_t)random(0x7FFFFFFF);
    return high | low;
}

uint64_t getRandom35(){
    uint64_t high = (uint64_t)random(7) << 32;
    uint64_t low = (uint64_t)random(0x7FFFFFFF);
    return high | low;
}

void ecc_connect(){
    points  G,Q,D;
    G.x={0xDE4E6D5E5C94EEE8,0x7BBC11ACAA07D793,0X2FE13C053};
    G.y={0x0536D538CCDAA3D9,0x5D38FF58321F2E80,0X289070FB0};
    mbits K;
    //Serial.print("Random 64-bit number: 0x");
    for(int i=0;i<2;i++){
        uint64_t rand = getRandom64();
        K.a[i]=rand;
      //Serial.print(rand,HEX);
    }

    uint64_t rand1 = getRandom35();
    K.a[2]=rand1;
    /* test message
    Serial.println(rand1,HEX);
    for(int i=0;i<3;i++){
        Serial.print("K[");
        Serial.print(i);
        Serial.print("] = ");
        Serial.println(K.a[i],HEX);
    }
    */

    D=scalarmA2(K,G);
    /*
    showword(D.x);
    showword(D.y);
    */

    int h;
    String str1[]={"0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F"};                   
    String str="";
    for(int i=wsize-1;i>=0;i--){
        for(int j=(63-3);j>=0;j=j-4)
        str=str+str1[(D.x.a[i]>>j)&0x0f];
    }
    for(int i=wsize-1;i>=0;i--){
        for(int j=(63-3);j>=0;j=j-4)
        str=str+str1[(D.y.a[i]>>j)&0x0f];
    }

    ecc_send_msg(str);
    String re_str2;     //接收到的D*G
    re_str2=ecc_receive_msg();

    //Serial.print("re_str2-->");
    //Serial.println(re_str2);
    ecc_cut_key(re_str2,ecc_key,K);
    Serial.println("ECC over-A");
}

//AES area
void AES_setting(){
    S_box_invS_box(s_box,invs_box);
    rcv(nr, rc);
    keyexpansion(ecc_key, w, nk, nr,rc,s_box);
}

String AES_Encryption(String str){
    uint64_t len = str.length();
    Serial.print("len: ");
    Serial.println(len);
    uint64_t num_blocks = (len + 15) / 16;
    while (len % 16 != 0) {
        str += ' '; // 用空格補齊
        len++;
    }
    
    String encrypted_text = "";
    // 每 16 字節一個區塊處理
    uint64_t byte_array[16];
    for (uint64_t block = 0; block < num_blocks; block++) {
        String block_str = str.substring(block * 16, (block + 1) * 16);
        const char *utf8Str = block_str.c_str();

        for (int i = 0; i < 16; i++) {
            byte_array[i] = (uint8_t)utf8Str[i];
        }

        // 將 byte array 轉換成 state
        uint64_t state[4];
        for (int i = 0; i < 4; i++) {
            state[i] = byte_array[i * 4] << 24 | byte_array[i * 4 + 1] << 16 |
            byte_array[i * 4 + 2] << 8 | byte_array[i * 4 + 3];
        }
    
        // 顯示加密前 state
        Serial.println("加密前 state:");
        for (int i = 0; i < 4; i++) {
            Serial.println(state[i], HEX);
        }

        // AES 加密
        cipher(state, nr, s_box, w);

        // 將 state 轉回 byte array
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                byte_array[i * 4 + j] = (state[i] >> (24 - j * 8)) & 0xFF;
            }
        }

        // 顯示加密後 state
        Serial.println("加密後 state:");
        for (int i = 0; i < 4; i++) {
            Serial.println(state[i], HEX);
        }

        // 加密結果轉換為字串
        for (int i = 0; i < 16; i++) {
            encrypted_text += (char)byte_array[i];
        }
        Serial.print("[]encrypted_text: ");
        Serial.println(encrypted_text);
    }
    

    Serial.print("加密後字串: ");
    encrypted_text.trim(); // 去除補齊的空格
    Serial.println(encrypted_text);
    return encrypted_text;
}

String AES_Decryption(String str){
    str.remove(str.length()-1);
    Serial.print("收到的加密字串: ");
    Serial.println(str);
    uint64_t len = str.length();
    Serial.print("len: ");
    Serial.println(len);
    uint64_t num_blocks = (len + 15) / 16;
    while (len % 16 != 0) {
        str += ' '; // 用空格補齊
        len++;
    }
    
    String decrypted_text = "";
    // 每 16 字節一個區塊處理
    uint64_t byte_array[16];
    for (uint64_t block = 0; block < num_blocks; block++) {
        String block_str = str.substring(block * 16, (block + 1) * 16);
        const char *utf8Str = block_str.c_str();
        Serial.print("block_str: ");
        Serial.println(block_str);

        for (int i = 0; i < 16; i++) {
            byte_array[i] = (uint8_t)utf8Str[i];
        }

        // 將 byte array 轉換成 state
        uint64_t state[4];
        for (int i = 0; i < 4; i++) {
            state[i] = byte_array[i * 4] << 24 | byte_array[i * 4 + 1] << 16 |
            byte_array[i * 4 + 2] << 8 | byte_array[i * 4 + 3];
        }
    
        // AES 解密
        invcipher(state, nr, invs_box, w);
/*
        // 顯示解密後 state
        Serial.println("解密後 state:");
        for (int i = 0; i < 4; i++) {
            Serial.println(state[i], HEX);
        }
*/
        // 將解密結果轉換為字串
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                byte_array[i * 4 + j] = (state[i] >> (24 - j * 8)) & 0xFF;
            }
        }

        for (int i = 0; i < 16; i++) {
            decrypted_text += (char)byte_array[i];
        }
        Serial.print("[]dncrypted_text: ");
        Serial.println(decrypted_text);
    }

    //Serial.print("解密後字串: ");
    decrypted_text.trim(); // 去除補齊的空格
    //Serial.println(decrypted_text);
    return decrypted_text;
}

//message receive and send
void receive_msg(){
    static uint8_t buffer_spase[16];
    //a to b
    WiFiClient client = server.available();
    if (client) {
        //Serial.println("Client connected");
        while (client.connected()) {
            if (client.available()) {
                String message = client.readStringUntil('\n');
                Serial.print("[receive]");
                Serial.print(WiFi.SSID());
                Serial.print(" : ");

                // 解密後清理原始訊息
                String decrypted = AES_Decryption(message);
                message.clear();
                Serial.println(decrypted);
                
                // 清理解密後的訊息和緩衝區
                decrypted.clear();
                memset(buffer_spase, 0, sizeof(buffer_spase));
                memset(r_msg, 0, sizeof(r_msg));

                // clear the message buffer
                message = "";
                client.flush();
            }
        }
        client.stop();
        // 強制清理WiFi Client資源
        client.flush();
        client = WiFiClient();
        //Serial.println("Client disconnected");
    }
}

void send_msg(){
    //a to b
    static uint8_t buffer_spase[16];
    
    if (Serial.available() > 0) {
        String message = Serial.readStringUntil('\n');
        message.trim();
        if (!message.isEmpty()) {
            Serial.println("Sending to B:" + message);
            if (client.connect(WiFi.gatewayIP(), c_ServerPort)) {
                String encrypted = AES_Encryption(message);
                message.clear();
                
                client.println(encrypted);
                // 清理加密後的訊息
                encrypted.clear();
                
                // 清理緩衝區
                memset(buffer_spase, 0, sizeof(buffer_spase));
                memset(s_msg, 0, sizeof(s_msg));

                // clear the message buffer
                client.flush();
                client.stop();
                client = WiFiClient();
            }
            else {
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
    WiFi.mode(WIFI_AP_STA);

    pinMode(LED_PIN, OUTPUT);
    led_off();

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to B WiFi...");
    }

    show_wifi_info();
    server.begin();
    ecc_connect();
    AES_setting();
    Serial.println("setup OK");
}

void loop() {
    check_connect();
    receive_msg();
    delay(10);
    send_msg();    
    delay(10);
}
