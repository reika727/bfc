#include "brainfuck.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std::literals::string_literals;

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cerr << "usage: bfc SOURCE_FILE" << std::endl;
        return EXIT_FAILURE;
    }
    std::ifstream input(argv[1]);
    if (!input) {
        std::cerr << "cannot open file "s + argv[1] << std::endl;
        return EXIT_FAILURE;
    }
    const std::string code{std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
    std::cout << brainfuck::translate_into_assembly_language(code) << std::endl;
    return EXIT_SUCCESS;
}
