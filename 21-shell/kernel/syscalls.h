#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "../cpu/types.h"
#include "../cpu/isr.h"
#include "privilege.h"

// System call interface
#define SYSCALL_INTERRUPT 0x80

// System call numbers (matching privilege.h)
#define SYS_CALL_EXIT     1
#define SYS_CALL_WRITE    2
#define SYS_CALL_READ     3
#define SYS_CALL_ALLOC    4
#define SYS_CALL_FREE     5

// System call function declarations
void init_syscall_interface(void);
void syscall_handler(registers_t *regs);
void register_syscall_handler(u32 syscall_number, void (*handler)(registers_t *));

// System call handlers
void syscall_exit(registers_t *regs);
void syscall_write(registers_t *regs);
void syscall_read(registers_t *regs);
void syscall_alloc(registers_t *regs);
void syscall_free(registers_t *regs);

#endif // SYSCALLS_H 