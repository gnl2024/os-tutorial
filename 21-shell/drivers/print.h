#ifndef PRINT_H
#define PRINT_H

#include "../cpu/types.h"

// BIOS-based print functions (for boot time)
void bios_print_char(char c);
void bios_print_string(const char* str);
void bios_print_newline(void);
void bios_print_hex(u16 value);
void bios_print_hex_32(u32 value);

#endif // PRINT_H 