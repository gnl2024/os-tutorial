#include "../cpu/isr.h"
#include "../drivers/screen.h"
#include "kernel.h"
#include "process.h"
#include "memory.h"
#include "mpu.h"
#include "../cpu/segment_protection.h"
#include "privilege.h"
#include "syscalls.h"
#include "../libc/string.h"
#include "../libc/mem.h"

// Test process function
void test_process_function(void) {
    kprint("Test process running! PID: ");
    char pid_str[10];
    int_to_ascii(get_current_pid(), pid_str);
    kprint(pid_str);
    kprint("\n");
}

void main() {
    // Initialize interrupts
    isr_install();
    irq_install();

        // Initialize memory regions
    init_memory_regions();

    // Initialize MPU
    init_mpu();

    // Initialize segment protection
    init_segment_protection();

    // Initialize privilege manager
    init_privilege_manager();

    // Initialize system call interface
    init_syscall_interface();

    // Initialize process manager
    init_process_manager();

    kprint("Hello from kernel!\n");
    kprint("System initialized with process management and memory protection!\n");
    
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
    
    // Test process creation
    kprint("Creating test process...\n");
    void *test_stack = (void*)0x30000;  // Test process stack
    process_t *test_proc = create_process(test_process_function, test_stack, PRIVILEGE_USER);
    
    if (test_proc) {
        kprint("Test process created successfully!\n");
        kprint("Current process PID: ");
        char pid_str[10];
        int_to_ascii(get_current_pid(), pid_str);
        kprint(pid_str);
        kprint("\n");
    } else {
        kprint("Failed to create test process!\n");
    }
    
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
