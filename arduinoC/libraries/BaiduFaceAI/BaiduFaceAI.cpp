#include "BaiduFaceAI.h"

BaiduFaceAI::BaiduFaceAI() : _accessToken("") {}
BaiduFaceAI::~BaiduFaceAI() {}

// 接口1实现: 获取 Token
bool BaiduFaceAI::init(String apiKey, String secretKey) {
    if (apiKey.length() == 0 || secretKey.length() == 0) return false;
    
    WiFiClientSecure client;
    client.setInsecure(); // 忽略SSL证书验证
    HTTPClient http;
    
    String url = "https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials&client_id=" + apiKey + "&client_secret=" + secretKey;
    
    Serial.println("🔑 正在获取百度 Access Token...");
    
    if (http.begin(client, url)) {
        int httpCode = http.GET();
        if (httpCode == 200) {
            String payload = http.getString();
            int start = payload.indexOf("\"access_token\":\"");
            if (start != -1) {
                start += 16;
                int end = payload.indexOf("\"", start);
                _accessToken = payload.substring(start, end);
                Serial.println("✅ Token 获取成功");
                http.end();
                return true;
            }
        } else {
            Serial.printf("❌ Token 请求失败: %d\n", httpCode);
        }
        http.end();
    }
    return false;
}

// 接口2实现: 分析表情
String BaiduFaceAI::analyzeEmotion(String imageBase64) {
    if (_accessToken.length() == 0) return "错误: 未初始化Token";
    if (imageBase64.length() == 0) return "错误: 图片数据为空";

    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;

    String url = "https://aip.baidubce.com/rest/2.0/face/v3/detect?access_token=" + _accessToken;
    
    if (!http.begin(client, url)) return "错误: 连接百度失败";

    http.addHeader("Content-Type", "application/json");
    
    // 构建请求体，只请求 emotion 字段以节省流量和时间
    String payload = "{\"image\":\"" + imageBase64 + "\",\"image_type\":\"BASE64\",\"face_field\":\"emotion\"}";
    
    int httpCode = http.POST(payload);
    String resultText = "未知";

    if (httpCode == 200) {
        String response = http.getString();
        
        // 检查是否有错误码
        if (response.indexOf("\"error_code\":0") != -1) {
            // 解析 result -> face_list -> emotion -> type
            int emoIndex = response.indexOf("\"emotion\":{\"type\":\"");
            if (emoIndex != -1) {
                int start = emoIndex + 19;
                int end = response.indexOf("\"", start);
                String rawEmotion = response.substring(start, end);
                resultText = translateEmotion(rawEmotion);
            } else {
                resultText = "解析失败";
            }
        } else {
            if (response.indexOf("pic not has face") != -1) resultText = "未检测到人脸";
            else resultText = "API调用错误";
        }
    } else {
        resultText = "网络错误: " + String(httpCode);
    }
    
    http.end();
    return resultText;
}

// 内部辅助: 翻译字典
String BaiduFaceAI::translateEmotion(String rawEmotion) {
    if (rawEmotion == "angry") return "愤怒 😡";
    if (rawEmotion == "disgust") return "厌恶 🤢";
    if (rawEmotion == "fear") return "恐惧 😱";
    if (rawEmotion == "happy") return "开心 😄";
    if (rawEmotion == "sad") return "伤心 😢";
    if (rawEmotion == "surprise") return "惊讶 😲";
    if (rawEmotion == "neutral") return "平静 😐";
    if (rawEmotion == "pouty") return "嘟嘴 😚";
    if (rawEmotion == "grimace") return "鬼脸 🤡";
    return rawEmotion;
}