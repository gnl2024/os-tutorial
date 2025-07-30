# Modified Makefile to work with existing GCC for bare metal compilation
CC = gcc
LD = ld
AS = as

# Flags for bare metal compilation
CFLAGS = -ffreestanding -O2 -Wall -Wextra -std=gnu99 -m32 -nostdlib -fno-builtin -fno-stack-protector -I/usr/include
LDFLAGS = -T linker.ld -melf_i386 --oformat binary --gc-sections

SRC_C = kernel/kernel.c kernel/idt.c kernel/pic.c kernel/timer.c kernel/scheduler.c kernel/memory.c
SRC_S = boot/boot.s boot/isr_stubs.s

OBJ = $(SRC_C:.c=.o) $(SRC_S:.s=.o)

all: build/kernel.bin iso

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) --32 $< -o $@

build/kernel.bin: $(OBJ)
	mkdir -p build
	$(LD) $(LDFLAGS) -o $@ $^

iso: build/kernel.bin
	mkdir -p iso/boot/grub
	cp build/kernel.bin iso/boot/kernel.bin
	cp iso/grub/grub.cfg iso/boot/grub/
	grub-mkrescue -o build/os.iso iso

run: iso
	qemu-system-i386 -cdrom build/os.iso

clean:
	rm -rf build/*.bin build/*.o iso/boot/kernel.bin build/os.iso 

verify:
	cbmc kernel/scheduler.c --function scheduler_add || exit 1
	cbmc kernel/scheduler.c --function scheduler_remove || exit 1
	cbmc kernel/scheduler.c --function scheduler_yield || exit 1
	cbmc kernel/timer.c --function timer_tick_handler || exit 1 

check-lines:
	@find . -type f \( -name "*.c" -o -name "*.h" -o -name "*.s" \) \
		-exec awk 'END{if(NR>500){print FILENAME ": exceeds 500 lines"; exit 1}}' {} \; 