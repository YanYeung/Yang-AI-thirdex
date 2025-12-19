# BaiduFaceAI Arduino Library

这是一个用于 ESP32 (或其他支持 HTTPClient/WiFiClientSecure 的 Arduino 平台) 的百度人脸识别 API 客户端库。

## 功能特性

- **人脸库管理**: 创建人脸库 (`createFaceGroup`)
- **人脸注册**: 将图片注册到指定人脸库 (`addFace`)
- **人脸搜索**: 在人脸库中搜索匹配的人脸 (`searchFace`)
- **表情分析**: 分析图片中的人脸表情 (`analyzeEmotion`)

## 依赖

- Arduino core for ESP32 (推荐)
- WiFi
- HTTPClient
- WiFiClientSecure

## 使用说明

### 1. 初始化

在使用任何功能前，需要先连接 WiFi 并初始化库。

```cpp
#include "BaiduFaceAI.h"

BaiduFaceAI faceAI;

void setup() {
    // 连接 WiFi...
    
    // 初始化 (替换为你的 API Key 和 Secret Key)
    if (faceAI.init("YOUR_API_KEY", "YOUR_SECRET_KEY")) {
        Serial.println("初始化成功");
    }
}
```

### 2. 创建人脸库

```cpp
// 创建名为 "my_group" 的人脸库
faceAI.createFaceGroup("my_group");
```

### 3. 注册人脸

```cpp
// 将 base64 编码的图片注册到 "my_group"，人脸名为 "user_alice"
// 注意: base64 字符串不应包含 "data:image/..." 前缀
faceAI.addFace(imageBase64String, "user_alice", "my_group");
```

### 4. 人脸搜索 (1:N)

```cpp
// 在 "my_group" 中搜索匹配的人脸
String resultName = faceAI.searchFace(searchImageBase64, "my_group");
Serial.println("匹配结果: " + resultName);
```

### 5. 表情分析

```cpp
String emotion = faceAI.analyzeEmotion(imageBase64String);
Serial.println("表情: " + emotion); // 例如 "开心 😄"
```

## 注意事项

1. **图片大小**: Base64 编码后的图片字符串可能很长，请确保 ESP32 有足够的 RAM。对于大图片，建议先进行压缩。
2. **HTTPS**: 库使用了 `setInsecure()` 跳过 SSL 验证，以减少资源消耗和避免证书过期问题。
3. **API 配额**: 请留意百度云控制台的 API 调用配额。
4. **错误处理**: 大部分函数在失败时会打印错误信息到 Serial 或返回错误字符串。

## 示例

请查看 `examples/FaceSearch/FaceSearch.ino` 获取完整示例。
