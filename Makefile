SHELL  := /bin/bash
CFLAGS := -Wall -Wextra

%.out: libbf.o %.o
	$(LD) -dynamic-linker=/lib64/ld-linux-x86-64.so.2 $^ -lc -o $@

%.o: %.bf
	$(MAKE) -C bf_to_asm bf_to_asm
	$(AS) <(bf_to_asm/bf_to_asm $<) -o $@

.PHONY: clean
clean:
	$(MAKE) -C bf_to_asm $@
