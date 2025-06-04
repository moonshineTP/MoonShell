// Tác giả: moonshineTP
// Bài tập lớn môn OS

//-------------Các macros của chương trình------------------------

// ------------Các thư viện cần thiết cho chương trình------------------------
// Thư viện chuẩn
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <vector>
#include <windows.h>
#include <fcntl.h>
#include <io.h>

// ------------Các cấu trúc dữ liệu của chương trình------------------------
/*
    * Cấu trúc dữ liệu này được sử dụng để lưu trữ thông tin về lệnh và cờ
    * trong phần mềm MoonShell.
    *
    * - Command: Biểu trưng về lệnh được nhập vào từ bàn phím,
    * - CommandDescription: Mô tả lệnh, bao gồm tên lệnh, mô tả và các cờ
    * - Flag: Biểu trưng về cờ được nhập vào từ lệnh từ bàn phím.
    * - FlagDescription: Mô tả cờ, bao gồm tên cờ và mô tả.
    *
    * Các cấu trúc dữ liệu này sẽ được sử dụng để quản lý và thao tác với các đối
    * tượng trong phần mềm,
*/

// ---Cấu trúc dữ liệu lưu trữ lệnh và cờ
struct Command {
    std::wstring name;                                                  // Tên lệnh
    std::unordered_map<std::wstring, std::vector<std::wstring>> map;    // Cặp cờ - dãy tham số

    Command(const std::wstring& _name,
            const std::unordered_map<std::wstring, std::vector<std::wstring>> _map)
        : name(_name), map(_map) {}
};

struct CommandDescription {
    std::wstring name;                                  // Tên lệnh
    std::wstring description;                           // Mô tả lệnh
    std::unordered_set<std::wstring> possible_flags;    // Tập hợp các cờ khả dụng cho lệnh

    CommandDescription(const std::wstring& _name,
                        const std::wstring& _description,
                        const std::unordered_set<std::wstring>& _flags)
        : name(_name), description(_description), possible_flags(_flags) {}
    CommandDescription() = default;
};

struct Flag {
    std::wstring name;          // Tên cờ
    Flag (const std::wstring& _name)
        : name(_name) {}
};

struct FlagDescription {
    std::wstring name;           // Tên cờ
    std::wstring description;    // Mô tả cờ

    FlagDescription(const std::wstring& _name, const std::wstring& _description)
        : name(_name), description(_description) {}
    FlagDescription() = default;
};

struct CommandDictionary {
    std::map<std::wstring, CommandDescription> commandMap;
    std::map<std::wstring, FlagDescription> flagMap;

    CommandDictionary() = default;
};

// ------------Các biến toàn cục của chương trình------------------------
CommandDictionary commandDict = CommandDictionary();        // Lưu trữ các lệnh và cờ
std::unordered_map<std::wstring, HANDLE> currentProcesses;  // Lưu trữ các tiến trình
std::wstring rootPath;                                      // Đường dẫn gốc
std::wstring currentPath;                                   // Đường dẫn hiện tại
std::unordered_map<std::wstring, std::wstring> variables;   // Biến của tiến trình

// ------------Các hàm tiện ích của chương trình------------------------
void initializeRootDirectory() {
    // Lấy đường dẫn thư mục hiện tại khi chạy chương trình
    wchar_t buffer[MAX_PATH];
    GetCurrentDirectoryW(MAX_PATH, buffer);
    std::wstring repoPath = buffer;

    // Thiết lập rootDirectory là thư mục "root" trong repo này
    rootPath = repoPath + L"\\root";
    currentPath = rootPath;
    SetCurrentDirectoryW(rootPath.c_str());
}

void initializeCommandDictionary() {
    // Khởi tạo các lệnh đặc biệt
    commandDict.commandMap[L"NULL"] = {
        L"NULL", L"Lệnh trống", {}
    };
    commandDict.commandMap[L"INVALID"] = {
        L"INVALID", L"Lệnh không hợp lệ", {}
    };
    commandDict.commandMap[L"WRONG_VAR"] = {
        L"WRONG_VAR", L"Biến không tồn tại", {}
    };

    // Lệnh không có cờ
    commandDict.commandMap[L"intro"] = {
        L"intro", L"Giới thiệu phần mềm MoonShell", {}
    };
    commandDict.commandMap[L"help"] = {
        L"help", L"Hiển thị danh sách các lệnh và cờ", {}
    };
    commandDict.commandMap[L"exit"] = {
        L"exit", L"Thoát khỏi phần mềm", {}
    };
    commandDict.commandMap[L"example"] = {
        L"example", L"Hiển thị ví dụ sử dụng phần mềm", {}
    };
    commandDict.commandMap[L"clrscr"] = {
        L"clrscr", L"Làm sạch console", {}
    };
    commandDict.commandMap[L"viewvar"] = {
        L"viewvar", L"Hiển thị tất cả các biến shell lưu giữ", {}
    };
    commandDict.commandMap[L"viewpath"] = {
        L"viewpath", L"Hiển thị các biến trong PATH", {}
    };
    commandDict.commandMap[L"backward"] = {
        L"backward", L"Quay lại thư mục cha", {}
    };
    commandDict.commandMap[L"return"] = {
        L"return", L"Quay về thư mục gốc", {}
    };
    commandDict.commandMap[L"viewdir"] = {
        L"viewdir", L"Hiển thị danh sách file và thư mục trong thư mục hiện tại", {}
    };
    commandDict.commandMap[L"sysinfo"] = {
        L"sysinfo", L"Hiển thị thông tin hệ thống", {}
    };
    commandDict.commandMap[L"timeinfo"] = {
        L"timeinfo", L"Hiển thị thời gian hệ thống", {}
    };
    commandDict.commandMap[L"userinfo"] = {
        L"userinfo", L"Hiển thị thông tin người dùng", {}
    };
    commandDict.commandMap[L"ipinfo"] = {
        L"ipinfo", L"Hiển thị thông tin IP/network adapters", {}
    };

    // Lệnh có cờ
    commandDict.commandMap[L"echo"] = {
        L"echo", L"In ra nội dung lệnh", {L"-argS"}
    };
    commandDict.commandMap[L"setfgcol"] = {
        L"setfgcol", L"Thay đổi màu sắc của chữ", {L"-color"}
    };
    commandDict.commandMap[L"setbgcol"] = {
        L"setbgcol", L"Thay đổi màu sắc của console", {L"-color"}
    };
    commandDict.commandMap[L"setvar"] = {
        L"setvar", L"Thiết lập biến shell", {L"-key", L"-value"}
    };
    commandDict.commandMap[L"getvar"] = {
        L"getvar", L"Lấy giá trị của biến trong shell", {L"-key"}
    };
    commandDict.commandMap[L"rmvar"] = {
        L"rmvar", L"Xóa biến trong shell", {L"-key"}
    };
    commandDict.commandMap[L"addpath"] = {
        L"addpath", L"Thêm biến trong PATH", {L"-path"}
    };
    commandDict.commandMap[L"rmpath"] = {
        L"rmpath", L"Xóa biến trong PATH", {L"-path"}
    };
    commandDict.commandMap[L"forward"] = {
        L"forward", L"Chuyển đến thư mục con", {L"-dir"}
    };
    commandDict.commandMap[L"adddir"] = {
        L"adddir", L"Tạo thư mục", {L"-dir"}
    };
    commandDict.commandMap[L"addfile"] = {
        L"addfile", L"Tạo file", {L"-csv", L"-txt", L"-json", L"-bat"}
    };
    commandDict.commandMap[L"viewfile"] = {
        L"viewfile", L"Xem nội dung file", {L"-csv", L"-txt", L"-json", L"-bat"}
    };
    commandDict.commandMap[L"writefile"] = {
        L"writefile", L"Viết đè lên file", {L"-csv", L"-txt", L"-json", L"-bat"}
    };
    commandDict.commandMap[L"rename"] = {
        L"rename", L"Đổi tên thư mục hoặc file", {L"-targetname", L"-newname"}
    };
    commandDict.commandMap[L"delete"] = {
        L"delete", L"Xóa thư mục hoặc file", {L"-targetname"}
    };
    commandDict.commandMap[L"build"] = {
        L"build", L"Xây dựng file thực thi từ file chương trình", {L"-cpp", L"-exe", L"-java"}
    };
    commandDict.commandMap[L"run"] = {
        L"run", L"Chạy file (được hỗ trợ trong PATHEXT) cùng với dãy tham số", {L"-argS", L"-bg"}
    };
    commandDict.commandMap[L"runbatch"] = {
        L"runbatch", L"Chạy file .bat cùng với dãy tham số", {L"-argS", L"-bg"}
    };
    commandDict.commandMap[L"runclass"] = {
        L"runclass", L"Chạy file .class cùng với dãy tham số", {L"-argS", L"-optionS", L"-bg"}
    };
    commandDict.commandMap[L"runjar"] = {
        L"runjar", L"Chạy file .jar cùng với dãy tham số", {L"-argS", L"-optionS", L"-bg"}
    };
    commandDict.commandMap[L"ping"] = {
        L"ping", L"Ping một host", {L"-host"}
    };
    commandDict.commandMap[L"traceroute"] = {
        L"traceroute", L"Traceroute tới một host", {L"-host"}
    };
    commandDict.commandMap[L"nslookup"] = {
        L"nslookup", L"Tra cứu DNS cho một host", {L"-host"}
    };
    commandDict.commandMap[L"portcheck"] = {
        L"portcheck", L"Kiểm tra cổng mạng", {L"-port"}
    };

    // Khởi tạo các cờ
    commandDict.flagMap[L"-dir"] = {
        L"-dir", L"Tên thư mục đích khi chuyển đến thư mục con hoặc tạo thư mục"
    };
    commandDict.flagMap[L"-targetname"] = {
        L"-targetname", L"Tên thư mục/file được nhắm đến trong lệnh"
    };
    commandDict.flagMap[L"-newname"] = {
        L"-newname", L"Tên mới của thư mục hoặc file"
    };
    commandDict.flagMap[L"-cpp"] = {
        L"-cpp", L"File C++"
    };
    commandDict.flagMap[L"-java"] = {
        L"-java", L"File Java"
    };
    commandDict.flagMap[L"-exe"] = {
        L"-exe", L"File exe"
    };
    commandDict.flagMap[L"-bg"] = {
        L"-bg", L"Chạy file trong nền không (background), phải đi kèm với tham số <yes>"
    };
    commandDict.flagMap[L"-argS"] = {
        L"-argS", L"Tham số truyền vào khi chạy file"
    };
    commandDict.flagMap[L"-optionS"] = {
        L"-optionS", L"Các tùy chọn bổ sung khi chạy file .class hoặc .jar"
    };
    commandDict.flagMap[L"-csv"] = {
        L"-csv", L"File CSV"
    };
    commandDict.flagMap[L"-txt"] = {
        L"-txt", L"File TXT"
    };
    commandDict.flagMap[L"-json"] = {
        L"-json", L"File JSON"
    };
    commandDict.flagMap[L"-bat"] = {
        L"-bat", L"File BAT"
    };
    commandDict.flagMap[L"-color"] = {
        L"-color",
        L"Màu sắc của console. Các giá trị hợp lệ:\n"
        L"        * black\n"
        L"        * red\n"
        L"        * green\n"
        L"        * blue\n"
        L"        * yellow\n"
        L"        * magenta\n"
        L"        * cyan\n"
        L"        * white"
    };
    commandDict.flagMap[L"-key"] = {
        L"-key", L"Tên biến trong tiến trình"
    };
    commandDict.flagMap[L"-value"] = {
        L"-value", L"Giá trị của biến trong tiến trình"
    };
    commandDict.flagMap[L"-path"] = {
        L"-path", L"Đường dẫn của biến trong PATH"
    };
    commandDict.flagMap[L"-host"] = {
        L"-host", L"Địa chỉ mạng"
    };
    commandDict.flagMap[L"-port"] = {
        L"-port", L"Cổng kết nối"
    };
}

// Hàm lấy lệnh thô từ người dùng
std::wstring getRawCommandLine() {
    std::wcout << currentPath << L">";    // Hiển thị đường dẫn hiện tại
    
    std::wstring commandLine;
    std::getline(std::wcin, commandLine);
    return commandLine;
}

Command decomposeCommandLine(const std::wstring& commandLine) {
    // Tách lệnh, cờ và tham số từ chuỗi lệnh theo quy tắc đặc biệt
    std::wstring commandName;
    std::unordered_map<std::wstring, std::vector<std::wstring>> commandMap;

    // Collect tokens
    std::vector<std::wstring> tokens;
    size_t start = 0, end;
    while ((end = commandLine.find(' ', start)) != std::wstring::npos) {
        tokens.push_back(commandLine.substr(start, end - start));
        start = end + 1;
    }
    if (start < commandLine.size())
        tokens.push_back(commandLine.substr(start));

    // If empty command, return nullptr
    if (tokens.empty()) return Command(
        L"NULL",
        std::unordered_map<std::wstring, std::vector<std::wstring>>{}
    );

    // The first word must be the command
    commandName = tokens[0];
    if (commandDict.commandMap.find(commandName) == commandDict.commandMap.end()) {
        return Command(
            L"INVALID",
            std::unordered_map<std::wstring, std::vector<std::wstring>>{}
        );
    }
        
    size_t idx = 1;
    while (idx < tokens.size()) {
        std::wstring flag = tokens[idx];
        if (commandDict.flagMap.find(flag) == commandDict.flagMap.end()
            || commandDict.commandMap[commandName].possible_flags.find(flag)
                == commandDict.commandMap[commandName].possible_flags.end())
        {
            return Command(
                L"INVALID",
                std::unordered_map<std::wstring, std::vector<std::wstring>>{}
            );
        }
        
        commandMap[flag] = {};
        idx++;

        // Kiểm tra cờ có phải là dạng số nhiều hay khôn
        bool is_plural = flag[flag.size() - 1] == 'S';

        // Đọc dãy tham số ứng với cờ
        int cnt = 0;
        while (idx < tokens.size()) {
            std::wstring param = tokens[idx];

            // Nếu là biến,
            if (param[0] == L'%' && param.size() > 1) {
                // Nếu là biến, thay thế bằng giá trị của biến
                std::wstring varKey = param.substr(1);
                if (variables.find(varKey) != variables.end()) {
                    param = variables[varKey];
                } else {
                    // Biến không tồn tại, trả về lệnh không hợp lệ
                    return Command(
                        L"WRONG_VAR",
                        std::unordered_map<std::wstring, std::vector<std::wstring>>{}
                    );
                }
            }

            // Nếu gặp cờ mới, dừng lại
            if (commandDict.flagMap.find(param) != commandDict.flagMap.end()) {
                break;
            }

            // Cập nhật tham số vào cờ
            commandMap[flag].push_back(param);
            cnt++;
            idx++;
            
            // Nếu là cờ số ít, chỉ cho phép 1 tham số
            if (!is_plural && cnt > 1) {
                return Command(
                    L"INVALID",
                    std::unordered_map<std::wstring, std::vector<std::wstring>>{}
                );
            }
        }

        // Mọi cờ phải có ít nhất 1 tham số
        if (cnt == 0) {
            return Command(
                L"INVALID",
                std::unordered_map<std::wstring, std::vector<std::wstring>>{}
            );
        }
    }

    // Trả về lệnh đã phân tích
    return Command(commandName, commandMap);
}

bool checkCommand(Command& command) {
    // Kiểm tra lệnh trống
    if (command.name == L"NULL") {
        std::wcout << L"Lệnh trống.\n";
        return false;
    }
    // Kiểm tra lệnh không hợp lệ
    if (command.name == L"INVALID") {
        std::wcout << L"Lệnh sai cú pháp!\n";
        return false;
    }

    if (command.name == L"WRONG_VAR") {
        std::wcout << L"Biến/các biến được chèn không tồn tại trong shell. "
                        L"Gõ lệnh viewvar hoặc getvar để kiểm tra.\n";
        return false;
    }

    return true;
}

// ---Các hàm thực thi các lệnh
void printCommandList() {
    for (const auto& [_, command] : commandDict.commandMap) {
        std::wcout << L"    > " << command.name << L": " << command.description << L".\n";
    }
}

void printFlagList() {
    for (const auto& [_,flag] : commandDict.flagMap) {
        std::wcout << L"    > " << flag.name << L": " << flag.description << L".\n";
    }
}

// ------------Các hàm lệnh của chương trình------------------------
// Hàm hiển thị thông tin chương trình
void cmd_intro() {
    std::wcout << L"--------------------------------------------------------\n";
    std::wcout << L"Phần mềm MoonShell - Phương tiện giao tiếp hệ thống\n";
    std::wcout << L"Phiên bản 1.0         \n";
    std::wcout << L"Chào mừng bạn =^.^=   \n";
    std::wcout << L"                      \n";
    std::wcout << L"      _..._           \n";
    std::wcout << L"   .::'    ``.        \n";
    std::wcout << L"  :::        ::       \n";
    std::wcout << L"  :::        ::       \n";
    std::wcout << L"  `::.       ;'       \n";
    std::wcout << L"    `':...-''         \n";
    std::wcout << L"                      \n";
    std::wcout << L"--------------------------------------------------------\n";
    std::wcout << L"Gõ lệnh 'intro' để xem giới thiệu phần mềm.             \n";
    std::wcout << L"Gõ lệnh 'help' để xem danh sách các lệnh và cờ.         \n";
    std::wcout << L"Gõ lệnh 'example' để xem ví dụ sử dụng phần mềm.        \n";
    std::wcout << L"Gõ lệnh 'exit' để thoát khỏi phần mềm.                  \n";
    std::wcout << L"--------------------------------------------------------\n";
}

void cmd_intro(Command& command) {
    if (!command.map.empty()) {
        std::wcout << L"Lệnh intro không có cờ tham số.\n";
        return;
    }
    
    std::wcout << L"--------------------------------------------------------\n";
    std::wcout << L"Phần mềm MoonShell - Phương tiện giao tiếp hệ thống\n";
    std::wcout << L"Phiên bản 1.0         \n";
    std::wcout << L"Chào mừng bạn =^.^=   \n";
    std::wcout << L"                      \n";
    std::wcout << L"      _..._           \n";
    std::wcout << L"   .::'    ``.        \n";
    std::wcout << L"  :::        ::       \n";
    std::wcout << L"  :::        ::       \n";
    std::wcout << L"  `::.       ;'       \n";
    std::wcout << L"    `':...-''         \n";
    std::wcout << L"                      \n";
    std::wcout << L"--------------------------------------------------------\n";
    std::wcout << L"Gõ lệnh 'intro' để xem giới thiệu phần mềm.             \n";
    std::wcout << L"Gõ lệnh 'help' để xem danh sách các lệnh và cờ.         \n";
    std::wcout << L"Gõ lệnh 'example' để xem ví dụ sử dụng phần mềm.        \n";
    std::wcout << L"Gõ lệnh 'exit' để thoát khỏi phần mềm.                  \n";
    std::wcout << L"--------------------------------------------------------\n";
}

void cmd_help(Command& command) {
    if (!command.map.empty()) {
        std::wcout << L"Lệnh help không có cờ tham số.\n";
        return;
    }

    std::wcout << L"Một lệnh hợp lệ có dạng:\n";
    std::wcout << L"    <command> <flag_1> <params_1> <flag_2> <params_2> ... <flag_n> <params_n>\n";
    std::wcout << L"Trong đó:\n";
    std::wcout << L"    - command: lệnh của chương trình.\n";
    std::wcout << L"    - flag_i: cờ của chương trình. Các cờ luôn có đầu là -;\n";
    std::wcout << L"              Nếu đuôi là \'S\' thì đó là cờ số nhiều, còn lại là cờ số ít.\n";
    std::wcout << L"    - params_i: dãy tham số ứng với cờ.\n";
    std::wcout << L"        + Nếu là cờ số ít, dãy chỉ được có đúng 1 tham số.\n";
    std::wcout << L"        + Nếu là cờ số nhiều, dãy phải có ít nhất 1 tham số.\n";
    std::wcout << L"        + Nếu params_i là biến (bắt đầu bởi %), tham số được thay thế bằng giá trị của biến.\n";
    std::wcout << L"\n";

    std::wcout << L"Danh sách các lệnh khả dụng:\n";
    printCommandList();
    std::wcout << L"\n";

    std::wcout << L"Danh sách các cờ khả dụng:\n";
    printFlagList();
    std::wcout << L"\n";
}

void cmd_exit(Command& command){
    if (!command.map.empty()) {
        std::wcout << L"Lệnh exit không có cờ tham số.\n";
        return;
    }

    std::wcout << L"Bạn chắc chăn muốn thoát khỏi phần mềm MoonShell (gõ Y để thoát): ";
    std::wstring choice;
    std::getline(std::wcin, choice);
    if (choice != L"Y") {
        std::wcout << L"Đã hủy thoát phần mềm.\n";
        return;
    }

    std::wcout << L"Chào tạm biệt! =^.^=";
    exit(0);
}

void cmd_example(Command& command) {
    if (!command.map.empty()) {
        std::wcout << L"Lệnh example không có cờ tham số.\n";
        return;
    }

    std::wcout << L"Ví dụ sử dụng phần mềm MoonShell:\n";
    std::wcout << L"1. Chuyển đến thư mục con: forward -dir example_matmul\n";
    std::wcout << L"2. Xây dựng file .exe từ file .cpp: build -cpp matrix_generator.cpp -exe matrix_generator.exe\n";
    std::wcout << L"3. Chạy file: run -argS matrix_generator.exe 128 256 512\n";
    std::wcout << L"4. Chạy file .class: runclass -argS matrix_multiplication matrix1.csv matrix2.csv matrix3.csv\n";
    std::wcout << L"5. Đổi tên thư mục hoặc file: rename -targetname matrix_generator.cpp -newname matgen.cpp\n";
    std::wcout << L"6. Xóa thư mục hoặc file: delete -targetname matgen.cpp\n";
}

void cmd_clrscr(Command& command) {
    if (!command.map.empty()) {
        std::wcout << L"Lệnh clrscr không có cờ tham số.\n";
        return;
    }

    // Các biến
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD count, cellCount;
    COORD homeCoords = { 0, 0 };

    // Kiểm tra handle hợp lệ
    if (hConsole == INVALID_HANDLE_VALUE) return;

    // Lấy thông tin về console
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;
    cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    // Xóa
    if (!FillConsoleOutputCharacterW(
        hConsole, L' ', cellCount, homeCoords, &count)) return;

    // Đặt thuộc tính mặc định
    if (!FillConsoleOutputAttribute(
        hConsole, csbi.wAttributes, cellCount, homeCoords, &count)) return;

    // Đặt con trỏ về vị trí đầu tiên
    SetConsoleCursorPosition(hConsole, homeCoords);
}

void cmd_setfgcol(Command& command) {
    // Kiểm tra cờ tham số tồn tại không
    if (command.map.size() != 1 ||
        command.map.find(L"-color") == command.map.end())
    {
        std::wcout << L"Lệnh color thiếu cờ tham số!\n";
        return;
    }

    // Lấy màu từ cờ
    std::wstring color = command.map[L"-color"][0];
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Lấy thuộc tính hiện tại
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        std::wcout << L"Không thể lấy thông tin console.\n";
        return;
    }
    WORD attr = csbi.wAttributes;

    // Xóa phần foreground cũ
    attr &= ~(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

    // Thêm màu mới
    if (color == L"black") {
        // Không thêm gì, giữ nguyên attr đã xóa fg
    } else if (color == L"red") {
        attr |= FOREGROUND_RED | FOREGROUND_INTENSITY;
    } else if (color == L"green") {
        attr |= FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    } else if (color == L"blue") {
        attr |= FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    } else if (color == L"yellow") {
        attr |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    } else if (color == L"magenta") {
        attr |= FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    } else if (color == L"cyan") {
        attr |= FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    } else if (color == L"white") {
        attr |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    } else {
        std::wcout << L"Màu không hợp lệ!\n";
        return;
    }

    if (!SetConsoleTextAttribute(hConsole, attr)) {
        std::wcout << L"Không thể đổi màu chữ.\n";
    } else {
        std::wcout << L"Đã đổi màu chữ thành công.\n";
    }
}

void cmd_setbgcol(Command& command) {
    // Kiểm tra cờ tham số tồn tại không
    if (command.map.size() != 1 ||
        command.map.find(L"-color") == command.map.end()) 
    {
        std::wcout << L"Lệnh sai cờ!\n";
        return;
    }

    // Lấy màu từ cờ
    std::wstring color = command.map[L"-color"][0];
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Lấy thuộc tính hiện tại
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        std::wcout << L"Không thể lấy thông tin console.\n";
        return;
    }
    WORD attr = csbi.wAttributes;

    // Xóa phần background cũ
    attr &= ~(BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY);

    // Thêm màu mới
    if (color == L"black") {
        // Không thêm gì, giữ nguyên attr đã xóa bg
    } else if (color == L"red") {
        attr |= BACKGROUND_RED | BACKGROUND_INTENSITY;
    } else if (color == L"green") {
        attr |= BACKGROUND_GREEN | BACKGROUND_INTENSITY;
    } else if (color == L"blue") {
        attr |= BACKGROUND_BLUE | BACKGROUND_INTENSITY;
    } else if (color == L"yellow") {
        attr |= BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
    } else if (color == L"magenta") {
        attr |= BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
    } else if (color == L"cyan") {
        attr |= BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
    } else if (color == L"white") {
        attr |= BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
    } else {
        std::wcout << L"Màu không hợp lệ!\n";
        return;
    }

    if (!SetConsoleTextAttribute(hConsole, attr)) {
        std::wcout << L"Không thể đổi màu nền.\n";
    } else {
        std::wcout << L"Đã đổi màu nền thành công.\n";
    }
}

void cmd_echo(Command& command) {
    // Kiểm tra cờ tham số tồn tại không
    if (command.map.size() != 1 ||
        command.map.find(L"-argS") == command.map.end())
    {
        std::wcout << L"Lệnh echo thiếu cờ tham số!\n";
        return;
    }

    // In ra nội dung lệnh
    std::vector<std::wstring> params = command.map[L"-argS"];
    for (const auto& param : params) {
        std::wcout << std::wstring(param.begin(), param.end()) << L" ";
    }
    std::wcout << '\n';
}

void cmd_viewvar(Command& command) {
    if (!command.map.empty()) {
        std::wcout << L"Lệnh viewvar không có cờ tham số.\n";
        return;
    }

    if (variables.empty()) {
        std::wcout << L"Không có biến nào trong tiến trình.\n";
        return;
    }

    std::wcout << L"Các biến trong tiến trình:\n";
    for (const auto& [key, value] : variables) {
        std::wcout << L"    " << key << L": " << value << L"\n";
    }
}

void cmd_setvar(Command& command) {
    // Kiểm tra cờ tham số tồn tại không
    if (command.map.size() != 2 ||
        command.map.find(L"-key") == command.map.end() ||
        command.map.find(L"-value") == command.map.end())
    {
        std::wcout << L"Lệnh setvar thiếu cờ tham số.\n";
        return;
    }

    // Lấy tên biến và giá trị từ cờ
    std::wstring varKey = command.map[L"-key"][0];
    std::wstring varValue = command.map[L"-value"][0];

    // Thiết lập biến trong tiến trình
    variables[varKey] = varValue;
    std::wcout << L"Thiết lập thành công.\n";
}

void cmd_getvar(Command& command) {
    // Kiểm tra cờ tham số tồn tại không
    if (command.map.size() != 1 ||
        command.map.find(L"-key") == command.map.end())
    {
        std::wcout << L"Lệnh getvar thiếu cờ tham số.\n";
        return;
    }

    // Lấy tên biến từ cờ
    std::wstring varKey = command.map[L"-key"][0];

    // Kiểm tra biến có tồn tại không
    auto it = variables.find(varKey);
    if (it != variables.end()) {
        std::wcout << L"Giá trị của biến " << varKey << L": " << it->second << L"\n";
    } else {
        std::wcout << L"Biến " << varKey << L" không tồn tại trong tiến trình.\n";
    }
}

void cmd_rmvar(Command& command) {
    // Kiểm tra cờ tham số tồn tại không
    if (command.map.size() != 1 ||
        command.map.find(L"-key") == command.map.end())
    {
        std::wcout << L"Lệnh hiếu cờ -key.\n";
        return;
    }

    // Lấy tên biến từ cờ
    std::wstring varKey = command.map[L"-key"][0];

    // Xóa biến trong tiến trình
    auto it = variables.find(varKey);
    if (it != variables.end()) {
        variables.erase(it);
        std::wcout << L"Đã xóa biến " << varKey << L" khỏi tiến trình.\n";
    } else {
        std::wcout << L"Biến " << varKey << L" không tồn tại trong tiến trình.\n";
    }
}

void cmd_viewpath(Command& command) {
    if (!command.map.empty()) {
        std::wcout << L"Lệnh viewpath không có cờ tham số.\n";
        return;
    }

    // Lấy độ dài thực sự của PATH
    DWORD len = GetEnvironmentVariableW(L"PATH", nullptr, 0);
    if (len == 0) {
        std::wcout << L"Không thể lấy biến PATH.\n";
        return;
    }
    std::vector<wchar_t> pathBuffer(len);
    GetEnvironmentVariableW(L"PATH", pathBuffer.data(), len);

    // In ra các đường dẫn trong PATH
    std::wcout << L"Các đường dẫn trong PATH:\n";
    std::wstring path(pathBuffer.data());
    size_t start = 0, end, cnt = 1;
    while ((end = path.find(L';', start)) != std::wstring::npos) {
        std::wstring dir = path.substr(start, end - start);
        // Bỏ qua các đường dẫn rỗng
        if (!dir.empty()) {
            std::wcout << L"    [" << cnt << L"]  " << dir << L"\n";
            cnt++;
        }
        start = end + 1;
    }
    // In phần cuối nếu có
    if (start < path.size()) {
        std::wstring dir = path.substr(start);
        if (!dir.empty()) {
            std::wcout << L"    [" << cnt << L"]  " << dir << L"\n";
        }
    }
}

void cmd_addpath(Command& command) {
    // Kiểm tra cờ tham số tồn tại không
    if (command.map.size() != 1 ||
        command.map.find(L"-path") == command.map.end())
    {
        std::wcout << L"Lệnh addpath thiếu cờ -path.\n";
        return;
    }

    // Lấy đường dẫn từ cờ
    std::wstring newPath = command.map[L"-path"][0];

    // Lấy PATH hiện tại
    DWORD len = GetEnvironmentVariableW(L"PATH", nullptr, 0);
    std::vector<wchar_t> pathBuffer(len);
    if (len > 0) {
        GetEnvironmentVariableW(L"PATH", pathBuffer.data(), len);
    }
    std::wstring currentPathEnv = (len > 0) ? std::wstring(pathBuffer.data()) : L"";

    // Kiểm tra nếu newPath đã tồn tại trong PATH
    if (currentPathEnv.find(newPath) != std::wstring::npos) {
        std::wcout << L"Đường dẫn đã tồn tại trong PATH.\n";
        return;
    }

    // Thêm dấu ; nếu PATH hiện tại không rỗng và không kết thúc bằng ;
    if (!currentPathEnv.empty() && currentPathEnv.back() != L';') {
        currentPathEnv += L";";
    }

    // Nối đường dẫn mới vào cuối PATH
    currentPathEnv += newPath;

    // Cập nhật PATH
    if (SetEnvironmentVariableW(L"PATH", currentPathEnv.c_str())) {
        std::wcout << L"Đã thêm đường dẫn vào PATH thành công.\n";
    } else {
        std::wcout << L"Không thể thêm đường dẫn vào PATH.\n";
    }
}

void cmd_rmpath(Command& command) {
    // Kiểm tra cờ tham số tồn tại không
    if (command.map.size() != 1 ||
        command.map.find(L"-path") == command.map.end()) 
    {
        std::wcout << L"Thiếu cờ tham số -path.\n";
        return;
    }

    // Lấy đường dẫn từ cờ
    std::wstring path = command.map[L"-path"][0];

    // Lấy độ dài thực sự của PATH
    DWORD len = GetEnvironmentVariableW(L"PATH", nullptr, 0);
    if (len == 0) {
        std::wcout << L"Không thể lấy biến PATH.\n";
        return;
    }
    std::vector<wchar_t> pathBuffer(len);
    GetEnvironmentVariableW(L"PATH", pathBuffer.data(), len);

    // Chuyển đổi PATH thành chuỗi để thao tác
    std::wstring pathStr(pathBuffer.data());
    
    // Tìm và xóa đường dẫn
    size_t pos = pathStr.find(path);
    if (pos != std::wstring::npos) {
        pathStr.erase(pos, path.length());
        if (pathStr[pos] == L';') {
            pathStr.erase(pos, 1); // Xóa dấu chấm phẩy nếu có
        }
        
        // Cập nhật PATH mới
        if (SetEnvironmentVariableW(L"PATH", pathStr.c_str())) {
            std::wcout << L"Xóa đường dẫn khỏi PATH thành công.\n";
        } else {
            std::wcout << L"Không thể cập nhật PATH.\n";
        }
    } else {
        std::wcout << L"Đường dẫn không tồn tại trong PATH.\n";
    }
} 

void cmd_viewfile(Command& command) {
    // Mở và hiển thị nội dung file
    for (const auto& [flag, params] : command.map) {
        if (params.empty()) continue;
        std::wstring filename = params[0];
        std::wifstream file(filename.c_str());
        if (!file) {
            std::wcout << L"Không thể mở file: " << filename << L".\n";
            continue;
        }
        std::wcout << L"Mở file " << filename << L" thành công. " << L"Nội dung file:\n";
        std::wstring line;

        // Trích lấy console và info
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        WORD saved_attributes = 0;

        // Đặt màu mới cho console
        if (GetConsoleScreenBufferInfo(hConsole, &consoleInfo)) {   // Lấy màu console
            saved_attributes = consoleInfo.wAttributes;             // Lưu trữ màu ban đầu
            SetConsoleTextAttribute(                                // Đặt màu xanh cho console
                hConsole,
                FOREGROUND_BLUE | FOREGROUND_INTENSITY
            );
        }

        // In lên console với màu mới
        while (std::getline(file, line)) {
            std::wcout << line << L"\n";
        }

        // Đặt lại màu ban đầu cho console
        if (saved_attributes != 0) {
            SetConsoleTextAttribute(hConsole, saved_attributes);
        }
        file.close();
    }
}

void cmd_addfile(Command& command) {
    for (const auto& [flag, params] : command.map) {
        if (params.empty()) continue;
        std::wstring filename = params[0];
        std::wofstream file(filename.c_str());
        if (!file) {
            std::wcout << L"Không thể tạo file: " << filename << L".\n";
            continue;
        }
        file.close();
        std::wcout << L"Đã tạo file: " << filename << L".\n";
    }
}

void cmd_writefile(Command& command) {
    // Ghi đè nội dung file
    for (const auto& [flag, params] : command.map) {
        if (params.empty()) continue;
        std::wstring filename = params[0];
        std::wofstream file(filename.c_str(), std::ios::trunc);
        if (!file) {
            std::wcout << L"Không thể mở file: " << filename << L".\n";
            continue;
        }
        std::wcout << L"Ghi đè nội dung file " << filename << L".\n";
        std::wcout << L"Để thoát khỏi trình ghi, nhập ### rồi Enter.\n";
        
        // Trích lấy console và info
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        WORD saved_attributes = 0;

        // Đặt màu mới cho console
        if (GetConsoleScreenBufferInfo(hConsole, &consoleInfo)) {   // Lấy màu console
            saved_attributes = consoleInfo.wAttributes;             // Lưu trữ màu ban đầu
            SetConsoleTextAttribute(                                // Đặt màu xanh cho console
                hConsole,
                FOREGROUND_GREEN | FOREGROUND_INTENSITY
            );
        }

        std::wstring content;
        while (true) {
            std::getline(std::wcin, content);
            if (content == L"###") break;
            file << content << '\n';
        }

        // Đặt lại màu ban đầu cho console
        if (saved_attributes != 0) {
            SetConsoleTextAttribute(hConsole, saved_attributes);
        }

        file.close();
        std::wcout << L"Ghi đè thành công.\n";
    }
}

void cmd_viewdir(Command& command) {
    // Hiển thị danh sách file và thư mục trong thư mục hiện tại
    if (command.map.size() != 0) {
        std::wcout << L"Lệnh viewdir không được chứa cờ.\n";
        return;
    }

    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = FindFirstFileW((currentPath + L"\\*").c_str(), &findFileData);

    std::wcout << L"Các file và thư mục trong " << currentPath << L":\n";
    
    int cnt = 1;
    do {
        if (wcscmp(findFileData.cFileName, L".") != 0 &&
            wcscmp(findFileData.cFileName, L"..") != 0)
        {
            std::wcout << L"    " << L"[" << cnt << L"]" << findFileData.cFileName;
        }
        cnt++;
        // Nếu là thư mục, in thêm dấu /
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            std::wcout << L"    (Thư mục)\n";
        } else {
            std::wcout << L"    (File)\n";
        }
    } while (FindNextFileW(hFind, &findFileData) != 0);

    FindClose(hFind);
}

void cmd_adddir(Command& command) {
    // Cần cờ -name
    if (command.map.size() != 1 ||
        command.map.find(L"-dir") == command.map.end())
    {
        std::wcout << L"Lệnh adddir chỉ chứa cờ -dir.\n";
        return;
    }

    // Lấy tên thư mục từ cờ
    std::wstring dirName = command.map[L"-name"][0];
    std::wstring dirPath = currentPath + L"\\" + std::wstring(dirName.begin(), dirName.end());

    // Tạo thư mục mới
    if (CreateDirectoryW(dirPath.c_str(), nullptr)) {
        std::wcout << L"Đã tạo thư mục: " << dirName << L".\n";
    } else {
        std::wcout << L"Không thể tạo thư mục: " << dirName << L". Kiểm tra lại tên thư mục.\n";
    }
}

void cmd_rename(Command& command) {
    // Cần cả -targetname và -newname
    if (command.map.find(L"-targetname") == command.map.end() ||
        command.map.find(L"-newname") == command.map.end()) {
        std::wcout << L"Lệnh đổi tên thiếu tham số!\n";
        return;
    }

    std::wstring oldName = command.map[L"-targetname"][0];
    std::wstring newName = command.map[L"-newname"][0];
    std::wstring oldNameW =std::wstring(oldName.begin(), oldName.end());
    std::wstring newNameW =std::wstring(newName.begin(), newName.end());

    // Đổi tên file/thư mục
    if (MoveFileW(oldNameW.c_str(), newNameW.c_str())) {
        std::wcout << L"Đổi tên thành công: " << oldName << L" -> " << newName << L".\n";
    } else {
        std::wcout << L"Đổi tên thất bại! Kiểm tra lại tên file/thư mục.\n";
    }
}

void cmd_delete(Command& command) {
    // Cần -targetname
    if (command.map.find(L"-targetname") == command.map.end()) {
        std::wcout << L"Lệnh xóa thiếu tham số!\n";
        return;
    }
    std::wstring target = command.map[L"-targetname"][0];
    std::wstring targetW =std::wstring(target.begin(), target.end());

    // Thử xóa file trước
    if (DeleteFileW(targetW.c_str())) {
        std::wcout << L"Đã xóa file: " << target << L".\n";
        return;
    }
    // Nếu không phải file, thử xóa thư mục rỗng
    if (RemoveDirectoryW(targetW.c_str())) {
        std::wcout << L"Đã xóa thư mục: " << target << L".\n";
        return;
    }

    std::wcout << L"Xóa thất bại! Kiểm tra lại tên file/thư mục.\n";
}

void cmd_forward(Command& command) {
    if (command.map.size() != 1 ||
        command.map.find(L"-dir") == command.map.end())
    {
        std::wcout << L"Lệnh forward thiếu cờ tham số.\n";
        return;
    }
    
    std::wstring subdir = command.map.at(L"-dir")[0]; // Lấy tên thư mục con
    
    // Xây dựng địa chỉ thư mục mới
    std::wstring newPath = currentPath + L"\\" +std::wstring(subdir.begin(), subdir.end()); 
    
    // Truy nhập thư mục trên
    DWORD target_attribs = GetFileAttributesW(newPath.c_str());
    if (target_attribs == INVALID_FILE_ATTRIBUTES
        || !(target_attribs & FILE_ATTRIBUTE_DIRECTORY))
    {
        std::wcout << L"Thư mục không tồn tại.\n" << std::endl;
        return;
    }

    currentPath = newPath;
    SetCurrentDirectoryW(currentPath.c_str());
}

void cmd_backward(Command& command) {
    if (command.map.size() != 0) {
        std::wcout << L"Lệnh backward không có cờ tham số.\n";
        return;
    }
    
    // Kiểm tra có ở thư mục gốc không
    if (currentPath == rootPath) {
        std::wcout << L"Đã ở thư mục gốc, không thể quay lại nữa.\n";
        return;
    }
    
    // Lấy đường dẫn thư mục cha
    size_t pos = currentPath.find_last_of(L"\\/");
    if (pos ==std::wstring::npos) return;
    std::wstring parentPath = currentPath.substr(0, pos);
    
    // Kiểm tra nếu thư mục cha vượt ra ngoài rootPath
    if (parentPath.length() < rootPath.length() 
        || parentPath.substr(0, rootPath.length()) != rootPath) 
    {
        std::wcout << L"Không được vượt ra ngoài root directory.\n";
        return;
    } else {
        currentPath = parentPath;
    }

    // Đặt thư mục hiện tại
    SetCurrentDirectoryW(currentPath.c_str());
}

void cmd_return(Command& command) {
    currentPath = rootPath;
    SetCurrentDirectoryW(rootPath.c_str());
}

void cmd_build(Command& command) {
    bool supported = false;
    std::wstring cmd;
    
    if (command.map.size() == 2
        && command.map.find(L"-cpp") != command.map.end()
        && command.map.find(L"-exe") != command.map.end())
    {
        // srcFile: .cpp, outFile: .exe
        supported = true;

        std::wstring srcFile =std::wstring(
            command.map[L"-cpp"][0].begin(), command.map[L"-cpp"][0].end()
        );
        std::wstring outFile =std::wstring(
            command.map[L"-exe"][0].begin(), command.map[L"-exe"][0].end()
        );
        
        cmd = L"g++ \"" + srcFile + L"\" -o \"" + outFile + L"\"";
    } else if (command.map.size() == 1
        && command.map.find(L"-java") != command.map.end())
    {
        // srcFile: .java, outFile: .class
        supported = true;
        
        std::wstring srcFile =std::wstring(
            command.map[L"-java"][0].begin(), command.map[L"-java"][0].end()
        );
        
        cmd = L"javac \"" + srcFile + L"\"";
    }

    if (!supported) {
        std::wcout << L"Lệnh build không hỗ trợ bộ tham số được truyền.\n";
        return;
    }
    
    if (_wsystem(cmd.c_str()) == -1) {
        std::wcout << L"Build file thất bại, hãy thử lại.\n";
    } else {
        std::wcout << L"Build file thành công.\n";
    }
}

void cmd_run(Command& command) {
    // Lấy tham số
    if (command.map.find(L"-argS") == command.map.end()) {
        std::wcout << L"Lệnh thiếu cờ tham số!\n";
        return;
    }
    std::vector<std::wstring> params = command.map[L"-argS"];

    // Kiểm tra -bg flag
    bool runInBackground = false;
    if (command.map.find(L"-bg") != command.map.end()) {
        const auto& bgParams = command.map[L"-bg"];
        if (!bgParams.empty() && (bgParams[0] == L"yes")) {
            runInBackground = true;
        }
    }

    std::wstring exePath = params[0];
    std::wstring cmd;

    if (runInBackground) {
        // Chạy nền: start không chờ, không hiện cửa sổ console mới
        cmd = L"start \"\" /B \"" + exePath + L"\"";
    } else {
        // Chạy foreground: chạy và chờ kết thúc
        cmd = L"\"" + exePath + L"\"";
    }

    for (size_t i = 1; i < params.size(); ++i) {
        cmd += L" " + params[i];
    }

    int ret = _wsystem(cmd.c_str());
    if (ret == -1) {
        std::wcout << L"Chạy file thất bại. Hãy thử lại.\n";
    } else {
        std::wcout << L"Chạy file thành công.\n";
    }
}

void cmd_runbatch(Command& command) {
    // Kiểm tra cờ tham số tồn tại không
    if (command.map.find(L"-argS") == command.map.end()) {
        std::wcout << L"Lệnh thiếu cờ tham số!\n";
        return;
    }

    // Kiểm tra -bg flag
    bool runInBackground = false;
    if (command.map.find(L"-bg") != command.map.end()) {
        const auto& bgParams = command.map[L"-bg"];
        if (!bgParams.empty() && (bgParams[0] == L"yes")) {
            runInBackground = true;
        }
    }

    // Xây dựng lệnh
    std::wstring cmd;
    if (runInBackground) {
        cmd = L"start \"\" /B cmd /c ";
    } else {
        cmd = L"cmd /c ";
    }

    // Thêm các tham số vào lệnh
    std::vector<std::wstring> params = command.map[L"-argS"];
    for (auto& param: params) {
        cmd += L"\"" + param + L"\" ";
    }

    // Thực thi lệnh
    if (_wsystem(cmd.c_str()) == -1) {
        std::wcout << L"Chạy file thất bại. Hãy thử lại.\n";
    } else {
        std::wcout << L"Chạy file thành công.\n";
    }
}

void cmd_runclass(Command& command) {
    // Kiểm tra cờ tham số tồn tại không
    if (command.map.find(L"-argS") == command.map.end()) {
        std::wcout << L"Lệnh thiếu cờ tham số!\n";
        return;
    }

    // Kiểm tra -bg flag
    bool runInBackground = false;
    if (command.map.find(L"-bg") != command.map.end()) {
        const auto& bgParams = command.map[L"-bg"];
        if (!bgParams.empty() && (bgParams[0] == L"yes")) {
            runInBackground = true;
        }
    }

    // Xây dựng lệnh
    std::wstring classpath = rootPath.substr(0, rootPath.find_last_of(L"\\/"));
    std::wstring cmd;

    if (runInBackground) {
        cmd = L"start \"\" /B java -cp \"" + classpath + L"\"";
    } else {
        cmd = L"java -cp \"" + classpath + L"\"";
    }

    // Thêm các options vào lệnh
    if (command.map.find(L"-optionS") != command.map.end()) {
        std::vector<std::wstring> options = command.map.at(L"-optionS");
        for (auto& option: options) {
            cmd += L" " + std::wstring(option.begin(), option.end());
        }
    }

    // Thêm các tham số vào lệnh
    std::vector<std::wstring> params = command.map[L"-argS"];
    for (auto& param: params) {
        cmd += L" " + std::wstring(param.begin(), param.end());
    }

    // Thực thi lệnh
    if (_wsystem(cmd.c_str()) == -1) {
        std::wcout << L"Chạy file thất bại. Hãy thử lại.\n";
    } else {
        std::wcout << L"Chạy file thành công.\n";
    }
}

void cmd_runjar(Command& command) {
    // Kiểm tra cờ tham số tồn tại không
    if (command.map.find(L"-argS") == command.map.end()) {
        std::wcout << L"Lệnh thiếu cờ tham số!\n";
        return;
    }

    // Kiểm tra -bg flag
    bool runInBackground = false;
    if (command.map.find(L"-bg") != command.map.end()) {
        const auto& bgParams = command.map[L"-bg"];
        if (!bgParams.empty() && (bgParams[0] == L"yes")) {
            runInBackground = true;
        }
    }

    // Xây dựng lệnh
    std::wstring cmd;
    if (runInBackground) {
        cmd = L"start \"\" /B java";
    } else {
        cmd = L"java";
    }

    // Thêm các options vào lệnh
    if (command.map.find(L"-optionS") != command.map.end()) {
        std::vector<std::wstring> options = command.map.at(L"-optionS");
        for (auto& option: options) {
            cmd += L" " + std::wstring(option.begin(), option.end());
        }
    }

    cmd += L" -jar";

    // Thêm các tham số vào lệnh
    std::vector<std::wstring> params = command.map[L"-argS"];
    for (auto& param: params) {
        cmd += L" " + std::wstring(param.begin(), param.end());
    }

    // Thực thi lệnh
    if (_wsystem(cmd.c_str()) == -1) {
        std::wcout << L"Chạy file thất bại. Hãy thử lại.\n";
    } else {
        std::wcout << L"Chạy file thành công.\n";
    }
}

void cmd_sysinfo(Command& command) {
    if (!command.map.empty()) {
        std::wcout << L"Lệnh sysinfo không có cờ tham số.\n";
        return;
    }

    // Lấy thông tin hệ thống
    SYSTEM_INFO si;
    GetSystemInfo(&si);

    // Lấy thông tin bộ nhớ
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);

    // Lấy tên máy tính
    wchar_t computerName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
    GetComputerNameW(computerName, &size);

    // In thông tin hệ thống
    std::wcout << L"--- Thông tin hệ thống ---\n";
    std::wcout << L"Tên máy: " << computerName << L"\n";
    std::wcout << L"Số lượng CPU: " << si.dwNumberOfProcessors << L"\n";
    std::wcout << L"Kiểu CPU: " << si.dwProcessorType << L"\n";
    std::wcout << L"Tổng RAM: " << (statex.ullTotalPhys / (1024 * 1024)) << L" MB\n";
    std::wcout << L"RAM khả dụng: " << (statex.ullAvailPhys / (1024 * 1024)) << L" MB\n";
}

void cmd_timeinfo(Command& command) {
    if (!command.map.empty()) {
        std::wcout << L"Lệnh timeinfo không có cờ tham số.\n";
        return;
    }

    // Lấy thời gian hệ thống
    SYSTEMTIME st;
    GetLocalTime(&st);

    // In thông tin thời gian
    std::wcout << L"--- Thời gian hệ thống ---\n";
    std::wcout << L"Ngày: " << st.wDay << L"/" << st.wMonth << L"/" << st.wYear << L"\n";
    std::wcout << L"Giờ: " << st.wHour << L":" << st.wMinute << L":" << st.wSecond << L"\n";
}

void cmd_userinfo(Command& command) {
    if (!command.map.empty()) {
        std::wcout << L"Lệnh userinfo không có cờ tham số.\n";
        return;
    }

    // Lấy thông tin người dùng
    wchar_t username[256];
    DWORD size;
    if (GetUserNameW(username, &size)) {
        std::wcout << L"--- Thông tin người dùng ---\n";
        std::wcout << L"Tên người dùng: " << username << L"\n";
    } else {
        std::wcout << L"Không thể lấy thông tin người dùng.\n";
    }
}

void cmd_ipinfo(Command& command) {
    if (!command.map.empty()) {
        std::wcout << L"Lệnh ipinfo không có cờ tham số.\n";
        return;
    }

    std::wcout << L"--- Thông tin IP/network adapters (ipconfig) ---\n";
    int ret = _wsystem(L"ipconfig /all");
    if (ret == -1) {
        std::wcout << L"Không thể thực thi lệnh ipconfig.\n";
    }
}

void cmd_ping(Command& command) {
    // Kiểm tra cờ tham số tồn tại không
    if (command.map.size() != 1 ||
        command.map.find(L"-host") == command.map.end())
    {
        std::wcout << L"Lệnh ping thiếu cờ tham số.\n";
        return;
    }

    std::wstring host = command.map[L"-host"][0];
    std::wstring cmd = L"ping " + host;

    if (_wsystem(cmd.c_str()) == -1) {
        std::wcout << L"Ping thất bại. Hãy thử lại.\n";
    }
}

void cmd_traceroute(Command& command) {
    // Kiểm tra cờ tham số tồn tại không
    if (command.map.size() != 1 ||
        command.map.find(L"-host") == command.map.end())
    {
        std::wcout << L"Lệnh traceroute thiếu cờ tham số.\n";
        return;
    }

    std::wstring host = command.map[L"-host"][0];
    std::wstring cmd = L"tracert " + host;

    if (_wsystem(cmd.c_str()) == -1) {
        std::wcout << L"Traceroute thất bại. Hãy thử lại.\n";
    }
}

void cmd_nslookup(Command& command) {
    // Kiểm tra cờ tham số tồn tại không
    if (command.map.size() != 1 ||
        command.map.find(L"-host") == command.map.end())
    {
        std::wcout << L"Lệnh nslookup thiếu cờ tham số.\n";
        return;
    }

    std::wstring host = command.map[L"-host"][0];
    std::wstring cmd = L"nslookup " + host;

    if (_wsystem(cmd.c_str()) == -1) {
        std::wcout << L"Nslookup thất bại. Hãy thử lại.\n";
    }
}

void cmd_portcheck(Command& command) {
    // Kiểm tra cờ tham số tồn tại không
    if (command.map.size() != 1 ||
        command.map.find(L"-port") == command.map.end())
    {
        std::wcout << L"Lệnh portcheck thiếu cờ tham số.\n";
        return;
    }

    std::wstring port = command.map[L"-port"][0];
    std::wstring cmd = L"netstat -ano | findstr :" + port;

    if (_wsystem(cmd.c_str()) == -1) {
        std::wcout << L"Kiểm tra cổng thất bại. Hãy thử lại.\n";
    }
}

void executeCommand(Command& command) {
    if (command.name == L"intro") {
        cmd_intro(command);
    } else if (command.name == L"help") {
        cmd_help(command);
    } else if (command.name == L"exit") {
        cmd_exit(command);
    } else if (command.name == L"example") {
        cmd_example(command);
    } else if (command.name == L"echo") {
        cmd_echo(command);
    } else if (command.name == L"clrscr") {
        cmd_clrscr(command);
    } else if (command.name == L"setfgcol") {
        cmd_setfgcol(command);
    } else if (command.name == L"setbgcol") {
        cmd_setbgcol(command);
    } else if (command.name == L"viewvar") {
        cmd_viewvar(command);
    } else if (command.name == L"setvar") {
        cmd_setvar(command);
    } else if (command.name == L"getvar") {
        cmd_getvar(command);
    } else if (command.name == L"rmvar") {
        cmd_rmvar(command);
    } else if (command.name == L"viewpath") {
        cmd_viewpath(command);
    } else if (command.name == L"addpath") {
        cmd_addpath(command);
    } else if (command.name == L"rmpath") {
        cmd_rmpath(command);
    } else if (command.name == L"viewdir") {
        cmd_viewdir(command);
    } else if (command.name == L"adddir") {
        cmd_adddir(command);
    } else if (command.name == L"viewfile") {
        cmd_viewfile(command);
    } else if (command.name == L"addfile") {
        cmd_addfile(command);
    } else if (command.name == L"writefile") {
        cmd_writefile(command);
    } else if (command.name == L"rename") {
        cmd_rename(command);
    } else if (command.name == L"delete") {
        cmd_delete(command);
    } else if (command.name == L"forward") {
        cmd_forward(command);
    } else if (command.name == L"backward") {
        cmd_backward(command);
    } else if (command.name == L"return") {
        cmd_return(command);
    } else if (command.name == L"build") {
        cmd_build(command);
    } else if (command.name == L"run") {
        cmd_run(command);
    } else if (command.name == L"runbatch") {
        cmd_runbatch(command);
    } else if (command.name == L"runclass") {
        cmd_runclass(command);
    } else if (command.name == L"runjar") {
        cmd_runjar(command);
    } else if (command.name == L"sysinfo") {
        cmd_sysinfo(command);
    } else if (command.name == L"timeinfo") {
        cmd_timeinfo(command);
    } else if (command.name == L"userinfo") {
        cmd_userinfo(command);
    } else if (command.name == L"ipinfo") {
        cmd_ipinfo(command);
    } else if (command.name == L"ping") {
        cmd_ping(command);
    } else if (command.name == L"traceroute") {
        cmd_traceroute(command);
    } else if (command.name == L"nslookup") {
        cmd_nslookup(command);
    } else if (command.name == L"portcheck") {
        cmd_portcheck(command);
    } else {
        std::wcout << L"Lệnh này không được hỗ trợ.\n";
    }
}


// ------------Các hàm chính của chương trình------------------------
// Hàm khởi tạo toàn bộ hệ thống
void initialize() {
    _setmode(_fileno(stdout), _O_U16TEXT);
    initializeRootDirectory();
    initializeCommandDictionary();
}


// Hàm vòng lặp chính của chương trình
void mainloop() {
    int continuous_fail_count = 0;
    bool exit = false;
    while (!exit) { 
        std::wstring commandline = getRawCommandLine();
        auto command = decomposeCommandLine(commandline);

        // Kiểm tra và thực hiện lệnh
        if (!checkCommand(command)) {
            continuous_fail_count++;
            if (continuous_fail_count >= 3) {
                std::wcout << L"Có vẻ bạn đang gặp khó khăn. "
                            << L"Hãy gõ lệnh help để tra cú pháp lệnh đúng.\n";
            }
            continue;
        } 

        continuous_fail_count = 0;
        executeCommand(command);
    }
}

// ------------Hàm chính của chương trình------------------------
int main() {
    // Khởi tạo chương trình
    initialize();
    // Hiển thị thông tin chào mừng
    cmd_intro();
    // Bắt đầu chương trình
    mainloop();
}