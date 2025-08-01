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