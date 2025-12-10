#ifndef BAIDU_OCR_H
#define BAIDU_OCR_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

class BaiduOCR {
public:
    BaiduOCR();
    ~BaiduOCR();

    /**
     * @brief 接口1: 初始化并获取 Access Token
     * @param apiKey 百度云控制台获取的 API Key
     * @param secretKey 百度云控制台获取的 Secret Key
     * @return true 获取Token成功
     */
    bool init(String apiKey, String secretKey);

    /**
     * @brief 接口2: 识别图片中的文字
     * @param imageBase64 图像的Base64字符串
     * @return String 识别到的所有文字（已拼接，已转码）
     */
    String recognizeText(String imageBase64);

private:
    String _accessToken;
    
    // 内部辅助函数
    String decodeUnicode(String str);
    String urlEncode(String str); // Base64必须进行URL编码才能发给OCR
};

#endif