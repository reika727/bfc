#include "brainfuck.hpp"
#include <regex>
#include <stdexcept>
#include <syscall.h>
#include <unistd.h>

using namespace std::literals::string_literals;

std::string brainfuck::translator::get_unique_label(const std::string &prefix) const
{
    static unsigned int n = 0;
    return prefix + std::to_string(++n);
}

std::string brainfuck::translator::get_code() const
{
    return alw.get_code();
}

void brainfuck::translator::prologue()
{
    alw.write(".globl _start");
    alw.write_label("_start");
    alw.write_instruction("push", "%rbp");
    alw.write_instruction("mov", "%rsp", "%rbp");
    alw.write_instruction("call", "bfc_initialize");
}

void brainfuck::translator::epilogue()
{
    if (!label_stack.empty()) {
        throw std::runtime_error("bracket correspondence error");
    }
    alw.write_instruction("call", "bfc_finalize");
    alw.write_instruction("mov", "%rbp", "%rsp");
    alw.write_instruction("pop", "%rbp");
    alw.write_instruction("mov", SYS_exit, "%rax");
    alw.write_instruction("mov", EXIT_SUCCESS, "%rdi");
    alw.write_instruction("syscall");
}

void brainfuck::translator::increment_ptr(const unsigned int n)
{
    std::string label = get_unique_label(".bfc_if_");
    alw.write_instruction("addq", n, "bf_ptr(%rip)");
    alw.write_instruction("mov", "alloc_ptr(%rip)", "%rax");
    alw.write_instruction("add", "sz(%rip)", "%rax");
    alw.write_instruction("cmp", "bf_ptr(%rip)", "%rax");
    alw.write_instruction("jg", label);
    alw.write_instruction("call", "bfc_realloc");
    alw.write_label(label);
}

void brainfuck::translator::decrement_ptr(const unsigned int n)
{
    std::string label = get_unique_label(".bfc_if_");
    alw.write_instruction("subq", n, "bf_ptr(%rip)");
    alw.write_instruction("mov", "alloc_ptr(%rip)", "%rax");
    alw.write_instruction("cmp", "bf_ptr(%rip)", "%rax");
    alw.write_instruction("jle", label);
    alw.write_instruction("call", "bfc_realloc");
    alw.write_label(label);
}

void brainfuck::translator::increment_data(const unsigned int n)
{
    alw.write_instruction("mov", "bf_ptr(%rip)", "%rax");
    alw.write_instruction("addb", n, "(%rax)");
}

void brainfuck::translator::decrement_data(const unsigned int n)
{
    alw.write_instruction("mov", "bf_ptr(%rip)", "%rax");
    alw.write_instruction("subb", n, "(%rax)");
}

void brainfuck::translator::put_current_char(const unsigned int n)
{
    repeat(
        n,
        [this] {
            alw.write_instruction("mov", SYS_write, "%rax");
            alw.write_instruction("mov", STDOUT_FILENO, "%rdi");
            alw.write_instruction("mov", "bf_ptr(%rip)", "%rsi");
            alw.write_instruction("mov", 1, "%rdx");
            alw.write_instruction("syscall");
        }
    );
}

void brainfuck::translator::get_char(const unsigned int n)
{
    repeat(
        n,
        [this] {
            alw.write_instruction("mov", SYS_read, "%rax");
            alw.write_instruction("mov", STDIN_FILENO, "%rdi");
            alw.write_instruction("mov", "bf_ptr(%rip)", "%rsi");
            alw.write_instruction("mov", 1, "%rdx");
            alw.write_instruction("syscall");
        }
    );
}

void brainfuck::translator::begin_loop()
{
    const std::string loop_begin = get_unique_label(".bfc_loop_begin_");
    label_stack.push(loop_begin);
    const std::string loop_end = get_unique_label(".bfc_loop_end_");
    label_stack.push(loop_end);
    alw.write_label(loop_begin);
    alw.write_instruction("mov", "bf_ptr(%rip)", "%rax");
    alw.write_instruction("movzbq", "(%rax)", "%rax");
    alw.write_instruction("cmp", 0, "%al");
    alw.write_instruction("je", loop_end);
}

void brainfuck::translator::end_loop()
{
    if (label_stack.size() < 2) {
        throw std::runtime_error("bracket correspondence error");
    }
    const std::string loop_end = label_stack.top();
    label_stack.pop();
    const std::string loop_begin = label_stack.top();
    label_stack.pop();
    alw.write_instruction("jmp", loop_begin);
    alw.write_label(loop_end);
}

std::string brainfuck::translate_into_assembly_language(std::string code)
{
    code = std::regex_replace(code, std::regex(R"([^><+-.,\[\]])"), "");
    translator t;
    t.prologue();
    for (std::string::size_type i = 0; i < code.length(); ++i) {
        if (code[i] == '[') {
            t.begin_loop();
        } else if (code[i] == ']') {
            t.end_loop();
        } else {
            const auto len = std::min(code.find_first_not_of(code[i], i), code.length()) - i;
              code[i] == '>' ? t.increment_ptr(len)
            : code[i] == '<' ? t.decrement_ptr(len)
            : code[i] == '+' ? t.increment_data(len)
            : code[i] == '-' ? t.decrement_data(len)
            : code[i] == '.' ? t.put_current_char(len)
            :                  t.get_char(len);
            i += len - 1;
        }
    }
    t.epilogue();
    return t.get_code();
}
