; Multiboot header for GRUB2
section .multiboot_header
align 4

MULTIBOOT_MAGIC equ 0x1BADB002
MULTIBOOT_FLAGS equ 0x00000003  ; ALIGN + MEMINFO
MULTIBOOT_CHECKSUM equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

; Multiboot header must be in the first 8KB of the kernel file
dd MULTIBOOT_MAGIC
dd MULTIBOOT_FLAGS
dd MULTIBOOT_CHECKSUM

; Optional: Additional multiboot fields
; dd 0  ; header_addr
; dd 0  ; load_addr
; dd 0  ; load_end_addr
; dd 0  ; bss_end_addr
; dd 0  ; entry_addr 