#include "countUtf8Characters.h"

/**
 * 计算 UTF-8 字符串中的字符数 (而非字节数)
 */
int countUtf8Characters(const char* s) {
  int count = 0;
  for (int i = 0; s[i] != '\0'; i++) {
    // 检查字节是否为非后续字节 (即字符的起始字节)
    if ((s[i] & 0xC0) != 0x80) {
      count++;
    }
  }
  return count;
}

int countUtf8Characters(String s) {
  return countUtf8Characters(s.c_str());
}

/**
 * 获取 UTF-8 字符串中指定索引位置的字符
 */
String getUtf8CharPointer(const char* s, int charIndex) {
  int currentChar = 1;
  for (int i = 0; s[i] != '\0'; ) {
    // 获取当前字节
    unsigned char c = (unsigned char)s[i];
    int charLen = 0;
    
    // 判断 UTF-8 字符长度
    if ((c & 0x80) == 0) charLen = 1;
    else if ((c & 0xE0) == 0xC0) charLen = 2;
    else if ((c & 0xF0) == 0xE0) charLen = 3;
    else if ((c & 0xF8) == 0xF0) charLen = 4;
    else {
      // 无效字节，当做1字节处理继续跳过
      i++;
      continue; 
    }

    if (currentChar == charIndex) {
      // 找到目标字符，构建 String 返回
      // 检查边界防止溢出
      char buffer[5] = {0}; // 最多4字节 + null
      for (int k = 0; k < charLen && s[i+k] != '\0'; k++) {
        buffer[k] = s[i+k];
      }
      return String(buffer);
    }
    
    // 移动到下一个字符
    i += charLen;
    currentChar++;
  }
  return ""; // 未找到
}

String getUtf8CharPointer(String s, int charIndex) {
  return getUtf8CharPointer(s.c_str(), charIndex);
}

/**
 * 获取 UTF-8 字符在字符串中的位置 (索引)
 */
int getUtf8CharIndex(const char* s, const char* targetChar) {
  if (s == nullptr || targetChar == nullptr) return -1;
  
  int targetLen = strlen(targetChar);
  if (targetLen == 0) return -1;
  
  int charIndex = 1;
  for (int i = 0; s[i] != '\0'; i++) {
    // 检查字节是否为非后续字节 (即字符的起始字节)
    if ((s[i] & 0xC0) != 0x80) {
      // 检查当前位置是否匹配 targetChar
      if (strncmp(&s[i], targetChar, targetLen) == 0) {
        return charIndex;
      }
      charIndex++;
    }
  }
  return -1; // 未找到
}

int getUtf8CharIndex(String s, const char* targetChar) {
  return getUtf8CharIndex(s.c_str(), targetChar);
}

int getUtf8CharIndex(String s, String targetChar) {
  return getUtf8CharIndex(s.c_str(), targetChar.c_str());
}