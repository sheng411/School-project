// TCP echo test

#include <WiFi.h>
#define LED_PIN LED_BUILTIN

const char* ssid     = "ESP32-AP";
const char* password = "password123";
int port=1880;


WiFiServer server(port);

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
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      digitalWrite(LED_PIN, HIGH);
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        buf[length++] = c; 
        if (c == '\n') {                    // if the byte is a newline character
          // append '\0' to buf[] as the end of the string
          buf[length++] = '\0';
          // Output to serial port
          Serial.print(buf);
          client.print(buf);
          length = 0;
        }
      }
    }
    
    digitalWrite(LED_PIN, LOW);
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
