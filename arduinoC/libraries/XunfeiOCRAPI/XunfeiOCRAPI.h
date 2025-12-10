/**
 * 科大讯飞 OCR API 客户端
 * 用于 K10 设备端与 Python 服务器通信
 * 
 * 功能：
 * 1. 连接到本地 Python 服务器
 * 2. 发送 Base64 编码的图像数据
 * 3. 接收 OCR 识别结果
 */

#ifndef XUNFEI_OCR_API_H
#define XUNFEI_OCR_API_H

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "mbedtls/md5.h"
#include "mbedtls/base64.h"

// ==================== 结果结构体 ====================
// 避免与其他库(例如 LocalOCRAPI)重复定义相同名称的结构体，
// 使用预处理宏在第一次定义时声明 OCRResult。
// 为避免与其他模块冲突，使用独立的结果类型名 `XunfeiOCRResult`。
#ifndef XUNFEI_OCRRESULT_STRUCT_DEFINED
#define XUNFEI_OCRRESULT_STRUCT_DEFINED
struct XunfeiOCRResult {
    bool success;              // 识别是否成功
    String message;            // 状态或错误信息
    String text;               // 识别的文本内容
    float confidence;          // 置信度 (0.0 - 1.0)
};
#endif

// ==================== API 客户端类 ====================
class XunfeiOCRAPI {
private:
    String _appId;
    String _apiKey;
    
    // MD5 加密
    String md5(String data);
    
    // Base64 编码
    String base64Encode(const unsigned char* data, size_t length);
    
public:
    XunfeiOCRAPI();
    
    /**
     * 初始化 API 客户端
     * @param appId 科大讯飞 APPID
     * @param apiKey 科大讯飞 APIKey
     */
    bool init(String appId, String apiKey);
    
    /**
     * 调用讯飞 OCR API 进行文字识别
     * @param base64Image Base64 编码的 JPG 图像数据
     */
    XunfeiOCRResult recognize(const String& base64Image);
};

#endif
