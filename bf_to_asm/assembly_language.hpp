#include <sstream>
#include <string>

namespace assembly_language {
    /* AT&T記法でアセンブリを出力するための便利関数を集めたクラス */
    /* 必要な機能しか実装してないのでガバガバなところはある */
    class writer final {
    private:
        std::stringstream code;
    public:
        std::string get_code() const;
        void write(const std::string &str);
        void write_instruction(const std::string &opcode);
        void write_instruction(const std::string &opcode, const std::string &arg);
        void write_instruction(const std::string &opcode, const std::string &arg1, const std::string &arg2);
        void write_instruction(const std::string &opcode, int arg1, const std::string &arg2);
        void write_label(const std::string &label);
    };
}
