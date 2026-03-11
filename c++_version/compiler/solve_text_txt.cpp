#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <locale>

using namespace std;

map<string, string> base_map;
map<char, map<string, string>> diacritic_map;
map<string, map<string, string>> tone_map;
map<string, string> t_map; // t0-t7: toán tử đơn

void initialize_maps() {
    // Ký tự a-z: s1→a ... s26→z
    for (int i = 1; i <= 26; ++i) {
        base_map["s" + to_string(i)] = string(1, 'a' + i - 1);
    }

    // Ký tự đặc biệt
    base_map["s27"] = ","; // dấu phẩy
    base_map["s28"] = "."; // dấu chấm

    // Chữ số: c0→0 ... c9→9
    for (int i = 0; i <= 9; ++i) {
        base_map["c" + to_string(i)] = to_string(i);
    }

    // s0 → dấu cách
    base_map["s0"] = " ";

    // Dấu phụ (diacritics)
    diacritic_map['a']["a1"] = "ă";
    diacritic_map['a']["a2"] = "â";
    diacritic_map['e']["a2"] = "ê";
    diacritic_map['o']["a2"] = "ô";
    diacritic_map['o']["a3"] = "ơ";
    diacritic_map['u']["a3"] = "ư";

    // Thanh điệu
    auto add_tones = [&](const string& base, const string& S, const string& H,
                         const string& O, const string& N, const string& A) {
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

// Giải mã chuỗi escape trong oth[...]
string decode_escape_sequence(const string& str) {
    string result;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '*' && i + 1 < str.length()) {
            // *tab → \t
            if (i + 3 < str.length() && str.substr(i+1, 3) == "tab") {
                result += '\t';
                i += 3;
            // *\n → \n
            } else if (str[i+1] == '\\' && i + 2 < str.length() && str[i+2] == 'n') {
                result += '\n';
                i += 2;
            } else {
                i++;
                if (str[i] == 'n') {
                    result += '\n';
                } else if (str[i] == 't') {
                    result += '\t';
                } else {
                    result += str[i];
                }
            }
        } else if (str[i] == '\\' && i + 1 < str.length()) {
            i++;
            if (str[i] == 'n') result += '\n';
            else if (str[i] == 't') result += '\t';
            else { result += '\\'; result += str[i]; }
        } else {
            result += str[i];
        }
    }
    return result;
}

// Giải mã một token dạng sX[modifiers] hoặc cX[modifiers]
string solve_token(const string& token) {
    size_t bracket_pos = token.find('[');
    string base_code = token.substr(0, bracket_pos);
    string decoded_char;

    if (base_map.count(base_code)) {
        decoded_char = base_map[base_code];
    } else {
        return "";
    }

    if (bracket_pos == string::npos) {
        return decoded_char;
    }

    string mods_str = token.substr(bracket_pos + 1, token.length() - bracket_pos - 2);
    stringstream mods_ss(mods_str);
    string mod;
    vector<string> modifiers;
    while (mods_ss >> mod) {
        modifiers.push_back(mod);
    }

    bool make_uppercase = false;
    string diacritic_mod = "";
    string tone_mod = "";
    int repeat_count = 1;

    for (const auto& m : modifiers) {
        if (m == "cl") {
            make_uppercase = true;
        } else if (m == "a0") {
            // đ / Đ (chỉ áp dụng với s4 = d)
            if (base_code == "s4") {
                return make_uppercase ? "Đ" : "đ";
            }
        } else if (m == "a1" || m == "a2" || m == "a3") {
            diacritic_mod = m;
        } else if (m >= "a4" && m <= "a8") {
            tone_mod = m;
        } else if (!m.empty() && isdigit(m[0])) {
            try { repeat_count = stoi(m); } catch (...) {}
        }
    }

    // Viết hoa ký tự đơn
    if (make_uppercase && decoded_char.length() == 1) {
        decoded_char[0] = toupper(decoded_char[0]);
    }

    // Dấu phụ
    if (!diacritic_mod.empty() && decoded_char.length() == 1) {
        char base_vowel = tolower(decoded_char[0]);
        if (diacritic_map.count(base_vowel) && diacritic_map.at(base_vowel).count(diacritic_mod)) {
            decoded_char = diacritic_map.at(base_vowel).at(diacritic_mod);
            if (make_uppercase) {
                map<string,string> upper_diac = {
                    {"ă","Ă"},{"â","Â"},{"ê","Ê"},{"ô","Ô"},{"ơ","Ơ"},{"ư","Ư"}
                };
                if (upper_diac.count(decoded_char)) decoded_char = upper_diac[decoded_char];
            }
        }
    }

    // Thanh điệu
    if (!tone_mod.empty()) {
        if (tone_map.count(decoded_char) && tone_map.at(decoded_char).count(tone_mod)) {
            decoded_char = tone_map.at(decoded_char).at(tone_mod);
        }
    }

    // Lặp lại
    string result;
    for (int i = 0; i < repeat_count; ++i) {
        result += decoded_char;
    }
    return result;
}

// Tách token từ chuỗi bên trong S(...)E, hỗ trợ dấu phân cách: space, comma, tab
// Ngoài ra xử lý ngoặc đơn bọc như (oth[*tab],s19,...)
vector<string> tokenize_input(const string& input) {
    vector<string> tokens;
    string current_token;
    bool in_bracket = false;  // trong []
    bool in_paren = false;    // trong ()

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
            // Bắt đầu nhóm (): bỏ qua dấu ngoặc, tiếp tục tokenize bên trong
            in_paren = true;
        } else if (c == ')' && !in_bracket) {
            // Kết thúc nhóm
            if (!current_token.empty()) {
                tokens.push_back(current_token);
                current_token.clear();
            }
            in_paren = false;
        } else if ((c == ' ' || c == ',' || c == '\t') && !in_bracket) {
            if (!current_token.empty()) {
                tokens.push_back(current_token);
                current_token.clear();
            }
        } else {
            current_token += c;
        }
    }

    if (!current_token.empty()) {
        tokens.push_back(current_token);
    }

    return tokens;
}

// Trích xuất nội dung bên trong S(...)E từ một dòng
// Trả về vector các block S(...)E tìm được trong dòng
vector<string> extract_SE_blocks(const string& line) {
    vector<string> blocks;
    size_t pos = 0;
    while (pos < line.length()) {
        size_t s_pos = line.find("S(", pos);
        if (s_pos == string::npos) break;
        // Tìm )E tương ứng — cần đếm ngoặc để tránh nhầm
        int depth = 0;
        size_t end_pos = string::npos;
        for (size_t i = s_pos + 2; i < line.length(); ++i) {
            if (line[i] == '(') depth++;
            else if (line[i] == ')') {
                if (depth == 0) {
                    // Kiểm tra xem sau ) có 'E' không
                    if (i + 1 < line.length() && line[i+1] == 'E') {
                        end_pos = i;
                        break;
                    } else {
                        // ) bình thường, không phải )E
                    }
                } else {
                    depth--;
                }
            }
        }
        if (end_pos == string::npos) break;
        string inner = line.substr(s_pos + 2, end_pos - s_pos - 2);
        blocks.push_back(inner);
        pos = end_pos + 2; // bỏ qua )E
    }
    return blocks;
}

// Xử lý và giải mã một block nội dung (đã trích từ S(...)E)
string decode_block(const string& content) {
    vector<string> tokens = tokenize_input(content);
    string result;

    for (const auto& token : tokens) {
        if (token == "s0") {
            result += " ";
        } else if (token.rfind("oth[", 0) == 0 && token.back() == ']') {
            string inner = token.substr(4, token.length() - 5);
            result += decode_escape_sequence(inner);
        } else if (!token.empty() && token[0] == 'c' && token.find('[') == string::npos) {
            // cX không có bracket → in số
            if (token.length() > 1 && isdigit(token[1])) {
                result += token.substr(1);
            }
        } else if (!token.empty() && (token[0] == 's' || token[0] == 'c')) {
            result += solve_token(token);
        // Token toán tử: t0-t7 (đơn), t8[...] (bọc ngoặc)
        } else if (!token.empty() && token[0] == 't' && token.length() > 1 && isdigit(token[1])) {
            size_t br = token.find('[');
            string t_code = token.substr(0, br == string::npos ? token.length() : br);
            if (t_code == "t8") {
                // t8[a,b,c,...] → (nội_dung)
                if (br != string::npos && token.back() == ']') {
                    string inner = token.substr(br + 1, token.length() - br - 2);
                    result += "(" + decode_block(inner) + ")";
                }
            } else if (t_map.count(t_code)) {
                result += t_map.at(t_code);
            }
        }
        // Bỏ qua các token không nhận ra
    }

    return result;
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "en_US.UTF-8");

    if (argc != 3) {
        cerr << "Cách dùng: " << argv[0] << " <input_file> <output_file>" << endl;
        cerr << "Ví dụ:    " << argv[0] << " input.txt output.txt" << endl;
        return 1;
    }

    string input_filename  = argv[1];
    string output_filename = argv[2];

    ifstream input_file(input_filename);
    if (!input_file.is_open()) {
        cerr << "Lỗi: Không thể mở file " << input_filename << endl;
        return 1;
    }

    ofstream output_file(output_filename);
    if (!output_file.is_open()) {
        cerr << "Lỗi: Không thể tạo file " << output_filename << endl;
        input_file.close();
        return 1;
    }

    output_file.imbue(locale(""));
    initialize_maps();

    // Đọc toàn bộ file vào một string để hỗ trợ S(...)E trải nhiều dòng
    string full_input((istreambuf_iterator<char>(input_file)),
                       istreambuf_iterator<char>());

    size_t pos = 0;
    while (pos <= full_input.length()) {
        size_t se_start = full_input.find("S(", pos);

        // Phần text thô TRƯỚC S( → fallback xử lý line-by-line
        string raw_before = (se_start == string::npos)
                            ? full_input.substr(pos)
                            : full_input.substr(pos, se_start - pos);

        if (!raw_before.empty()) {
            stringstream ss(raw_before);
            string raw_line;
            while (getline(ss, raw_line)) {
                if (raw_line.empty()) {
                    output_file << "\n";
                    continue;
                }
                vector<string> tokens = tokenize_input(raw_line);
                for (const auto& token : tokens) {
                    if (token == "s0") {
                        output_file << " ";
                    } else if (token.rfind("oth[", 0) == 0 && token.back() == ']') {
                        string inner2 = token.substr(4, token.length() - 5);
                        output_file << decode_escape_sequence(inner2);
                    } else if (!token.empty() && token[0] == 'c' && token.find('[') == string::npos) {
                        if (token.length() > 1 && isdigit(token[1])) {
                            output_file << token.substr(1);
                        }
                    } else if (!token.empty() && (token[0] == 's' || token[0] == 'c')) {
                        output_file << solve_token(token);
                    }
                }
                output_file << "\n";
            }
        }

        if (se_start == string::npos) break;

        // Tìm )E đóng tương ứng — đếm depth ngoặc, bỏ qua nội dung trong []
        int depth = 0;
        size_t end_pos = string::npos;
        bool in_sq = false;
        for (size_t i = se_start + 2; i < full_input.length(); ++i) {
            char c = full_input[i];
            if (c == '[') { in_sq = true; continue; }
            if (c == ']') { in_sq = false; continue; }
            if (in_sq) continue;
            if (c == '(') { depth++; }
            else if (c == ')') {
                if (depth == 0) {
                    if (i + 1 < full_input.length() && full_input[i+1] == 'E') {
                        end_pos = i;
                        break;
                    }
                } else {
                    depth--;
                }
            }
        }

        if (end_pos == string::npos) break;

        // Trích nội dung trong S(...)E, thay newline → space cho tokenizer
        string inner = full_input.substr(se_start + 2, end_pos - se_start - 2);
        for (char& c : inner) {
            if (c == '\n') c = ' ';
        }

        output_file << decode_block(inner);
        // Newline sau S(...)E do người dùng tự encode bằng oth[*\n]

        pos = end_pos + 2; // bỏ qua )E
    }

    input_file.close();
    output_file.close();

    cout << "✅ Đã giải mã xong! Kết quả được lưu vào: " << output_filename << endl;
    return 0;
}