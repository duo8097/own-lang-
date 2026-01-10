# 🧠 BroLang (tạm đặt tên 😎)

> Một ngôn ngữ mã hoá / giải mã ký tự kiểu “não to”,  
> viết chữ tiếng Việt có dấu, code được cả C++ 😭🔥

BroLang là một hệ mã hoá dạng token, trong đó:
- `s<number>` đại diện cho chữ cái
- `c<number>` đại diện cho chữ số
- modifier trong `[]` để xử lý:
  - viết hoa
  - dấu tiếng Việt
  - lặp ký tự
- `oth[...]` dùng cho ký tự đặc biệt / escape sequence

---

## 📦 Cài đặt & Build

### Yêu cầu
- C++17 trở lên
- Windows (khuyến nghị, vì xử lý UTF-8 console)

### Build
```bash
g++ solve_text.cpp -std=gnu++17 -O2 -o solve_text_cpp
```
---

## Dự định
- phát triển thêm bản python
- cải tiến để dễ dùng hơn (or not =))
- tạo thêm bản in & out từ file cho code
---

## Hướng dẫn
- [bảng mã](char solve.md)