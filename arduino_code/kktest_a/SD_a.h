#include <SD.h>
#include <SPI.h>
#include <ArduinoJson.h>

extern uint64_t s_box[256];
extern uint64_t invs_box[256];
extern uint64_t nk;
extern uint64_t nr;
extern uint64_t rc[10];
extern uint64_t w[];

/*  write to SD */
String writeFileFromJson(const char *jsonString){
    StaticJsonDocument<256> jsonDoc;

    // 解析 JSON 字串
    DeserializationError error = deserializeJson(jsonDoc, jsonString);
    if (error){
        //Serial.print("Failed to parse JSON: ");
        //Serial.println(error.f_str());
        return String("");
    }

    // 提取文件名稱和內容
    const char *fileName = jsonDoc["file_name"];
    const char *fileData = jsonDoc["file_data"];

    // 確保文件名稱正確，避免空值或格式錯誤
    if (!fileName || !fileData){
        //Serial.println("Invalid JSON data: Missing file_name or file_data!");
        return String("");
    }

    String fullFileName = "/" + String(fileName);

    // 打開 SD 卡文件進行寫入
    File file = SD.open(fullFileName.c_str(), FILE_WRITE);
    if (!file){
        //Serial.println("Failed to open file for writing!");
        return String("");
    }

    // 寫入文件內容並檢查結果
    /*if (file.print(fileData) == 0)
        //Serial.println("Failed to write data to file!");
    else{
        Serial.print("Data written to file successfully! File: ");
        Serial.println(fullFileName);
    }*/

    file.close();

    return fullFileName;
}

//writeFileFromJson(jsonString);


//顯示檔案內容(input file)
/*
void readFile(const char* filename){
    File file = SD.open(filename, FILE_READ);
    if (!file){
        Serial.print("Failed to open file: ");
        Serial.println(filename);
        return;
    }
    Serial.print("Reading file: ");
    Serial.println(filename);

    while (file.available()){
        uint8_t byte = file.read();
        Serial.print("0x");
        Serial.print(byte, HEX);
        Serial.print(" ");
    }
    file.close();
    Serial.println("\nDone successfully.");
}
*/

// 加密檔案(input file/output file)
void encryptFile(const char* inputfilename, const char* outputfilename){
    File inputfile = SD.open(inputfilename, FILE_READ);
    File outputfile = SD.open(outputfilename, FILE_WRITE);

    if (!inputfile || !outputfile){
        //Serial.println("Failed to open input/output files!");
        return;
    }

    uint8_t byte_array[16] = {0};
    while (inputfile.available()){
        for (int i = 0; i < 16; i++){
        if (inputfile.available())
            byte_array[i] = inputfile.read();
        else
            byte_array[i] = 0;
        }

        // 將 byte array 轉換成 state
        uint64_t state[4] = {0};
        for (int i = 0; i < 4; i++){
        state[i] = (uint64_t)byte_array[i * 4] << 24 |
                    (uint64_t)byte_array[i * 4 + 1] << 16 |
                    (uint64_t)byte_array[i * 4 + 2] << 8 |
                    (uint64_t)byte_array[i * 4 + 3];
        }

        cipher(state, nr, s_box, w);

        // 將加密後的 state 寫入檔案
        for (int i = 0; i < 4; i++){
            for (int j = 0; j < 4; j++) 
            byte_array[i * 4 + j] = (state[i] >> (24 - j * 8)) & 0xFF;
        }
        outputfile.write(byte_array, sizeof(byte_array));
    }

    inputfile.close();
    outputfile.close();
    //Serial.println("File encrypted successfully.");
}


// 解密檔案(input file/output file)
void decryptFile(const char* inputfilename, const char* outputfilename){
    File inputfile = SD.open(inputfilename, FILE_READ);
    File outputfile = SD.open(outputfilename, FILE_WRITE);

    if (!inputfile || !outputfile){
        //Serial.println("Failed to open input/output files!");
        return;
    }

    uint8_t byte_array[16] = {0};
    while (inputfile.available()){
        // 讀取加密區塊
        for (int i = 0; i < 16; i++){
        if (inputfile.available())
            byte_array[i] = inputfile.read();
        else
            byte_array[i] = 0; // 填充0
        }

        uint64_t state[4] = {0};
        for (int i = 0; i < 4; i++){
        state[i] = (uint64_t)byte_array[i * 4] << 24 |
                    (uint64_t)byte_array[i * 4 + 1] << 16 |
                    (uint64_t)byte_array[i * 4 + 2] << 8 |
                    (uint64_t)byte_array[i * 4 + 3];
        }

    invcipher(state, nr, invs_box, w);

        for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++)
            byte_array[i * 4 + j] = (state[i] >> (24 - j * 8)) & 0xFF;
        }
        outputfile.write(byte_array, sizeof(byte_array));
    }

    inputfile.close();
    outputfile.close();
    //Serial.println("File decrypted successfully.");
}


/*
readFile("/test.txt");
encryptFile("/test.txt", "/encrypted.txt");
decryptFile("/encrypted.txt", "/decrypted.txt");
*/

/*  SD to JSON  */
String readFileToJson(const char *filename) {
    File file = SD.open(filename, FILE_READ);
    if (!file){
        //Serial.println("Failed to open file for reading!");
        return String("");
    }

    // 讀取檔案內容
    String fileContent = "";
    while (file.available()){
        fileContent += (char)file.read();
    }
    file.close();

    //Serial.println("File content read successfully!");

    // 建立 JSON 資料結構
    StaticJsonDocument<256> jsonDoc;
    jsonDoc["file_name"] = filename;           // 設定檔案名稱
    //jsonDoc["file_size"] = fileContent.length(); // 設定檔案大小
    jsonDoc["file_data"] = fileContent;       // 設定檔案內容

    Serial.print("[[txt]file name]:");
    Serial.println(filename);
    Serial.print("[[txt]file data]:");
    Serial.println(fileContent);

    // 將 JSON 資料轉換為字串
    String jsonString;
    serializeJson(jsonDoc, jsonString);

    // 在序列監視器上打印 JSON 字串
    //Serial.println("Generated JSON:");
    //Serial.println(jsonString);

    return jsonString;
}

//readFileToJson("/123.txt");