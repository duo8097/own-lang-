#include <iostream>
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

    // Dấu phụ
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

// Giải mã escape sequence trong oth[...]
string decode_escape_sequence(const string& str) {
    string result;
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
    while (mods_ss >> mod) modifiers.push_back(mod);

    bool make_uppercase = false;
    string diacritic_mod = "";
    string tone_mod = "";
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
            try { repeat_count = stoi(m); } catch (...) {}
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
                map<string, string> upper_diac = {
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
    string result;
    for (int i = 0; i < repeat_count; ++i) result += decoded_char;
    return result;
}

// Tokenizer: tách token từ chuỗi, hỗ trợ phân cách bằng space/comma/tab
// Dấu () bên ngoài [] được bỏ qua (grouping)
vector<string> tokenize_input(const string& input) {
    vector<string> tokens;
    string current_token;
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
            // grouping — bỏ qua dấu ngoặc
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
string decode_block(const string& content);

// Giải mã một block nội dung
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
            if (token.length() > 1 && isdigit(token[1]))
                result += token.substr(1);
        } else if (!token.empty() && (token[0] == 's' || token[0] == 'c')) {
            result += solve_token(token);
        } else if (!token.empty() && token[0] == 't' && token.length() > 1 && isdigit(token[1])) {
            size_t br = token.find('[');
            string t_code = token.substr(0, br == string::npos ? token.length() : br);
            if (t_code == "t8") {
                if (br != string::npos && token.back() == ']') {
                    string inner = token.substr(br + 1, token.length() - br - 2);
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
void process_raw_line(const string& line) {
    if (line.empty()) { cout << "\n"; return; }
    vector<string> tokens = tokenize_input(line);
    for (const auto& token : tokens) {
        if (token == "s0") {
            cout << " ";
        } else if (token.rfind("oth[", 0) == 0 && token.back() == ']') {
            cout << decode_escape_sequence(token.substr(4, token.length() - 5));
        } else if (!token.empty() && token[0] == 'c' && token.find('[') == string::npos) {
            if (token.length() > 1 && isdigit(token[1])) cout << token.substr(1);
        } else if (!token.empty() && (token[0] == 's' || token[0] == 'c')) {
            cout << solve_token(token);
        }
    }
    cout << "\n";
    cout.flush();
}

int main() {
    setlocale(LC_ALL, "en_US.UTF-8");
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.imbue(locale(""));

    initialize_maps();

    string line;
    string se_buffer;    // buffer gom các dòng bên trong S(...)E
    bool in_se = false;  // đang trong S(...)E chưa đóng
    int se_depth = 0;    // đếm ngoặc ( ) bên trong (bỏ qua nội dung [])

    while (getline(cin, line)) {
        if (!in_se) {
            // Tìm S( trên dòng này
            size_t se_pos = line.find("S(");
            if (se_pos == string::npos) {
                // Dòng thô bình thường
                process_raw_line(line);
                continue;
            }

            // Có S( → xử lý phần trước S( như dòng thô
            if (se_pos > 0) {
                process_raw_line(line.substr(0, se_pos));
            }

            // Bắt đầu gom buffer từ sau S(
            in_se = true;
            se_depth = 0;
            se_buffer = line.substr(se_pos + 2); // bỏ "S("
        } else {
            // Đang trong S(...)E — gom tiếp, thay newline bằng space
            se_buffer += " " + line;
        }

        // Kiểm tra xem )E đã xuất hiện trong buffer chưa
        // Scan buffer tìm )E, đếm depth ngoặc (bỏ qua nội dung trong [])
        bool found_end = false;
        bool in_sq = false;
        size_t end_pos = string::npos;

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
            // Giải mã block S(...)E
            string inner = se_buffer.substr(0, end_pos);
            cout << decode_block(inner);
            cout.flush();

            // Phần còn lại sau )E trên cùng dòng (nếu có)
            string remainder = se_buffer.substr(end_pos + 2);
            in_se = false;
            se_buffer.clear();
            se_depth = 0;

            if (!remainder.empty()) {
                // Có thể còn S(...)E khác hoặc text thô — đẩy lại vào logic
                // bằng cách tái xử lý như một dòng mới
                size_t next_se = remainder.find("S(");
                if (next_se == string::npos) {
                    // Không có S( nữa → xử lý thô
                    if (!remainder.empty()) process_raw_line(remainder);
                } else {
                    // Có S( tiếp → giả lập như đọc lại dòng này
                    // (push vào cin không được, dùng cách đơn giản: gọi lại logic)
                    if (next_se > 0) process_raw_line(remainder.substr(0, next_se));
                    in_se = true;
                    se_buffer = remainder.substr(next_se + 2);
                }
            }
        }
        // Nếu chưa tìm được )E → tiếp tục getline, gom thêm dòng sau
    }

    // Nếu stream kết thúc mà vẫn còn buffer chưa đóng → in raw
    if (in_se && !se_buffer.empty()) {
        process_raw_line(se_buffer);
    }

    return 0;
}