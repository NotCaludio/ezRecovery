#pragma once
#include <string>
#include <windows.h>
std::string calculateSHA256(const std::string& fileName);
std::string bytesToHex(const BYTE* bytes, size_t length);
int hexToInt(const std::string& hexString);
std::string sizeToString(size_t value);
size_t returnOffsetSizePosition(std::string fileType);
unsigned int returnLengthOfSize(std::string fileType);
int recoverMain(std::string restoreFolder, std::string drive);

