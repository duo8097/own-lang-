## 🔤 Bảng mã cơ bản

### Chữ cái (a–z)

| Token | Ký tự | Token | Ký tự | Token | Ký tự |
|------|------|------|------|------|------|
| s1  | a | s10 | j | s19 | s |
| s2  | b | s11 | k | s20 | t |
| s3  | c | s12 | l | s21 | u |
| s4  | d | s13 | m | s22 | v |
| s5  | e | s14 | n | s23 | w |
| s6  | f | s15 | o | s24 | x |
| s7  | g | s16 | p | s25 | y |
| s8  | h | s17 | q | s26 | z |
| s9  | i | s18 | r |  |  |

---

### Chữ số

c1 → 1
c2 → 2
...
c9 → 9

---

### Ký tự đặc biệt

s0 → space
a0 → đ
cl → uppercase


---

## 🇻🇳 Modifier tiếng Việt

### Dấu phụ

a1 → ă
a2 → â, ê, ô, ...
a3 → ơ, ư, ...


### Thanh điệu

a4 → sắc
a5 → huyền
a6 → hỏi
a7 → ngã
a8 → nặng


---

## 🧩 Format

[token][modifier...]


### Ví dụ

(s4[cl a0 2] s21[a3 2] s15[a2 2])
-> ĐĐ ưư ôô

---

## 🔣 oth – ký tự khác

`oth[...]` dùng cho các ký tự không thuộc bảng mã.

- `*` biểu thị phím đặc biệt / escape sequence

### Ví dụ

oth[*tab] → 1 dấu tab


---

## 🧪 Ví dụ minh hoạ

### Ví dụ 1

**Input**
```text
s1 s14 s20 s9 oth[-] s6 s1 s14 s0 s15 s6 s0 s4[cl] s9 s14 s15 s0 s19[cl] s1 s11 s21 s18 s1 oth[=)]
```
**Output**
anti-fan of Dino Sakura =)

---

### Ví dụ 2

**Input**
```text
s12[cl] s9 s5[a2 a7] s21 s0 s14[cl] s8 s21[a3] s0 s25[cl] s5[a2] s14 oth[.]
```
**Output**
Liễu Như Yên.

---

### Ví dụ 3 – Viết code C++

**Input**
```text
oth[#],s0,s9,s14,s3,s12,s21,s4,s5,s0,oth[<],s9,s15,s19,s20,s18,s5,s1,s13,oth[>]
s9,s14,s20,s0,s13,s1,s9,s14,oth[(],oth[)],oth[{]
oth[tab],s19,s20,s4,oth[:],oth[:],s3,s15,s21,s20,s0,oth[<],oth[<],s0,oth["],s8[cl],s5,s12,s12,s15,s0,s23[cl],s15,s18,s12,s4,oth["],s0,oth[<],oth[<],s0,s19,s20,s4,oth[:],oth[:],s5,s14,s4,s12,oth[;]
oth[tab],s18,s5,s20,s21,s18,s14,s0,c0,oth[;]
oth[}]
```
**Output**
```cpp
#include <iostream>
int main(){
    std::cout << "Hello world" << endl;
    return 0;
}
```
---
