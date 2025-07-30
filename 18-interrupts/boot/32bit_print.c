#include "32bit_print.h"
#include "types.h"

#define VIDEO_MEMORY 0xb8000
#define WHITE_ON_BLACK 0x0f

// Print string in 32-bit protected mode
void print_string_pm(const char* str) {
    u16* video_memory = (u16*)VIDEO_MEMORY;
    
    while (*str != '\0') {
        // Combine character and attribute
        u16 character = (u16)*str | (WHITE_ON_BLACK << 8);
        *video_memory = character;
        
        str++;
        video_memory++;
    }
} 