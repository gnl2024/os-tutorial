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
#include "../cpu/timer.h"

#define NULL ((void*)0)
#define UNUSED(x) (void)(x)

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
    
    // Initialize IPC system
    init_ipc_system();
    
    // Initialize process manager
    init_process_manager();
    
    // Create some test processes to make commands show meaningful data
    create_process(test_process_function, (void*)0x30000, PRIVILEGE_USER);
    create_process(test_process_function, (void*)0x40000, PRIVILEGE_USER);
    
    // Allocate some memory to test memory statistics
    kmalloc(1024, 1, NULL);
    kmalloc(2048, 1, NULL);
    kmalloc(512, 1, NULL);
    
    // Create some test IPC activity to make STATS command show meaningful data
    u32 test_queue1 = ipc_create_queue(0, 10);  // Kernel process queue
    u32 test_queue2 = ipc_create_queue(1, 5);   // User process 1 queue
    UNUSED(test_queue1);
    UNUSED(test_queue2);
    
    // Send some test messages
    char test_msg1[] = "Hello from kernel!";
    char test_msg2[] = "Test message 2";
    char test_msg3[] = "Broadcast test";
    
    ipc_send_message(0, 1, 1, test_msg1, sizeof(test_msg1));
    ipc_send_message(0, 1, 2, test_msg2, sizeof(test_msg2));
    ipc_broadcast_message(0, 3, test_msg3, sizeof(test_msg3));
    
    kprint("Interrupts initialized!\n");
    kprint("IPC system initialized!\n");
    kprint("Process manager initialized!\n");
    kprint("Test processes created!\n");
    kprint("Test memory allocations created!\n");
    kprint("Test IPC activity created!\n");
    kprint("System ready!\n> ");
}

void user_input(char *input) {
    if (strcmp(input, "END") == 0) {
        kprint("Stopping the CPU. Bye!\n");
        asm volatile("hlt");
    } else if (strcmp(input, "MEMORY") == 0) {
        u32 total, count, max;
        get_memory_stats(&total, &count, &max);
        
        kprint("Memory Statistics:\n");
        kprint("Total allocated: ");
        char total_str[10];
        int_to_ascii(total, total_str);
        kprint(total_str);
        kprint(" bytes\n");
        
        kprint("Allocation count: ");
        char count_str[10];
        int_to_ascii(count, count_str);
        kprint(count_str);
        kprint("\n");
        
        kprint("Max allocation: ");
        char max_str[10];
        int_to_ascii(max, max_str);
        kprint(max_str);
        kprint(" bytes\n");
        
        kprint("> ");
    } else if (strcmp(input, "STATS") == 0) {
        ipc_print_system_stats();
        kprint("> ");
    } else if (strcmp(input, "PROCESSES") == 0) {
        print_all_processes();
        kprint("> ");
    } else if (strcmp(input, "CLEAR") == 0) {
        clear_screen();
        kprint("> ");
    } else if (strcmp(input, "TIME") == 0) {
        // Convert ticks to time (50 ticks per second)
        u32 seconds = tick / 50;
        u32 minutes = seconds / 60;
        u32 hours = minutes / 60;
        seconds = seconds % 60;
        minutes = minutes % 60;
        
        kprint("System uptime: ");
        
        if (hours > 0) {
            char hours_str[10];
            int_to_ascii(hours, hours_str);
            kprint(hours_str);
            kprint("h ");
        }
        
        if (minutes > 0 || hours > 0) {
            char minutes_str[10];
            int_to_ascii(minutes, minutes_str);
            kprint(minutes_str);
            kprint("m ");
        }
        
        char seconds_str[10];
        int_to_ascii(seconds, seconds_str);
        kprint(seconds_str);
        kprint("s (");
        
        char tick_str[10];
        int_to_ascii(tick, tick_str);
        kprint(tick_str);
        kprint(" ticks)\n");
        
        kprint("> ");
    } else if (strcmp(input, "HELP") == 0) {
        kprint("=== Available Commands ===\n");
        kprint("END       - Stop the CPU and exit\n");
        kprint("MEMORY    - Display memory statistics\n");
        kprint("STATS     - Display IPC system statistics\n");
        kprint("PROCESSES - Display all active processes\n");
        kprint("CLEAR     - Clear the screen\n");
        kprint("TIME      - Show system uptime\n");
        kprint("HELP      - Show this help message\n");
        kprint("=======================\n");
        kprint("> ");
    } else {
        kprint("You said: ");
        kprint(input);
        kprint("\n> ");
    }
}
