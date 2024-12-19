#include <SD.h>
#include <SPI.h>
#include <ArduinoJson.h>

#define SD_CS_PIN 5 // 定義SD卡的CS腳位

void writeFileFromJson(const char *jsonString) 
{
  StaticJsonDocument<256> jsonDoc;

  // 解析 JSON 字串
  DeserializationError error = deserializeJson(jsonDoc, jsonString);
  if (error) 
  {
    Serial.print("Failed to parse JSON: ");
    Serial.println(error.f_str());
    return;
  }

  // 提取文件名稱和內容
  const char *fileName = jsonDoc["file_name"];
  const char *fileData = jsonDoc["file_data"];

  // 確保文件名稱正確，避免空值或格式錯誤
  if (!fileName || !fileData) 
  {
    Serial.println("Invalid JSON data: Missing file_name or file_data!");
    return;
  }

  String fullFileName = "/" + String(fileName);

  // 打開 SD 卡文件進行寫入
  File file = SD.open(fullFileName.c_str(), FILE_WRITE);
  if (!file) 
  {
    Serial.println("Failed to open file for writing!");
    return;
  }

  // 寫入文件內容並檢查結果
  if (file.print(fileData) == 0)
    Serial.println("Failed to write data to file!");
  else 
  {
    Serial.print("Data written to file successfully! File: ");
    Serial.println(fullFileName);
  }

  file.close();
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

  // 使用原始字串常數撰寫 JSON 字串
  const char *jsonString = R"({
    "sender": "bbc",
    "timestamp": "2024-12-11 12:46:43",
    "file_type": "txt",
    "file_name": "123.txt",
    "file_data": "1234567890AbCd一二三test 。
    789456",
    "file_size": 23,
    "content_type": "file"
  })";

  writeFileFromJson(jsonString);
}

void loop() 
{

}