#include "syscalls.h"
#include "../drivers/screen.h"
#include "../libc/string.h"
#include "process.h"
#include "privilege.h"
#include "ipc.h"

#define NULL ((void*)0)

// System call handler table
typedef void (*syscall_handler_t)(registers_t *);
syscall_handler_t syscall_handlers[30]; // Increased for enhanced IPC calls

// Initialize system call interface
void init_syscall_interface(void) {
    // Clear all handlers
    for (int i = 0; i < 30; i++) {
        syscall_handlers[i] = NULL;
    }
    
    // Register default handlers
    register_syscall_handler(SYS_CALL_EXIT, syscall_exit);
    register_syscall_handler(SYS_CALL_WRITE, syscall_write);
    register_syscall_handler(SYS_CALL_READ, syscall_read);
    register_syscall_handler(SYS_CALL_ALLOC, syscall_alloc);
    register_syscall_handler(SYS_CALL_FREE, syscall_free);
    
    // Register basic IPC handlers
    register_syscall_handler(SYS_IPC_SEND, syscall_ipc_send);
    register_syscall_handler(SYS_IPC_RECEIVE, syscall_ipc_receive);
    register_syscall_handler(SYS_IPC_CREATE_QUEUE, syscall_ipc_create_queue);
    register_syscall_handler(SYS_IPC_DELETE_QUEUE, syscall_ipc_delete_queue);
    
    // Register enhanced IPC handlers
    register_syscall_handler(SYS_IPC_SEND_PRIORITY, syscall_ipc_send_priority);
    register_syscall_handler(SYS_IPC_RECEIVE_TIMEOUT, syscall_ipc_receive_timeout);
    register_syscall_handler(SYS_IPC_BROADCAST, syscall_ipc_broadcast);
    register_syscall_handler(SYS_IPC_GET_STATS, syscall_ipc_get_stats);
    
    kprint("Enhanced system call interface initialized\n");
}

// Main system call handler
void syscall_handler(registers_t *regs) {
    u32 syscall_number = regs->eax;
    
    // Validate system call number
    if (syscall_number >= 30 || syscall_handlers[syscall_number] == NULL) {
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
    if (syscall_number < 30) {
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

// System call: IPC_SEND
void syscall_ipc_send(registers_t *regs) {
    u32 receiver_pid = regs->ebx;
    u32 message_type = regs->ecx;
    u32 data_ptr = regs->edx;
    u32 data_size = regs->esi;
    
    void *data = (void*)data_ptr;
    
    u32 result = sys_ipc_send(receiver_pid, message_type, data, data_size);
    regs->eax = result;
}

// System call: IPC_RECEIVE
void syscall_ipc_receive(registers_t *regs) {
    u32 message_ptr = regs->ebx;
    
    ipc_message_t *message = (ipc_message_t*)message_ptr;
    
    u32 result = sys_ipc_receive(message);
    regs->eax = result;
}

// System call: IPC_CREATE_QUEUE
void syscall_ipc_create_queue(registers_t *regs) {
    u32 max_messages = regs->ebx;
    
    u32 result = sys_ipc_create_queue(max_messages);
    regs->eax = result;
}

// System call: IPC_DELETE_QUEUE
void syscall_ipc_delete_queue(registers_t *regs) {
    u32 queue_id = regs->ebx;
    
    u32 result = sys_ipc_delete_queue(queue_id);
    regs->eax = result;
}

// NEW: Enhanced IPC System Call Handlers

// System call: IPC_SEND_PRIORITY
void syscall_ipc_send_priority(registers_t *regs) {
    u32 receiver_pid = regs->ebx;
    u32 message_type = regs->ecx;
    u32 data_ptr = regs->edx;
    u32 data_size = regs->esi;
    u32 priority = regs->edi;
    
    void *data = (void*)data_ptr;
    
    u32 result = sys_ipc_send_priority(receiver_pid, message_type, data, data_size, priority);
    regs->eax = result;
}

// System call: IPC_RECEIVE_TIMEOUT
void syscall_ipc_receive_timeout(registers_t *regs) {
    u32 message_ptr = regs->ebx;
    u32 timeout = regs->ecx;
    
    ipc_message_t *message = (ipc_message_t*)message_ptr;
    
    u32 result = sys_ipc_receive_timeout(message, timeout);
    regs->eax = result;
}

// System call: IPC_BROADCAST
void syscall_ipc_broadcast(registers_t *regs) {
    u32 message_type = regs->ebx;
    u32 data_ptr = regs->ecx;
    u32 data_size = regs->edx;
    
    void *data = (void*)data_ptr;
    
    u32 result = sys_ipc_broadcast(message_type, data, data_size);
    regs->eax = result;
}

// System call: IPC_GET_STATS
void syscall_ipc_get_stats(registers_t *regs) {
    u32 stats_ptr = regs->ebx;
    
    ipc_system_stats_t *stats = (ipc_system_stats_t*)stats_ptr;
    
    u32 result = sys_ipc_get_stats(stats);
    regs->eax = result;
}

// System call: WRITE
void syscall_write(registers_t *regs) {
    u32 fd = regs->ebx;
    u32 buf = regs->ecx;
    u32 count = regs->edx;
    
    kprint("Write syscall: fd=");
    char fd_str[10];
    int_to_ascii(fd, fd_str);
    kprint(fd_str);
    kprint(", count=");
    char count_str[10];
    int_to_ascii(count, count_str);
    kprint(count_str);
    kprint("\n");
    
    // For now, just print the data
    char *data = (char*)buf;
    for (u32 i = 0; i < count; i++) {
        char str[2] = {data[i], '\0'};
        kprint(str);
    }
    
    regs->eax = count; // Return number of bytes written
}

// System call: READ
void syscall_read(registers_t *regs) {
    u32 fd = regs->ebx;
    u32 count = regs->edx;
    
    kprint("Read syscall: fd=");
    char fd_str[10];
    int_to_ascii(fd, fd_str);
    kprint(fd_str);
    kprint(", count=");
    char count_str[10];
    int_to_ascii(count, count_str);
    kprint(count_str);
    kprint("\n");
    
    // For now, return 0 (no data read)
    regs->eax = 0;
}

// System call: ALLOC
void syscall_alloc(registers_t *regs) {
    u32 size = regs->ebx;
    
    kprint("Alloc syscall: size=");
    char size_str[10];
    int_to_ascii(size, size_str);
    kprint(size_str);
    kprint("\n");
    
    // For now, return a dummy address
    regs->eax = 0x1000000; // Return dummy address
}

// System call: FREE
void syscall_free(registers_t *regs) {
    u32 ptr = regs->ebx;
    
    kprint("Free syscall: ptr=");
    char ptr_str[10];
    int_to_ascii(ptr, ptr_str);
    kprint(ptr_str);
    kprint("\n");
    
    regs->eax = 0; // Success
} 