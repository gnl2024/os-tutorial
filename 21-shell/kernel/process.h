#ifndef PROCESS_H
#define PROCESS_H

#include "../cpu/types.h"

// Process states
#define PROCESS_RUNNING  0
#define PROCESS_READY    1
#define PROCESS_BLOCKED  2
#define PROCESS_TERMINATED 3

// Process privileges
#define PRIVILEGE_KERNEL 0
#define PRIVILEGE_USER   1

// Maximum processes
#define MAX_PROCESSES 16

// Process structure
typedef struct {
    int pid;
    void *stack;
    void *heap;
    int privileges;
    int state;
    struct {
        u32 eax, ebx, ecx, edx;
        u32 esi, edi, ebp, esp;
        u32 eip, eflags;
        u32 cs, ds, es, fs, gs, ss;
    } regs;
    int code_segment;
    int data_segment;
} process_t;

// Process management
extern process_t *current_process;
extern process_t processes[MAX_PROCESSES];
extern int next_pid;

// Function declarations
process_t *create_process(void (*entry_point)(void), void *stack, int privileges);
void init_process_manager(void);
void schedule(void);
void switch_to_process(process_t *proc);
int get_current_pid(void);
process_t *get_process(int pid);

#endif // PROCESS_H 