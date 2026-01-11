#!/bin/bash

# Script quản lý encoder/decoder cho own-lang
# Author: duo8097
# Usage: ./lang.sh [command] [options]

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
NC='\033[0m' # No Color

# Đường dẫn
COMPILER_DIR="compiler"
BIN_DIR="bin"

# Tên file source
ENCODER_SRC="$COMPILER_DIR/text_char.cpp"
DECODER_LINUX_SRC="$COMPILER_DIR/solve_text_linux.cpp"
DECODER_TXT_SRC="$COMPILER_DIR/solve_text_txt.cpp"
DECODER_WIN_SRC="$COMPILER_DIR/solve_text_win.cpp"

# Tên file binary
ENCODER_BIN="$BIN_DIR/text_char"
DECODER_LINUX_BIN="$BIN_DIR/solve_text_linux"
DECODER_TXT_BIN="$BIN_DIR/solve_text_txt"
DECODER_WIN_BIN="$BIN_DIR/solve_text_win"

# Binary mặc định cho hệ thống hiện tại
DECODER_BIN="$DECODER_LINUX_BIN"

# Tạo thư mục bin nếu chưa có
mkdir -p "$BIN_DIR"

# Hàm hiển thị banner
show_banner() {
    echo -e "${PURPLE}"
    echo "╔════════════════════════════════════════════╗"
    echo "║   Own-Lang Encoder/Decoder Manager        ║"
    echo "║              by duo8097                    ║"
    echo "╚════════════════════════════════════════════╝"
    echo -e "${NC}"
}

# Hàm hiển thị help
show_help() {
    show_banner
    echo -e "${GREEN}Cách dùng:${NC}"
    echo -e "  ./lang.sh ${YELLOW}[command]${NC} ${BLUE}[options]${NC}"
    echo ""
    echo -e "${GREEN}Commands:${NC}"
    echo -e "  ${YELLOW}build${NC}              - Compile tất cả (encoder + decoders)"
    echo -e "  ${YELLOW}build-enc${NC}          - Compile encoder (text → code)"
    echo -e "  ${YELLOW}build-dec${NC}          - Compile decoder Linux"
    echo -e "  ${YELLOW}build-dec-txt${NC}      - Compile decoder cho file txt"
    echo -e "  ${YELLOW}build-dec-win${NC}      - Compile decoder Windows"
    echo -e "  ${YELLOW}build-all-dec${NC}      - Compile tất cả decoders"
    echo -e "  ${YELLOW}encode${NC} <in> <out>  - Mã hóa file text → code"
    echo -e "  ${YELLOW}decode${NC} <in> <out>  - Giải mã code → text (Linux)"
    echo -e "  ${YELLOW}decode-txt${NC} <in> <out> - Giải mã với decoder txt"
    echo -e "  ${YELLOW}run-enc${NC} <in> <out> - Compile & encode"
    echo -e "  ${YELLOW}run-dec${NC} <in> <out> - Compile & decode"
    echo -e "  ${YELLOW}test${NC}               - Test encode → decode"
    echo -e "  ${YELLOW}clean${NC}              - Xóa các file binary"
    echo -e "  ${YELLOW}info${NC}               - Hiển thị thông tin binaries"
    echo -e "  ${YELLOW}help${NC}               - Hiển thị hướng dẫn này"
    echo ""
    echo -e "${GREEN}Ví dụ:${NC}"
    echo -e "  ${BLUE}./lang.sh build${NC}                     # Compile tất cả"
    echo -e "  ${BLUE}./lang.sh encode input.txt code.txt${NC}  # Mã hóa"
    echo -e "  ${BLUE}./lang.sh decode code.txt out.txt${NC}   # Giải mã"
    echo -e "  ${BLUE}./lang.sh decode-txt in.txt out.txt${NC} # Dùng decoder txt"
    echo -e "  ${BLUE}./lang.sh test${NC}                      # Test tự động"
    echo -e "  ${BLUE}./lang.sh info${NC}                      # Xem thông tin"
    echo ""
    echo -e "${GREEN}Quick workflow:${NC}"
    echo -e "  ${BLUE}./lang.sh build${NC}                     # Lần đầu"
    echo -e "  ${BLUE}./lang.sh decode in.txt out.txt${NC}     # Sử dụng"
    echo ""
}

# Hàm compile
compile() {
    local src=$1
    local bin=$2
    local name=$3
    
    echo -e "${YELLOW}⚙️  Đang compile ${name}...${NC}"
    
    if [ ! -f "$src" ]; then
        echo -e "${RED}❌ Lỗi: Không tìm thấy file ${src}${NC}"
        return 1
    fi
    
    g++ -std=c++11 "$src" -o "$bin" 2>&1
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✅ Compile ${name} thành công! → ${bin}${NC}"
        return 0
    else
        echo -e "${RED}❌ Compile ${name} thất bại!${NC}"
        return 1
    fi
}

# Build all
build_all() {
    show_banner
    echo -e "${BLUE}Building all components...${NC}"
    echo ""
    
    compile "$ENCODER_SRC" "$ENCODER_BIN" "Encoder"
    local enc_status=$?
    echo ""
    
    compile "$DECODER_LINUX_SRC" "$DECODER_LINUX_BIN" "Decoder (Linux)"
    local dec_linux_status=$?
    echo ""
    
    compile "$DECODER_TXT_SRC" "$DECODER_TXT_BIN" "Decoder (TXT)"
    local dec_txt_status=$?
    echo ""
    
    compile "$DECODER_WIN_SRC" "$DECODER_WIN_BIN" "Decoder (Windows)"
    local dec_win_status=$?
    
    echo ""
    echo -e "${BLUE}════════════════════════════════════════${NC}"
    if [ $enc_status -eq 0 ] && [ $dec_linux_status -eq 0 ] && [ $dec_txt_status -eq 0 ] && [ $dec_win_status -eq 0 ]; then
        echo -e "${GREEN}🎉 Build hoàn tất! Tất cả binary trong folder: ${BIN_DIR}/${NC}"
        ls -lh "$BIN_DIR"
        return 0
    else
        echo -e "${RED}⚠️  Build có lỗi! Vui lòng kiểm tra lại.${NC}"
        return 1
    fi
}

# Build all decoders
build_all_decoders() {
    show_banner
    echo -e "${BLUE}Building all decoders...${NC}"
    echo ""
    
    compile "$DECODER_LINUX_SRC" "$DECODER_LINUX_BIN" "Decoder (Linux)"
    local dec_linux_status=$?
    echo ""
    
    compile "$DECODER_TXT_SRC" "$DECODER_TXT_BIN" "Decoder (TXT)"
    local dec_txt_status=$?
    echo ""
    
    compile "$DECODER_WIN_SRC" "$DECODER_WIN_BIN" "Decoder (Windows)"
    local dec_win_status=$?
    
    echo ""
    echo -e "${BLUE}════════════════════════════════════════${NC}"
    if [ $dec_linux_status -eq 0 ] && [ $dec_txt_status -eq 0 ] && [ $dec_win_status -eq 0 ]; then
        echo -e "${GREEN}🎉 Tất cả decoders đã build xong!${NC}"
        return 0
    else
        echo -e "${RED}⚠️  Build có lỗi!${NC}"
        return 1
    fi
}

# Encode function
encode_file() {
    local input=$1
    local output=$2
    
    if [ ! -f "$ENCODER_BIN" ]; then
        echo -e "${YELLOW}⚙️  Encoder chưa được compile, đang compile...${NC}"
        compile "$ENCODER_SRC" "$ENCODER_BIN" "Encoder"
        if [ $? -ne 0 ]; then
            return 1
        fi
        echo ""
    fi
    
    if [ ! -f "$input" ]; then
        echo -e "${RED}❌ File input không tồn tại: ${input}${NC}"
        return 1
    fi
    
    echo -e "${BLUE}🔐 Đang mã hóa: ${input} → ${output}${NC}"
    "$ENCODER_BIN" "$input" "$output"
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✅ Mã hóa thành công!${NC}"
        return 0
    else
        echo -e "${RED}❌ Mã hóa thất bại!${NC}"
        return 1
    fi
}

# Decode function
decode_file() {
    local input=$1
    local output=$2
    
    if [ ! -f "$DECODER_BIN" ]; then
        echo -e "${YELLOW}⚙️  Decoder chưa được compile, đang compile...${NC}"
        compile "$DECODER_LINUX_SRC" "$DECODER_BIN" "Decoder"
        if [ $? -ne 0 ]; then
            return 1
        fi
        echo ""
    fi
    
    if [ ! -f "$input" ]; then
        echo -e "${RED}❌ File input không tồn tại: ${input}${NC}"
        return 1
    fi
    
    echo -e "${BLUE}🔓 Đang giải mã: ${input} → ${output}${NC}"
    "$DECODER_BIN" "$input" "$output"
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✅ Giải mã thành công!${NC}"
        return 0
    else
        echo -e "${RED}❌ Giải mã thất bại!${NC}"
        return 1
    fi
}

# Test function
test_lang() {
    show_banner
    echo -e "${BLUE}Testing Encoder/Decoder...${NC}"
    echo ""
    
    # Build nếu chưa có
    if [ ! -f "$ENCODER_BIN" ] || [ ! -f "$DECODER_BIN" ]; then
        echo -e "${YELLOW}Compiling required binaries...${NC}"
        build_all
        if [ $? -ne 0 ]; then
            return 1
        fi
        echo ""
    fi
    
    # Tạo file test
    TEST_INPUT="test_input.txt"
    TEST_ENCODED="test_encoded.txt"
    TEST_OUTPUT="test_output.txt"
    
    echo -e "${YELLOW}📝 Tạo file test...${NC}"
    cat > "$TEST_INPUT" << 'EOF'
Hello World!
Liễu Như Yên.
Xin chào Việt Nam!
Code by duo8097
EOF
    
    echo -e "${BLUE}🔐 Encoding...${NC}"
    encode_file "$TEST_INPUT" "$TEST_ENCODED"
    
    echo ""
    echo -e "${BLUE}🔓 Decoding...${NC}"
    decode_file "$TEST_ENCODED" "$TEST_OUTPUT"
    
    echo ""
    echo -e "${PURPLE}════════════════════════════════════════${NC}"
    echo -e "${GREEN}📄 Kết quả Test:${NC}"
    echo -e "${PURPLE}════════════════════════════════════════${NC}"
    
    echo -e "${YELLOW}[1] Original Input:${NC}"
    cat "$TEST_INPUT"
    echo ""
    
    echo -e "${YELLOW}[2] Encoded Code:${NC}"
    head -n 3 "$TEST_ENCODED"
    echo "..."
    echo ""
    
    echo -e "${YELLOW}[3] Decoded Output:${NC}"
    cat "$TEST_OUTPUT"
    echo ""
    
    echo -e "${PURPLE}════════════════════════════════════════${NC}"
    
    # So sánh
    if diff -q "$TEST_INPUT" "$TEST_OUTPUT" > /dev/null 2>&1; then
        echo -e "${GREEN}✅ TEST PASSED! Input ≡ Output${NC}"
        echo -e "${GREEN}🎉 Encoder & Decoder hoạt động hoàn hảo!${NC}"
    else
        echo -e "${RED}❌ TEST FAILED! Input ≠ Output${NC}"
        echo -e "${YELLOW}Differences:${NC}"
        diff "$TEST_INPUT" "$TEST_OUTPUT"
    fi
    
    # Cleanup
    echo ""
    read -p "Xóa các file test? (y/n): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        rm -f "$TEST_INPUT" "$TEST_ENCODED" "$TEST_OUTPUT"
        echo -e "${GREEN}🗑️  Đã xóa file test${NC}"
    fi
}

# Decode with TXT decoder
decode_file_txt() {
    local input=$1
    local output=$2
    
    if [ ! -f "$DECODER_TXT_BIN" ]; then
        echo -e "${YELLOW}⚙️  Decoder TXT chưa được compile, đang compile...${NC}"
        compile "$DECODER_TXT_SRC" "$DECODER_TXT_BIN" "Decoder (TXT)"
        if [ $? -ne 0 ]; then
            return 1
        fi
        echo ""
    fi
    
    if [ ! -f "$input" ]; then
        echo -e "${RED}❌ File input không tồn tại: ${input}${NC}"
        return 1
    fi
    
    echo -e "${BLUE}🔓 Đang giải mã (TXT decoder): ${input} → ${output}${NC}"
    "$DECODER_TXT_BIN" "$input" "$output"
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✅ Giải mã thành công!${NC}"
        return 0
    else
        echo -e "${RED}❌ Giải mã thất bại!${NC}"
        return 1
    fi
}

# Show info about binaries
show_info() {
    show_banner
    echo -e "${GREEN}📊 Thông tin Binaries:${NC}"
    echo ""
    
    if [ -d "$BIN_DIR" ]; then
        echo -e "${BLUE}Thư mục:${NC} $BIN_DIR/"
        echo ""
        
        for bin in "$ENCODER_BIN" "$DECODER_LINUX_BIN" "$DECODER_TXT_BIN" "$DECODER_WIN_BIN"; do
            if [ -f "$bin" ]; then
                size=$(ls -lh "$bin" | awk '{print $5}')
                date=$(ls -l "$bin" | awk '{print $6, $7, $8}')
                echo -e "  ${GREEN}✓${NC} $(basename $bin) ${YELLOW}($size)${NC} - $date"
            else
                echo -e "  ${RED}✗${NC} $(basename $bin) - ${RED}chưa compile${NC}"
            fi
        done
    else
        echo -e "${RED}❌ Thư mục bin/ chưa tồn tại${NC}"
    fi
    
    echo ""
    echo -e "${BLUE}Source files:${NC}"
    echo -e "  Encoder:       $ENCODER_SRC"
    echo -e "  Decoder Linux: $DECODER_LINUX_SRC"
    echo -e "  Decoder TXT:   $DECODER_TXT_SRC"
    echo -e "  Decoder Win:   $DECODER_WIN_SRC"
    echo ""
}

# Clean function
clean() {
    echo -e "${YELLOW}🗑️  Đang xóa file binary trong ${BIN_DIR}/...${NC}"
    rm -f "$ENCODER_BIN" "$DECODER_LINUX_BIN" "$DECODER_TXT_BIN" "$DECODER_WIN_BIN"
    echo -e "${GREEN}✅ Đã xóa xong!${NC}"
}

# Main
case "$1" in
    build)
        build_all
        ;;
    build-enc)
        compile "$ENCODER_SRC" "$ENCODER_BIN" "Encoder"
        ;;
    build-dec)
        compile "$DECODER_LINUX_SRC" "$DECODER_LINUX_BIN" "Decoder (Linux)"
        ;;
    build-dec-txt)
        compile "$DECODER_TXT_SRC" "$DECODER_TXT_BIN" "Decoder (TXT)"
        ;;
    build-dec-win)
        compile "$DECODER_WIN_SRC" "$DECODER_WIN_BIN" "Decoder (Windows)"
        ;;
    build-all-dec)
        build_all_decoders
        ;;
    encode)
        if [ -z "$2" ] || [ -z "$3" ]; then
            echo -e "${RED}❌ Thiếu tham số!${NC}"
            echo -e "Cách dùng: ./lang.sh encode <input_file> <output_file>"
            exit 1
        fi
        encode_file "$2" "$3"
        ;;
    decode)
        if [ -z "$2" ] || [ -z "$3" ]; then
            echo -e "${RED}❌ Thiếu tham số!${NC}"
            echo -e "Cách dùng: ./lang.sh decode <input_file> <output_file>"
            exit 1
        fi
        decode_file "$2" "$3"
        ;;
    decode-txt)
        if [ -z "$2" ] || [ -z "$3" ]; then
            echo -e "${RED}❌ Thiếu tham số!${NC}"
            echo -e "Cách dùng: ./lang.sh decode-txt <input_file> <output_file>"
            exit 1
        fi
        decode_file_txt "$2" "$3"
        ;;
    run-enc)
        if [ -z "$2" ] || [ -z "$3" ]; then
            echo -e "${RED}❌ Thiếu tham số!${NC}"
            echo -e "Cách dùng: ./lang.sh run-enc <input_file> <output_file>"
            exit 1
        fi
        compile "$ENCODER_SRC" "$ENCODER_BIN" "Encoder"
        if [ $? -eq 0 ]; then
            echo ""
            encode_file "$2" "$3"
        fi
        ;;
    run-dec)
        if [ -z "$2" ] || [ -z "$3" ]; then
            echo -e "${RED}❌ Thiếu tham số!${NC}"
            echo -e "Cách dùng: ./lang.sh run-dec <input_file> <output_file>"
            exit 1
        fi
        compile "$DECODER_LINUX_SRC" "$DECODER_BIN" "Decoder"
        if [ $? -eq 0 ]; then
            echo ""
            decode_file "$2" "$3"
        fi
        ;;
    test)
        test_lang
        ;;
    info)
        show_info
        ;;
    clean)
        clean
        ;;
    help|--help|-h|"")
        show_help
        ;;
    *)
        echo -e "${RED}❌ Lệnh không hợp lệ: $1${NC}"
        echo -e "Dùng './lang.sh help' để xem hướng dẫn"
        exit 1
        ;;
esac