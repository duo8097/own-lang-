#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <locale>
#include <codecvt>

using namespace std;

map<string, string> char_to_code;
map<string, string> special_chars;

void initialize_encode_maps() {
    // Ký tự a-z
    for (int i = 0; i < 26; ++i) {
        char c = 'a' + i;
        char_to_code[string(1, c)] = "s" + to_string(i + 1);
    }
    
    // Chữ số 0-9
    for (int i = 0; i <= 9; ++i) {
        char_to_code[to_string(i)] = "c" + to_string(i);
    }
    
    // Khoảng trắng
    char_to_code[" "] = "s0";
    
    // Ký tự đặc biệt tiếng Việt
    // Chữ thường
    char_to_code["ă"] = "s1[a1]";
    char_to_code["â"] = "s1[a2]";
    char_to_code["đ"] = "s4[a0]";
    char_to_code["ê"] = "s5[a2]";
    char_to_code["ô"] = "s15[a2]";
    char_to_code["ơ"] = "s15[a3]";
    char_to_code["ư"] = "s21[a3]";
    
    // Chữ hoa
    char_to_code["Ă"] = "s1[cl a1]";
    char_to_code["Â"] = "s1[cl a2]";
    char_to_code["Đ"] = "s4[cl a0]";
    char_to_code["Ê"] = "s5[cl a2]";
    char_to_code["Ô"] = "s15[cl a2]";
    char_to_code["Ơ"] = "s15[cl a3]";
    char_to_code["Ư"] = "s21[cl a3]";
    
    // Thanh điệu - chữ thường
    char_to_code["á"] = "s1[a4]"; char_to_code["à"] = "s1[a5]"; char_to_code["ả"] = "s1[a6]"; 
    char_to_code["ã"] = "s1[a7]"; char_to_code["ạ"] = "s1[a8]";
    
    char_to_code["ắ"] = "s1[a1 a4]"; char_to_code["ằ"] = "s1[a1 a5]"; char_to_code["ẳ"] = "s1[a1 a6]";
    char_to_code["ẵ"] = "s1[a1 a7]"; char_to_code["ặ"] = "s1[a1 a8]";
    
    char_to_code["ấ"] = "s1[a2 a4]"; char_to_code["ầ"] = "s1[a2 a5]"; char_to_code["ẩ"] = "s1[a2 a6]";
    char_to_code["ẫ"] = "s1[a2 a7]"; char_to_code["ậ"] = "s1[a2 a8]";
    
    char_to_code["é"] = "s5[a4]"; char_to_code["è"] = "s5[a5]"; char_to_code["ẻ"] = "s5[a6]";
    char_to_code["ẽ"] = "s5[a7]"; char_to_code["ẹ"] = "s5[a8]";
    
    char_to_code["ế"] = "s5[a2 a4]"; char_to_code["ề"] = "s5[a2 a5]"; char_to_code["ể"] = "s5[a2 a6]";
    char_to_code["ễ"] = "s5[a2 a7]"; char_to_code["ệ"] = "s5[a2 a8]";
    
    char_to_code["í"] = "s9[a4]"; char_to_code["ì"] = "s9[a5]"; char_to_code["ỉ"] = "s9[a6]";
    char_to_code["ĩ"] = "s9[a7]"; char_to_code["ị"] = "s9[a8]";
    
    char_to_code["ó"] = "s15[a4]"; char_to_code["ò"] = "s15[a5]"; char_to_code["ỏ"] = "s15[a6]";
    char_to_code["õ"] = "s15[a7]"; char_to_code["ọ"] = "s15[a8]";
    
    char_to_code["ố"] = "s15[a2 a4]"; char_to_code["ồ"] = "s15[a2 a5]"; char_to_code["ổ"] = "s15[a2 a6]";
    char_to_code["ỗ"] = "s15[a2 a7]"; char_to_code["ộ"] = "s15[a2 a8]";
    
    char_to_code["ớ"] = "s15[a3 a4]"; char_to_code["ờ"] = "s15[a3 a5]"; char_to_code["ở"] = "s15[a3 a6]";
    char_to_code["ỡ"] = "s15[a3 a7]"; char_to_code["ợ"] = "s15[a3 a8]";
    
    char_to_code["ú"] = "s21[a4]"; char_to_code["ù"] = "s21[a5]"; char_to_code["ủ"] = "s21[a6]";
    char_to_code["ũ"] = "s21[a7]"; char_to_code["ụ"] = "s21[a8]";
    
    char_to_code["ứ"] = "s21[a3 a4]"; char_to_code["ừ"] = "s21[a3 a5]"; char_to_code["ử"] = "s21[a3 a6]";
    char_to_code["ữ"] = "s21[a3 a7]"; char_to_code["ự"] = "s21[a3 a8]";
    
    char_to_code["ý"] = "s25[a4]"; char_to_code["ỳ"] = "s25[a5]"; char_to_code["ỷ"] = "s25[a6]";
    char_to_code["ỹ"] = "s25[a7]"; char_to_code["ỵ"] = "s25[a8]";
    
    // Thanh điệu - chữ hoa
    char_to_code["Á"] = "s1[cl a4]"; char_to_code["À"] = "s1[cl a5]"; char_to_code["Ả"] = "s1[cl a6]";
    char_to_code["Ã"] = "s1[cl a7]"; char_to_code["Ạ"] = "s1[cl a8]";
    
    char_to_code["Ắ"] = "s1[cl a1 a4]"; char_to_code["Ằ"] = "s1[cl a1 a5]"; char_to_code["Ẳ"] = "s1[cl a1 a6]";
    char_to_code["Ẵ"] = "s1[cl a1 a7]"; char_to_code["Ặ"] = "s1[cl a1 a8]";
    
    char_to_code["Ấ"] = "s1[cl a2 a4]"; char_to_code["Ầ"] = "s1[cl a2 a5]"; char_to_code["Ẩ"] = "s1[cl a2 a6]";
    char_to_code["Ẫ"] = "s1[cl a2 a7]"; char_to_code["Ậ"] = "s1[cl a2 a8]";
    
    char_to_code["É"] = "s5[cl a4]"; char_to_code["È"] = "s5[cl a5]"; char_to_code["Ẻ"] = "s5[cl a6]";
    char_to_code["Ẽ"] = "s5[cl a7]"; char_to_code["Ẹ"] = "s5[cl a8]";
    
    char_to_code["Ế"] = "s5[cl a2 a4]"; char_to_code["Ề"] = "s5[cl a2 a5]"; char_to_code["Ể"] = "s5[cl a2 a6]";
    char_to_code["Ễ"] = "s5[cl a2 a7]"; char_to_code["Ệ"] = "s5[cl a2 a8]";
    
    char_to_code["Í"] = "s9[cl a4]"; char_to_code["Ì"] = "s9[cl a5]"; char_to_code["Ỉ"] = "s9[cl a6]";
    char_to_code["Ĩ"] = "s9[cl a7]"; char_to_code["Ị"] = "s9[cl a8]";
    
    char_to_code["Ó"] = "s15[cl a4]"; char_to_code["Ò"] = "s15[cl a5]"; char_to_code["Ỏ"] = "s15[cl a6]";
    char_to_code["Õ"] = "s15[cl a7]"; char_to_code["Ọ"] = "s15[cl a8]";
    
    char_to_code["Ố"] = "s15[cl a2 a4]"; char_to_code["Ồ"] = "s15[cl a2 a5]"; char_to_code["Ổ"] = "s15[cl a2 a6]";
    char_to_code["Ỗ"] = "s15[cl a2 a7]"; char_to_code["Ộ"] = "s15[cl a2 a8]";
    
    char_to_code["Ớ"] = "s15[cl a3 a4]"; char_to_code["Ờ"] = "s15[cl a3 a5]"; char_to_code["Ở"] = "s15[cl a3 a6]";
    char_to_code["Ỡ"] = "s15[cl a3 a7]"; char_to_code["Ợ"] = "s15[cl a3 a8]";
    
    char_to_code["Ú"] = "s21[cl a4]"; char_to_code["Ù"] = "s21[cl a5]"; char_to_code["Ủ"] = "s21[cl a6]";
    char_to_code["Ũ"] = "s21[cl a7]"; char_to_code["Ụ"] = "s21[cl a8]";
    
    char_to_code["Ứ"] = "s21[cl a3 a4]"; char_to_code["Ừ"] = "s21[cl a3 a5]"; char_to_code["Ử"] = "s21[cl a3 a6]";
    char_to_code["Ữ"] = "s21[cl a3 a7]"; char_to_code["Ự"] = "s21[cl a3 a8]";
    
    char_to_code["Ý"] = "s25[cl a4]"; char_to_code["Ỳ"] = "s25[cl a5]"; char_to_code["Ỷ"] = "s25[cl a6]";
    char_to_code["Ỹ"] = "s25[cl a7]"; char_to_code["Ỵ"] = "s25[cl a8]";
    
    // Chữ hoa A-Z (không có dấu)
    for (int i = 0; i < 26; ++i) {
        char c = 'A' + i;
        char_to_code[string(1, c)] = "s" + to_string(i + 1) + "[cl]";
    }
}

string encode_text(const string& text) {
    string result;
    
    for (size_t i = 0; i < text.length(); ++i) {
        unsigned char c = text[i];
        
        // Xử lý ký tự đặc biệt
        if (c == '\n') {
            if (!result.empty()) result += ",";
            result += "oth[\\n]";
            continue;
        } else if (c == '\t') {
            if (!result.empty()) result += ",";
            result += "oth[tab]";
            continue;
        }
        
        // Ký tự ASCII đặc biệt
        string single_char(1, c);
        if (c < 128 && !isalnum(c) && c != ' ') {
            if (!result.empty()) result += ",";
            result += "oth[" + single_char + "]";
            continue;
        }
        
        // Xử lý UTF-8 (tiếng Việt có dấu)
        if (c >= 128) {
            // Đọc ký tự UTF-8 (2-4 bytes)
            string utf8_char;
            utf8_char += c;
            
            int bytes_to_read = 0;
            if ((c & 0xE0) == 0xC0) bytes_to_read = 1;
            else if ((c & 0xF0) == 0xE0) bytes_to_read = 2;
            else if ((c & 0xF8) == 0xF0) bytes_to_read = 3;
            
            for (int j = 0; j < bytes_to_read && i + 1 < text.length(); ++j) {
                utf8_char += text[++i];
            }
            
            if (char_to_code.count(utf8_char)) {
                if (!result.empty()) result += ",";
                result += char_to_code[utf8_char];
            } else {
                if (!result.empty()) result += ",";
                result += "oth[" + utf8_char + "]";
            }
            continue;
        }
        
        // Ký tự ASCII thông thường
        if (char_to_code.count(single_char)) {
            if (!result.empty()) result += ",";
            result += char_to_code[single_char];
        } else {
            if (!result.empty()) result += ",";
            result += "oth[" + single_char + "]";
        }
    }
    
    return result;
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "en_US.UTF-8");
    
    if (argc != 3) {
        cerr << "Cách dùng: " << argv[0] << " <input_file> <output_file>" << endl;
        cerr << "Ví dụ: " << argv[0] << " text.txt encoded.txt" << endl;
        return 1;
    }
    
    string input_filename = argv[1];
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
    
    initialize_encode_maps();
    
    string line;
    while (getline(input_file, line)) {
        string encoded = encode_text(line);
        output_file << encoded << endl;
    }
    
    input_file.close();
    output_file.close();
    
    cout << "✅ Đã mã hóa xong! Kết quả được lưu vào: " << output_filename << endl;
    
    return 0;
}