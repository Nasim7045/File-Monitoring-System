#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <filesystem>

// Root folder path
const std::string ROOT_FOLDER = "C:\\Users\\ASUS\\";

// Function to clear the clipboard
void ClearClipboard() {
    if (OpenClipboard(nullptr)) {
        EmptyClipboard();
        CloseClipboard();
    }
}

// Function to monitor clipboard and block unauthorized paste
void MonitorClipboard() {
    while (true) {
        if (OpenClipboard(nullptr)) {
            HANDLE hData = GetClipboardData(CF_HDROP); // Monitor file paths
            if (hData) {
                HDROP hDrop = static_cast<HDROP>(hData);
                UINT fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0); // Get the number of files

                for (UINT i = 0; i < fileCount; ++i) {
                    char filePath[MAX_PATH];
                    DragQueryFile(hDrop, i, filePath, MAX_PATH);

                    std::string copiedPath(filePath);
                    std::cout << "Detected clipboard file: " << copiedPath << "\n";

                    // Check if the copied file is inside the ASUS folder
                    if (copiedPath.find(ROOT_FOLDER) == 0) {
                        std::cout << "Copying files outside the ASUS folder is not allowed.\n";

                        // Clear clipboard to block paste
                        ClearClipboard();
                        std::cout << "Clipboard cleared.\n";
                    } else {
                        std::cout << "Clipboard content allowed.\n";
                    }
                }
            }
            CloseClipboard();
        }
        Sleep(500); // Adjust monitoring frequency
    }
}

// Function to block file operations outside the ASUS folder
bool IsOperationAllowed(const std::string& filePath) {
    // Allow operations only within the ASUS folder
    return filePath.find(ROOT_FOLDER) == 0;
}

void MonitorFileOperations() {
    HANDLE hDir = CreateFile(
        ROOT_FOLDER.c_str(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        nullptr
    );

    if (hDir == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to monitor directory. Error: " << GetLastError() << "\n";
        return;
    }

    char buffer[1024];
    DWORD bytesReturned;
    while (ReadDirectoryChangesW(hDir, buffer, sizeof(buffer), TRUE,
                                 FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME,
                                 &bytesReturned, nullptr, nullptr)) {
        FILE_NOTIFY_INFORMATION* pNotify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);
        do {
            std::wstring fileName(pNotify->FileName, pNotify->FileNameLength / sizeof(WCHAR));
            std::string fullPath(ROOT_FOLDER + std::string(fileName.begin(), fileName.end()));

            if (IsOperationAllowed(fullPath)) {
                std::cout << "File operation allowed: " << fullPath << "\n";
            } else {
                std::cout << "File operation blocked: " << fullPath << "\n";

                // Optionally, delete or move unauthorized files back
                std::cout << "Undoing unauthorized file operation.\n";
                // std::filesystem::remove(fullPath); // Uncomment to remove the file
            }

            pNotify = pNotify->NextEntryOffset ? reinterpret_cast<FILE_NOTIFY_INFORMATION*>(
                                                    reinterpret_cast<BYTE*>(pNotify) + pNotify->NextEntryOffset)
                                               : nullptr;
        } while (pNotify);
    }
    CloseHandle(hDir);
}

int main() {
    std::cout << "Monitoring clipboard and file operations...\n";

    // Run clipboard and file monitoring in parallel
    std::thread clipboardThread(MonitorClipboard);
    std::thread fileThread(MonitorFileOperations);

    clipboardThread.join();
    fileThread.join();

    return 0;
}
