CXXFLAGS := -Wall -Wextra -pedantic-errors -MMD -MP
SRC      := bf_to_asm.cpp assembly_language.cpp brainfuck.cpp
OBJ      := $(SRC:%.cpp=%.o)
DEP      := $(SRC:%.cpp=%.d)
TARGET   := bf_to_asm

$(TARGET): $(OBJ)
	$(CXX) $^ -o $@

-include $(DEP)

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJ) $(DEP)
