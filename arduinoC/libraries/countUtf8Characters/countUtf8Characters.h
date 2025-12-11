#ifndef COUNT_UTF8_CHARACTERS_H
#define COUNT_UTF8_CHARACTERS_H

#include <Arduino.h>

/**
 * 计算 UTF-8 字符串中的字符数 (而非字节数)
 * @param s UTF-8 编码的字符串
 * @return 字符数量
 */
int countUtf8Characters(const char* s);
int countUtf8Characters(String s);

/**
 * 获取 UTF-8 字符串中指定索引位置的字符
 * @param s UTF-8 编码的字符串
 * @param charIndex 目标字符的索引 (从1开始)
 * @return 包含该位置字符的字符串，如果索引越界则返回空字符串
 */
String getUtf8CharPointer(const char* s, int charIndex);
String getUtf8CharPointer(String s, int charIndex);

/**
 * 获取 UTF-8 字符在字符串中的位置 (索引)
 * @param s UTF-8 编码的字符串
 * @param targetChar 要查找的字符 (UTF-8 字符串)
 * @return 字符在字符串中的索引 (从1开始), 如果未找到返回 -1
 */
int getUtf8CharIndex(const char* s, const char* targetChar);
int getUtf8CharIndex(String s, const char* targetChar);
int getUtf8CharIndex(String s, String targetChar);

#endif // COUNT_UTF8_CHARACTERS_H
