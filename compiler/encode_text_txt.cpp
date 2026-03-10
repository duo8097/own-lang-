#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <locale>

using namespace std;

// Bảng ngược: UTF-8 string → token BroLang
map<string, string> char_to_token;

// Khởi tạo bảng encode
void initialize_encode_map() {
    // s1–s26: a–z
    for (int i = 1; i <= 26; ++i) {
        string ch(1, 'a' + i - 1);
        char_to_token[ch] = "s" + to_string(i);
    }

    // s1–s26 viết hoa: A–Z → sX[cl]
    for (int i = 1; i <= 26; ++i) {
        string ch(1, 'A' + i - 1);
        char_to_token[ch] = "s" + to_string(i) + "[cl]";
    }

    // c0–c9: chữ số
    for (int i = 0; i <= 9; ++i) {
        char_to_token[to_string(i)] = "c" + to_string(i);
    }

    // s0: space
    char_to_token[" "] = "s0";

    // s27, s28
    char_to_token[","] = "s27";
    char_to_token["."] = "s28";

    // đ / Đ
    char_to_token["đ"] = "s4[a0]";
    char_to_token["Đ"] = "s4[cl a0]";

    // Dấu phụ (chữ thường)
    char_to_token["ă"] = "s1[a1]";
    char_to_token["â"] = "s1[a2]";
    char_to_token["ê"] = "s5[a2]";
    char_to_token["ô"] = "s15[a2]";
    char_to_token["ơ"] = "s15[a3]";
    char_to_token["ư"] = "s21[a3]";

    // Dấu phụ (chữ hoa)
    char_to_token["Ă"] = "s1[cl a1]";
    char_to_token["Â"] = "s1[cl a2]";
    char_to_token["Ê"] = "s5[cl a2]";
    char_to_token["Ô"] = "s15[cl a2]";
    char_to_token["Ơ"] = "s15[cl a3]";
    char_to_token["Ư"] = "s21[cl a3]";

    // Thanh điệu — chữ thường
    // a
    char_to_token["á"] = "s1[a4]"; char_to_token["à"] = "s1[a5]";
    char_to_token["ả"] = "s1[a6]"; char_to_token["ã"] = "s1[a7]"; char_to_token["ạ"] = "s1[a8]";
    // ă
    char_to_token["ắ"] = "s1[a1 a4]"; char_to_token["ằ"] = "s1[a1 a5]";
    char_to_token["ẳ"] = "s1[a1 a6]"; char_to_token["ẵ"] = "s1[a1 a7]"; char_to_token["ặ"] = "s1[a1 a8]";
    // â
    char_to_token["ấ"] = "s1[a2 a4]"; char_to_token["ầ"] = "s1[a2 a5]";
    char_to_token["ẩ"] = "s1[a2 a6]"; char_to_token["ẫ"] = "s1[a2 a7]"; char_to_token["ậ"] = "s1[a2 a8]";
    // e
    char_to_token["é"] = "s5[a4]"; char_to_token["è"] = "s5[a5]";
    char_to_token["ẻ"] = "s5[a6]"; char_to_token["ẽ"] = "s5[a7]"; char_to_token["ẹ"] = "s5[a8]";
    // ê
    char_to_token["ế"] = "s5[a2 a4]"; char_to_token["ề"] = "s5[a2 a5]";
    char_to_token["ể"] = "s5[a2 a6]"; char_to_token["ễ"] = "s5[a2 a7]"; char_to_token["ệ"] = "s5[a2 a8]";
    // i
    char_to_token["í"] = "s9[a4]"; char_to_token["ì"] = "s9[a5]";
    char_to_token["ỉ"] = "s9[a6]"; char_to_token["ĩ"] = "s9[a7]"; char_to_token["ị"] = "s9[a8]";
    // o
    char_to_token["ó"] = "s15[a4]"; char_to_token["ò"] = "s15[a5]";
    char_to_token["ỏ"] = "s15[a6]"; char_to_token["õ"] = "s15[a7]"; char_to_token["ọ"] = "s15[a8]";
    // ô
    char_to_token["ố"] = "s15[a2 a4]"; char_to_token["ồ"] = "s15[a2 a5]";
    char_to_token["ổ"] = "s15[a2 a6]"; char_to_token["ỗ"] = "s15[a2 a7]"; char_to_token["ộ"] = "s15[a2 a8]";
    // ơ
    char_to_token["ớ"] = "s15[a3 a4]"; char_to_token["ờ"] = "s15[a3 a5]";
    char_to_token["ở"] = "s15[a3 a6]"; char_to_token["ỡ"] = "s15[a3 a7]"; char_to_token["ợ"] = "s15[a3 a8]";
    // u
    char_to_token["ú"] = "s21[a4]"; char_to_token["ù"] = "s21[a5]";
    char_to_token["ủ"] = "s21[a6]"; char_to_token["ũ"] = "s21[a7]"; char_to_token["ụ"] = "s21[a8]";
    // ư
    char_to_token["ứ"] = "s21[a3 a4]"; char_to_token["ừ"] = "s21[a3 a5]";
    char_to_token["ử"] = "s21[a3 a6]"; char_to_token["ữ"] = "s21[a3 a7]"; char_to_token["ự"] = "s21[a3 a8]";
    // y
    char_to_token["ý"] = "s25[a4]"; char_to_token["ỳ"] = "s25[a5]";
    char_to_token["ỷ"] = "s25[a6]"; char_to_token["ỹ"] = "s25[a7]"; char_to_token["ỵ"] = "s25[a8]";

    // Thanh điệu — chữ HoA
    // A
    char_to_token["Á"] = "s1[cl a4]"; char_to_token["À"] = "s1[cl a5]";
    char_to_token["Ả"] = "s1[cl a6]"; char_to_token["Ã"] = "s1[cl a7]"; char_to_token["Ạ"] = "s1[cl a8]";
    // Ă
    char_to_token["Ắ"] = "s1[cl a1 a4]"; char_to_token["Ằ"] = "s1[cl a1 a5]";
    char_to_token["Ẳ"] = "s1[cl a1 a6]"; char_to_token["Ẵ"] = "s1[cl a1 a7]"; char_to_token["Ặ"] = "s1[cl a1 a8]";
    // Â
    char_to_token["Ấ"] = "s1[cl a2 a4]"; char_to_token["Ầ"] = "s1[cl a2 a5]";
    char_to_token["Ẩ"] = "s1[cl a2 a6]"; char_to_token["Ẫ"] = "s1[cl a2 a7]"; char_to_token["Ậ"] = "s1[cl a2 a8]";
    // E
    char_to_token["É"] = "s5[cl a4]"; char_to_token["È"] = "s5[cl a5]";
    char_to_token["Ẻ"] = "s5[cl a6]"; char_to_token["Ẽ"] = "s5[cl a7]"; char_to_token["Ẹ"] = "s5[cl a8]";
    // Ê
    char_to_token["Ế"] = "s5[cl a2 a4]"; char_to_token["Ề"] = "s5[cl a2 a5]";
    char_to_token["Ể"] = "s5[cl a2 a6]"; char_to_token["Ễ"] = "s5[cl a2 a7]"; char_to_token["Ệ"] = "s5[cl a2 a8]";
    // I
    char_to_token["Í"] = "s9[cl a4]"; char_to_token["Ì"] = "s9[cl a5]";
    char_to_token["Ỉ"] = "s9[cl a6]"; char_to_token["Ĩ"] = "s9[cl a7]"; char_to_token["Ị"] = "s9[cl a8]";
    // O
    char_to_token["Ó"] = "s15[cl a4]"; char_to_token["Ò"] = "s15[cl a5]";
    char_to_token["Ỏ"] = "s15[cl a6]"; char_to_token["Õ"] = "s15[cl a7]"; char_to_token["Ọ"] = "s15[cl a8]";
    // Ô
    char_to_token["Ố"] = "s15[cl a2 a4]"; char_to_token["Ồ"] = "s15[cl a2 a5]";
    char_to_token["Ổ"] = "s15[cl a2 a6]"; char_to_token["Ỗ"] = "s15[cl a2 a7]"; char_to_token["Ộ"] = "s15[cl a2 a8]";
    // Ơ
    char_to_token["Ớ"] = "s15[cl a3 a4]"; char_to_token["Ờ"] = "s15[cl a3 a5]";
    char_to_token["Ở"] = "s15[cl a3 a6]"; char_to_token["Ỡ"] = "s15[cl a3 a7]"; char_to_token["Ợ"] = "s15[cl a3 a8]";
    // U
    char_to_token["Ú"] = "s21[cl a4]"; char_to_token["Ù"] = "s21[cl a5]";
    char_to_token["Ủ"] = "s21[cl a6]"; char_to_token["Ũ"] = "s21[cl a7]"; char_to_token["Ụ"] = "s21[cl a8]";
    // Ư
    char_to_token["Ứ"] = "s21[cl a3 a4]"; char_to_token["Ừ"] = "s21[cl a3 a5]";
    char_to_token["Ử"] = "s21[cl a3 a6]"; char_to_token["Ữ"] = "s21[cl a3 a7]"; char_to_token["Ự"] = "s21[cl a3 a8]";
    // Y
    char_to_token["Ý"] = "s25[cl a4]"; char_to_token["Ỳ"] = "s25[cl a5]";
    char_to_token["Ỷ"] = "s25[cl a6]"; char_to_token["Ỹ"] = "s25[cl a7]"; char_to_token["Ỵ"] = "s25[cl a8]";
}

// Lấy số byte của ký tự UTF-8 bắt đầu từ byte c
int utf8_char_len(unsigned char c) {
    if (c < 0x80) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

// Encode một ký tự (có thể multibyte UTF-8) sang token BroLang
// Trả về token nếu biết, rỗng nếu không
string encode_char(const string& ch) {
    if (char_to_token.count(ch)) {
        return char_to_token.at(ch);
    }
    return "";
}

// Encode chuỗi oth cho ký tự đặc biệt không có trong bảng
// Escape các ký tự cần thiết
string make_oth(const string& ch) {
    if (ch == "\n") return "oth[*\\n]";
    if (ch == "\t") return "oth[*tab]";
    // Ký tự bình thường → oth[ch]
    return "oth[" + ch + "]";
}

// Encode toàn bộ một dòng → chuỗi token, KHÔNG bọc S(...)E
string encode_line(const string& line) {
    vector<string> tokens;
    size_t i = 0;

    while (i < line.size()) {
        int len = utf8_char_len((unsigned char)line[i]);
        string ch = line.substr(i, len);
        i += len;

        string token = encode_char(ch);
        if (!token.empty()) {
            tokens.push_back(token);
        } else {
            // Không có trong bảng → oth
            tokens.push_back(make_oth(ch));
        }
    }

    // Ghép các token bằng dấu phẩy
    string result;
    for (size_t j = 0; j < tokens.size(); ++j) {
        if (j > 0) result += ",";
        result += tokens[j];
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

    ifstream input_file(argv[1]);
    if (!input_file.is_open()) {
        cerr << "Lỗi: Không thể mở file " << argv[1] << endl;
        return 1;
    }

    ofstream output_file(argv[2]);
    if (!output_file.is_open()) {
        cerr << "Lỗi: Không thể tạo file " << argv[2] << endl;
        return 1;
    }

    output_file.imbue(locale(""));
    initialize_encode_map();

    string line;
    bool first_line = true;
    // Đọc từng dòng, mỗi dòng encode thành một block S(...)E
    // Newline giữa các dòng encode bằng oth[*\n] bên trong block
    // → ta dùng chiến lược: gộp tất cả thành 1 block lớn, newline → oth[*\n]
    string full_content;
    while (getline(input_file, line)) {
        if (!first_line) full_content += "\n";
        first_line = false;
        full_content += line;
    }

    // Encode từng ký tự, newline → oth[*\n]
    vector<string> tokens;
    size_t i = 0;
    while (i < full_content.size()) {
        if (full_content[i] == '\n') {
            tokens.push_back("oth[*\\n]");
            i++;
            continue;
        }
        if (full_content[i] == '\t') {
            tokens.push_back("oth[*tab]");
            i++;
            continue;
        }
        int len = utf8_char_len((unsigned char)full_content[i]);
        string ch = full_content.substr(i, len);
        i += len;

        string token = encode_char(ch);
        if (!token.empty()) {
            tokens.push_back(token);
        } else {
            tokens.push_back(make_oth(ch));
        }
    }

    // Xuất ra file: S(token1,token2,...)E
    output_file << "S(";
    for (size_t j = 0; j < tokens.size(); ++j) {
        if (j > 0) output_file << ",";
        output_file << tokens[j];
    }
    output_file << ")E" << endl;

    input_file.close();
    output_file.close();

    cout << "✅ Đã mã hoá xong! Kết quả được lưu vào: " << argv[2] << endl;
    return 0;
}