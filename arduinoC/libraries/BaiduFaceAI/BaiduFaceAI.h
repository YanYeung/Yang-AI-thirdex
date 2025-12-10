#ifndef BAIDU_FACE_AI_H
#define BAIDU_FACE_AI_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

class BaiduFaceAI {
public:
    BaiduFaceAI();
    ~BaiduFaceAI();

    /**
     * @brief 接口1: 初始化并获取 Access Token
     * @param apiKey 百度云控制台获取的 API Key
     * @param secretKey 百度云控制台获取的 Secret Key
     * @return true 获取Token成功, false 失败
     */
    bool init(String apiKey, String secretKey);

    /**
     * @brief 接口2: 分析表情
     * @param imageBase64 图像的Base64字符串
     * @return String 返回中文表情结果 (如 "开心 😄")，如果失败返回错误信息
     */
    String analyzeEmotion(String imageBase64);

private:
    String _accessToken;
    
    // 内部辅助：解析表情英文为中文
    String translateEmotion(String rawEmotion);
};

#endif