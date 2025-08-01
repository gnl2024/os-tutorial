#include "../cpu/isr.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "kernel.h"
#include "process.h"
#include "memory.h"
#include "mpu.h"
#include "../cpu/segment_protection.h"
#include "privilege.h"
#include "syscalls.h"
#include "../libc/string.h"
#include "../libc/mem.h"
#include "../drivers/print.h"
#include "ipc.h"

// Test process function
void test_process_function(void) {
    kprint("Test process running! PID: ");
    char pid_str[10];
    int_to_ascii(get_current_pid(), pid_str);
    kprint(pid_str);
    kprint("\n");
}

void main(void) {
    // Simple test to see if kernel loads
    kprint("Hello from kernel!\n");
    kprint("Kernel loaded successfully!\n");
    
    // Basic initialization only
    isr_install();
    irq_install();
    init_keyboard(); // <-- Enable keyboard input
    
    // Initialize IPC system
    init_ipc_system();
    
    kprint("Interrupts initialized!\n");
    kprint("IPC system initialized!\n");
    kprint("System ready!\n> ");
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
