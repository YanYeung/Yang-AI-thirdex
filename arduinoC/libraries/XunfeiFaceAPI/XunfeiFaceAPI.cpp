#include "XunfeiFaceAPI.h"

// ==================== 构造函数 ====================
XunfeiFaceAPI::XunfeiFaceAPI() : _appId(""), _apiKey(""), _apiSecret("") {}

// ==================== 初始化 ====================
bool XunfeiFaceAPI::init(String appId, String apiKey, String apiSecret) {
    _appId = appId;
    _apiKey = apiKey;
    _apiSecret = apiSecret;
    
    // 初始化 NTP 时间，因为讯飞鉴权需要准确的 GMT 时间
    // 使用阿里云 NTP 和 0 时区（方便计算 GMT）
    configTime(0, 0, "ntp1.aliyun.com", "pool.ntp.org");
    
    Serial.println("🔧 [XunfeiFace] 初始化完成, 等待时间同步...");
    
    // 简单检查时间是否同步
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo, 2000)){ // 等待最多2秒
        Serial.println("⚠️ [XunfeiFace] 时间同步超时, 可能会导致鉴权失败");
    } else {
        Serial.println("✅ [XunfeiFace] 时间已同步");
    }
    
    return true;
}

// ==================== 辅助: HMAC-SHA256 ====================
String XunfeiFaceAPI::hmacSha256(String data, String key) {
    uint8_t hmacResult[32];
    mbedtls_md_context_t ctx;
    mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 1);
    mbedtls_md_hmac_starts(&ctx, (const unsigned char *)key.c_str(), key.length());
    mbedtls_md_hmac_update(&ctx, (const unsigned char *)data.c_str(), data.length());
    mbedtls_md_hmac_finish(&ctx, hmacResult);
    mbedtls_md_free(&ctx);

    // 将结果转换为字符串 (这里其实不需要转 hex，直接给 base64Encode 用 raw bytes 更好，
    // 但为了接口统一，我们让 base64Encode 接受 raw bytes)
    return base64Encode(hmacResult, 32);
}

// ==================== 辅助: Base64 Encode ====================
String XunfeiFaceAPI::base64Encode(const unsigned char* data, size_t length) {
    size_t olen = 0;
    mbedtls_base64_encode(NULL, 0, &olen, data, length); // 获取所需长度
    
    unsigned char *output = (unsigned char *)malloc(olen);
    if (output == NULL) return "";
    
    mbedtls_base64_encode(output, olen, &olen, data, length);
    String result = (char *)output;
    free(output);
    return result;
}

// ==================== 生成鉴权 URL ====================
String XunfeiFaceAPI::getAuthUrl(String host, String path) {
    // 1. 获取 GMT 时间
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
        Serial.println("❌ [XunfeiFace] 无法获取时间!");
        return "";
    }
    
    char dateStr[64];
    // 格式: Thu, 12 Dec 2019 01:57:27 GMT
    strftime(dateStr, sizeof(dateStr), "%a, %d %b %Y %H:%M:%S GMT", &timeinfo);
    
    // 2. 构建 signature_origin
    // signature_origin = "host: " + host + "\n" +
    //                    "date: " + date + "\n" +
    //                    "POST " + path + " HTTP/1.1";
    String signatureOrigin = "host: " + host + "\n";
    signatureOrigin += "date: " + String(dateStr) + "\n";
    signatureOrigin += "POST " + path + " HTTP/1.1";
    
    // 3. HMAC-SHA256 签名
    String signatureSha = hmacSha256(signatureOrigin, _apiSecret);
    
    // 4. 构建 authorization_origin
    String authOrigin = "api_key=\"" + _apiKey + "\", algorithm=\"hmac-sha256\", headers=\"host date request-line\", signature=\"" + signatureSha + "\"";
    
    // 5. Base64 编码 authorization
    String authorization = base64Encode((const unsigned char*)authOrigin.c_str(), authOrigin.length());
    
    // 6. 构建 URL 参数 (需要 URL Encode)
    // 这里简单处理，因为 base64 可能包含 + / =，需要替换
    // ESP32 HTTPClient 可能会自动处理，但手动处理更稳妥
    // 注意：HTTPClient .begin 会处理 URL，但参数部分最好自己 encode
    // dateStr 中的空格和逗号也需要 encode
    
    // 简单的 URL Encode 实现 (只针对关键字符)
    auto simpleUrlEncode = [](String s) -> String {
        String encoded = "";
        for (int i = 0; i < s.length(); i++) {
            char c = s[i];
            if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') encoded += c;
            else {
                char buf[4];
                sprintf(buf, "%%%02X", c);
                encoded += buf;
            }
        }
        return encoded;
    };
    
    String url = "https://" + host + path;
    url += "?authorization=" + simpleUrlEncode(authorization);
    url += "&date=" + simpleUrlEncode(dateStr);
    url += "&host=" + host;
    
    return url;
}

// ==================== 调用 人脸检测 API ====================
XunfeiFaceResult XunfeiFaceAPI::detect(const String& base64Image) {
    XunfeiFaceResult result;
    result.success = false;
    result.message = "";
    result.expression = "未知";
    result.gender = "未知";
    result.score = 0.0;
    
    if (_appId == "" || _apiKey == "" || _apiSecret == "") {
        result.message = "API Key 未配置";
        return result;
    }

    String host = "api.xf-yun.com";
    String path = "/v1/private/s67c9c78c";
    
    // 1. 生成鉴权 URL
    String requestUrl = getAuthUrl(host, path);
    if (requestUrl == "") {
        result.message = "时间同步失败";
        return result;
    }
    
    // 2. 构建 JSON Body
    // 必须使用 DynamicJsonDocument，因为图片 Base64 很大
    // 图片 Base64 大小通常为几十KB，加上 JSON 结构，需要分配足够内存
    // ESP32 堆内存有限，尽量优化
    
    // 直接拼接字符串比 JsonDocument 省内存且快
    String jsonBody = "{";
    jsonBody += "\"header\":{\"app_id\":\"" + _appId + "\",\"status\":3},";
    // 增加 "detect_property":"1" 以获取人脸属性（性别、表情等）
    jsonBody += "\"parameter\":{\"s67c9c78c\":{\"service_kind\":\"face_detect\",\"detect_property\":\"1\",\"face_detect_result\":{\"encoding\":\"utf8\",\"compress\":\"raw\",\"format\":\"json\"}}},";
    jsonBody += "\"payload\":{\"input1\":{\"encoding\":\"jpg\",\"status\":3,\"image\":\"" + base64Image + "\"}}";
    jsonBody += "}";

    Serial.println("\n📤 [XunfeiFace] 发送请求...");
    Serial.printf("   URL: %s\n", requestUrl.c_str());
    Serial.printf("   Body长度: %d\n", jsonBody.length());
    
    // 3. 发送请求
    HTTPClient http;
    WiFiClientSecure client;
    client.setInsecure(); // 忽略证书
    
    // 设置超时
    client.setTimeout(10); // 10秒 TCP
    http.setTimeout(10000); // 10秒 HTTP
    
    if (!http.begin(client, requestUrl)) {
        result.message = "连接失败";
        return result;
    }
    
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Host", host);
    http.addHeader("app_id", _appId);
    
    int httpCode = http.POST(jsonBody);
    
    if (httpCode == 200) {
        String response = http.getString();
        Serial.println("✅ [XunfeiFace] 收到响应");
        
        // 解析响应
        // 响应可能很大，包含 Base64 结果，我们只需要 header.code 和 payload.face_detect_result.text
        
        // 适配 ArduinoJson 5
        DynamicJsonBuffer jsonBuffer;
        JsonObject& doc = jsonBuffer.parseObject(response);
        
        if (!doc.success()) {
            Serial.println("❌ JSON 解析失败");
            result.message = "JSON解析失败";
        } else {
            int code = doc["header"]["code"];
            if (code == 0) {
                // 成功，解析 payload
                const char* resultBase64 = doc["payload"]["face_detect_result"]["text"];
                if (resultBase64) {
                    // 解码 Base64 结果
                    // 结果是 JSON 字符串的 Base64
                    size_t outLen = 0;
                    mbedtls_base64_decode(NULL, 0, &outLen, (const unsigned char*)resultBase64, strlen(resultBase64));
                    unsigned char* decodedJson = (unsigned char*)malloc(outLen + 1);
                    if (decodedJson) {
                        mbedtls_base64_decode(decodedJson, outLen, &outLen, (const unsigned char*)resultBase64, strlen(resultBase64));
                        decodedJson[outLen] = '\0';
                        String resultJsonStr = (char*)decodedJson;
                        free(decodedJson);
                        
                        Serial.println("🔎 [XunfeiFace] 解码后的 JSON: " + resultJsonStr);

                        // 解析内部 JSON
                        // {"face_1": {"attribute": {"gender": {"range": 1, "score": 0.99, "description": "male"}, "emotion": {"range": 1, "score": 0.9, "description": "happy"}}}}
                        DynamicJsonBuffer resBuffer;
                        JsonObject& resDoc = resBuffer.parseObject(resultJsonStr);
                        
                        // 尝试适配不同的 JSON 结构
                        // 结构 1: {"face_1": ...}
                        // 结构 2: {"face_list": [...]}
                        
                        JsonVariant faceObj;
                        bool faceFound = false;

                        if (resDoc["face_1"].is<JsonObject>()) {
                            faceObj = resDoc["face_1"];
                            faceFound = true;
                        } else if (resDoc["face_list"].is<JsonArray>()) {
                            JsonArray& faces = resDoc["face_list"];
                            if (faces.size() > 0) {
                                faceObj = faces[0];
                                faceFound = true;
                            }
                        }

                        if (faceFound) {
                            String emotion = "未知";
                            String gender = "未知";
                            float score = 0.0;

                            // 解析属性
                            if (faceObj["property"].is<JsonObject>()) {
                                JsonObject& props = faceObj["property"];
                                
                                // 解析表情 (0:不笑, 1:微笑, 2:大笑) - 注意：不同版本API定义可能不同，这里假设是新版
                                // 实际观察到的值: 3? (需要查阅文档或推测)
                                // 根据官方文档或通常定义: 0-不笑 1-微笑 2-大笑
                                // 但这里返回了3，可能是 neutral?
                                // 让我们宽泛地处理
                                int expressionVal = -1;
                                if (props["expression"].is<int>()) expressionVal = props["expression"];
                                
                                switch (expressionVal) {
                                    case 0: emotion = "惊讶"; break;
                                    case 1: emotion = "害怕"; break;
                                    case 2: emotion = "厌恶"; break;
                                    case 3: emotion = "高兴"; break;
                                    case 4: emotion = "悲伤"; break;
                                    case 5: emotion = "生气"; break;
                                    case 6: emotion = "正常"; break;
                                    default: emotion = "未知 (" + String(expressionVal) + ")"; break;
                                }

                                // 解析性别 (0:男, 1:女)
                                int genderVal = -1;
                                if (props["gender"].is<int>()) genderVal = props["gender"];
                                
                                switch (genderVal) {
                                    case 0: gender = "男"; break;
                                    case 1: gender = "女"; break;
                                    default: gender = "未知"; break;
                                }
                            }
                            
                            
                            result.expression = emotion;
                            result.gender = gender;
                            result.score = faceObj["score"] | 0.0;
                            result.success = true;
                        } else {
                            result.message = "未检测到人脸";
                        }
                    } else {
                        result.message = "内存不足(解码)";
                    }
                } else {
                    result.message = "无结果数据";
                }
            } else {
                result.message = doc["header"]["message"].as<const char*>();
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
