#ifndef GDT_H
#define GDT_H

#include "../cpu/types.h"
#include "../kernel/process.h"

// GDT segment selectors
#define GDT_KERNEL_CODE 0x08
#define GDT_KERNEL_DATA 0x10
#define GDT_USER_CODE   0x18
#define GDT_USER_DATA   0x20

// GDT functions
void gdt_init(void);
void gdt_set_gate(int num, u32 base, u32 limit, u8 access, u8 gran);

// Process segment management
void setup_process_segments(process_t *proc);
void assign_process_segments(process_t *proc);

// GDT flush function (assembly)
extern void gdt_flush(u32);

#endif // GDT_H 