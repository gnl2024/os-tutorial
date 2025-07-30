#include "switch_pm.h"
#include "types.h"

// External GDT descriptor
extern void gdt_descriptor(void);
extern void init_pm(void);

// Switch to protected mode
void switch_to_pm() {
    // 1. Disable interrupts
    __asm__ volatile("cli");
    
    // 2. Load GDT descriptor
    __asm__ volatile("lgdt [gdt_descriptor]");
    
    // 3. Set 32-bit mode bit in cr0
    __asm__ volatile(
        "mov %%cr0, %%eax\n"
        "or $0x1, %%eax\n"
        "mov %%eax, %%cr0\n"
        : : : "eax"
    );
    
    // 4. Far jump to 32-bit code
    __asm__ volatile("jmp $0x08, $init_pm");
}

// Initialize protected mode (called from assembly)
void init_pm() {
    // 5. Update segment registers
    __asm__ volatile(
        "mov $0x10, %%ax\n"
        "mov %%ax, %%ds\n"
        "mov %%ax, %%ss\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        : : : "ax"
    );
    
    // 6. Update stack
    __asm__ volatile(
        "mov $0x90000, %%ebp\n"
        "mov %%ebp, %%esp\n"
        : : : "ebp", "esp"
    );
    
    // 7. Call kernel main
    // This will be handled by the kernel entry point
} 