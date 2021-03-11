SHELL  := /bin/bash
CFLAGS := -Wall -Wextra -pedantic-errors
BTA    := bf_to_asm/bf_to_asm

%.out: libbf.o %.o
	$(LD) $^ -o $@

%.o: %.bf
	$(MAKE) -C $(dir $(BTA)) $(notdir $(BTA))
	$(AS) <($(BTA) $<) -o $@

.PHONY: clean
clean:
	$(MAKE) -C $(dir $(BTA)) $@
