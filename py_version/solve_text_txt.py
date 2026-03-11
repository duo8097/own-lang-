#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
BroLang Decoder — Python version (file I/O)
Dùng: python solve_text.py <input_file> <output_file>
"""

import sys
import re

# ──────────────────────────────────────────────
# Bảng mã
# ──────────────────────────────────────────────

# s1–s26: a–z
BASE_MAP = {f"s{i}": chr(ord('a') + i - 1) for i in range(1, 27)}
# s27: ,  s28: .
BASE_MAP["s27"] = ","
BASE_MAP["s28"] = "."
# s0: space
BASE_MAP["s0"] = " "
# c0–c9: chữ số
for _i in range(10):
    BASE_MAP[f"c{_i}"] = str(_i)

# Dấu phụ: base_vowel → { modifier → char }
DIACRITIC_MAP = {
    'a': {"a1": "ă", "a2": "â"},
    'e': {"a2": "ê"},
    'o': {"a2": "ô", "a3": "ơ"},
    'u': {"a3": "ư"},
}

# Thanh điệu: base_char → { modifier → char }
TONE_MAP = {}

def _add_tones(base, sac, huyen, hoi, nga, nang):
    TONE_MAP[base] = {"a4": sac, "a5": huyen, "a6": hoi, "a7": nga, "a8": nang}

_add_tones("a","á","à","ả","ã","ạ"); _add_tones("A","Á","À","Ả","Ã","Ạ")
_add_tones("ă","ắ","ằ","ẳ","ẵ","ặ"); _add_tones("Ă","Ắ","Ằ","Ẳ","Ẵ","Ặ")
_add_tones("â","ấ","ầ","ẩ","ẫ","ậ"); _add_tones("Â","Ấ","Ầ","Ẩ","Ẫ","Ậ")
_add_tones("e","é","è","ẻ","ẽ","ẹ"); _add_tones("E","É","È","Ẻ","Ẽ","Ẹ")
_add_tones("ê","ế","ề","ể","ễ","ệ"); _add_tones("Ê","Ế","Ề","Ể","Ễ","Ệ")
_add_tones("i","í","ì","ỉ","ĩ","ị"); _add_tones("I","Í","Ì","Ỉ","Ĩ","Ị")
_add_tones("o","ó","ò","ỏ","õ","ọ"); _add_tones("O","Ó","Ò","Ỏ","Õ","Ọ")
_add_tones("ô","ố","ồ","ổ","ỗ","ộ"); _add_tones("Ô","Ố","Ồ","Ổ","Ỗ","Ộ")
_add_tones("ơ","ớ","ờ","ở","ỡ","ợ"); _add_tones("Ơ","Ớ","Ờ","Ở","Ỡ","Ợ")
_add_tones("u","ú","ù","ủ","ũ","ụ"); _add_tones("U","Ú","Ù","Ủ","Ũ","Ụ")
_add_tones("ư","ứ","ừ","ử","ữ","ự"); _add_tones("Ư","Ứ","Ừ","Ử","Ữ","Ự")
_add_tones("y","ý","ỳ","ỷ","ỹ","ỵ"); _add_tones("Y","Ý","Ỳ","Ỷ","Ỹ","Ỵ")

# Dấu phụ viết hoa
UPPER_DIAC = {"ă":"Ă","â":"Â","ê":"Ê","ô":"Ô","ơ":"Ơ","ư":"Ư"}

# Toán tử: t0–t7
T_MAP = {
    "t0": "=", "t1": "+", "t2": "-", "t3": "*",
    "t4": "/", "t5": "^", "t6": "//", "t7": "%",
}


# ──────────────────────────────────────────────
# Decode escape sequence trong oth[...]
# ──────────────────────────────────────────────

def decode_escape(s: str) -> str:
    result = []
    i = 0
    while i < len(s):
        if s[i] == '*' and i + 1 < len(s):
            # *tab → \t
            if s[i+1:i+4] == "tab":
                result.append('\t')
                i += 4
            # *\n → \n
            elif s[i+1:i+3] == "\\n":
                result.append('\n')
                i += 3
            elif s[i+1] == 'n':
                result.append('\n')
                i += 2
            elif s[i+1] == 't':
                result.append('\t')
                i += 2
            else:
                result.append(s[i+1])
                i += 2
        elif s[i] == '\\' and i + 1 < len(s):
            if s[i+1] == 'n':
                result.append('\n'); i += 2
            elif s[i+1] == 't':
                result.append('\t'); i += 2
            else:
                result.append('\\'); result.append(s[i+1]); i += 2
        else:
            result.append(s[i]); i += 1
    return ''.join(result)


# ──────────────────────────────────────────────
# Giải mã một token sX/cX[modifiers]
# ──────────────────────────────────────────────

def solve_token(token: str) -> str:
    bracket = token.find('[')
    base_code = token[:bracket] if bracket != -1 else token

    if base_code not in BASE_MAP:
        return ""
    decoded = BASE_MAP[base_code]

    if bracket == -1:
        return decoded

    mods_str = token[bracket+1:-1]  # nội dung trong []
    mods = mods_str.split()

    make_upper = False
    diacritic_mod = ""
    tone_mod = ""
    repeat = 1

    for m in mods:
        if m == "cl":
            make_upper = True
        elif m == "a0":
            if base_code == "s4":
                return "Đ" if make_upper else "đ"
        elif m in ("a1", "a2", "a3"):
            diacritic_mod = m
        elif m in ("a4", "a5", "a6", "a7", "a8"):
            tone_mod = m
        elif m.isdigit():
            repeat = int(m)

    # a0 có thể đến sau cl → kiểm tra lại
    if "a0" in mods and base_code == "s4":
        return "Đ" if make_upper else "đ"

    # Viết hoa ký tự đơn ASCII
    if make_upper and len(decoded) == 1:
        decoded = decoded.upper()

    # Dấu phụ
    if diacritic_mod and len(decoded) == 1:
        base_vowel = decoded.lower()
        if base_vowel in DIACRITIC_MAP and diacritic_mod in DIACRITIC_MAP[base_vowel]:
            decoded = DIACRITIC_MAP[base_vowel][diacritic_mod]
            if make_upper and decoded in UPPER_DIAC:
                decoded = UPPER_DIAC[decoded]

    # Thanh điệu
    if tone_mod and decoded in TONE_MAP and tone_mod in TONE_MAP[decoded]:
        decoded = TONE_MAP[decoded][tone_mod]

    return decoded * repeat


# ──────────────────────────────────────────────
# Tokenizer
# ──────────────────────────────────────────────

def tokenize(s: str) -> list:
    tokens = []
    current = []
    in_bracket = False

    for ch in s:
        if ch == '[':
            in_bracket = True
            current.append(ch)
        elif ch == ']':
            in_bracket = False
            current.append(ch)
            tokens.append(''.join(current))
            current = []
        elif ch == '(' and not in_bracket:
            pass  # grouping — bỏ qua
        elif ch == ')' and not in_bracket:
            if current:
                tokens.append(''.join(current))
                current = []
        elif ch in (' ', ',', '\t') and not in_bracket:
            if current:
                tokens.append(''.join(current))
                current = []
        else:
            current.append(ch)

    if current:
        tokens.append(''.join(current))
    return tokens


# ──────────────────────────────────────────────
# Decode block nội dung S(...)E
# ──────────────────────────────────────────────

def decode_block(content: str) -> str:
    tokens = tokenize(content)
    result = []

    for token in tokens:
        if token == "s0":
            result.append(" ")
        elif token.startswith("oth[") and token.endswith("]"):
            inner = token[4:-1]
            result.append(decode_escape(inner))
        elif token and token[0] == 'c' and '[' not in token:
            if len(token) > 1 and token[1:].isdigit():
                result.append(token[1:])
        elif token and token[0] in ('s', 'c'):
            result.append(solve_token(token))
        elif token and token[0] == 't' and len(token) > 1 and token[1].isdigit():
            br = token.find('[')
            t_code = token[:br] if br != -1 else token
            if t_code == "t8":
                if br != -1 and token.endswith(']'):
                    inner = token[br+1:-1]
                    result.append("(" + decode_block(inner) + ")")
            elif t_code in T_MAP:
                result.append(T_MAP[t_code])

    return ''.join(result)


# ──────────────────────────────────────────────
# Xử lý một dòng thô (fallback, không có S(...)E)
# ──────────────────────────────────────────────

def process_raw_line(line: str) -> str:
    if not line:
        return ""
    tokens = tokenize(line)
    result = []
    for token in tokens:
        if token == "s0":
            result.append(" ")
        elif token.startswith("oth[") and token.endswith("]"):
            result.append(decode_escape(token[4:-1]))
        elif token and token[0] == 'c' and '[' not in token:
            if len(token) > 1 and token[1:].isdigit():
                result.append(token[1:])
        elif token and token[0] in ('s', 'c'):
            result.append(solve_token(token))
    return ''.join(result)


# ──────────────────────────────────────────────
# Main: đọc file, xử lý S(...)E multiline
# ──────────────────────────────────────────────

def process(input_path: str, output_path: str):
    with open(input_path, 'r', encoding='utf-8') as f:
        full_input = f.read()

    out_parts = []
    pos = 0

    while pos <= len(full_input):
        se_start = full_input.find("S(", pos)

        # Phần thô trước S(
        raw_before = full_input[pos:se_start] if se_start != -1 else full_input[pos:]
        if raw_before:
            for raw_line in raw_before.splitlines(keepends=True):
                stripped = raw_line.rstrip('\n').rstrip('\r')
                out_parts.append(process_raw_line(stripped) + "\n")

        if se_start == -1:
            break

        # Tìm )E đóng — đếm depth, bỏ qua nội dung trong []
        depth = 0
        in_sq = False
        end_pos = -1
        i = se_start + 2
        while i < len(full_input):
            c = full_input[i]
            if c == '[':
                in_sq = True
            elif c == ']':
                in_sq = False
            elif not in_sq:
                if c == '(':
                    depth += 1
                elif c == ')':
                    if depth == 0:
                        if i + 1 < len(full_input) and full_input[i+1] == 'E':
                            end_pos = i
                            break
                    else:
                        depth -= 1
            i += 1

        if end_pos == -1:
            break

        # Trích nội dung, thay newline → space
        inner = full_input[se_start+2:end_pos].replace('\n', ' ')
        out_parts.append(decode_block(inner))

        pos = end_pos + 2  # bỏ qua )E

    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(''.join(out_parts))

    print(f"✅ Đã giải mã xong! Kết quả được lưu vào: {output_path}")


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(f"Cách dùng: python {sys.argv[0]} <input_file> <output_file>")
        print(f"Ví dụ:     python {sys.argv[0]} input.txt output.txt")
        sys.exit(1)
    process(sys.argv[1], sys.argv[2])