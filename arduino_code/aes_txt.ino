#include <SPI.h>
#include <SD.h>
#include "AES.h"

// AES 參數設定
uint64_t s_box[256] = {0};
uint64_t invs_box[256] = {0};
uint64_t nk = 4;
uint64_t nr = 10;
uint64_t rc[10] = {0};
uint64_t w[(10 + 1) * 4] = {0};
uint64_t ecc_key[16] = {0x4E, 0xA0, 0xBB, 0xE6, 0x32, 0xAE, 0xBA, 0x9D, 0x47, 0x13, 0x20, 0x50, 0xDC, 0x75, 0xAD, 0xE2};

const int chipSelect = 5;

// 顯示檔案內容
void readFile(const char* filename)
{
  File file = SD.open(filename, FILE_READ);
  if (!file)
  {
    Serial.print("Failed to open file: ");
    Serial.println(filename);
    return;
  }
  Serial.print("Reading file: ");
  Serial.println(filename);

  while (file.available()) 
  {
    uint8_t byte = file.read();
    Serial.print("0x");
    Serial.print(byte, HEX);
    Serial.print(" ");
  }
  file.close();
  Serial.println("\nDone successfully.");
}

// 加密檔案
void encryptFile(const char* inputfilename, const char* outputfilename)
{
  File inputfile = SD.open(inputfilename, FILE_READ);
  File outputfile = SD.open(outputfilename, FILE_WRITE);

  if (!inputfile || !outputfile)
  {
    Serial.println("Failed to open input/output files!");
    return;
  }

  uint8_t byte_array[16] = {0};
  while (inputfile.available())
  {
    for (int i = 0; i < 16; i++)
    {
      if (inputfile.available())
        byte_array[i] = inputfile.read();
      else
        byte_array[i] = 0;
    }

    // 將 byte array 轉換成 state
    uint64_t state[4] = {0};
    for (int i = 0; i < 4; i++) 
    {
      state[i] = (uint64_t)byte_array[i * 4] << 24 |
                 (uint64_t)byte_array[i * 4 + 1] << 16 |
                 (uint64_t)byte_array[i * 4 + 2] << 8 |
                 (uint64_t)byte_array[i * 4 + 3];
    }

    cipher(state, nr, s_box, w);

    // 將加密後的 state 寫入檔案
    for (int i = 0; i < 4; i++) 
    {
      for (int j = 0; j < 4; j++) 
        byte_array[i * 4 + j] = (state[i] >> (24 - j * 8)) & 0xFF;
    }
    outputfile.write(byte_array, sizeof(byte_array));
  }

  inputfile.close();
  outputfile.close();
  Serial.println("File encrypted successfully.");
}

// 解密檔案
void decryptFile(const char* inputfilename, const char* outputfilename) 
{
  File inputfile = SD.open(inputfilename, FILE_READ);
  File outputfile = SD.open(outputfilename, FILE_WRITE);

  if (!inputfile || !outputfile)
  {
    Serial.println("Failed to open input/output files!");
    return;
  }

  uint8_t byte_array[16] = {0};
  while (inputfile.available()) 
  {
    // 讀取加密區塊
    for (int i = 0; i < 16; i++) 
    {
      if (inputfile.available())
        byte_array[i] = inputfile.read();
      else
        byte_array[i] = 0; // 填充0
    }

    uint64_t state[4] = {0};
    for (int i = 0; i < 4; i++) 
    {
      state[i] = (uint64_t)byte_array[i * 4] << 24 |
                 (uint64_t)byte_array[i * 4 + 1] << 16 |
                 (uint64_t)byte_array[i * 4 + 2] << 8 |
                 (uint64_t)byte_array[i * 4 + 3];
    }

   invcipher(state, nr, invs_box, w);

    for (int i = 0; i < 4; i++) 
    {
      for (int j = 0; j < 4; j++)
        byte_array[i * 4 + j] = (state[i] >> (24 - j * 8)) & 0xFF;
    }
    outputfile.write(byte_array, sizeof(byte_array));
  }

  inputfile.close();
  outputfile.close();
  Serial.println("File decrypted successfully.");
}


void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  // 初始化 SD 卡
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    while (true);
  }
  Serial.println("SD card initialized successfully!");

  // 初始化 AES
  S_box_invS_box(s_box, invs_box);
  rcv(nr, rc); 
  keyexpansion(ecc_key, w, nk, nr, rc, s_box);
}

void loop() {
  readFile("/test.txt");
  delay(10000);
  encryptFile("/test.txt", "/encrypted.txt");
  delay(10000);
  decryptFile("/encrypted.txt", "/decrypted.txt");
  delay(10000);
}
