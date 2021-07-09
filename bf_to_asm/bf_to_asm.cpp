#include "brainfuck.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std::literals::string_literals;

int main(int argc, char *argv[])
{
    try {
        if (argc != 2) {
            throw std::invalid_argument("usage: bfc SOURCE_FILE");
        }
        std::ifstream input(argv[1]);
        if (!input) {
            throw std::invalid_argument("cannot open file "s + argv[1]);
        }
        const std::string code{std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
        std::cout << brainfuck::translate_into_assembly_language(code) << std::endl;
        return EXIT_SUCCESS;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
