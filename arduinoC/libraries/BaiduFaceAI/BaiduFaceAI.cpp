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

    // 构建请求体，只请求 emotion 字段以节省流量和时间
    String payload = "{\"image\":\"" + imageBase64 + "\",\"image_type\":\"BASE64\",\"face_field\":\"emotion\"}";
    String url = "https://aip.baidubce.com/rest/2.0/face/v3/detect?access_token=" + _accessToken;
    
    String response = sendPostRequest(url, payload);
    
    if (response.length() == 0) return "网络错误";
    
    // 检查是否有错误码
    if (response.indexOf("\"error_code\":0") != -1) {
        // 解析 result -> face_list -> emotion -> type
        int emoIndex = response.indexOf("\"emotion\":{\"type\":\"");
        if (emoIndex != -1) {
            int start = emoIndex + 19;
            int end = response.indexOf("\"", start);
            String rawEmotion = response.substring(start, end);
            return translateEmotion(rawEmotion);
        } else {
            return "解析失败";
        }
    } else {
        if (response.indexOf("pic not has face") != -1) return "未检测到人脸";
        
        // 尝试提取错误信息
        int msgStart = response.indexOf("\"error_msg\":\"");
        if (msgStart != -1) {
            msgStart += 13;
            int msgEnd = response.indexOf("\"", msgStart);
            return "API错误: " + response.substring(msgStart, msgEnd);
        }
        return "API调用错误";
    }
}

// 接口3实现: 创建人脸库
bool BaiduFaceAI::createFaceGroup(String group_name) {
    if (_accessToken.length() == 0) {
        Serial.println("❌ 错误: 未初始化Token");
        return false;
    }
    
    String url = "https://aip.baidubce.com/rest/2.0/face/v3/faceset/group/add?access_token=" + _accessToken;
    String payload = "{\"group_id\":\"" + group_name + "\"}";
    
    String response = sendPostRequest(url, payload);
    
    if (response.indexOf("\"error_code\":0") != -1) {
        Serial.println("✅ 人脸库创建成功: " + group_name);
        return true;
    } else {
        // 错误处理，如果错误是 "group already exist" (error_code 223101)，也可以视为成功或打印特定信息
        // 使用 error_code 判断更准确，避免 error_msg 文本差异
        if (response.indexOf("\"error_code\":223101") != -1) {
             Serial.println("⚠️ 人脸库已存在: " + group_name);
             return true; 
        }
        Serial.println("❌ 人脸库创建失败: " + response);
        return false;
    }
}

// 接口4实现: 注册人脸
bool BaiduFaceAI::addFace(String image_base64, String face_name, String group_name) {
    if (_accessToken.length() == 0) return false;
    
    String url = "https://aip.baidubce.com/rest/2.0/face/v3/faceset/user/add?access_token=" + _accessToken;
    String payload = "{\"image\":\"" + image_base64 + "\",\"image_type\":\"BASE64\",\"group_id\":\"" + group_name + "\",\"user_id\":\"" + face_name + "\"}";
    
    String response = sendPostRequest(url, payload);
    
    if (response.indexOf("\"error_code\":0") != -1) {
        Serial.println("✅ 人脸注册成功: " + face_name);
        return true;
    } else {
        Serial.println("❌ 人脸注册失败: " + response);
        return false;
    }
}

// 接口5实现: 搜索人脸
String BaiduFaceAI::searchFace(String image_base64, String group_name) {
    if (_accessToken.length() == 0) return "错误: 未初始化Token";
    
    String url = "https://aip.baidubce.com/rest/2.0/face/v3/search?access_token=" + _accessToken;
    // user_list 默认为空，返回score最高的
    String payload = "{\"image\":\"" + image_base64 + "\",\"image_type\":\"BASE64\",\"group_id_list\":\"" + group_name + "\"}";
    
    String response = sendPostRequest(url, payload);
    
    if (response.indexOf("\"error_code\":0") != -1) {
        // 解析 result -> user_list -> [0] -> user_id, score
        // 简单解析，假设第一个就是最高的
        int listStart = response.indexOf("\"user_list\":[");
        if (listStart != -1) {
            // 查找 score
            int scoreIndex = response.indexOf("\"score\":", listStart);
            if (scoreIndex != -1) {
                scoreIndex += 8; // len("\"score\":")
                int scoreEnd = response.indexOf(",", scoreIndex);
                if (scoreEnd == -1) scoreEnd = response.indexOf("}", scoreIndex); // 可能是最后一个字段
                
                String scoreStr = response.substring(scoreIndex, scoreEnd);
                float score = scoreStr.toFloat();
                
                if (score > 80.0) { // 阈值设定，通常80分以上比较可信
                    int userIndex = response.indexOf("\"user_id\":\"", listStart);
                    if (userIndex != -1) {
                        userIndex += 11;
                        int userEnd = response.indexOf("\"", userIndex);
                        String userId = response.substring(userIndex, userEnd);
                        return userId; // 返回匹配到的名字
                    }
                } else {
                    return "未匹配到已知人脸 (置信度低: " + String(score) + ")";
                }
            }
        }
        return "解析搜索结果失败";
    } else {
        int msgStart = response.indexOf("\"error_msg\":\"");
        if (msgStart != -1) {
             msgStart += 13;
             int msgEnd = response.indexOf("\"", msgStart);
             return "搜索失败: " + response.substring(msgStart, msgEnd);
        }
        return "搜索失败: API调用错误";
    }
}

// 内部辅助：发送HTTP POST请求
String BaiduFaceAI::sendPostRequest(String url, String payload) {
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;
    
    // 设置超时
    http.setTimeout(10000); // 10s
    
    if (!http.begin(client, url)) {
        return "";
    }
    
    http.addHeader("Content-Type", "application/json");
    
    int httpCode = http.POST(payload);
    String response = "";
    
    if (httpCode > 0) {
        response = http.getString();
    } else {
        Serial.printf("❌ HTTP POST Failed: %s\n", http.errorToString(httpCode).c_str());
    }
    
    http.end();
    return response;
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
