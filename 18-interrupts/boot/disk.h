#ifndef DISK_H
#define DISK_H

#include "types.h"

// Disk operations
int disk_load(u8 sectors, u8 drive);

// Print functions for error messages
void print_string(const char* str);
void print_newline(void);
void print_hex(u8 value);
void print_char(char c);

#endif // DISK_H 