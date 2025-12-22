#ifndef OPTIMIZED_CAMERA_UPLOAD_H
#define OPTIMIZED_CAMERA_UPLOAD_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "esp_camera.h"

// 引用 K10 库中的摄像头队列，无需重新初始化摄像头
extern QueueHandle_t xQueueCamer;

/**
 * @brief 最省资源的摄像头上传函数 (直接二进制流，无Base64，无大内存分配)
 * 
 * @param url 服务器接收地址 (例如 http://192.168.1.100:8000/upload)
 * @param quality JPEG质量 (10-63)，建议 30-40，画质够用且体积极小
 * @return String 服务器响应内容或错误信息
 */
String uploadCameraFrameOptimized(String url, int quality = 30) {
    camera_fb_t *frame = NULL;
    
    // 1. 从队列获取一帧 (通常是 RGB565 格式，取决于 K10 初始化配置)
    // 使用短超时，避免阻塞主循环
    if (xQueueReceive(xQueueCamer, &frame, pdMS_TO_TICKS(200)) != pdTRUE) {
        return "Error: Camera Busy or No Frame";
    }

    // 2. 转换为 JPEG (软件压缩)
    // frame2jpg 会自动分配内存，需要手动释放
    uint8_t *jpg_buf = NULL;
    size_t jpg_len = 0;
    
    // 使用较低质量(如30)可大幅减小体积 (通常 < 10KB)，提升上传速度并减少内存占用
    bool converted = frame2jpg(frame, quality, &jpg_buf, &jpg_len);
    
    // 关键优化：在转换完 JPEG 后立即归还原始帧给摄像头驱动
    // 这样摄像头任务可以立即开始采集下一帧，无需等待网络上传完成
    esp_camera_fb_return(frame);

    if (!converted) {
        return "Error: JPEG Compression Failed";
    }

    // 3. 开始上传 (直接使用二进制流)
    HTTPClient http;
    WiFiClient client; // 如果是 HTTPS，改用 WiFiClientSecure
    
    // 启动连接
    if (!http.begin(client, url)) {
        free(jpg_buf); // 务必释放内存
        return "Error: Connection Failed";
    }

    // 设置超时，避免网络卡死导致看门狗复位
    http.setTimeout(5000); 

    // 使用最轻量的二进制流上传
    // 服务器端应直接读取 request body 作为文件内容
    http.addHeader("Content-Type", "image/jpeg");
    
    // 也可以添加自定义头告诉服务器文件名
    http.addHeader("X-File-Name", "cam_capture.jpg");

    // 发送数据
    int httpCode = http.POST(jpg_buf, jpg_len);

    String response = "";
    if (httpCode > 0) {
        // 读取响应 (仅当有响应内容时)
        if (http.getSize() > 0) {
            response = http.getString();
        } else {
            response = "Success: " + String(httpCode);
        }
    } else {
        response = "Error: HTTP " + String(httpCode) + " " + http.errorToString(httpCode);
    }

    // 4. 清理资源
    free(jpg_buf); // 核心：释放 JPEG 缓冲区，防止内存泄漏
    http.end();

    return response;
}

#endif // OPTIMIZED_CAMERA_UPLOAD_H
