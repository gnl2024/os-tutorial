#include "process.h"
#include "../libc/mem.h"
#include "../libc/string.h"
#include "../drivers/screen.h"
#include "../cpu/gdt.h"
#include "memory.h"

#define NULL ((void*)0)

// Global process management variables
process_t *current_process = NULL;
process_t processes[MAX_PROCESSES];
int next_pid = 1;

// Initialize process manager
void init_process_manager(void) {
    // Clear all processes
    memset(processes, 0, sizeof(processes));
    
    // Create kernel process (PID 0)
    process_t *kernel_proc = &processes[0];
    kernel_proc->pid = 0;
    kernel_proc->state = PROCESS_RUNNING;
    kernel_proc->privileges = PRIVILEGE_KERNEL;
    kernel_proc->stack = (void*)0x10000;  // Kernel stack
    kernel_proc->heap = (void*)0x20000;   // Kernel heap
    
    // Set current process to kernel
    current_process = kernel_proc;
    
    kprint("Process manager initialized\n");
}

// Create a new process
process_t *create_process(void (*entry_point)(void), void *stack, int privileges) {
    if (next_pid >= MAX_PROCESSES) {
        kprint("Error: Maximum processes reached\n");
        return NULL;
    }
    
    process_t *proc = &processes[next_pid];
    
    // Initialize process structure
    proc->pid = next_pid++;
    proc->stack = stack;
    proc->heap = (void*)kmalloc(0x1000, 1, NULL);  // 4KB heap
    proc->privileges = privileges;
    proc->state = PROCESS_READY;
    
    // Initialize registers
    proc->regs.eip = (u32)entry_point;
    proc->regs.esp = (u32)stack + 0x1000;  // Stack grows down
    proc->regs.ebp = proc->regs.esp;
    proc->regs.eflags = 0x202;  // Interrupts enabled
    
    // Allocate memory regions for process
    u32 heap_start = (u32)proc->heap;
    allocate_memory_region(heap_start, 0x1000, 
                          PERMISSION_READ | PERMISSION_WRITE, 
                          proc->pid, MEMORY_TYPE_HEAP);
    
    u32 stack_start = (u32)proc->stack;
    allocate_memory_region(stack_start, 0x1000, 
                          PERMISSION_READ | PERMISSION_WRITE, 
                          proc->pid, MEMORY_TYPE_STACK);
    
    // Setup process-specific GDT segments
    setup_process_segments(proc);
    
    // Set segment registers based on process segments
    if (privileges == PRIVILEGE_KERNEL) {
        proc->regs.cs = 0x08;  // Kernel code segment
        proc->regs.ds = 0x10;  // Kernel data segment
        proc->regs.es = 0x10;
        proc->regs.fs = 0x10;
        proc->regs.gs = 0x10;
        proc->regs.ss = 0x10;
    } else {
        proc->regs.cs = proc->code_segment;  // Process-specific code segment
        proc->regs.ds = proc->data_segment;  // Process-specific data segment
        proc->regs.es = proc->data_segment;
        proc->regs.fs = proc->data_segment;
        proc->regs.gs = proc->data_segment;
        proc->regs.ss = proc->data_segment;
    }
    
    kprint("Created process PID: ");
    char pid_str[10];
    int_to_ascii(proc->pid, pid_str);
    kprint(pid_str);
    kprint("\n");
    
    return proc;
}

// Basic round-robin scheduler
void schedule(void) {
    static int current_index = 0;
    
    // Find next ready process
    for (int i = 0; i < MAX_PROCESSES; i++) {
        int index = (current_index + i) % MAX_PROCESSES;
        if (processes[index].state == PROCESS_READY) {
            // Switch to this process
            switch_to_process(&processes[index]);
            current_index = (index + 1) % MAX_PROCESSES;
            break;
        }
    }
}

// Get current process PID
int get_current_pid(void) {
    return current_process ? current_process->pid : -1;
}

// Get process by PID
process_t *get_process(int pid) {
    if (pid < 0 || pid >= MAX_PROCESSES) {
        return NULL;
    }
    return &processes[pid];
}

// Terminate a process
void terminate_process(int pid) {
    process_t *proc = get_process(pid);
    if (proc) {
        proc->state = PROCESS_TERMINATED;
        
        // Free process memory regions
        for (int i = 0; i < region_count; i++) {
            if (memory_regions[i].active && memory_regions[i].process_id == pid) {
                free_memory_region(i);
            }
        }
        
        kprint("Process terminated PID: ");
        char pid_str[10];
        int_to_ascii(pid, pid_str);
        kprint(pid_str);
        kprint(" (memory freed)\n");
    }
}

// Block a process
void block_process(int pid) {
    process_t *proc = get_process(pid);
    if (proc) {
        proc->state = PROCESS_BLOCKED;
    }
}

// Unblock a process
void unblock_process(int pid) {
    process_t *proc = get_process(pid);
    if (proc) {
        proc->state = PROCESS_READY;
    }
} 