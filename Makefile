SHELL  := /bin/bash
CFLAGS := -Wall -Wextra
BTA    := bf_to_asm/bf_to_asm

%.out: libbf.o %.o
	$(LD) -dynamic-linker=/lib64/ld-linux-x86-64.so.2 $^ -lc -o $@

%.o: %.bf
	$(MAKE) -C $(dir $(BTA)) $(notdir $(BTA))
	$(AS) <($(BTA) $<) -o $@

.PHONY: clean
clean:
	$(MAKE) -C $(dir $(BTA)) $@
