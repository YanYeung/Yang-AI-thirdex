/**
 * 科大讯飞 人脸检测 API 客户端
 * 用于 K10 设备端与 Python 服务器通信
 */

#ifndef XUNFEI_FACE_API_H
#define XUNFEI_FACE_API_H

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "mbedtls/md.h"
#include "mbedtls/base64.h"
#include "mbedtls/sha256.h"

// ==================== 结果结构体 ====================
struct XunfeiFaceResult {
    bool success;              // 是否成功
    String message;            // 状态或错误信息
    String expression;         // 表情 (例如: happy, sad, neutral)
    float score;               // 置信度 (0.0 - 1.0)
    String gender;             // 性别
};

// ==================== API 客户端类 ====================
class XunfeiFaceAPI {
private:
    String _appId;
    String _apiKey;
    String _apiSecret;
    
    // 生成鉴权 URL
    String getAuthUrl(String host, String path);
    
    // HMAC-SHA256 加密
    String hmacSha256(String data, String key);
    
    // Base64 编码
    String base64Encode(const unsigned char* data, size_t length);
    
public:
    XunfeiFaceAPI();
    
    /**
     * 初始化 API 客户端
     * @param appId 科大讯飞 APPID
     * @param apiKey 科大讯飞 APIKey
     * @param apiSecret 科大讯飞 APISecret
     */
    bool init(String appId, String apiKey, String apiSecret);
    
    /**
     * 调用讯飞 人脸检测 API
     * @param base64Image Base64 编码的 JPG 图像数据
     */
    XunfeiFaceResult detect(const String& base64Image);
};

#endif
