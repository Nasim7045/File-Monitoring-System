FileMonitor - Clipboard and File Operations Monitoring Tool
FileMonitor is a C++ program that monitors clipboard content and file operations in a specified root folder. It helps track file modifications, additions, deletions, and clipboard activity within a specific directory.

Features
Clipboard Monitoring: Continuously checks the clipboard for any file paths copied from the specified root folder. It prints the file path if it's within the allowed folder.
File Operations Monitoring: Monitors a specific directory for any file operations (create, delete, rename, modify) and logs these changes.
Requirements
Before you can run the project, you need the following tools installed:

C++ Compiler: The project uses C++11 features. You can use MinGW or any other compatible C++ compiler.
Windows Operating System: Since this project uses Windows-specific APIs like OpenClipboard, CreateFile, etc., it’s designed to run on Windows systems.
Development Environment:
IDE (Optional): Any IDE that supports C++ development (e.g., Visual Studio, Code::Blocks, or Visual Studio Code).
MinGW (for GCC): If using MinGW, install it from the MinGW website and ensure it is added to your system’s PATH.
Installation
Clone or Download the Project:

Clone the repository or download the project files to your local machine.
bash
Set the Root Folder Path:

Open the main.cpp file in a text editor.
Change the value of ROOT_FOLDER to the folder path you want to monitor (e.g., C:\\Users\\ASUS\\).
cpp
Copy code
const std::string ROOT_FOLDER = "C:\\your\\folder\\"; // Modify to your desired folder
Build the Project:

If you are using MinGW, you can compile the project from the command line:
bash
Copy code
g++ -std=c++11 main.cpp -o filemonitor.exe
If you are using Visual Studio or another IDE, you can open the project and compile it within your development environment.
Run the Project:

Once the program is compiled successfully, run the executable (filemonitor.exe).
bash
Copy code
./filemonitor.exe
The program will start monitoring clipboard content and file operations in the specified root folder.
How It Works
Clipboard Monitoring:

The program runs an infinite loop that checks the clipboard content every 500 milliseconds.
If the clipboard contains a file path and it starts with the specified root folder path, it logs the path.
If the clipboard content is outside the root folder, it prints a message stating so.
File Operations Monitoring:

The program continuously monitors the root folder for file operations, such as creating, deleting, or renaming files.
When a file operation is detected, the program logs the name of the affected file.
Example:
If a file path C:\Users\ASUS\Documents\myfile.txt is copied to the clipboard, the program will log:

mathematica
Copy code
Clipboard contains: C:\Users\your\Docs\myfile.txt
If a file named myfile.txt is created or modified in the C:\Users\ASUS\Documents directory, the program will log:

arduino
Copy code
File operation detected: myfile.txt
Potential Future Enhancements
Logging: Save logs to a file for persistent monitoring.
User Interface: Implement a basic GUI to show real-time clipboard and file operations.
Advanced Filtering: Add the ability to filter operations by file types or specific directories.
Security Features: Implement features like blocking file operations or restricting clipboard access to certain directories.
Troubleshooting
"Failed to monitor directory" Error:

Ensure that the folder exists and that you have the required permissions to access it.
Check if the root folder path is correctly set.
Clipboard Not Being Monitored:

Ensure the clipboard contains text data (paths).
Verify that the program is running with sufficient privileges to access clipboard data.
Program Not Running:

Make sure all dependencies (compiler, IDE, etc.) are correctly installed.
Check that you have built the project without errors and that the filemonitor.exe exists in the correct location.
