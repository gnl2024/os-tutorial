#ifndef PRIVILEGE_H
#define PRIVILEGE_H

#include "../cpu/types.h"

// Privilege context structure
typedef struct {
    u8 current_privilege;
    u8 target_privilege;
    u32 saved_registers[8];
    u16 saved_segments[6];
} privilege_context_t;

// Privilege levels
#define PRIVILEGE_KERNEL_MODE 0  // Ring 0
#define PRIVILEGE_USER_MODE    3  // Ring 3

// System call numbers
#define SYS_CALL_EXIT     1
#define SYS_CALL_WRITE    2
#define SYS_CALL_READ     3
#define SYS_CALL_ALLOC    4
#define SYS_CALL_FREE     5

// System call structure
typedef struct {
    u32 syscall_number;
    u32 arg1;
    u32 arg2;
    u32 arg3;
    u32 return_value;
} syscall_t;

// Global privilege management
extern privilege_context_t current_privilege_context;
extern u8 current_privilege_level;

// Function declarations
void init_privilege_manager(void);
void switch_to_user_mode(void);
void switch_to_kernel_mode(void);
int check_privilege_access(u8 required_privilege);
void privilege_violation_handler(u8 attempted_privilege);
u32 handle_system_call(u32 syscall_number, u32 arg1, u32 arg2, u32 arg3);
void enter_user_mode(void);
void exit_user_mode(void);

#endif // PRIVILEGE_H 