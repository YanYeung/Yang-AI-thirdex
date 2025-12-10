#ifndef K10_IMAGE_PROCESSOR_H
#define K10_IMAGE_PROCESSOR_H

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include "JPEGENC.h"

class K10ImageProcessor {
public:
    K10ImageProcessor();
    ~K10ImageProcessor();

    // 初始化 SD 卡硬件
    bool begin();

    /**
     * @brief 核心功能：读取路径图片并转换为 Base64
     * @param imagePath 图片路径 (如 "/photo.bmp")
     * @return String Base64字符串 (失败返回 "")
     */
    String processImageToBase64(String imagePath);

private:
    // SD卡引脚
    const int _sd_cs = 40;
    const int _sd_mosi = 42;
    const int _sd_miso = 41;
    const int _sd_sck = 44;

    JPEGENC _jpeg;

    // 内部结构和函数
    struct BMPHeader;
    bool readBMPHeader(File& file, BMPHeader& header);
    bool convertBMPtoRGB(File& file, const BMPHeader& header, uint8_t* rgbBuffer);
    bool convertRGBtoJPEG(const uint8_t* rgbBuffer, int width, int height, String& jpegBase64);
    uint8_t extractColorComponent(uint16_t pixel, uint32_t mask);
    bool base64EncodeOptimized(const uint8_t* data, size_t length, String& encodedString);
};

#endif