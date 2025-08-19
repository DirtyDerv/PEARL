#include <iostream>
#include <string>
#include <vector>

// ASCII art for 7-segment digits in a 3x3 grid
const char* big_digits[10][3] = {
    {" _ ", "| |", "|_|"}, // 0
    {"   ", "  |", "  |"}, // 1
    {" _ ", " _|", "|_ "}, // 2
    {" _ ", " _|", " _|"}, // 3
    {"   ", "|_|", "  |"}, // 4
    {" _ ", "|_ ", " _|"}, // 5
    {" _ ", "|_ ", "|_|"}, // 6
    {" _ ", "  |", "  |"}, // 7
    {" _ ", "|_|", "|_|"}, // 8
    {" _ ", "|_|", " _|"}  // 9
};

void print_big_number(const std::string& number) {
    for (int row = 0; row < 3; ++row) {
        for (char c : number) {
            if (c >= '0' && c <= '9') {
                std::cout << big_digits[c - '0'][row];
            } else if (c == '.') {
                if (row == 2) std::cout << " . ";
                else std::cout << "   ";
            } else if (c == '-') {
                if (row == 1) std::cout << " _ ";
                else std::cout << "   ";
            } else {
                std::cout << "   ";
            }
        }
        std::cout << std::endl;
    }
}

int main() {
    std::string test = "-12.34";
    print_big_number(test);
    return 0;
}
