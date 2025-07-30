#include "../cpu/isr.h"
#include "../drivers/screen.h"
#include "kernel.h"
#include "../libc/string.h"
#include "../libc/mem.h"

void main() {
    // Initialize interrupts
    isr_install();
    irq_install();

    kprint("Hello from kernel!\n");
    kprint("System initialized with memory allocator!\n");
    
    // Test memory allocator
    u32 phys_addr;
    u32 mem1 = kmalloc(1000, 1, &phys_addr);
    kprint("Allocated 1000 bytes at: ");
    char addr_str[20];
    int_to_ascii(mem1, addr_str);
    kprint(addr_str);
    kprint("\n");
    
    u32 mem2 = kmalloc(1000, 1, &phys_addr);
    kprint("Allocated another 1000 bytes at: ");
    int_to_ascii(mem2, addr_str);
    kprint(addr_str);
    kprint("\n");
    
    kprint("Type something, it will go through the kernel\n"
        "Type END to halt the CPU\n> ");
}

void user_input(char *input) {
    if (strcmp(input, "END") == 0) {
        kprint("Stopping the CPU. Bye!\n");
        asm volatile("hlt");
    }
    kprint("You said: ");
    kprint(input);
    kprint("\n> ");
}
