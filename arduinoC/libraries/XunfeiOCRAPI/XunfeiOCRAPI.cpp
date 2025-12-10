#include "XunfeiOCRAPI.h"

// ==================== 构造函数 ====================
XunfeiOCRAPI::XunfeiOCRAPI() : _appId(""), _apiKey("") {}

// ==================== 初始化 ====================
bool XunfeiOCRAPI::init(String appId, String apiKey) {
    _appId = appId;
    _apiKey = apiKey;
    
    // 初始化 NTP 时间，因为讯飞鉴权需要准确的时间戳
    // 使用阿里云 NTP 和 0 时区
    configTime(0, 0, "ntp1.aliyun.com", "pool.ntp.org");
    
    Serial.println("🔧 [XunfeiOCR] 初始化完成, 等待时间同步...");
    
    // 简单检查时间是否同步
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo, 2000)){ // 等待最多2秒
        Serial.println("⚠️ [XunfeiOCR] 时间同步超时, 可能会导致鉴权失败");
    } else {
        Serial.println("✅ [XunfeiOCR] 时间已同步");
    }
    
    return true;
}

// ==================== 辅助: MD5 ====================
String XunfeiOCRAPI::md5(String data) {
    uint8_t md5Result[16];
    mbedtls_md_context_t ctx;
    mbedtls_md_type_t md_type = MBEDTLS_MD_MD5;

    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
    mbedtls_md_starts(&ctx);
    mbedtls_md_update(&ctx, (const unsigned char *)data.c_str(), data.length());
    mbedtls_md_finish(&ctx, md5Result);
    mbedtls_md_free(&ctx);

    // 转换为 Hex 字符串
    String result = "";
    char buf[3];
    for (int i = 0; i < 16; i++) {
        sprintf(buf, "%02x", md5Result[i]);
        result += buf;
    }
    return result;
}

// ==================== 辅助: Base64 Encode ====================
String XunfeiOCRAPI::base64Encode(const unsigned char* data, size_t length) {
    size_t olen = 0;
    mbedtls_base64_encode(NULL, 0, &olen, data, length);
    
    unsigned char *output = (unsigned char *)malloc(olen);
    if (output == NULL) return "";
    
    mbedtls_base64_encode(output, olen, &olen, data, length);
    String result = (char *)output;
    free(output);
    return result;
}

// ==================== 调用 OCR API ====================
XunfeiOCRResult XunfeiOCRAPI::recognize(const String& base64Image) {
    XunfeiOCRResult result;
    result.success = false;
    result.message = "";
    result.text = "";
    result.confidence = 0.0;
    
    if (_appId == "" || _apiKey == "") {
        result.message = "API Key 未配置";
        return result;
    }

    // 1. 获取时间戳
    time_t now;
    time(&now);
    if (now < 100000) { // 简单判断时间是否有效 (1970年)
        result.message = "时间同步失败";
        return result;
    }
    String curTime = String((long)now);
    
    // 2. 构建 Param Base64
    String param = "{\"language\":\"cn|en\",\"location\":\"false\"}";
    String paramBase64 = base64Encode((const unsigned char*)param.c_str(), param.length());
    
    // 3. 计算 CheckSum = MD5(ApiKey + CurTime + ParamBase64)
    String checkStr = _apiKey + curTime + paramBase64;
    String checkSum = md5(checkStr);
    
    // 4. 构建 Body (image=<UrlEncodedBase64>)
    // 手动 URL Encode Base64Image (主要是 + / =)
    // 简单的 URL Encode 实现
    auto simpleUrlEncode = [](String s) -> String {
        String encoded = "";
        encoded.reserve(s.length() + s.length() / 10);
        for (int i = 0; i < s.length(); i++) {
            char c = s[i];
            if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') encoded += c;
            else if (c == '+') encoded += "%2B";
            else if (c == '/') encoded += "%2F";
            else if (c == '=') encoded += "%3D";
            else {
                char buf[4];
                sprintf(buf, "%%%02X", c);
                encoded += buf;
            }
        }
        return encoded;
    };
    
    String body = "image=" + simpleUrlEncode(base64Image);
    
    // 5. 发送请求
    HTTPClient http;
    WiFiClient client; // OCR API 支持 HTTP
    
    // 设置超时
    client.setTimeout(10); // 10秒 TCP
    http.setTimeout(15000); // 15秒 HTTP
    
    String url = "http://webapi.xfyun.cn/v1/service/v1/ocr/general";
    
    Serial.println("\n📤 [XunfeiOCR] 发送请求...");
    Serial.printf("   URL: %s\n", url.c_str());
    Serial.printf("   Body长度: %d\n", body.length());
    
    if (!http.begin(client, url)) {
        result.message = "连接失败";
        return result;
    }
    
    http.addHeader("X-Appid", _appId);
    http.addHeader("X-CurTime", curTime);
    http.addHeader("X-Param", paramBase64);
    http.addHeader("X-CheckSum", checkSum);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded; charset=utf-8");
    
    int httpCode = http.POST(body);
    
    if (httpCode == 200) {
        String response = http.getString();
        Serial.println("✅ [XunfeiOCR] 收到响应: " + response);
        
        // 使用 ArduinoJson v5 语法
        DynamicJsonBuffer jsonBuffer;
        JsonObject& doc = jsonBuffer.parseObject(response);
        
        if (!doc.success()) {
            Serial.println("❌ JSON 解析失败");
            result.message = "JSON解析失败";
        } else {
            String code = doc["code"].as<String>();
            if (code == "0") {
                result.success = true;
                
                // 尝试适配 block_list 或 block
                JsonArray& blocks = doc["data"]["block_list"].success() ? doc["data"]["block_list"] : doc["data"]["block"];

                if (blocks.success()) {
                    for (JsonArray::iterator itBlock = blocks.begin(); itBlock != blocks.end(); ++itBlock) {
                        JsonObject& block = *itBlock;
                        
                        JsonArray& lines = block["line_list"].success() ? block["line_list"] : block["line"];
                        
                        if (lines.success()) {
                            for (JsonArray::iterator itLine = lines.begin(); itLine != lines.end(); ++itLine) {
                                JsonObject& line = *itLine;
                                
                                JsonArray& words = line["word_list"].success() ? line["word_list"] : line["word"];
                                
                                if (words.success()) {
                                    for (JsonArray::iterator itWord = words.begin(); itWord != words.end(); ++itWord) {
                                        JsonObject& word = *itWord;
                                        String content = word["content"].as<String>();
                                        if (result.text.length() > 0) result.text += "\n";
                                        result.text += content;
                                    }
                                }
                            }
                        }
                    }
                }
                
                if (result.text.length() == 0) {
                    result.text = "未识别到文字";
                }
                
            } else {
                result.message = doc["desc"].as<String>();
                Serial.printf("❌ API 错误: %s\n", result.message.c_str());
            }
        }
    } else {
        result.message = "HTTP " + String(httpCode);
        Serial.printf("❌ 请求失败: %d\n", httpCode);
    }
    
    http.end();
    return result;
}
