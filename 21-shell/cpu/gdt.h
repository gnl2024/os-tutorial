#ifndef GDT_H
#define GDT_H

#include "../cpu/types.h"

// GDT functions
void gdt_init(void);
void gdt_set_gate(int num, u32 base, u32 limit, u8 access, u8 gran);

// GDT flush function (assembly)
extern void gdt_flush(u32);

#endif // GDT_H 