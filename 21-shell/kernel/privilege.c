#include "privilege.h"
#include "../drivers/screen.h"
#include "../libc/string.h"
#include "process.h"
#include "../cpu/segment_protection.h"

// Global privilege management
privilege_context_t current_privilege_context;
u8 current_privilege_level = PRIVILEGE_KERNEL_MODE;

// Initialize privilege manager
void init_privilege_manager(void) {
    current_privilege_level = PRIVILEGE_KERNEL_MODE;
    memset(&current_privilege_context, 0, sizeof(privilege_context_t));
    
    kprint("Privilege manager initialized\n");
}

// Switch to user mode
void switch_to_user_mode(void) {
    if (current_privilege_level == PRIVILEGE_KERNEL_MODE) {
        current_privilege_level = PRIVILEGE_USER_MODE;
        current_privilege_context.current_privilege = PRIVILEGE_USER_MODE;
        
        kprint("Switched to user mode\n");
    }
}

// Switch to kernel mode
void switch_to_kernel_mode(void) {
    if (current_privilege_level == PRIVILEGE_USER_MODE) {
        current_privilege_level = PRIVILEGE_KERNEL_MODE;
        current_privilege_context.current_privilege = PRIVILEGE_KERNEL_MODE;
        
        kprint("Switched to kernel mode\n");
    }
}

// Check privilege access
int check_privilege_access(u8 required_privilege) {
    if (current_privilege_level <= required_privilege) {
        return 1; // Access allowed
    } else {
        return 0; // Access denied
    }
}

// Handle privilege violations
void privilege_violation_handler(u8 attempted_privilege) {
    kprint("Privilege violation! Attempted: ");
    char priv_str[5];
    int_to_ascii(attempted_privilege, priv_str);
    kprint(priv_str);
    kprint(" Current: ");
    int_to_ascii(current_privilege_level, priv_str);
    kprint(priv_str);
    kprint("\n");
}

// Handle system calls
u32 handle_system_call(u32 syscall_number, u32 arg1, u32 arg2, u32 arg3) {
    (void)arg1; // Suppress unused parameter warning
    (void)arg2; // Suppress unused parameter warning
    (void)arg3; // Suppress unused parameter warning
    // Validate system call number
    if (syscall_number < 1 || syscall_number > 5) {
        kprint("Invalid system call number: ");
        char num_str[10];
        int_to_ascii(syscall_number, num_str);
        kprint(num_str);
        kprint("\n");
        return -1;
    }
    
    // Switch to kernel mode for system call handling
    u8 previous_privilege = current_privilege_level;
    switch_to_kernel_mode();
    
    u32 result = 0;
    
    switch (syscall_number) {
        case SYS_CALL_EXIT:
            kprint("System call: EXIT\n");
            result = 0;
            break;
            
        case SYS_CALL_WRITE:
            kprint("System call: WRITE\n");
            result = 0;
            break;
            
        case SYS_CALL_READ:
            kprint("System call: READ\n");
            result = 0;
            break;
            
        case SYS_CALL_ALLOC:
            kprint("System call: ALLOC\n");
            result = 0;
            break;
            
        case SYS_CALL_FREE:
            kprint("System call: FREE\n");
            result = 0;
            break;
            
        default:
            kprint("Unknown system call\n");
            result = -1;
            break;
    }
    
    // Restore previous privilege level
    if (previous_privilege == PRIVILEGE_USER_MODE) {
        switch_to_user_mode();
    }
    
    return result;
}

// Enter user mode
void enter_user_mode(void) {
    switch_to_user_mode();
    kprint("Entered user mode\n");
}

// Exit user mode
void exit_user_mode(void) {
    switch_to_kernel_mode();
    kprint("Exited user mode\n");
} 