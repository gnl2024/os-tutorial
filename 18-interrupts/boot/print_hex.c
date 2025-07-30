#include "print_hex.h"
#include "types.h"

// Print hex value
void print_hex(u16 value) {
    char hex_str[7] = "0x0000";
    char hex_chars[] = "0123456789ABCDEF";
    
    // Convert each nibble to hex character
    for (int i = 0; i < 4; i++) {
        u8 digit = (value >> (i * 4)) & 0x0F;
        hex_str[5 - i] = hex_chars[digit];
    }
    
    // Print the hex string
    print(hex_str);
} 