#include <windows.h>
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <openssl/evp.h>
#include <iomanip>

std::string calculateSHA256(const std::string& fileName) {
    std::ifstream file(fileName, std::ios::binary);
    if (!file) {
        return "";
    }

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (ctx == nullptr) {
        return "";
    }

    if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1) {
        EVP_MD_CTX_free(ctx);
        return "";
    }

    const int bufferSize = 4096;
    char buffer[bufferSize];
    while (file.good()) {
        file.read(buffer, bufferSize);
        if (EVP_DigestUpdate(ctx, buffer, file.gcount()) != 1) {
            EVP_MD_CTX_free(ctx);
            return "";
        }
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLength = 0;
    if (EVP_DigestFinal_ex(ctx, hash, &hashLength) != 1) {
        EVP_MD_CTX_free(ctx);
        return "";
    }

    EVP_MD_CTX_free(ctx);

    std::stringstream ss;
    for (unsigned int i = 0; i < hashLength; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return ss.str();
}

// Function to convert byte array to hexadecimal string
std::string bytesToHex(const BYTE* bytes, size_t length) {
    std::string hex;
    for (size_t i = 0; i < length; ++i) {
        char hexChar[3];
        sprintf_s(hexChar, "%02X", bytes[i]);
        hex += hexChar;
    }
    return hex;
}

int hexToInt(const std::string& hexString) {
    int value = 0;

    for (char c : hexString) {
        if (c >= '0' && c <= '9') {
            value = value * 16 + (c - '0');
        }
        else if (c >= 'A' && c <= 'F') {
            value = value * 16 + (c - 'A' + 10);
        }
        else if (c >= 'a' && c <= 'f') {
            value = value * 16 + (c - 'a' + 10);
        }
        else {
            // Invalid character, return 0 or handle the error as needed
            return 0;
        }
    }

    return value;
}
std::string sizeToString(size_t value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

size_t returnOffsetSizePosition(std::string fileType) {
    std::transform(fileType.begin(), fileType.end(), fileType.begin(),
        [](unsigned char c) { return std::tolower(c); });
    if (fileType == "mp5") {
        return 36; // This returns where space is located
    }
    return 0;
}
unsigned int returnLengthOfSize(std::string fileType) {
    std::transform(fileType.begin(), fileType.end(), fileType.begin(),
        [](unsigned char c) { return std::tolower(c); });
    if (fileType == "mp5") {
        return 4; // This returns how many bytes are used for the space
    }
    return 0;
}

int main() {

    std::string restoreFolder;
    std::cout << "Path where restore files will be stored:" << std::endl;
    std::cin >> restoreFolder;


    HANDLE hDrive = CreateFile(
        L"\\\\.\\G:",
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (hDrive == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open physical drive. Error: " << GetLastError() << std::endl;
        return 1;
    }

    const size_t bufferSize = 1024;
    BYTE buffer[bufferSize];
    DWORD bytesRead;

    // Map of file types and their corresponding multiple signatures
    std::map<std::string, std::vector<std::string>> fileSignatures = {
        {"jpeg", {"FFD8FFE000104A4649460001", "FFD8FFDB", "FFD8FFEE", "FFD8FFE0"}},
        {"png", {"89504E470D0A1A0A"}},
        {"gif", {"474946383761", "474946383961"}},
        {"pdf", {"255044462D"}},
        {"zip", {"504B0304", "504B0506", "504B0708"}},
        {"txt", {"EFBBBF"}},
        //{"docx", {"504B030414000600"}}, Current implementation doesn't support office files
        {"mp4", {"667479704D534E56", "6674797069736F6D"}}
    };

    std::map<size_t, std::string> foundFiles;

    size_t currentOffset = 0;


    while (ReadFile(hDrive, buffer, bufferSize, &bytesRead, NULL) && bytesRead > 0) {
        // Convert the buffer to hexadecimal string
        std::string hexData = bytesToHex(buffer, bytesRead);
        // Check for file signatures in the hexadecimal data
        for (const auto& filePair : fileSignatures) {
            const std::string& fileType = filePair.first;
            const std::vector<std::string>& signatures = filePair.second;
            for (const std::string& signature : signatures) {
                size_t pos = hexData.find(signature);
                if (pos != std::string::npos) {
                     std::cout << "Found " << fileType << " file signature at offset: "
                        << pos / 2 << " bytes" << std::endl;
                    size_t startOffset = currentOffset + pos / 2;
                    foundFiles[startOffset] = fileType;
                    
                    break;  // Move to the next file type
                }
            }
        }
        currentOffset += bytesRead;
    }
    std::ofstream hashFile(restoreFolder + "\\hashes.txt", std::ios::app);
    
    
    // Restore the found files
    for (std::map<size_t, std::string>::iterator filePairIt = foundFiles.begin(); filePairIt != foundFiles.end(); ++filePairIt) {
        size_t startOffset = filePairIt->first;
        const std::string& fileType = filePairIt->second;

        // Move the file pointer to the start offset
        SetFilePointer(hDrive, static_cast<LONG>(startOffset), NULL, FILE_BEGIN);

        // Create a new file for writing the extracted data
        std::string fileName = restoreFolder + "\\recovered_" + sizeToString(startOffset) + "." + fileType;
        std::ofstream outputFile(fileName, std::ios::binary);

        // Read and write the file contents until the end of the file or the next found file
        size_t bytesToRead = bufferSize;
        size_t currentBytesRead = 0;
        size_t endOffset = 0;
        size_t positionOfSize = returnOffsetSizePosition(fileType);
        size_t fileSize;
        bool endOfFileFound = false;
        while (ReadFile(hDrive, buffer, static_cast<DWORD>(bytesToRead), &bytesRead, NULL) && bytesRead > 0) {
            outputFile.write(reinterpret_cast<const char*>(buffer), bytesRead);
            if (std::next(filePairIt) != foundFiles.end()) //if there is another file, the file ends before that file
                endOffset = std::next(filePairIt)->first - bufferSize; // it's necessary to substract buffersize, or it will consume bytes from the other file
            // Check if it is a mp4 file so it can search for the mdat chunk
            else if (fileType == "mp4" && !endOfFileFound)
            {
                std::string hexData = bytesToHex(buffer, bytesRead);
                size_t pos = hexData.find("6D646174");
                if (pos != std::string::npos) {
                    size_t startSizeOffset =  pos / 2 +4;
                    BYTE bufferOfSize[bufferSize];
                    for (unsigned int i = 0; i < 4; ++i) 
                        bufferOfSize[i] = buffer[i + startSizeOffset];
                    fileSize = hexToInt(bytesToHex(bufferOfSize, 4));
                    endOffset = fileSize + startOffset;
                    endOfFileFound = true;
                }
            }
            // File formats that have specific location of their size
            else if (positionOfSize && !endOfFileFound) {
                unsigned int lengthOfSize = returnLengthOfSize(fileType);
                BYTE bufferOfSize[bufferSize];
                for (unsigned int i = 0; i < lengthOfSize; ++i) {
                    bufferOfSize[i] = buffer[i + positionOfSize];
                }
                fileSize = hexToInt(bytesToHex(bufferOfSize, lengthOfSize));
                endOffset = fileSize + startOffset;
                endOfFileFound = true;
            }
            bytesToRead = endOffset - (startOffset + currentBytesRead);
            // Until the bytes to read are less than 1024 it should read 1024
            if (bytesToRead > 1024)
                bytesToRead = 1024;
            currentBytesRead += bytesRead;
        }

        outputFile.close();
        std::cout << "Restored file: " << fileName << std::endl;

        // Calculate and store the hash of the restored file
        std::string hash = calculateSHA256(fileName);
        hashFile << fileName << ": " << hash << std::endl;
        
    }
    if(hashFile.is_open())
        hashFile.close();

    
    return 0;
}