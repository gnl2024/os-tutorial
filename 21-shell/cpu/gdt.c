#include "gdt.h"
#include "../cpu/types.h"
#include "../drivers/screen.h"
#include "../libc/string.h"

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

// GDT entries (extended for process segments)
#define MAX_GDT_ENTRIES 32  // Support up to 16 processes (2 segments each) + kernel segments
gdt_entry_t gdt[MAX_GDT_ENTRIES];
gdt_ptr_t gdt_ptr;

// External function to load GDT
extern void gdt_flush(u32);

// Initialize GDT
void gdt_init() {
    gdt_ptr.limit = (sizeof(gdt_entry_t) * MAX_GDT_ENTRIES) - 1;
    gdt_ptr.base = (u32)&gdt;

    // Null segment
    gdt_set_gate(0, 0, 0, 0, 0);
    
    // Kernel code segment
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    
    // Kernel data segment
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

// Setup process-specific GDT segments
void setup_process_segments(process_t *proc) {
    int segment_index = 5 + (proc->pid * 2);  // Start after kernel segments (0-4)
    
    if (segment_index >= MAX_GDT_ENTRIES - 1) {
        kprint("Error: Too many processes for GDT\n");
        return;
    }
    
    // Code segment for process
    gdt_set_gate(segment_index, (u32)proc->heap, 0x1000, 0x9A, 0xCF);
    
    // Data segment for process
    gdt_set_gate(segment_index + 1, (u32)proc->heap, 0x1000, 0x92, 0xCF);
    
    proc->code_segment = segment_index * 8;
    proc->data_segment = (segment_index + 1) * 8;
    
    kprint("Process segments created: Code=");
    char seg_str[10];
    int_to_ascii(proc->code_segment, seg_str);
    kprint(seg_str);
    kprint(" Data=");
    int_to_ascii(proc->data_segment, seg_str);
    kprint(seg_str);
    kprint("\n");
}

// Assign process segments (for existing processes)
void assign_process_segments(process_t *proc) {
    // This function can be used to reassign segments if needed
    setup_process_segments(proc);
} 