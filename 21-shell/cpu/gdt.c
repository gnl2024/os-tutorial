#include "gdt.h"
#include "../cpu/types.h"

// GDT entry structure
typedef struct {
    u16 limit_low;
    u16 base_low;
    u8 base_middle;
    u8 access;
    u8 granularity;
    u8 base_high;
} __attribute__((packed)) gdt_entry_t;

// GDT pointer structure
typedef struct {
    u16 limit;
    u32 base;
} __attribute__((packed)) gdt_ptr_t;

// GDT entries
gdt_entry_t gdt[5];
gdt_ptr_t gdt_ptr;

// External function to load GDT
extern void gdt_flush(u32);

// Initialize GDT
void gdt_init() {
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
    gdt_ptr.base = (u32)&gdt;

    // Null segment
    gdt_set_gate(0, 0, 0, 0, 0);
    
    // Code segment
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    
    // Data segment
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    
    // User mode code segment
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    
    // User mode data segment
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    // Load the GDT
    gdt_flush((u32)&gdt_ptr);
}

// Set a GDT entry
void gdt_set_gate(int num, u32 base, u32 limit, u8 access, u8 gran) {
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F);
    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access = access;
} 