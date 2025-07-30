#include "disk.h"
#include "types.h"

// Disk error messages
static const char* DISK_ERROR = "Disk read error";
static const char* SECTORS_ERROR = "Incorrect number of sectors read";

// BIOS disk read function
int disk_load(u8 sectors, u8 drive) {
    u8 sectors_read;
    u8 error_code;
    
    // BIOS interrupt 0x13 function 0x02 - read sectors
    __asm__ volatile(
        "int $0x13\n"
        "jc 1f\n"
        "movb %%al, %0\n"
        "jmp 2f\n"
        "1:\n"
        "movb %%ah, %1\n"
        "2:\n"
        : "=r" (sectors_read), "=r" (error_code)
        : "a" (0x0200 | sectors), "b" (0x1000), "c" (0x0200), "d" (drive)
        : "memory"
    );
    
    // Check for carry flag (error)
    if (error_code != 0) {
        // Disk error occurred
        print_string(DISK_ERROR);
        print_newline();
        print_hex(error_code);
        return -1;
    }
    
    // Check if correct number of sectors were read
    if (sectors_read != sectors) {
        print_string(SECTORS_ERROR);
        return -1;
    }
    
    return 0;
}

// Print string function (for error messages)
void print_string(const char* str) {
    while (*str != '\0') {
        print_char(*str);
        str++;
    }
}

// Print newline
void print_newline() {
    print_char('\n');
    print_char('\r');
}

// Print hex value
void print_hex(u8 value) {
    char hex_chars[] = "0123456789ABCDEF";
    print_char('0');
    print_char('x');
    print_char(hex_chars[(value >> 4) & 0x0F]);
    print_char(hex_chars[value & 0x0F]);
}

// Print character (BIOS call)
void print_char(char c) {
    __asm__ volatile(
        "movb $0x0e, %%ah\n"
        "int $0x10\n"
        : : "a" (c)
    );
} 