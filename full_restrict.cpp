#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <filesystem>

namespace fs = std::filesystem;

// Define the root folder where operations are allowed
const fs::path ROOT_FOLDER = "D:\\Projects\\";   // change the path to your root directory

// Normalize file paths for consistent comparisons
std::string NormalizePath(const fs::path& path) {
    return path.string();
}

// Clear clipboard content to block unauthorized pasting
void ClearClipboard() {
    if (OpenClipboard(nullptr)) {
        EmptyClipboard();
        CloseClipboard();
    }
}

// Check if a path is within a directory
bool isWithinDirectory(const fs::path& directory, const fs::path& path) {
    try {
        return fs::relative(path, directory).has_parent_path();
    } catch (const fs::filesystem_error& e) {
        return false;
    }
}

// Monitor clipboard to restrict unauthorized pasting outside the root folder
void MonitorClipboard() {
    while (true) {
        if (OpenClipboard(nullptr)) {
            HANDLE hData = GetClipboardData(CF_HDROP);
            if (hData) {
                HDROP hDrop = static_cast<HDROP>(hData);
                UINT fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);

                for (UINT i = 0; i < fileCount; ++i) {
                    char filePath[MAX_PATH];
                    DragQueryFile(hDrop, i, filePath, MAX_PATH);
                    fs::path copiedPath(filePath);

                    if (!isWithinDirectory(ROOT_FOLDER, copiedPath)) {
                        std::cout << "Error: File cannot be copied outside the root folder: " << copiedPath << std::endl;
                        ClearClipboard();
                    } else {
                        std::cout << "Operation allowed within root folder or its subdirectories: " << copiedPath << std::endl;
                    }
                }
                CloseClipboard();
            }
        }
        Sleep(500);
    }
}

int main() {
    std::cout << "Clipboard monitoring started...\n";

    std::thread clipboardThread(MonitorClipboard);
    clipboardThread.join();

    return 0;
}
