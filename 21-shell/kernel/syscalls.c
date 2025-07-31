#include "syscalls.h"
#include "../drivers/screen.h"
#include "../libc/string.h"
#include "process.h"
#include "privilege.h"

#define NULL ((void*)0)

// System call handler table
typedef void (*syscall_handler_t)(registers_t *);
syscall_handler_t syscall_handlers[10];

// Initialize system call interface
void init_syscall_interface(void) {
    // Clear all handlers
    for (int i = 0; i < 10; i++) {
        syscall_handlers[i] = NULL;
    }
    
    // Register default handlers
    register_syscall_handler(SYS_CALL_EXIT, syscall_exit);
    register_syscall_handler(SYS_CALL_WRITE, syscall_write);
    register_syscall_handler(SYS_CALL_READ, syscall_read);
    register_syscall_handler(SYS_CALL_ALLOC, syscall_alloc);
    register_syscall_handler(SYS_CALL_FREE, syscall_free);
    
    kprint("System call interface initialized\n");
}

// Main system call handler
void syscall_handler(registers_t *regs) {
    u32 syscall_number = regs->eax;
    
    // Validate system call number
    if (syscall_number >= 10 || syscall_handlers[syscall_number] == NULL) {
        kprint("Invalid system call: ");
        char num_str[10];
        int_to_ascii(syscall_number, num_str);
        kprint(num_str);
        kprint("\n");
        regs->eax = -1; // Return error
        return;
    }
    
    // Call the appropriate handler
    syscall_handlers[syscall_number](regs);
}

// Register a system call handler
void register_syscall_handler(u32 syscall_number, void (*handler)(registers_t *)) {
    if (syscall_number < 10) {
        syscall_handlers[syscall_number] = handler;
    }
}

// System call: EXIT
void syscall_exit(registers_t *regs) {
    u32 exit_code = regs->ebx;
    
    kprint("Process exit with code: ");
    char code_str[10];
    int_to_ascii(exit_code, code_str);
    kprint(code_str);
    kprint("\n");
    
    // Terminate current process
    int current_pid = get_current_pid();
    if (current_pid >= 0) {
        terminate_process(current_pid);
    }
    
    regs->eax = 0; // Success
}

// System call: WRITE
void syscall_write(registers_t *regs) {
    u32 fd = regs->ebx;
    u32 buffer = regs->ecx;
    u32 size = regs->edx;
    (void)buffer; // Suppress unused variable warning
    
    kprint("Write to fd: ");
    char fd_str[5];
    int_to_ascii(fd, fd_str);
    kprint(fd_str);
    kprint(" size: ");
    int_to_ascii(size, fd_str);
    kprint(fd_str);
    kprint("\n");
    
    regs->eax = size; // Return bytes written
}

// System call: READ
void syscall_read(registers_t *regs) {
    u32 fd = regs->ebx;
    u32 buffer = regs->ecx;
    u32 size = regs->edx;
    (void)buffer; // Suppress unused variable warning
    
    kprint("Read from fd: ");
    char fd_str[5];
    int_to_ascii(fd, fd_str);
    kprint(fd_str);
    kprint(" size: ");
    int_to_ascii(size, fd_str);
    kprint(fd_str);
    kprint("\n");
    
    regs->eax = 0; // Return bytes read
}

// System call: ALLOC
void syscall_alloc(registers_t *regs) {
    u32 size = regs->ebx;
    
    kprint("Allocate memory: ");
    char size_str[10];
    int_to_ascii(size, size_str);
    kprint(size_str);
    kprint(" bytes\n");
    
    // For now, return a dummy address
    regs->eax = 0x100000; // Return allocated address
}

// System call: FREE
void syscall_free(registers_t *regs) {
    u32 address = regs->ebx;
    
    kprint("Free memory at: ");
    char addr_str[10];
    int_to_ascii(address, addr_str);
    kprint(addr_str);
    kprint("\n");
    
    regs->eax = 0; // Success
} 