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

void initialize_maps() {
    // Ký tự a-z
    for (int i = 1; i <= 26; ++i) {
        base_map["s" + to_string(i)] = string(1, 'a' + i - 1);
    }
    
    // Chữ số 0-9
    for (int i = 0; i <= 9; ++i) {
        base_map["c" + to_string(i)] = to_string(i);
    }
    
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
        tone_map[base]["a4"] = S; tone_map[base]["a5"] = H; tone_map[base]["a6"] = O;
        tone_map[base]["a7"] = N; tone_map[base]["a8"] = A;
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
}

string decode_escape_sequence(const string& str) {
    string result;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '*' && i + 1 < str.length()) {
            i++; // Bỏ qua dấu *
            if (str[i] == 'n') result += '\n';
            else if (str[i] == 't') result += '\t';
            else if (str[i] == '\\' && i + 1 < str.length() && str[i+1] == 'n') {
                result += '\n';
                i++;
            } else {
                result += str[i];
            }
        } else {
            result += str[i];
        }
    }
    return result;
}

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
    while(mods_ss >> mod) {
        modifiers.push_back(mod);
    }

    bool make_uppercase = false;
    string diacritic_mod = "";
    string tone_mod = "";
    int repeat_count = 1;

    for (const auto& m : modifiers) {
        if (m == "cl") make_uppercase = true;
        else if (m == "a0") {
            // Trường hợp đặc biệt: đ/Đ
            if (base_code == "s4") {
                return make_uppercase ? "Đ" : "đ";
            }
        }
        else if (m == "a1" || m == "a2" || m == "a3") diacritic_mod = m;
        else if (m >= "a4" && m <= "a8") tone_mod = m;
        else if (isdigit(m[0])) repeat_count = stoi(m);
    }

    // Viết hoa
    if (make_uppercase && decoded_char.length() == 1) {
        decoded_char[0] = toupper(decoded_char[0]);
    }
    
    // Dấu phụ
    if (!diacritic_mod.empty() && decoded_char.length() == 1) {
        char base_vowel = tolower(decoded_char[0]);
        if (diacritic_map.count(base_vowel) && diacritic_map.at(base_vowel).count(diacritic_mod)) {
            decoded_char = diacritic_map.at(base_vowel).at(diacritic_mod);
            if (make_uppercase) {
                if (decoded_char == "ă") decoded_char = "Ă";
                else if (decoded_char == "â") decoded_char = "Â";
                else if (decoded_char == "ê") decoded_char = "Ê";
                else if (decoded_char == "ô") decoded_char = "Ô";
                else if (decoded_char == "ơ") decoded_char = "Ơ";
                else if (decoded_char == "ư") decoded_char = "Ư";
            }
        }
    }

    // Thanh điệu
    if (!tone_mod.empty()) {
        if (tone_map.count(decoded_char) && tone_map.at(decoded_char).count(tone_mod)) {
            decoded_char = tone_map.at(decoded_char).at(tone_mod);
        }
    }
    
    // Lặp lại nếu có số
    string result;
    for (int i = 0; i < repeat_count; ++i) {
        result += decoded_char;
    }
    return result;
}

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

int main() {
    // Set locale UTF-8 cho Linux
    setlocale(LC_ALL, "en_US.UTF-8");
    
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    // Đảm bảo cout sử dụng UTF-8
    cout.imbue(locale(""));

    initialize_maps();

    string line;
    while (getline(cin, line)) {
        if (line.empty()) continue;
        
        vector<string> tokens = tokenize_input(line);
        
        for (const auto& token : tokens) {
            if (token == "s0") {
                cout << " ";
            } else if (token.rfind("oth[", 0) == 0) {
                string content = token.substr(4, token.length() - 5);
                cout << decode_escape_sequence(content);
            } else if (token[0] == 'c' && token.find('[') == string::npos) {
                cout << token.substr(1);
            } else if (token[0] == 's' || token[0] == 'c') {
                cout << solve_token(token);
            }
        }
        cout << endl;
    }

    return 0;
}