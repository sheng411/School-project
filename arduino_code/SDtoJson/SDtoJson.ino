#include <SD.h>
#include <SPI.h>
#include <ArduinoJson.h>

#define SD_CS_PIN 5 // 定義SD卡的CS腳位

void readFileToJson(const char *filename);

void readFileToJson(const char *filename) 
{
  File file = SD.open(filename, FILE_READ);
  if (!file) 
  {
    Serial.println("Failed to open file for reading!");
    return;
  }

  // 讀取檔案內容
  String fileContent = "";
  while (file.available()) 
  {
    fileContent += (char)file.read();
  }
  file.close();

  Serial.println("File content read successfully!");

  // 建立 JSON 資料結構
  StaticJsonDocument<256> jsonDoc;
  jsonDoc["file_name"] = filename;           // 設定檔案名稱
  //jsonDoc["file_size"] = fileContent.length(); // 設定檔案大小
  jsonDoc["file_data"] = fileContent;       // 設定檔案內容

  // 將 JSON 資料轉換為字串
  String jsonString;
  serializeJson(jsonDoc, jsonString);

  // 在序列監視器上打印 JSON 字串
  Serial.println("Generated JSON:");
  Serial.println(jsonString);

  // 此處可擴展為將 JSON 資料透過其他方式送出（如 WiFi 或序列埠）
}

void setup() 
{
  Serial.begin(115200);
  while (!Serial) {}
  Serial.println("Initializing SD card...");

  if (!SD.begin(SD_CS_PIN)) 
  {
    Serial.println("SD card initialization failed!");
    while (1);
  }
  Serial.println("SD card initialized");

  // 從檔案讀取內容並轉換為 JSON
  readFileToJson("/123.txt");
}

void loop() 
{

}