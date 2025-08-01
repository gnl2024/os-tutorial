#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "../cpu/types.h"
#include "../cpu/isr.h"
#include "privilege.h"
// #include "ipc.h"

// System call interface
#define SYSCALL_INTERRUPT 0x80

// System call numbers (matching privilege.h)
#define SYS_CALL_EXIT     1
#define SYS_CALL_WRITE    2
#define SYS_CALL_READ     3
#define SYS_CALL_ALLOC    4
#define SYS_CALL_FREE     5

// IPC System call numbers (from ipc.h)
#define SYS_IPC_SEND        20
#define SYS_IPC_RECEIVE     21
#define SYS_IPC_CREATE_QUEUE 22
#define SYS_IPC_DELETE_QUEUE 23
#define SYS_IPC_SEND_PRIORITY 24
#define SYS_IPC_RECEIVE_TIMEOUT 25
#define SYS_IPC_BROADCAST 26
#define SYS_IPC_GET_STATS 27

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

// IPC System call handlers
void syscall_ipc_send(registers_t *regs);
void syscall_ipc_receive(registers_t *regs);
void syscall_ipc_create_queue(registers_t *regs);
void syscall_ipc_delete_queue(registers_t *regs);
void syscall_ipc_send_priority(registers_t *regs);
void syscall_ipc_receive_timeout(registers_t *regs);
void syscall_ipc_broadcast(registers_t *regs);
void syscall_ipc_get_stats(registers_t *regs);

#endif // SYSCALLS_H 