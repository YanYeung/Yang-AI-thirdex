#include "K10ImageProcessor.h"

// ==================== 1. 结构体定义 ====================
struct K10ImageProcessor::BMPHeader {
    char signature[2];
    uint32_t file_size;
    uint32_t reserved;
    uint32_t data_offset;
    uint32_t header_size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bit_count;
    uint32_t compression;
    uint32_t image_size;
    int32_t x_pixels_per_meter;
    int32_t y_pixels_per_meter;
    uint32_t colors_used;
    uint32_t colors_important;
    uint32_t red_mask;
    uint32_t green_mask;
    uint32_t blue_mask;
};

// ==================== 2. JPEGENC 回调 (必须是全局) ====================
static uint8_t* _jpegBuffer = NULL;
static int _jpegBufferSize = 0;
static int _jpegDataSize = 0;

void * jpegOpen(const char *filename) { return NULL; }
void jpegClose(JPEGE_FILE *p) {}
int32_t jpegRead(JPEGE_FILE *p, uint8_t *buffer, int32_t length) { return 0; }
int32_t jpegSeek(JPEGE_FILE *p, int32_t position) { return 0; }
int32_t jpegWrite(JPEGE_FILE *p, uint8_t *buffer, int32_t length) {
    if (_jpegBuffer && _jpegDataSize + length <= _jpegBufferSize) {
        memcpy(_jpegBuffer + _jpegDataSize, buffer, length);
        _jpegDataSize += length;
        return length;
    }
    return 0;
}

// ==================== 3. 类实现 ====================
K10ImageProcessor::K10ImageProcessor() {}
K10ImageProcessor::~K10ImageProcessor() {}

bool K10ImageProcessor::begin() {
    SPI.begin(_sd_sck, _sd_miso, _sd_mosi, _sd_cs);
    return SD.begin(_sd_cs);
}

String K10ImageProcessor::processImageToBase64(String imagePath) {
    if (!SD.exists(imagePath)) {
        Serial.printf("❌ 文件不存在: %s\n", imagePath.c_str());
        return "";
    }
    File bmpFile = SD.open(imagePath);
    if (!bmpFile) return "";

    BMPHeader header;
    if (!readBMPHeader(bmpFile, header)) {
        bmpFile.close(); return "";
    }

    int width = header.width;
    int height = abs(header.height);
    size_t rgbBufferSize = width * height * 3;
    
    // 支持更大的图片（增加到 500KB）
    if (rgbBufferSize > 500000) { 
         bmpFile.close(); return "";
    }

    uint8_t* rgbBuffer = (uint8_t*)malloc(rgbBufferSize);
    if (!rgbBuffer) {
        bmpFile.close(); return "";
    }

    if (!convertBMPtoRGB(bmpFile, header, rgbBuffer)) {
        free(rgbBuffer); bmpFile.close(); return "";
    }
    bmpFile.close();

    String jpegBase64 = "";
    convertRGBtoJPEG(rgbBuffer, width, height, jpegBase64); // 即使失败内部也会清理

    free(rgbBuffer);
    return jpegBase64;
}

// ==================== 4. 辅助函数 (完整实现) ====================
// ... 这里粘贴之前 convertBMPtoRGB, convertRGBtoJPEG, scaleImage, base64EncodeOptimized 的完整代码 ...
// ... 为了篇幅，请直接复用上一版修正后的 SmartMuseumK10.cpp 中的这些函数 ...
// ... 注意类名要改为 K10ImageProcessor:: ...
// 下面仅列出 convertRGBtoJPEG 的适配，其他大同小异

bool K10ImageProcessor::convertRGBtoJPEG(const uint8_t* rgbBuffer, int width, int height, String& jpegBase64) {
    // 使用原始尺寸，不进行缩放
    _jpegBufferSize = 200000;  // 增加缓冲区以容纳更大的 JPG
    _jpegBuffer = (uint8_t*)malloc(_jpegBufferSize);
    _jpegDataSize = 0;
    
    if (!_jpegBuffer) return false;
    
    if (_jpeg.open(_jpegBuffer, _jpegBufferSize) != JPEGE_SUCCESS) {
        free(_jpegBuffer); _jpegBuffer = NULL; return false;
    }

    JPEGENCODE jpe;
    int rc = _jpeg.encodeBegin(&jpe, width, height, JPEGE_PIXEL_RGB888, JPEGE_SUBSAMPLE_420, JPEGE_Q_MED);
    if (rc != JPEGE_SUCCESS) {
        _jpeg.close(); free(_jpegBuffer); _jpegBuffer = NULL; return false;
    }
    
    // 直接使用原始尺寸的缓冲区，不进行缩放
    rc = _jpeg.addFrame(&jpe, (uint8_t*)rgbBuffer, width * 3);
    
    if (rc != JPEGE_SUCCESS) {
        _jpeg.close(); free(_jpegBuffer); _jpegBuffer = NULL; return false;
    }
    
    int finalSize = _jpeg.close();
    bool encodeSuccess = base64EncodeOptimized(_jpegBuffer, finalSize, jpegBase64);
    free(_jpegBuffer); _jpegBuffer = NULL;
    return encodeSuccess;
}

// 补充缺失的辅助函数空壳，请务必填入之前的实现
bool K10ImageProcessor::readBMPHeader(File& file, BMPHeader& header) { 
    // ... 复制之前的代码 ...
    uint8_t buffer[54];
    if (file.read(buffer, 54) != 54) return false;
    memcpy(header.signature, buffer, 2);
    header.file_size = *(uint32_t*)(buffer + 2);
    header.data_offset = *(uint32_t*)(buffer + 10);
    header.width = *(int32_t*)(buffer + 18);
    header.height = *(int32_t*)(buffer + 22);
    header.bit_count = *(uint16_t*)(buffer + 28);
    header.compression = *(uint32_t*)(buffer + 30);
    if (header.compression == 3) {
        uint8_t maskBuffer[12];
        if (file.read(maskBuffer, 12) != 12) return false;
        header.red_mask = *(uint32_t*)maskBuffer;
        header.green_mask = *(uint32_t*)(maskBuffer + 4);
        header.blue_mask = *(uint32_t*)(maskBuffer + 8);
    } else {
        header.red_mask = 0xF800; header.green_mask = 0x07E0; header.blue_mask = 0x001F;
    }
    return true;
}

bool K10ImageProcessor::convertBMPtoRGB(File& file, const BMPHeader& header, uint8_t* rgbBuffer) {
    // ... 复制之前的代码 ...
    file.seek(header.data_offset);
    int width = header.width; int height = abs(header.height);
    int rowSize = ((width * header.bit_count + 31) / 32) * 4;
    for (int y = 0; y < height; y++) {
        uint8_t rowBuffer[1024];
        if (file.read(rowBuffer, rowSize) != rowSize) return false;
        for (int x = 0; x < width; x++) {
            int targetIndex = (header.height > 0 ? (height - 1 - y) : y) * width + x;
            if (header.height > 0) targetIndex = (height - 1 - y) * width + x; else targetIndex = y * width + x;
            int pixelIndex = x * (header.bit_count / 8);
            if (header.bit_count == 16) {
                uint16_t pixel = *((uint16_t*)(rowBuffer + pixelIndex));
                rgbBuffer[targetIndex * 3] = extractColorComponent(pixel, header.red_mask);
                rgbBuffer[targetIndex * 3 + 1] = extractColorComponent(pixel, header.green_mask);
                rgbBuffer[targetIndex * 3 + 2] = extractColorComponent(pixel, header.blue_mask);
            } else if (header.bit_count == 24 || header.bit_count == 32) {
                rgbBuffer[targetIndex * 3] = rowBuffer[pixelIndex + 2];
                rgbBuffer[targetIndex * 3 + 1] = rowBuffer[pixelIndex + 1];
                rgbBuffer[targetIndex * 3 + 2] = rowBuffer[pixelIndex];
            }
        }
    }
    return true;
}

uint8_t K10ImageProcessor::extractColorComponent(uint16_t pixel, uint32_t mask) {
    if (mask == 0) return 0;
    int shift = 0; int bits = 0; uint32_t temp = mask;
    while ((temp & 1) == 0) { shift++; temp >>= 1; }
    while (temp & 1) { bits++; temp >>= 1; }
    uint32_t component = (pixel & mask) >> shift;
    if (bits == 0) return 0;
    return (component << (8 - bits)) | (component >> (2 * bits - 8));
}

bool K10ImageProcessor::base64EncodeOptimized(const uint8_t* data, size_t length, String& encodedString) {
    // 使用标准 Base64 编码
    const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    encodedString = ""; 
    size_t totalEncodedSize = ((length + 2) / 3) * 4;
    if (totalEncodedSize > 500000) return false;  // 增加到 500KB
    encodedString.reserve(totalEncodedSize);
    
    for (size_t i = 0; i < length; i += 3) {
        uint32_t val = data[i] << 16;
        if (i + 1 < length) val |= data[i + 1] << 8;
        if (i + 2 < length) val |= data[i + 2];
        
        encodedString += base64_chars[(val >> 18) & 0x3F];
        encodedString += base64_chars[(val >> 12) & 0x3F];
        if (i + 1 < length) encodedString += base64_chars[(val >> 6) & 0x3F]; else encodedString += '=';
        if (i + 2 < length) encodedString += base64_chars[val & 0x3F]; else encodedString += '=';
        
        if ((i + 3) % 256 == 0) yield();
    }
    return true;
}