#include <Arduino.h>
#include <WiFi.h>
#include "BaiduFaceAI.h"

// 请替换为您的 WiFi 信息
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// 请替换为您的百度云 API Key 和 Secret Key
const char* apiKey = "YOUR_API_KEY";
const char* secretKey = "YOUR_SECRET_KEY";

BaiduFaceAI faceAI;

// 示例图片 Base64 (为了演示方便，这里是一个简化的占位符，实际使用请填入完整的人脸图片Base64编码，不带头部的 data:image/jpg;base64,)
String demoImageBase64 = "YOUR_IMAGE_BASE64_STRING_HERE"; 
String groupName = "test_group_001";
String faceName = "user_001";

void setup() {
    Serial.begin(115200);
    delay(1000);

    // 连接 WiFi
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");

    // 初始化百度AI
    if (faceAI.init(apiKey, secretKey)) {
        Serial.println("Baidu Face AI Initialized!");
    } else {
        Serial.println("Initialization Failed!");
        return;
    }

    // 1. 创建人脸库
    Serial.println("\n--- Step 1: Create Face Group ---");
    if (faceAI.createFaceGroup(groupName)) {
        Serial.println("Group created or already exists.");
    }

    // 2. 注册人脸 (仅当您有真实图片Base64时解开注释)
    /*
    Serial.println("\n--- Step 2: Add Face ---");
    if (faceAI.addFace(demoImageBase64, faceName, groupName)) {
        Serial.println("Face added successfully.");
    }
    */

    // 3. 搜索人脸
    /*
    Serial.println("\n--- Step 3: Search Face ---");
    String result = faceAI.searchFace(demoImageBase64, groupName);
    Serial.print("Search Result: ");
    Serial.println(result);
    */
}

void loop() {
    // Do nothing
    delay(1000);
}
