#include <windows.h>
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

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
std::string sizeToString(size_t value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

int main() {
    HANDLE hDrive = CreateFile(
        L"\\\\.\\F:",
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

    // Map of file types and their corresponding signatures
    std::map<std::string, std::vector<std::string>> fileSignatures = {
        {"JPEG", {"FFD8FFE000104A4649460001", "FFD8FFDB", "FFD8FFEE", "FFD8FFE0"}},
        {"PNG", {"89504E470D0A1A0A"}},
        {"GIF", {"474946383761", "474946383961"}},
        {"PDF", {"255044462D"}},
        {"ZIP", {"504B0304", "504B0506", "504B0708"}}
        // Add more file types and their signatures as needed
    };

    std::map<size_t, std::string> foundFiles;

    size_t currentOffset = 0;
    size_t counter = 0;

    while (ReadFile(hDrive, buffer, bufferSize, &bytesRead, NULL) && bytesRead > 0) {
        // Convert the buffer to hexadecimal string
        std::string hexData = bytesToHex(buffer, bytesRead);
        std::cout << hexData << std::endl;
        // Check for file signatures in the hexadecimal data
        for (const auto& filePair : fileSignatures) {
            const std::string& fileType = filePair.first;
            const std::vector<std::string>& signatures = filePair.second;
            counter ++ ;
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

    std::string restoreFolder = "C:\\Users\\claud\\Documents\\Sexto_semestre\\proyecto_integracion_tecnologica\\ezRecovery\\test\\recovered";

    // Restore the found files
    for (const auto& filePair : foundFiles) {
        size_t startOffset = filePair.first;
        const std::string& fileType = filePair.second;

        // Move the file pointer to the start offset
        SetFilePointer(hDrive, static_cast<LONG>(startOffset), NULL, FILE_BEGIN);

        // Create a new file for writing the extracted data
        std::string fileName = restoreFolder + "\\recovered_" + sizeToString(startOffset) + "." + fileType;
        std::ofstream outputFile(fileName, std::ios::binary);

        // Read and write the file contents until the end of the file or the next found file
        size_t bytesToRead = bufferSize;
        while (ReadFile(hDrive, buffer, static_cast<DWORD>(bytesToRead), &bytesRead, NULL) && bytesRead > 0) {
            outputFile.write(reinterpret_cast<const char*>(buffer), bytesRead);

            // Check if we have reached the next found file
            auto range = foundFiles.equal_range(startOffset);
            auto nextFile = range.second;
            if (nextFile != foundFiles.end()) {
                size_t endOffset = nextFile->first;
                bytesToRead = endOffset - (startOffset + bytesRead);
            }
        }

        outputFile.close();
        std::cout << "Restored file: " << fileName << std::endl;
    }


    return 0;
}