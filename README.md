# MoonShell

**MoonShell** là một phần mềm shell mini viết bằng C++ cho Windows, hỗ trợ thao tác file, thư mục, biên dịch/chạy chương trình C++/Java, và các tiện ích quản lý file cơ bản.  
Phần mềm được thiết kế cho bài tập lớn môn Hệ điều hành.

## Tính năng chính

- **Di chuyển thư mục:**  
  - `down -dir <subdir>`: Chuyển vào thư mục con  
  - `up`: Quay lại thư mục cha  
  - `return`: Quay về thư mục gốc

- **Quản lý file:**  
  - `create -csv <file>`, `-txt <file>`, `-json <file>`: Tạo file rỗng  
  - `view -csv <file>`, `-txt <file>`, `-json <file>`: Xem nội dung file  
  - `write -csv <file>`, `-txt <file>`, `-json <file>`: Ghi đè nội dung file (tạm thời chưa hỗ trợ Unicode/Vietnamese)  
  - `rename -targetname <old> -newname <new>`: Đổi tên file/thư mục  
  - `delete -targetname <file/dir>`: Xóa file hoặc thư mục rỗng

- **Biên dịch và chạy chương trình:**  
  - `build -c <src.c> -exe <out.exe>`: Biên dịch file C
  - `build -cpp <src.cpp> -exe <out.exe>`: Biên dịch file C++  
  - `build -java <java_package_path>`: Biên dịch file Java 
  - `run -argS [args...]`: Chạy file được hỗ trợ bởi máy (trong PATHEXT)
  - `runbatch -argS [args...]`: Chạy file .bat (được hỗ trợ riêng)
  - `runjar -optionS <java-options> -argS <jarfile> [args...]`: Chạy file `.jar` với tham số,
  - `runclass -optionS <java-options> -argS <JavaClass> [args...]`: Chạy class Java

- **Quản lý biến và đường dẫn:**  
  - `setvar -name <var> -value <val>`: Đặt biến  
  - `getvar -name <var>`: Lấy giá trị biến  
  - `rmvar -name <var>`: Xóa biến  
  - `viewvar`: Xem tất cả biến  
  - `addpath -path <dir>`: Thêm đường dẫn  
  - `rmpath -path <dir>`: Xóa đường dẫn  
  - `viewpath`: Xem các đường dẫn  
  - `adddir -dir <dir>`: Thêm thư mục  
  - `rmdir -dir <dir>`: Xóa thư mục  
  - `viewdir`: Xem các thư mục

- **Tiện ích hệ thống:**  
  - `sysinfo`: Thông tin hệ thống  
  - `timeinfo`: Thông tin thời gian  
  - `userinfo`: Thông tin người dùng  
  - `ipinfo`: Thông tin IP  
  - `ping -host <host>`: Ping host  
  - `traceroute -host <host>`: Traceroute  
  - `nslookup -host <host>`: Tra cứu DNS  
  - `portcheck -port <port>`: Kiểm tra cổng  

- **Trợ giúp:**  
  - `intro`: Giới thiệu phần mềm  
  - `help`: Danh sách lệnh và cờ  
  - `example`: Ví dụ sử dụng  
  - `exit`: Thoát chương trình

## Hướng dẫn sử dụng

2. **Chạy chương trình:**
    Bạn chỉ cần vào cmd, trỏ tới thư mục chính và chạy
    ```
    main.exe
    ```

3. **Một số ví dụ lệnh:**
    ```
    forward -dir example_matmul
    build -cpp matrix_generator.cpp -exe matrix_generator.exe
    runexe -argS matrix_generator.exe 128 256 512
    build -java MatrixMultiplication.java
    runclass -argS root.example_matmul.MatrixMultiplication matrix1.csv matrix2.csv matrix3.csv
    runjar -optionS --enable-preview -argS chessjava.jar -bg yes
    create -txt note.txt
    write -txt note.txt
    view -txt note.txt
    rename -targetname note.txt -newname note2.txt
    delete -targetname note2.txt
    setvar -name greeting -value "Xin chào"
    getvar -name greeting
    viewvar
    addpath -path C:\MyTools
    viewpath
    sysinfo
    ping -host google.com
    ```

## Lưu ý

- **Unicode:** Đầu ra console và file hỗ trợ tiếng Việt (UTF-8/UTF-16). Khi ghi file, nhập `###` để kết thúc ghi.
- **Về thư mục và file:** Đảm bảo classpath đúng, chỉ hỗ trợ chạy thư mục trong `root`, sử dụng cờ
`-bg` và tham số `yes` để chạy tiến trình nền (background)
- **Xóa thư mục:** Chỉ xóa được thư mục rỗng.
- **Các lệnh và cờ phân biệt số ít/số nhiều:**
  - Cờ kết thúc bằng `S` nhận nhiều tham số (ít nhất 1 trở lên)
  - Các cờ còn lại chỉ nhận 1 tham số.
- **Ctrl+C:** Khi nhấn Ctrl+C tại shell, tất cả tiến trình con sẽ bị kết thúc và shell sẽ thoát.

## Tác giả

moonshineTP
Bài tập lớn môn Hệ điều hành

---