#include <windows.h> // Windows-specific API for clipboard access, file monitoring, and handling paths
#include <iostream>  // Provides standard input/output stream objects (e.g., std::cout, std::cerr)
#include <string>    // Used for handling and manipulating string objects
#include <thread>    // Enables multithreading for concurrent execution of clipboard and file monitoring
#include <filesystem> // Provides utilities for file system operations (optional here but good practice for path handling)

// Define the root folder where file operations are allowed
const std::string ROOT_FOLDER = "C:\\Users\\ASUS\\";

// Define the MONITOR folder, which is the root and its subdirectories, MAKE SURE TO CHANGE THE CONTENT RELATED TO YOUR ROOT FILE U WANT TO MONITOR, HERE ITS ASUS
const std::string ASUS_FOLDER = "C:\\Users\\ASUS\\"; // make sure to change all the hardcoded line for monitor folder constant as your file

// Function to clear the clipboard
// Clears all data currently stored in the clipboard to prevent unauthorized pasting
void ClearClipboard() {
    if (OpenClipboard(nullptr)) { // Opens the clipboard for modification
        EmptyClipboard();         // Clears the clipboard content
        CloseClipboard();         // Closes the clipboard to release it for other processes
    }
}

// Function to monitor clipboard operations for restricted pasting
// Continuously checks clipboard content to enforce restrictions on pasting files
void MonitorClipboard() {
    while (true) { // Infinite loop to monitor the clipboard in real-time
        if (OpenClipboard(nullptr)) { // Opens the clipboard for reading
            HANDLE hData = GetClipboardData(CF_HDROP); // Retrieves file paths from the clipboard (CF_HDROP format)
            if (hData) { // If clipboard contains data
                HDROP hDrop = static_cast<HDROP>(hData); // Casts the data handle to an HDROP structure for file operations
                UINT fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0); // Gets the number of files in the clipboard

                // Iterate over all files in the clipboard
                for (UINT i = 0; i < fileCount; ++i) {
                    char filePath[MAX_PATH]; // Buffer to store the file path
                    DragQueryFile(hDrop, i, filePath, MAX_PATH); // Retrieves the file path

                    std::string copiedPath(filePath); // Converts the file path to a std::string
                    std::cout << "Detected clipboard file: " << copiedPath << "\n"; // Logs the file path

                    // Check if the copied file is from the ASUS/your folder as ASUS folder
                    if (copiedPath.find(ASUS_FOLDER) == 0) {
                        std::cout << "File from ASUS folder detected.\n";

                        // Allow pasting if it's inside the root folder
                        if (copiedPath.find(ROOT_FOLDER) == 0) {
                            std::cout << "File from ASUS folder can be pasted within the root folder.\n";
                        } else {
                            // Block pasting outside the root folder
                            std::cout << "Error: Files from ASUS (monitor folder) cannot be pasted outside the root folder.\n";
                            ClearClipboard(); // Clear the clipboard to prevent unauthorized pasting
                            std::cout << "Clipboard cleared to prevent unauthorized pasting.\n";
                        }
                    } else {
                        // Allow pasting files from outside the ASUS folder
                        std::cout << "File from outside ASUS folder detected. Paste allowed.\n";
                    }
                }
            }
            CloseClipboard(); // Closes the clipboard after processing
        }
        Sleep(500); // Pause for 500 milliseconds to reduce CPU usage and avoid excessive polling
    }
}

// Function to monitor file operations within the root folder
// Continuously tracks file creation, modification, or deletion within the specified directory
void MonitorFileOperations() {
    HANDLE hDir = CreateFile( // Creates a handle to monitor a directory
        ROOT_FOLDER.c_str(),          // Directory to monitor (ROOT_FOLDER)
        FILE_LIST_DIRECTORY,          // Requests change notifications for the directory
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // Allows shared read/write/delete access
        nullptr,                      // No specific security attributes
        OPEN_EXISTING,                // Opens the existing directory
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, // Backup semantics for directory handling and asynchronous I/O
        nullptr                       // No template file
    );

    if (hDir == INVALID_HANDLE_VALUE) { // Check if directory handle is valid
        std::cerr << "Failed to monitor directory. Error: " << GetLastError() << "\n"; // Logs the error
        return; // Exit the function if the handle is invalid
    }

    char buffer[1024]; // Buffer to store change notifications
    DWORD bytesReturned; // Variable to store the number of bytes returned

    // Infinite loop to monitor directory changes
    while (ReadDirectoryChangesW( // Reads changes in the specified directory
        hDir,                    // Directory handle
        buffer,                  // Buffer to store changes
        sizeof(buffer),          // Size of the buffer
        TRUE,                    // Monitor subdirectories as well
        FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME, // Track file and directory name changes
        &bytesReturned,          // Store the number of bytes returned
        nullptr,                 // No overlapped structure
        nullptr                  // No completion routine
    )) {
        FILE_NOTIFY_INFORMATION* pNotify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer); // Pointer to notification structure
        do {
            // Extract file name and convert it to a wide string
            std::wstring fileName(pNotify->FileName, pNotify->FileNameLength / sizeof(WCHAR));

            // Combine root folder path with the file name to get the full path
            std::string fullPath(ROOT_FOLDER + std::string(fileName.begin(), fileName.end()));

            // Check if the file operation is inside the root folder
            if (fullPath.find(ROOT_FOLDER) == 0) {
                std::cout << "File operation detected and allowed: " << fullPath << "\n";
            } else {
                std::cout << "File operation outside root folder detected: " << fullPath << "\n";
                std::cout << "Operation blocked.\n";
            }

            // Move to the next notification if it exists
            pNotify = pNotify->NextEntryOffset 
                          ? reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<BYTE*>(pNotify) + pNotify->NextEntryOffset)
                          : nullptr;
        } while (pNotify); // Continue until all notifications are processed
    }
    CloseHandle(hDir); // Close the directory handle when done
}

int main() {
    std::cout << "Starting clipboard and file operation monitoring...\n"; // Inform the user the program is starting

    // Create and start separate threads for clipboard and file operation monitoring
    std::thread clipboardThread(MonitorClipboard); // Start clipboard monitoring in a separate thread
    std::thread fileThread(MonitorFileOperations); // Start file operation monitoring in another thread

    clipboardThread.join(); // Wait for the clipboard monitoring thread to finish (infinite loop)
    fileThread.join();      // Wait for the file monitoring thread to finish (infinite loop)

    return 0; // End the program (though it won't reach here due to infinite loops)
}
