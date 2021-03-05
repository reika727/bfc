#include "assembly_language.hpp"
#include <iostream>

std::string assembly_language::writer::get_code() const
{
    return code.str();
}

void assembly_language::writer::write(const std::string &str)
{
    code << str << std::endl;
}

void assembly_language::writer::write_instruction(const std::string &opcode, const std::string &arg)
{
    write(opcode + ' ' + arg);
}

void assembly_language::writer::write_instruction(const std::string &opcode, const std::string &arg1, const std::string &arg2)
{
    write(opcode + ' ' + arg1 + ", " + arg2);
}

void assembly_language::writer::write_instruction(const std::string &opcode, int arg1, const std::string &arg2)
{
    write_instruction(opcode, '$' + std::to_string(arg1), arg2);
}

void assembly_language::writer::write_label(const std::string &label)
{
    write(label + ':');
}
