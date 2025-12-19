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

    /**
     * @brief 接口3: 创建人脸库
     * @param group_name 人脸库名称 (建议由数字、字母、下划线组成，长度限制48B)
     * @return true 创建成功, false 失败
     */
    bool createFaceGroup(String group_name);

    /**
     * @brief 接口4: 注册人脸
     * @param image_base64 图片数据(Base64编码)
     * @param face_name 人脸姓名 (作为user_id, 由数字、字母、下划线组成，长度限制128B)
     * @param group_name 人脸库名称
     * @return true 注册成功, false 失败
     */
    bool addFace(String image_base64, String face_name, String group_name);

    /**
     * @brief 接口5: 搜索人脸
     * @param image_base64 待搜索图片数据(Base64编码)
     * @param group_name 人脸库名称
     * @return String 匹配度最高的人脸姓名，如果未匹配或出错返回错误信息
     */
    String searchFace(String image_base64, String group_name);

private:
    String _accessToken;
    
    // 内部辅助：发送HTTP POST请求
    String sendPostRequest(String url, String payload);

    // 内部辅助：解析表情英文为中文
    String translateEmotion(String rawEmotion);
};

#endif