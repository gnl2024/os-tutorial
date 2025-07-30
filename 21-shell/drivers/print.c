#include "../cpu/types.h"

// BIOS interrupt for printing
#define BIOS_PRINT_INT 0x10
#define BIOS_PRINT_FUNC 0x0e

// Print a single character using BIOS
void bios_print_char(char c) {
    __asm__ volatile(
        "movb $0x0e, %%ah\n"
        "int $0x10\n"
        : : "a" (c)
    );
}

// Print a string using BIOS
void bios_print_string(const char* str) {
    while (*str != '\0') {
        bios_print_char(*str);
        str++;
    }
}

// Print newline using BIOS
void bios_print_newline() {
    bios_print_char('\n');
    bios_print_char('\r');
}

// Convert a single hex digit to ASCII
char hex_to_ascii(u8 digit) {
    if (digit < 10) {
        return '0' + digit;
    } else {
        return 'A' + (digit - 10);
    }
}

// Print a 16-bit value in hexadecimal using BIOS
void bios_print_hex(u16 value) {
    char hex_str[7] = "0x0000";
    
    // Convert each nibble to hex
    for (int i = 0; i < 4; i++) {
        u8 digit = (value >> (i * 4)) & 0x0F;
        hex_str[5 - i] = hex_to_ascii(digit);
    }
    
    bios_print_string(hex_str);
}

// Print a 32-bit value in hexadecimal using BIOS
void bios_print_hex_32(u32 value) {
    char hex_str[11] = "0x00000000";
    
    // Convert each nibble to hex
    for (int i = 0; i < 8; i++) {
        u8 digit = (value >> (i * 4)) & 0x0F;
        hex_str[9 - i] = hex_to_ascii(digit);
    }
    
    bios_print_string(hex_str);
} 