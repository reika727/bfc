#include "assembly_language.hpp"
#include <stack>
#include <string>

namespace brainfuck {
    /* Brainfuckをアセンブリ言語に翻訳するクラス */
    class translator final {
    private:
        assembly_language::writer alw;
        std::stack<std::string> label_stack;
        std::string get_unique_label(const std::string &prefix) const;
        template<class T>
        void repeat(const unsigned int rep, const T &operation)
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

    public:
        std::string get_code() const;
        void prologue();
        void epilogue();
        /* > */
        void increment_ptr(const unsigned int n);
        /* < */
        void decrement_ptr(const unsigned int n);
        /* + */
        void increment_data(const unsigned int n);
        /* - */
        void decrement_data(const unsigned int n);
        /* . */
        void put_current_char(const unsigned int n);
        /* , */
        void get_char(const unsigned int n);
        /* [ */
        void begin_loop();
        /* ] */
        void end_loop();
    };
    /* translatorを使って得られた翻訳結果を返す */
    std::string translate_into_assembly_language(std::string code);
}
