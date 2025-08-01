#include "../kernel/process.h"
#include "../drivers/screen.h"
#include "../libc/string.h"

// Global current process pointer
extern process_t *current_process;

// Forward declaration
void save_process_state(process_t *proc);

// Switch to a new process using inline assembly
void switch_to_process(process_t *proc) {
    if (!proc) {
        kprint("Error: Invalid process pointer\n");
        return;
    }
    
    // Save current process state if needed
    if (current_process) {
        save_process_state(current_process);
    }
    
    // Update current process pointer
    current_process = proc;
    
    // Load new process state using inline assembly
    // This is a simplified version - in practice, you'd need more complex assembly
    kprint("Switching to process PID: ");
    char pid_str[10];
    int_to_ascii(proc->pid, pid_str);
    kprint(pid_str);
    kprint("\n");
    
    // For now, just update the current process pointer
    // In a real implementation, you'd need proper context switching
    current_process = proc;
}

// Save current process state (simplified version)
void save_process_state(process_t *proc) {
    if (!proc) {
        kprint("Error: Invalid process pointer for save\n");
        return;
    }
    
    // In a real implementation, you'd save all registers here
    // For now, just log the operation
    kprint("Process state saved for PID: ");
    char pid_str[10];
    int_to_ascii(proc->pid, pid_str);
    kprint(pid_str);
    kprint("\n");
} 