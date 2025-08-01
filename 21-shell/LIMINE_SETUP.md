# Limine Bootloader Setup - WISP-BB

## 🎯 **Overview**

Limine is a modern, lightweight bootloader that's much simpler to configure than GRUB2. It supports multiboot2 and provides a clean, straightforward boot process.

## ✅ **Benefits of Limine**

### **Simplicity**
- **Easy Configuration**: Simple configuration file
- **Modern Design**: Clean, well-documented API
- **Fast Boot**: Lightweight and fast
- **Good Documentation**: Clear documentation and examples

### **Features**
- **Multiboot2 Support**: Modern multiboot standard
- **Module Loading**: Load additional modules
- **Memory Map**: Automatic memory detection
- **Terminal Interface**: Built-in terminal for debugging

## 📋 **Setup Steps**

### **Step 1: Download Limine**

```bash
# Download Limine
wget https://github.com/limine-bootloader/limine/releases/download/v5.20231210.0/limine-v5.20231210.0.tar.gz
tar -xf limine-v5.20231210.0.tar.gz
cd limine-v5.20231210.0

# Build Limine
make
```

### **Step 2: Create Limine Configuration**

#### **File**: `limine.cfg`
```
TIMEOUT=0
DEFAULT_ENTRY=wisp-bb

:wisp-bb
    PROTOCOL=multiboot2
    KERNEL_PATH=boot:///kernel.bin
    MODULE_PATH=boot:///limine.cfg
    KERNEL_CMDLINE=
```

### **Step 3: Update Kernel for Multiboot2**

#### **File**: `boot/multiboot2_header.asm`
```assembly
; Multiboot2 header for Limine
section .multiboot_header
align 8

MULTIBOOT2_MAGIC equ 0xE85250D6
MULTIBOOT2_ARCH equ 0x0  ; i386
MULTIBOOT2_HEADER_LEN equ header_end - header_start
MULTIBOOT2_CHECKSUM equ 0x100000000 - (MULTIBOOT2_MAGIC + MULTIBOOT2_ARCH + MULTIBOOT2_HEADER_LEN)

header_start:
    dd MULTIBOOT2_MAGIC
    dd MULTIBOOT2_ARCH
    dd MULTIBOOT2_HEADER_LEN
    dd MULTIBOOT2_CHECKSUM

    ; Information request tag
    dw 1, 0  ; type, flags
    dd 8      ; size
    dd 6      ; framebuffer
    dd 8      ; framebuffer
    dd 0      ; framebuffer
    dd 0      ; framebuffer
    dd 0      ; framebuffer

    ; Entry address tag
    dw 3, 0   ; type, flags
    dd 12     ; size
    dd 0x100000 ; entry address

    ; End tag
    dw 0, 0   ; type, flags
    dd 8      ; size

header_end:
```

### **Step 4: Update Kernel Entry**

#### **File**: `boot/kernel_entry_limine.asm`
```assembly
[bits 32]
[global _start]

extern kernel_main
extern multiboot_info

_start:
    ; Save multiboot2 info
    mov [multiboot_info], esi
    
    ; Set up stack
    mov esp, stack_top
    
    ; Call kernel main
    call kernel_main
    
    ; Infinite loop if kernel returns
    cli
    hlt
    jmp $

section .bss
align 4
stack_bottom:
    resb 16384  ; 16 KB stack
stack_top:

section .data
multiboot_info: dd 0
```

### **Step 5: Create Limine Makefile**

#### **File**: `Makefile_limine`
```makefile
# Limine Multiboot2 Makefile for WISP-BB
C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c libc/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h cpu/*.h libc/*.h)

# Multiboot2 kernel object files
OBJ = ${C_SOURCES:.c=.o boot/kernel_entry_limine.o boot/multiboot2_header.o cpu/isr_stubs_simple.o cpu/gdt_flush.o cpu/process_switch.o}

# Use the proper bare-metal cross-compiler
CC = i686-elf-gcc
GDB = gdb

# -g: Use debugging symbols in gcc
CFLAGS = -g -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs \
		 -Wall -Wextra -Werror -fno-pie -ffreestanding

# Limine kernel (ELF format)
kernel.bin: boot/multiboot2_header.o boot/kernel_entry_limine.o ${OBJ}
	i686-elf-ld -o $@ -Ttext 0x100000 $^ --oformat elf32-i386

# Create Limine ISO
os-image.iso: kernel.bin
	mkdir -p iso/boot
	cp kernel.bin iso/boot/
	cp limine.cfg iso/boot/
	cp limine/limine.sys iso/boot/
	cp limine/limine-cd.bin iso/boot/
	xorriso -as mkisofs -b boot/limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--protective-msdos-label iso/ -o os-image.iso

# Run with Limine
run-limine: os-image.iso
	qemu-system-i386 -cdrom os-image.iso -m 128

# Run with Limine (optimized)
run-limine-optimized: os-image.iso
	qemu-system-i386 -enable-kvm -cpu host -smp 1 -m 128 -cdrom os-image.iso

# Debug with Limine
debug-limine: os-image.iso kernel.bin
	qemu-system-i386 -s -cdrom os-image.iso -d guest_errors &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.bin"

# Generic rules for wildcards
# To make an object, always compile from its .c
%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -rf *.bin *.dis *.o os-image.iso *.elf
	rm -rf kernel/*.o boot/*.bin drivers/*.o boot/*.o cpu/*.o libc/*.o
	rm -rf iso/
```

## 🚀 **Quick Start**

### **1. Download and Build Limine**
```bash
# Download Limine
wget https://github.com/limine-bootloader/limine/releases/download/v5.20231210.0/limine-v5.20231210.0.tar.gz
tar -xf limine-v5.20231210.0.tar.gz
cd limine-v5.20231210.0
make
cd ..
```

### **2. Build WISP-BB with Limine**
```bash
# Build kernel
make -f Makefile_limine clean
make -f Makefile_limine kernel.bin

# Create ISO
make -f Makefile_limine os-image.iso

# Run
make -f Makefile_limine run-limine-optimized
```

## 📊 **Comparison: Limine vs GRUB2**

| Feature | Limine | GRUB2 |
|---------|--------|-------|
| **Setup Complexity** | Simple | Complex |
| **Configuration** | Easy | Complex |
| **Documentation** | Good | Extensive |
| **Boot Speed** | Fast | Slower |
| **Features** | Basic | Advanced |
| **Development** | Easy | Hard |

## 🎯 **Advantages of Limine**

### **For Development**
- **Simple Configuration**: Easy to understand and modify
- **Fast Boot**: Quick boot process
- **Good Debugging**: Built-in terminal interface
- **Modern API**: Clean, well-designed API

### **For WISP-BB**
- **Eliminates Boot Assembly**: No custom bootloader needed
- **Standard Compliance**: Multiboot2 standard
- **Easy Integration**: Simple to integrate with existing kernel
- **Better Development**: Focus on kernel, not bootloader

## 📋 **Next Steps**

1. **Download Limine**: Get the latest release
2. **Create Configuration**: Set up `limine.cfg`
3. **Update Kernel**: Add multiboot2 support
4. **Test Boot**: Verify everything works
5. **Optimize**: Fine-tune for performance

Limine will provide a much simpler and more reliable boot process than GRUB2! 