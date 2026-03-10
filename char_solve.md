# 📖 Bảng mã BroLang

---

## 🔤 Chữ cái — s1 đến s26

| Token | Ký tự | Token | Ký tự | Token | Ký tự |
|-------|-------|-------|-------|-------|-------|
| s1  | a | s10 | j | s19 | s |
| s2  | b | s11 | k | s20 | t |
| s3  | c | s12 | l | s21 | u |
| s4  | d | s13 | m | s22 | v |
| s5  | e | s14 | n | s23 | w |
| s6  | f | s15 | o | s24 | x |
| s7  | g | s16 | p | s25 | y |
| s8  | h | s17 | q | s26 | z |
| s9  | i | s18 | r |       |   |

---

## 🔢 Chữ số — c0 đến c9

| Token | c0 | c1 | c2 | c3 | c4 | c5 | c6 | c7 | c8 | c9 |
|-------|----|----|----|----|----|----|----|----|----|----|
| Ký tự | 0  | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8  | 9  |

---

## 🔣 Ký tự đặc biệt

| Token | Ký tự | Ghi chú |
|-------|-------|---------|
| s0  | ` ` | dấu cách |
| s27 | `,` | dấu phẩy |
| s28 | `.` | dấu chấm |

---

## ➕ Toán tử — t0 đến t8

| Token | Ký tự | Ghi chú |
|-------|-------|---------|
| t0 | `=`  | bằng |
| t1 | `+`  | cộng |
| t2 | `-`  | trừ |
| t3 | `*`  | nhân |
| t4 | `/`  | chia |
| t5 | `^`  | mũ |
| t6 | `//` | chia nguyên |
| t7 | `%`  | chia dư |
| t8 | `(…)` | bọc ngoặc — nội dung bên trong được giải mã đệ quy |

**Ví dụ t8:**
```
S(t8[c1,t1,c1,t0,c2])E  →  (1+1=2)
```

---

## ✏️ Modifier

Modifier đặt trong `[]` ngay sau token, cách nhau bằng dấu cách.

```
sX[modifier1 modifier2 ...]
```

### Viết hoa & đặc biệt

| Modifier | Tác dụng | Ví dụ |
|----------|----------|-------|
| `cl` | Viết hoa | `s1[cl]` → `A` |
| `a0` | Chuyển d → đ (chỉ với s4) | `s4[a0]` → `đ`, `s4[cl a0]` → `Đ` |
| `<số>` | Lặp lại n lần | `s1[3]` → `aaa` |

### Dấu phụ

| Modifier | Tác dụng | Áp dụng cho |
|----------|----------|-------------|
| `a1` | breve ˘  | `a` → `ă` |
| `a2` | circumflex ^ | `a` → `â`, `e` → `ê`, `o` → `ô` |
| `a3` | horn     | `o` → `ơ`, `u` → `ư` |

### Thanh điệu

| Modifier | Thanh | Ký hiệu |
|----------|-------|---------|
| `a4` | sắc   | ́ |
| `a5` | huyền | ̀ |
| `a6` | hỏi   | ̉ |
| `a7` | ngã   | ̃ |
| `a8` | nặng  | ̣ |

> Dấu phụ và thanh điệu kết hợp được: `s5[a2 a7]` → `ễ`, `s1[cl a1 a4]` → `Ắ`

---

## 🔣 oth — Ký tự ngoài bảng mã

Dùng `oth[...]` cho ký tự không có trong bảng, hoặc escape sequence.

| Cú pháp | Kết quả | Ghi chú |
|---------|---------|---------|
| `oth[#]` | `#` | ký tự bất kỳ |
| `oth[<<]` | `<<` | nhiều ký tự cùng lúc |
| `oth[*\n]` | newline | xuống dòng |
| `oth[*tab]` | tab | thụt đầu dòng |

---

## 🧩 Format tổng quát

Mọi đoạn mã được bọc trong `S(...)E`. Các token cách nhau bằng dấu cách, dấu phẩy, hoặc tab. Có thể viết trải nhiều dòng.

```
S(token1,token2,token3,...)E
```

---

## 🧪 Ví dụ

### Ví dụ 1 — Chuỗi ASCII

```
S(s1,s14,s20,s9,oth[-],s6,s1,s14,s0,s15,s6,s0,s4[cl],s9,s14,s15,s0,s19[cl],s1,s11,s21,s18,s1,s0,oth[=],oth[)])E
```
→ `anti-fan of Dino Sakura =)`

---

### Ví dụ 2 — Tiếng Việt có dấu

```
S(s12[cl],s9,s5[a2 a7],s21,s0,s14[cl],s8,s21[a3],s0,s25[cl],s5[a2],s14,s28)E
```
→ `Liễu Như Yên.`

---

### Ví dụ 3 — Lặp ký tự & đ/Đ

```
S(s4[cl a0 2],s21[a3 2],s15[a2 2])E
```
→ `ĐĐưưôô`

---

### Ví dụ 4 — Toán tử

```
S(t8[c1,t1,c1,t0,c2])E
```
→ `(1+1=2)`

---

### Ví dụ 5 — Code C++ (multiline)

```
S(oth[#],s0,s9,s14,s3,s12,s21,s4,s5,s0,oth[<],s9,s15,s19,s20,s18,s5,s1,s13,oth[>],oth[*\n],
s9,s14,s20,s0,s13,s1,s9,s14,oth[(],oth[)],oth[{],oth[*\n],
oth[*tab],s19,s20,s4,oth[::],s3,s15,s21,s20,s0,oth[<<],s0,oth["],s8[cl],s5,s12,s12,s15,s0,s23[cl],s15,s18,s12,s4,oth["],s0,oth[<<],s0,s5,s14,s4,s12,oth[;],oth[*\n],
oth[*tab],s18,s5,s20,s21,s18,s14,s0,c0,oth[;],oth[*\n],
oth[}])E
```

```cpp
# include <iostream>
int main(){
    std::cout << "Hello World" << endl;
    return 0;
}
```