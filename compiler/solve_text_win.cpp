#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <cctype>

#ifdef _WIN32
#include <windows.h>
#endif

std::map<std::string, std::string> base_map;
std::map<char, std::map<std::string, std::string>> diacritic_map;
std::map<std::string, std::map<std::string, std::string>> tone_map;
std::map<std::string, std::string> t_map; // t0-t7: toán tử đơn

void initialize_maps() {
    // Ký tự a-z: s1→a ... s26→z
    for (int i = 1; i <= 26; ++i) {
        base_map["s" + std::to_string(i)] = std::string(1, 'a' + i - 1);
    }

    // Ký tự đặc biệt
    base_map["s27"] = ","; // dấu phẩy
    base_map["s28"] = "."; // dấu chấm

    // Chữ số: c0→0 ... c9→9
    for (int i = 0; i <= 9; ++i) {
        base_map["c" + std::to_string(i)] = std::to_string(i);
    }

    // s0 → dấu cách
    base_map["s0"] = " ";

    // Dấu phụ
    diacritic_map['a']["a1"] = "ă";
    diacritic_map['a']["a2"] = "â";
    diacritic_map['e']["a2"] = "ê";
    diacritic_map['o']["a2"] = "ô";
    diacritic_map['o']["a3"] = "ơ";
    diacritic_map['u']["a3"] = "ư";

    // Thanh điệu
    auto add_tones = [&](const std::string& base, const std::string& S, const std::string& H,
                         const std::string& O, const std::string& N, const std::string& A) {
        tone_map[base]["a4"] = S; // sắc
        tone_map[base]["a5"] = H; // huyền
        tone_map[base]["a6"] = O; // hỏi
        tone_map[base]["a7"] = N; // ngã
        tone_map[base]["a8"] = A; // nặng
    };

    add_tones("a", "á", "à", "ả", "ã", "ạ"); add_tones("A", "Á", "À", "Ả", "Ã", "Ạ");
    add_tones("ă", "ắ", "ằ", "ẳ", "ẵ", "ặ"); add_tones("Ă", "Ắ", "Ằ", "Ẳ", "Ẵ", "Ặ");
    add_tones("â", "ấ", "ầ", "ẩ", "ẫ", "ậ"); add_tones("Â", "Ấ", "Ầ", "Ẩ", "Ẫ", "Ậ");
    add_tones("e", "é", "è", "ẻ", "ẽ", "ẹ"); add_tones("E", "É", "È", "Ẻ", "Ẽ", "Ẹ");
    add_tones("ê", "ế", "ề", "ể", "ễ", "ệ"); add_tones("Ê", "Ế", "Ề", "Ể", "Ễ", "Ệ");
    add_tones("i", "í", "ì", "ỉ", "ĩ", "ị"); add_tones("I", "Í", "Ì", "Ỉ", "Ĩ", "Ị");
    add_tones("o", "ó", "ò", "ỏ", "õ", "ọ"); add_tones("O", "Ó", "Ò", "Ỏ", "Õ", "Ọ");
    add_tones("ô", "ố", "ồ", "ổ", "ỗ", "ộ"); add_tones("Ô", "Ố", "Ồ", "Ổ", "Ỗ", "Ộ");
    add_tones("ơ", "ớ", "ờ", "ở", "ỡ", "ợ"); add_tones("Ơ", "Ớ", "Ờ", "Ở", "Ỡ", "Ợ");
    add_tones("u", "ú", "ù", "ủ", "ũ", "ụ"); add_tones("U", "Ú", "Ù", "Ủ", "Ũ", "Ụ");
    add_tones("ư", "ứ", "ừ", "ử", "ữ", "ự"); add_tones("Ư", "Ứ", "Ừ", "Ử", "Ữ", "Ự");
    add_tones("y", "ý", "ỳ", "ỷ", "ỹ", "ỵ"); add_tones("Y", "Ý", "Ỳ", "Ỷ", "Ỹ", "Ỵ");

    // Toán tử: t0–t7
    t_map["t0"] = "=";
    t_map["t1"] = "+";
    t_map["t2"] = "-";
    t_map["t3"] = "*";
    t_map["t4"] = "/";
    t_map["t5"] = "^";
    t_map["t6"] = "//";
    t_map["t7"] = "%";
    // t8 xử lý riêng (bọc ngoặc)
}

// Giải mã escape sequence trong oth[...]
std::string decode_escape_sequence(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '*' && i + 1 < str.length()) {
            // *tab → \t
            if (i + 3 < str.length() && str.substr(i + 1, 3) == "tab") {
                result += '\t';
                i += 3;
            // *\n → \n
            } else if (str[i + 1] == '\\' && i + 2 < str.length() && str[i + 2] == 'n') {
                result += '\n';
                i += 2;
            } else {
                i++;
                if (str[i] == 'n')      result += '\n';
                else if (str[i] == 't') result += '\t';
                else                    result += str[i];
            }
        } else if (str[i] == '\\' && i + 1 < str.length()) {
            i++;
            if (str[i] == 'n')      result += '\n';
            else if (str[i] == 't') result += '\t';
            else { result += '\\'; result += str[i]; }
        } else {
            result += str[i];
        }
    }
    return result;
}

// Giải mã một token sX/cX với modifier
std::string solve_token(const std::string& token) {
    size_t bracket_pos = token.find('[');
    std::string base_code = token.substr(0, bracket_pos);
    std::string decoded_char;

    if (base_map.count(base_code)) {
        decoded_char = base_map[base_code];
    } else {
        return "";
    }

    if (bracket_pos == std::string::npos) {
        return decoded_char;
    }

    std::string mods_str = token.substr(bracket_pos + 1, token.length() - bracket_pos - 2);
    std::stringstream mods_ss(mods_str);
    std::string mod;
    std::vector<std::string> modifiers;
    while (mods_ss >> mod) modifiers.push_back(mod);

    bool make_uppercase = false;
    std::string diacritic_mod = "";
    std::string tone_mod = "";
    int repeat_count = 1;

    for (const auto& m : modifiers) {
        if (m == "cl") {
            make_uppercase = true;
        } else if (m == "a0") {
            if (base_code == "s4") return make_uppercase ? "Đ" : "đ";
        } else if (m == "a1" || m == "a2" || m == "a3") {
            diacritic_mod = m;
        } else if (m >= "a4" && m <= "a8") {
            tone_mod = m;
        } else if (!m.empty() && isdigit(m[0])) {
            try { repeat_count = std::stoi(m); } catch (...) {}
        }
    }

    // Viết hoa
    if (make_uppercase && decoded_char.length() == 1)
        decoded_char[0] = toupper(decoded_char[0]);

    // Dấu phụ
    if (!diacritic_mod.empty() && decoded_char.length() == 1) {
        char base_vowel = tolower(decoded_char[0]);
        if (diacritic_map.count(base_vowel) && diacritic_map.at(base_vowel).count(diacritic_mod)) {
            decoded_char = diacritic_map.at(base_vowel).at(diacritic_mod);
            if (make_uppercase) {
                std::map<std::string, std::string> upper_diac = {
                    {"ă","Ă"},{"â","Â"},{"ê","Ê"},{"ô","Ô"},{"ơ","Ơ"},{"ư","Ư"}
                };
                if (upper_diac.count(decoded_char)) decoded_char = upper_diac[decoded_char];
            }
        }
    }

    // Thanh điệu
    if (!tone_mod.empty())
        if (tone_map.count(decoded_char) && tone_map.at(decoded_char).count(tone_mod))
            decoded_char = tone_map.at(decoded_char).at(tone_mod);

    // Lặp lại
    std::string result;
    for (int i = 0; i < repeat_count; ++i) result += decoded_char;
    return result;
}

// Tokenizer: tách token, hỗ trợ phân cách space/comma/tab, bỏ qua () grouping
std::vector<std::string> tokenize_input(const std::string& input) {
    std::vector<std::string> tokens;
    std::string current_token;
    bool in_bracket = false;

    for (size_t i = 0; i < input.length(); ++i) {
        char c = input[i];

        if (c == '[') {
            in_bracket = true;
            current_token += c;
        } else if (c == ']') {
            in_bracket = false;
            current_token += c;
            tokens.push_back(current_token);
            current_token.clear();
        } else if (c == '(' && !in_bracket) {
            // grouping — bỏ qua
        } else if (c == ')' && !in_bracket) {
            if (!current_token.empty()) {
                tokens.push_back(current_token);
                current_token.clear();
            }
        } else if ((c == ' ' || c == ',' || c == '\t') && !in_bracket) {
            if (!current_token.empty()) {
                tokens.push_back(current_token);
                current_token.clear();
            }
        } else {
            current_token += c;
        }
    }

    if (!current_token.empty()) tokens.push_back(current_token);
    return tokens;
}

// Forward declaration
std::string decode_block(const std::string& content);

std::string decode_block(const std::string& content) {
    std::vector<std::string> tokens = tokenize_input(content);
    std::string result;

    for (const auto& token : tokens) {
        if (token == "s0") {
            result += " ";
        } else if (token.rfind("oth[", 0) == 0 && token.back() == ']') {
            std::string inner = token.substr(4, token.length() - 5);
            result += decode_escape_sequence(inner);
        } else if (!token.empty() && token[0] == 'c' && token.find('[') == std::string::npos) {
            if (token.length() > 1 && isdigit(token[1]))
                result += token.substr(1);
        } else if (!token.empty() && (token[0] == 's' || token[0] == 'c')) {
            result += solve_token(token);
        } else if (!token.empty() && token[0] == 't' && token.length() > 1 && isdigit(token[1])) {
            size_t br = token.find('[');
            std::string t_code = token.substr(0, br == std::string::npos ? token.length() : br);
            if (t_code == "t8") {
                if (br != std::string::npos && token.back() == ']') {
                    std::string inner = token.substr(br + 1, token.length() - br - 2);
                    result += "(" + decode_block(inner) + ")";
                }
            } else if (t_map.count(t_code)) {
                result += t_map.at(t_code);
            }
        }
    }

    return result;
}

// Xử lý một dòng thô (không nằm trong S(...)E)
void process_raw_line(const std::string& line) {
    if (line.empty()) { std::cout << "\n"; return; }
    std::vector<std::string> tokens = tokenize_input(line);
    for (const auto& token : tokens) {
        if (token == "s0") {
            std::cout << " ";
        } else if (token.rfind("oth[", 0) == 0 && token.back() == ']') {
            std::cout << decode_escape_sequence(token.substr(4, token.length() - 5));
        } else if (!token.empty() && token[0] == 'c' && token.find('[') == std::string::npos) {
            if (token.length() > 1 && isdigit(token[1])) std::cout << token.substr(1);
        } else if (!token.empty() && (token[0] == 's' || token[0] == 'c')) {
            std::cout << solve_token(token);
        }
    }
    std::cout << "\n";
    std::cout.flush();
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    initialize_maps();

    std::string line;
    std::string se_buffer;
    bool in_se = false;
    int se_depth = 0;

    while (std::getline(std::cin, line)) {
        // Chuẩn hoá line ending Windows (\r\n → \n)
        if (!line.empty() && line.back() == '\r') line.pop_back();

        if (!in_se) {
            size_t se_pos = line.find("S(");
            if (se_pos == std::string::npos) {
                process_raw_line(line);
                continue;
            }

            // Xử lý phần trước S( như dòng thô
            if (se_pos > 0) process_raw_line(line.substr(0, se_pos));

            in_se = true;
            se_depth = 0;
            se_buffer = line.substr(se_pos + 2);
        } else {
            se_buffer += " " + line;
        }

        // Tìm )E trong buffer
        bool found_end = false;
        bool in_sq = false;
        size_t end_pos = std::string::npos;

        for (size_t i = 0; i < se_buffer.length(); ++i) {
            char c = se_buffer[i];
            if (c == '[') { in_sq = true; continue; }
            if (c == ']') { in_sq = false; continue; }
            if (in_sq) continue;
            if (c == '(') { se_depth++; }
            else if (c == ')') {
                if (se_depth == 0) {
                    if (i + 1 < se_buffer.length() && se_buffer[i + 1] == 'E') {
                        end_pos = i;
                        found_end = true;
                        break;
                    }
                } else { se_depth--; }
            }
        }

        if (found_end) {
            std::string inner = se_buffer.substr(0, end_pos);
            std::cout << decode_block(inner);
            std::cout.flush();

            std::string remainder = se_buffer.substr(end_pos + 2);
            in_se = false;
            se_buffer.clear();
            se_depth = 0;

            if (!remainder.empty()) {
                size_t next_se = remainder.find("S(");
                if (next_se == std::string::npos) {
                    process_raw_line(remainder);
                } else {
                    if (next_se > 0) process_raw_line(remainder.substr(0, next_se));
                    in_se = true;
                    se_buffer = remainder.substr(next_se + 2);
                }
            }
        }
    }

    // Stream kết thúc mà buffer chưa đóng → in raw
    if (in_se && !se_buffer.empty()) {
        process_raw_line(se_buffer);
    }

    return 0;
}