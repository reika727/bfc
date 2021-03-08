#include "brainfuck.hpp"
#include <stdexcept>

using namespace std::literals::string_literals;

std::string brainfuck::translator::get_unique_label(const std::string &prefix) const
{
    static unsigned int n = 0;
    return prefix + std::to_string(++n);
}

template<class T>
void brainfuck::translator::repeat(const unsigned int rep, const T &operation)
{
    const std::string loop_label = get_unique_label(".bfc_loop_");
    if (rep > 1) {
        alw.write_instruction("mov", rep, "%rcx");
        alw.write_label(loop_label);
        alw.write_instruction("push", "%rcx");
    }
    operation();
    if (rep > 1) {
        alw.write_instruction("pop", "%rcx");
        alw.write_instruction("loop", loop_label);
    }
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
    alw.write_instruction("call", "bf_initialize");
}

void brainfuck::translator::epilogue()
{
    if (!label_stack.empty()) {
        throw std::runtime_error("bracket correspondence error");
    }
    alw.write_instruction("call", "bf_finalize");
    alw.write_instruction("mov", "%rbp", "%rsp");
    alw.write_instruction("pop", "%rbp");
    alw.write_instruction("mov", 0, "%rdi");
    alw.write_instruction("mov", 60, "%rax");
    alw.write("syscall");
}

void brainfuck::translator::increment_ptr(const unsigned int n)
{
    std::string label = get_unique_label(".bfc_if_");
    alw.write_instruction("addq", n, "bf_ptr(%rip)");
    alw.write_instruction("mov", "alloc_ptr(%rip)", "%rax");
    alw.write_instruction("add", "sz(%rip)", "%rax");
    alw.write_instruction("cmp", "bf_ptr(%rip)", "%rax");
    alw.write_instruction("jg", label);
    alw.write_instruction("call", "bf_realloc");
    alw.write_label(label);
}

void brainfuck::translator::decrement_ptr(const unsigned int n)
{
    std::string label = get_unique_label(".bfc_if_");
    alw.write_instruction("subq", n, "bf_ptr(%rip)");
    alw.write_instruction("mov", "alloc_ptr(%rip)", "%rax");
    alw.write_instruction("cmp", "bf_ptr(%rip)", "%rax");
    alw.write_instruction("jle", label);
    alw.write_instruction("call", "bf_realloc");
    alw.write_label(label);
}

void brainfuck::translator::increment_data(const unsigned int n)
{
    alw.write_instruction("mov", "bf_ptr(%rip)", "%rax");
    alw.write_instruction("mov", "(%rax)", "%rdi");
    alw.write_instruction("add", n, "%rdi");
    alw.write_instruction("mov", "%dil", "(%rax)");
}

void brainfuck::translator::decrement_data(const unsigned int n)
{
    alw.write_instruction("mov", "bf_ptr(%rip)", "%rax");
    alw.write_instruction("mov", "(%rax)", "%rdi");
    alw.write_instruction("sub", n, "%rdi");
    alw.write_instruction("mov", "%dil", "(%rax)");
}

void brainfuck::translator::put_current_char(const unsigned int n)
{
    repeat(
        n,
        [this] {
            alw.write_instruction("mov", "bf_ptr(%rip)", "%rax");
            alw.write_instruction("mov", "(%rax)", "%rdi");
            alw.write_instruction("call", "putchar");
        }
    );
}

void brainfuck::translator::get_char(const unsigned int n)
{
    repeat(
        n,
        [this] {
            alw.write_instruction("call", "getchar");
            alw.write_instruction("mov", "bf_ptr(%rip)", "%rdi");
            alw.write_instruction("mov", "%rax", "(%rdi)");
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
    alw.write_instruction("mov", "(%rax)", "%rax");
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

std::string brainfuck::strip(const std::string &code)
{
    std::string stripped;
    for(auto c : code) {
        if ("><+-.,[]"s.find(c) != std::string::npos) {
            stripped.push_back(c);
        }
    }
    return stripped;
}

std::string brainfuck::translate_into_assembly_language(std::string code)
{
    code = strip(code);
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
