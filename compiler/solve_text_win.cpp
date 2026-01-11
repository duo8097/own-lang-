#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>

// Để hiển thị tiếng Việt trên console Windows
#ifdef _WIN32
#include <windows.h>
#endif

// Khai báo các map để lưu trữ quy tắc giải mã
std::map<std::string, std::string> base_map;
std::map<char, std::map<std::string, std::string>> diacritic_map; // Dấu phụ: ă, â, ê, ô, ơ, ư
std::map<std::string, std::map<std::string, std::string>> tone_map; // Thanh điệu: sắc, huyền, hỏi, ngã, nặng
std::map<std::string, std::string> uppercase_map;

/**
 * @brief Khởi tạo các map chứa tất cả quy tắc giải mã.
 */
void initialize_maps() {
    // --- Map cơ bản (s1->a, c1->1, ...) ---
    for (int i = 1; i <= 26; ++i) {
        base_map["s" + std::to_string(i)] = std::string(1, 'a' + i - 1);
    }
    // Chữ số sẽ được xử lý trực tiếp trong main, nhưng vẫn có thể thêm vào đây nếu muốn
    for (int i = 0; i <= 9; ++i) {
        base_map["c" + std::to_string(i)] = std::to_string(i);
    }
    base_map["s0"] = " ";

    // --- Map cho dấu phụ (brever, circumflex, horn) ---
    // a1: breve (dấu á)
    diacritic_map['a']["a1"] = "ă";
    // a2: circumflex (dấu mũ ^)
    diacritic_map['a']["a2"] = "â";
    diacritic_map['e']["a2"] = "ê";
    diacritic_map['o']["a2"] = "ô";
    // a3: horn (dấu móc)
    diacritic_map['o']["a3"] = "ơ";
    diacritic_map['u']["a3"] = "ư";

    // --- Map cho thanh điệu (sắc, huyền, hỏi, ngã, nặng) ---
    // Helper lambda để thêm các thanh điệu cho một cặp nguyên âm thường/hoa
    auto add_tones = [&](const std::string& base, const std::string& S, const std::string& H, const std::string& O, const std::string& N, const std::string& A) {
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

/**
 * @brief Giải mã một token đơn lẻ (ví dụ: "s12[cl]", "s5[a2 a7]").
 * @param token Chuỗi token cần giải mã.
 * @return Chuỗi ký tự đã được giải mã.
 */
std::string solve_token(const std::string& token) {
    size_t bracket_pos = token.find('[');
    std::string base_code = token.substr(0, bracket_pos);
    std::string decoded_char;

    // Tìm ký tự cơ bản
    if (base_map.count(base_code)) {
        decoded_char = base_map[base_code];
    } else {
        return "?"; // Không tìm thấy quy tắc
    }
    
    // Nếu không có modifier, trả về luôn
    if (bracket_pos == std::string::npos) {
        return decoded_char;
    }

    // Tách các modifier
    std::string mods_str = token.substr(bracket_pos + 1, token.length() - bracket_pos - 2);
    std::stringstream mods_ss(mods_str);
    std::string mod;
    std::vector<std::string> modifiers;
    while(mods_ss >> mod) {
        modifiers.push_back(mod);
    }

    bool make_uppercase = false;
    std::string diacritic_mod = "";
    std::string tone_mod = "";

    for (const auto& m : modifiers) {
        if (m == "cl") make_uppercase = true;
        else if (m == "a1" || m == "a2" || m == "a3") diacritic_mod = m;
        else if (m >= "a4" && m <= "a8") tone_mod = m;
    }

    // Xử lý trường hợp đặc biệt: s4[a0] -> đ
    if (base_code == "s4") {
         for (const auto& m : modifiers) {
            if (m == "a0") return make_uppercase ? "Đ" : "đ";
        }
    }
    
    // 1. Áp dụng viết hoa cho ký tự cơ bản (nếu cần) trước khi tra map dấu
    if (make_uppercase && decoded_char.length() == 1) {
        decoded_char[0] = toupper(decoded_char[0]);
    }
    
    // 2. Áp dụng dấu phụ (ă, â, ê, ...)
    if (!diacritic_mod.empty() && decoded_char.length() == 1) {
        char base_vowel = tolower(decoded_char[0]); // Luôn tra map với chữ thường
        if (diacritic_map.count(base_vowel) && diacritic_map.at(base_vowel).count(diacritic_mod)) {
            decoded_char = diacritic_map.at(base_vowel).at(diacritic_mod);
            // Áp dụng lại viết hoa nếu cần
            if (make_uppercase) {
                // Đơn giản hóa việc chuyển sang chữ hoa bằng cách tra map ngược
                if (decoded_char == "ă") decoded_char = "Ă";
                else if (decoded_char == "â") decoded_char = "Â";
                else if (decoded_char == "ê") decoded_char = "Ê";
                else if (decoded_char == "ô") decoded_char = "Ô";
                else if (decoded_char == "ơ") decoded_char = "Ơ";
                else if (decoded_char == "ư") decoded_char = "Ư";
            }
        }
    }

    // 3. Áp dụng thanh điệu (sắc, huyền, ...)
    if (!tone_mod.empty()) {
        if (tone_map.count(decoded_char) && tone_map.at(decoded_char).count(tone_mod)) {
            decoded_char = tone_map.at(decoded_char).at(tone_mod);
        }
    }
    
    return decoded_char;
}

int main() {
    // Thiết lập để console có thể hiển thị UTF-8
    #ifdef _WIN32
        SetConsoleOutputCP(65001);
    #endif
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    initialize_maps();

    std::string token;
    // Vòng lặp đọc input thông minh hơn, có thể ghép các token bị tách bởi dấu cách
    while (std::cin >> token) {
        // Nếu một token có '[' nhưng không có ']', ghép nó với token tiếp theo
        if (token.find('[') != std::string::npos && token.back() != ']') {
            std::string next_token;
            while (std::cin >> next_token) {
                token += " " + next_token;
                if (next_token.back() == ']') break;
            }
        }

        // Xử lý các loại token
        if (token == "s0") {
            std::cout << " ";
        } else if (token.rfind("oth[", 0) == 0) { // Bắt đầu bằng "oth["
            std::cout << token.substr(4, token.length() - 5);
        } else if (token[0] == 'c') {
            std::cout << token.substr(1, token.find('[') - 1);
        } else if (token[0] == 's') {
            std::cout << solve_token(token);
        } else {
            // In ra token không xác định nếu cần debug
            // std::cout << "{" << token << "}";
        }
    }
    std::cout << std::endl;

    return 0;
}
