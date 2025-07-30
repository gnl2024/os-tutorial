#include "print.h"
#include "types.h"

// Print string function
void print(const char* str) {
    while (*str != '\0') {
        print_char(*str);
        str++;
    }
}

// Print newline
void print_nl() {
    print_char('\n');
    print_char('\r');
}

// Print character (BIOS call)
void print_char(char c) {
    __asm__ volatile(
        "movb $0x0e, %%ah\n"
        "int $0x10\n"
        : : "a" (c)
    );
} 