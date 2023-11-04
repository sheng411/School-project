#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#define LED_PIN LED_BUILTIN

const char *ssid = "ESP32-AP";
const char *password = "password123";

IPAddress staticIP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

const int port = 12345;

WebServer server(port);

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.softAPConfig(staticIP, gateway, subnet);
  WiFi.softAP(ssid, password);
  delay(100);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", HTTP_POST, handlePost);
  server.begin();
}

void loop() {
  server.handleClient();
  if (WiFi.softAPgetStationNum() > 0) {   //check Wifi connect
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
  handlePost();
}

void handlePost() {
  if (server.hasArg("plain")) {
    String message = server.arg("plain");

    if (message == "file" or message=="text") {
      Serial.println("accept file");
      server.send(200, "text/plain", "file456");
    } else {
      Serial.println("accept message：" + message);
      server.send(200, "text/plain", "message123");
    }
  } else {
    server.send(400, "text/plain", "Error request");
  }
}
