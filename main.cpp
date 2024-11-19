#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <filesystem>

// Root folder path
const std::string ROOT_FOLDER = "C:\\Users\\directory\\";
const std::string FORBIDDEN_SUBFOLDER = "C:\\Users\\your\\Downloads\\";  // Example for Downloads
const std::string APPDATA_SUBFOLDER = "C:\\Users\\your\\AppData\\";      // Restrict AppData as well

// Function to monitor clipboard content
void MonitorClipboard() {
    while (true) {
        if (OpenClipboard(nullptr)) {
            HANDLE hData = GetClipboardData(CF_TEXT); // Monitor text for simplicity
            if (hData) {
                char* pszText = static_cast<char*>(GlobalLock(hData));
                if (pszText != nullptr) {
                    std::string copiedPath(pszText);

                    // Check if the copied path is inside the root folder
                    if (copiedPath.find(ROOT_FOLDER) == 0) {
                        std::cout << "Clipboard contains path inside the root folder: " << copiedPath << "\n";

                        // Prevent copying to external locations
                        std::cout << "Clearing clipboard to restrict external pasting...\n";
                        EmptyClipboard();
                    } else {
                        std::cout << "Clipboard content allowed for pasting into the root folder.\n";
                    }
                }
                GlobalUnlock(hData);
            }
            CloseClipboard();
        }
        Sleep(500); // Adjust monitoring frequency
    }
}

// Function to monitor file operations in the root folder
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

            // Combine the root folder path and the file name to get the full path
            std::string fullPath(ROOT_FOLDER + std::string(fileName.begin(), fileName.end()));

            // Clean up double backslashes if needed
            size_t found = fullPath.find("\\\\");
            while (found != std::string::npos) {
                fullPath.replace(found, 2, "\\");
                found = fullPath.find("\\\\");
            }

            // Ensure the file operation is happening inside the root folder
            if (fullPath.find(ROOT_FOLDER) == 0 && fullPath.length() == ROOT_FOLDER.length() + fileName.length()) {
                // Additional check to ensure it's not inside the forbidden subfolder (Downloads, AppData)
                if (fullPath.find(FORBIDDEN_SUBFOLDER) != 0 && fullPath.find(APPDATA_SUBFOLDER) != 0) {
                    std::cout << "File operation detected: " << fullPath << "\n";
                    std::cout << "Operation allowed on: " << fullPath << "\n";
                } else {
                    std::cout << "File operation detected: " << fullPath << "\n";
                    std::cout << "Operation on this file is not allowed: " << fullPath << "\n";
                }
            } else {
                std::cout << "File operation detected: " << fullPath << "\n";
                std::cout << "Operation on this file is not allowed: " << fullPath << "\n";
            }

            pNotify = pNotify->NextEntryOffset ? reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<BYTE*>(pNotify) + pNotify->NextEntryOffset)
                                               : nullptr;
        } while (pNotify);
    }
    CloseHandle(hDir);
}

int main() {
    std::cout << "Starting clipboard and file operation monitoring...\n";

    // Run clipboard and file monitoring in parallel
    std::thread clipboardThread(MonitorClipboard);
    std::thread fileThread(MonitorFileOperations);

    clipboardThread.join();
    fileThread.join();

    return 0;
}
