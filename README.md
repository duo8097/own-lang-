# 🧠 BroLang

> Một ngôn ngữ mã hoá / giải mã ký tự kiểu "não to",  
> viết chữ tiếng Việt có dấu, code được cả C++ 😭🔥

BroLang là một hệ mã hoá dạng token, trong đó mỗi ký tự được biểu diễn bằng một mã ngắn gọn. Hỗ trợ đầy đủ tiếng Việt có dấu, viết hoa, lặp ký tự, toán tử, và ký tự đặc biệt.

---

## 📦 Build

### Yêu cầu
- C++17 trở lên
- Linux / Windows (terminal hỗ trợ UTF-8)

### Decoder — BroLang → text
```bash
g++ solve_text.cpp -std=c++17 -O2 -o solve_text
./solve_text input.txt output.txt
```

### Encoder — text → BroLang
```bash
g++ encode_text.cpp -std=c++17 -O2 -o encode_text
./encode_text input.txt output.txt
```

---

## 📖 Bảng mã

Xem chi tiết tại [char solve.md](char_solve.md)

---

## 🧪 Ví dụ nhanh

**Encode:**
```
Liễu Như Yên.  →  S(s12[cl],s9,s5[a2 a7],s21,s0,s14[cl],s8,s21[a3],s0,s25[cl],s5[a2],s14,s28)E
```

**Decode:**
```
S(s12[cl],s9,s5[a2 a7],s21,s0,s14[cl],s8,s21[a3],s0,s25[cl],s5[a2],s14,s28)E  →  Liễu Như Yên.
```

---

## 🗺️ Dự định

- [ ] Bản Python
- [ ] `run.bat` cho Windows
- [ ] Interactive REPL
