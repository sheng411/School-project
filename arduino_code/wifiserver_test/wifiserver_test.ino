// TCP echo test

#include <WiFi.h>
#include "AES.h"
#define LED_PIN LED_BUILTIN

const char* ssid     = "ESP32-AP";
const char* password = "password123";
int port=1880;


WiFiServer server(port);

void send_to_client(WiFiClient client,String buf) {
  // This function sends a custom string to the client.
  Serial.println("send function");
  String data = "[server send]"+buf;    //data
  Serial.println(data);
  client.print(data);
}

void send_data(WiFiClient client) {
  String data = "";

  data="";
  while (Serial.available() > 0) {  // 檢查 Serial 介面上是否有可用的資料
    char c = Serial.read();  // 讀取 Serial 介面上的一個字元
    data += c;  // 將字元添加到 data 字串變數中
  }
  if (data.length() > 0) {  // 當 data 字串中有資料時才執行以下操作
    Serial.print("send data is-->");
    Serial.println(data);
    client.print(data);
    Serial.println("send function close");
  }
}
/*
void AES_cipher(int data){
  unsigned  int m[16]={}
  unsigned  int key[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
  unsigned  int s_box[256]={0}; unsigned  int invs_box[256]={0};unsigned  int nk=4;unsigned  int nr=10; unsigned int rc[nr]={0}; unsigned  int w[ (nr+1) * 4] ={0};
  S_box_invS_box(s_box,invs_box);
  rcv(nr, rc);
  keyexpansion(key, w, nk, nr,rc,s_box);
  printf("--Input message  -----------------------\n");
  for (int i = 0; i < 16; i++) {
        m[i]=data;
  }
  unsigned  int s0 = m[0]  << 24 | m[1]  << 16 | m[2]  << 8 | m[3];
  unsigned  int s1 = m[4]  << 24 | m[5]  << 16 | m[6]  << 8 | m[7];
  unsigned  int s2 = m[8]  << 24 | m[9]  << 16 | m[10] << 8 | m[11];
  unsigned  int s3 = m[12] << 24 | m[13] << 16 | m[14] << 8 | m[15];
  unsigned int state[4] = {s0, s1, s2, s3};
  cipher(state, nr,s_box,w);
  m[16]={}

  return state
}
*/
void setup(){
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    Serial.begin(115200);
    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.softAP(ssid, password);
    delay(100);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
    
    server.begin();
}

void loop(){
  static char buf[200];
  static int  length = 0; 
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    //Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      digitalWrite(LED_PIN, HIGH);
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        buf[length++] = c;
        
        //listen and return data
        if (c == '\n') {                    // if the byte is a newline character
          // append '\0' to buf[] as the end of the string
          buf[length++]='\0';
          // Output to serial port
          Serial.print("data->");
          Serial.print(buf);
          send_to_client(client,buf);
          //client.print(buf);      //return
          length = 0;
        }
        
        //send
        else if(c=='\''){
          send_data(client);
          length = 0;
        }
      }
    }
    digitalWrite(LED_PIN, LOW);
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");

  }
  /*
  //test local sending
  if (Serial.available()) {
    char bsf[2];
    bsf[1] = '\0';
    Serial.print("tls test");
    bsf[0] = Serial.read();
    Serial.print(bsf);
    String sdata=String(bsf);
    send_to_client(client,bsf);
    //client.stop();
    
    if (client.connected()) {
      String data = "Hello from the server!";
      client.print(data);
    }
  }
  */
}
