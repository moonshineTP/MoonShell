# MoonShell

**MoonShell** là một phần mềm shell mini viết bằng C++ cho Windows, hỗ trợ thao tác file, thư mục, biên dịch/chạy chương trình C++/Java, và các tiện ích quản lý file cơ bản.  
Phần mềm được thiết kế cho bài tập lớn môn Hệ điều hành.

## Tính năng chính

- **Di chuyển thư mục:**  
  - `forward -dir <subdir>`: Chuyển vào thư mục con  
  - `backward`: Quay lại thư mục cha  
  - `return`: Quay về thư mục gốc

- **Quản lý file:**  
  - `create -csv <file>`, `-txt <file>`, `-json <file>`: Tạo file rỗng  
  - `view -csv <file>`, `-txt <file>`, `-json <file>`: Xem nội dung file  
  - `write -csv <file>`, `-txt <file>`, `-json <file>`: Ghi đè nội dung file (hỗ trợ Unicode/Vietnamese)  
  - `rename -targetname <old> -newname <new>`: Đổi tên file/thư mục  
  - `delete -targetname <file/dir>`: Xóa file hoặc thư mục rỗng

- **Biên dịch và chạy chương trình:**  
  - `build -cpp <src.cpp> -exe <out.exe>`: Biên dịch file C++  
  - `build -java <src.java>`: Biên dịch file Java  
  - `runexe -argS <exe> [args...]`: Chạy file `.exe` với tham số  
  - `runclass -argS <JavaClass> [args...]`: Chạy file `.class` Java với tham số  
  - `runjar -argS <jarfile> [args...]`: Chạy file `.jar` với tham số

- **Trợ giúp:**  
  - `intro`: Giới thiệu phần mềm  
  - `help`: Danh sách lệnh và cờ  
  - `example`: Ví dụ sử dụng  
  - `exit`: Thoát chương trình

## Hướng dẫn sử dụng

1. **Biên dịch chương trình:**
    - Yêu cầu: MSVC hoặc MinGW-w64 (g++), Java JDK đã cài đặt và có trong PATH.
    - Biên dịch:  
      ```
      g++ -std=c++17 -o main.exe main.cpp
      ```

2. **Chạy chương trình:**
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
    create -txt note.txt
    write -txt note.txt
    view -txt note.txt
    rename -targetname note.txt -newname note2.txt
    delete -targetname note2.txt
    ```

## Lưu ý

- **Unicode:** Đầu ra console và file hỗ trợ tiếng Việt (UTF-8/UTF-16). Khi ghi file, nhập `###` để kết thúc ghi.
- **Chạy Java:** Đảm bảo classpath đúng, thường là thư mục cha của thư mục `root`.
- **Xóa thư mục:** Chỉ xóa được thư mục rỗng.
- **Các lệnh và cờ phân biệt số ít/số nhiều:**  
  - Cờ kết thúc bằng `S` nhận nhiều tham số (vd: `-argS`), còn lại chỉ nhận 1 tham số.

## Tác giả

moonshineTP  
Bài tập lớn môn Hệ điều hành

---