#include "gdt.h"
#include "../drivers/screen.h"

// GDT pointer structure (same as in gdt.c)
typedef struct {
    u16 limit;
    u32 base;
} __attribute__((packed)) gdt_ptr_t;

// External GDT pointer (defined in gdt.c)
extern gdt_ptr_t gdt_ptr;

// Flush the GDT using inline assembly
void gdt_flush(u32 gdt_ptr_addr) {
    (void)gdt_ptr_addr; // Suppress unused parameter warning
    
    // Load the new GDT pointer using inline assembly
    __asm__ volatile("lgdt %0" : : "m"(gdt_ptr));
    
    // Set all data segment selectors to 0x10 (kernel data segment)
    __asm__ volatile(
        "mov $0x10, %%ax\n"
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        "mov %%ax, %%ss\n"
        :
        :
        : "ax"
    );
    
    // Far jump to flush the instruction cache
    // This is necessary because we changed the GDT
    __asm__ volatile(
        "jmp $0x08, $1f\n"  // 0x08 is the offset to our code segment
        "1:\n"
        :
        :
        : "memory"
    );
    
    kprint("GDT flushed successfully\n");
} 