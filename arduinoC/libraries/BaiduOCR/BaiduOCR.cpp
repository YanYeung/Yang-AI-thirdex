#include "BaiduOCR.h"

BaiduOCR::BaiduOCR() : _accessToken("") {}
BaiduOCR::~BaiduOCR() {}

// 成员函数：Unicode解码
String BaiduOCR::decodeUnicode(String str) {
    String res = "";
    int len = str.length();
    int i = 0;
    while (i < len) {
        if (i + 5 < len && str[i] == '\\' && str[i+1] == 'u') {
            String hexStr = str.substring(i + 2, i + 6);
            char *endptr;
            unsigned long code = strtoul(hexStr.c_str(), &endptr, 16);
            if (code <= 0x7F) res += (char)code;
            else if (code <= 0x7FF) {
                res += (char)(0xC0 | (code >> 6));
                res += (char)(0x80 | (code & 0x3F));
            } else if (code <= 0xFFFF) {
                res += (char)(0xE0 | (code >> 12));
                res += (char)(0x80 | ((code >> 6) & 0x3F));
                res += (char)(0x80 | (code & 0x3F));
            } else {
                res += (char)(0xF0 | (code >> 18));
                res += (char)(0x80 | ((code >> 12) & 0x3F));
                res += (char)(0x80 | ((code >> 6) & 0x3F));
                res += (char)(0x80 | (code & 0x3F));
            }
            i += 6;
        } else {
            res += str[i];
            i++;
        }
    }
    return res;
}

// 成员函数：URL编码
String BaiduOCR::urlEncode(String str) {
    String encodedString = "";
    // 预分配内存：Base64中 +号变%2B 增加2字符，/号变%2F 增加2字符
    // 估算增加 10% 空间即可
    encodedString.reserve(str.length() + (str.length() / 10)); 
    char c;
    for (int i = 0; i < str.length(); i++) {
        c = str.charAt(i);
        if (c == '+') encodedString += "%2B";
        else if (c == '/') encodedString += "%2F";
        else if (c == '=') encodedString += "%3D";
        else encodedString += c;
    }
    return encodedString;
}

// 接口1: 获取 Token
bool BaiduOCR::init(String apiKey, String secretKey) {
    if (apiKey.length() == 0 || secretKey.length() == 0) return false;
    
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;
    
    String url = "https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials&client_id=" + apiKey + "&client_secret=" + secretKey;
    
    Serial.println("🔑 [OCR] 获取 Access Token...");
    http.setTimeout(10000); 
    
    if (http.begin(client, url)) {
        int httpCode = http.GET();
        if (httpCode == 200) {
            String payload = http.getString();
            int start = payload.indexOf("\"access_token\":\"");
            if (start != -1) {
                start += 16;
                int end = payload.indexOf("\"", start);
                _accessToken = payload.substring(start, end);
                Serial.println("✅ [OCR] Token 获取成功");
                http.end();
                return true;
            }
        }
        http.end();
    }
    Serial.println("❌ [OCR] Token 获取失败");
    return false;
}

// 接口2: 文字识别
String BaiduOCR::recognizeText(String imageBase64) {
    if (_accessToken.length() == 0) return "错误: 未初始化Token";
    if (imageBase64.length() == 0) return "错误: 图片为空";

    // 内存检查：如果图片 Base64 超过 50KB，ESP32 极大概率发送失败
    if (imageBase64.length() > 50000) {
        Serial.printf("❌ [OCR] 警告: 图片数据过大 (%d 字节), 可能导致发送崩溃\n", imageBase64.length());
    }

    WiFiClientSecure client;
    client.setInsecure();
    
    // === 关键优化：设置底层 Socket 超时 ===
    client.setTimeout(60); // 60秒 TCP 超时
    
    HTTPClient http;
    String url = "https://aip.baidubce.com/rest/2.0/ocr/v1/general_basic?access_token=" + _accessToken;
    
    http.setReuse(false); // 禁止连接复用
    http.setTimeout(60000); // 60秒 HTTP 超时

    if (!http.begin(client, url)) return "错误: 连接百度失败";
    
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Accept", "application/json");
    http.addHeader("Connection", "close");
    
    // 构建 Payload
    // ⚠️ 这一步最消耗内存，如果这里崩溃，说明必须减小图片分辨率
    String encodedImage = urlEncode(imageBase64);
    String payload = "image=" + encodedImage;
    
    // 立即释放 encodedImage 占用的空间 (虽然 payload 是拷贝，但这是 String 类的机制)
    // 这里的局部变量销毁依赖编译器优化，我们手动清空一下原来的 encodedImage 以防万一
    encodedImage = ""; 
    
    Serial.printf("📤 [OCR] 发送请求 (Payload长度: %d)...\n", payload.length());
    
    int httpCode = http.POST(payload);
    
    String finalResult = "";

    if (httpCode == 200) {
        String response = http.getString();
        
        int searchIndex = 0;
        int lineCount = 0;
        
        while (true) {
            int keyIndex = response.indexOf("\"words\":\"", searchIndex);
            if (keyIndex == -1) break;
            
            int valStart = keyIndex + 9; 
            int valEnd = response.indexOf("\"", valStart);
            
            if (valEnd > valStart) {
                String rawLine = response.substring(valStart, valEnd);
                String decodedLine = decodeUnicode(rawLine);
                if (finalResult.length() > 0) finalResult += "\n";
                finalResult += decodedLine;
                lineCount++;
            }
            searchIndex = valEnd;
        }
        
        if (lineCount == 0) {
            if (response.indexOf("error_code") != -1) {
                Serial.println("[OCR] API返回错误: " + response);
                finalResult = "API调用错误";
            } else {
                finalResult = "未识别到文字";
            }
        }
    } else {
        Serial.printf("❌ [OCR] 网络错误: %d\n", httpCode);
        if (httpCode == -3) finalResult = "发送失败(内存/超时)";
        else finalResult = "网络错误: " + String(httpCode);
    }
    
    http.end();
    payload = ""; 
    return finalResult;
}