#include "ipc.h"
#include "../drivers/print.h"
#include "../drivers/screen.h"
#include "../libc/string.h"
#include "process.h"
#include "memory.h"
#include "../libc/mem.h"

// Global IPC system state
static ipc_process_t ipc_processes[32]; // Support up to 32 processes
static u32 next_queue_id = 1;
static u32 next_message_id = 1;
static u32 total_queues_created = 0;
static u32 total_messages_sent = 0;
static u32 total_messages_received = 0;

// Initialize the IPC system
void init_ipc_system(void) {
    kprint("Initializing IPC system...\n");
    
    // Clear all IPC process structures
    for (int i = 0; i < 32; i++) {
        ipc_processes[i].pid = 0;
        ipc_processes[i].queue_count = 0;
        ipc_processes[i].pending_messages = 0;
        ipc_processes[i].total_messages_sent = 0;
        ipc_processes[i].total_messages_received = 0;
        
        // Clear all queues for this process
        for (int j = 0; j < IPC_MAX_QUEUES_PER_PROCESS; j++) {
            ipc_processes[i].queues[j].queue_id = 0;
            ipc_processes[i].queues[j].owner_pid = 0;
            ipc_processes[i].queues[j].max_messages = 0;
            ipc_processes[i].queues[j].current_count = 0;
            ipc_processes[i].queues[j].head = 0;
            ipc_processes[i].queues[j].tail = 0;
            ipc_processes[i].queues[j].status = IPC_QUEUE_EMPTY;
        }
    }
    
    next_queue_id = 1;
    next_message_id = 1;
    total_queues_created = 0;
    total_messages_sent = 0;
    total_messages_received = 0;
    
    kprint("IPC system initialized successfully!\n");
}

// Find IPC process structure by PID
static ipc_process_t* find_ipc_process(u32 pid) {
    for (int i = 0; i < 32; i++) {
        if (ipc_processes[i].pid == pid) {
            return &ipc_processes[i];
        }
    }
    return 0;
}

// Create a new IPC process entry
static ipc_process_t* create_ipc_process(u32 pid) {
    for (int i = 0; i < 32; i++) {
        if (ipc_processes[i].pid == 0) {
            ipc_processes[i].pid = pid;
            ipc_processes[i].queue_count = 0;
            ipc_processes[i].pending_messages = 0;
            ipc_processes[i].total_messages_sent = 0;
            ipc_processes[i].total_messages_received = 0;
            return &ipc_processes[i];
        }
    }
    return 0;
}

// Create an IPC queue for a process
u32 ipc_create_queue(u32 pid, u32 max_messages) {
    ipc_process_t *ipc_proc = find_ipc_process(pid);
    
    if (!ipc_proc) {
        ipc_proc = create_ipc_process(pid);
        if (!ipc_proc) {
            kprint("IPC: Failed to create process entry for PID ");
            char pid_str[10];
            int_to_ascii(pid, pid_str);
            kprint(pid_str);
            kprint("\n");
            return 0;
        }
    }
    
    if (ipc_proc->queue_count >= IPC_MAX_QUEUES_PER_PROCESS) {
        kprint("IPC: Process ");
        char pid_str[10];
        int_to_ascii(pid, pid_str);
        kprint(pid_str);
        kprint(" has reached maximum queue limit\n");
        return 0;
    }
    
    // Find available queue slot
    for (int i = 0; i < IPC_MAX_QUEUES_PER_PROCESS; i++) {
        if (ipc_proc->queues[i].queue_id == 0) {
            ipc_proc->queues[i].queue_id = next_queue_id++;
            ipc_proc->queues[i].owner_pid = pid;
            ipc_proc->queues[i].max_messages = max_messages > 16 ? 16 : max_messages;
            ipc_proc->queues[i].current_count = 0;
            ipc_proc->queues[i].head = 0;
            ipc_proc->queues[i].tail = 0;
            ipc_proc->queues[i].status = IPC_QUEUE_EMPTY;
            ipc_proc->queue_count++;
            total_queues_created++;
            
            kprint("IPC: Created queue ");
            char queue_str[10];
            char pid_str[10];
            int_to_ascii(ipc_proc->queues[i].queue_id, queue_str);
            kprint(queue_str);
            kprint(" for process ");
            int_to_ascii(pid, pid_str);
            kprint(pid_str);
            kprint("\n");
            
            return ipc_proc->queues[i].queue_id;
        }
    }
    
    return 0;
}

// Delete an IPC queue
u32 ipc_delete_queue(u32 queue_id) {
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < IPC_MAX_QUEUES_PER_PROCESS; j++) {
            if (ipc_processes[i].queues[j].queue_id == queue_id) {
                ipc_processes[i].queues[j].queue_id = 0;
                ipc_processes[i].queues[j].owner_pid = 0;
                ipc_processes[i].queues[j].max_messages = 0;
                ipc_processes[i].queues[j].current_count = 0;
                ipc_processes[i].queues[j].head = 0;
                ipc_processes[i].queues[j].tail = 0;
                ipc_processes[i].queues[j].status = IPC_QUEUE_EMPTY;
                ipc_processes[i].queue_count--;
                
                kprint("IPC: Deleted queue ");
                char queue_str[10];
                int_to_ascii(queue_id, queue_str);
                kprint(queue_str);
                kprint("\n");
                
                return 1;
            }
        }
    }
    
    kprint("IPC: Queue ");
    char queue_str[10];
    int_to_ascii(queue_id, queue_str);
    kprint(queue_str);
    kprint(" not found for deletion\n");
    return 0;
}

// Send a message between processes
u32 ipc_send_message(u32 sender_pid, u32 receiver_pid, 
                     u32 message_type, void *data, u32 data_size) {
    ipc_process_t *receiver_proc = find_ipc_process(receiver_pid);
    
    if (!receiver_proc) {
        kprint("IPC: Receiver process ");
        char pid_str[10];
        int_to_ascii(receiver_pid, pid_str);
        kprint(pid_str);
        kprint(" not found\n");
        return 0;
    }
    
    if (data_size > IPC_MAX_MESSAGE_SIZE) {
        kprint("IPC: Message too large (");
        char size_str[10];
        int_to_ascii(data_size, size_str);
        kprint(size_str);
        kprint(" bytes)\n");
        return 0;
    }
    
    // Find a queue for the receiver
    ipc_queue_t *target_queue = 0;
    for (int i = 0; i < IPC_MAX_QUEUES_PER_PROCESS; i++) {
        if (receiver_proc->queues[i].queue_id != 0) {
            if (receiver_proc->queues[i].current_count < receiver_proc->queues[i].max_messages) {
                target_queue = &receiver_proc->queues[i];
                break;
            }
        }
    }
    
    if (!target_queue) {
        kprint("IPC: No available queue for receiver process ");
        char pid_str[10];
        int_to_ascii(receiver_pid, pid_str);
        kprint(pid_str);
        kprint("\n");
        return 0;
    }
    
    // Create the message
    ipc_message_t *msg = &target_queue->messages[target_queue->tail];
    msg->sender_pid = sender_pid;
    msg->receiver_pid = receiver_pid;
    msg->message_type = message_type;
    msg->message_id = next_message_id++;
    msg->data_size = data_size;
    msg->status = IPC_MSG_STATUS_PENDING;
    msg->timestamp = 0; // Simple timestamp
    
    // Copy data
    if (data && data_size > 0) {
        memory_copy((u8*)data, msg->data, data_size);
    }
    
    // Update queue
    target_queue->tail = (target_queue->tail + 1) % 16;
    target_queue->current_count++;
    target_queue->status = IPC_QUEUE_AVAILABLE;
    
    // Update statistics
    receiver_proc->pending_messages++;
    total_messages_sent++;
    
    // Update sender statistics
    ipc_process_t *sender_proc = find_ipc_process(sender_pid);
    if (sender_proc) {
        sender_proc->total_messages_sent++;
    }
    
    kprint("IPC: Message ");
    char msg_str[10];
    char pid_str[10];
    int_to_ascii(msg->message_id, msg_str);
    kprint(msg_str);
    kprint(" sent from PID ");
    int_to_ascii(sender_pid, pid_str);
    kprint(pid_str);
    kprint(" to PID ");
    int_to_ascii(receiver_pid, pid_str);
    kprint(pid_str);
    kprint("\n");
    
    return msg->message_id;
}

// Receive a message for a process
u32 ipc_receive_message(u32 receiver_pid, ipc_message_t *message) {
    ipc_process_t *receiver_proc = find_ipc_process(receiver_pid);
    
    if (!receiver_proc) {
        kprint("IPC: Receiver process ");
        char pid_str[10];
        int_to_ascii(receiver_pid, pid_str);
        kprint(pid_str);
        kprint(" not found\n");
        return 0;
    }
    
    // Find a queue with messages
    for (int i = 0; i < IPC_MAX_QUEUES_PER_PROCESS; i++) {
        ipc_queue_t *queue = &receiver_proc->queues[i];
        if (queue->queue_id != 0 && queue->current_count > 0) {
            // Get the oldest message
            ipc_message_t *msg = &queue->messages[queue->head];
            
            // Copy message to output
            memory_copy((u8*)msg, (u8*)message, sizeof(ipc_message_t));
            message->status = IPC_MSG_STATUS_READ;
            
            // Update queue
            queue->head = (queue->head + 1) % 16;
            queue->current_count--;
            if (queue->current_count == 0) {
                queue->status = IPC_QUEUE_EMPTY;
            }
            
            // Update statistics
            receiver_proc->pending_messages--;
            receiver_proc->total_messages_received++;
            total_messages_received++;
            
            kprint("IPC: Message ");
            char msg_str[10];
            char pid_str[10];
            int_to_ascii(msg->message_id, msg_str);
            kprint(msg_str);
            kprint(" received by PID ");
            int_to_ascii(receiver_pid, pid_str);
            kprint(pid_str);
            kprint("\n");
            
            return msg->message_id;
        }
    }
    
    return 0; // No messages available
}

// Get queue status
u32 ipc_get_queue_status(u32 queue_id) {
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < IPC_MAX_QUEUES_PER_PROCESS; j++) {
            if (ipc_processes[i].queues[j].queue_id == queue_id) {
                return ipc_processes[i].queues[j].status;
            }
        }
    }
    return 0;
}

// Clean up IPC data for a process
void ipc_cleanup_process(u32 pid) {
    ipc_process_t *ipc_proc = find_ipc_process(pid);
    if (ipc_proc) {
        // Delete all queues for this process
        for (int i = 0; i < IPC_MAX_QUEUES_PER_PROCESS; i++) {
            if (ipc_proc->queues[i].queue_id != 0) {
                ipc_delete_queue(ipc_proc->queues[i].queue_id);
            }
        }
        
        // Clear process entry
        ipc_proc->pid = 0;
        ipc_proc->queue_count = 0;
        ipc_proc->pending_messages = 0;
        ipc_proc->total_messages_sent = 0;
        ipc_proc->total_messages_received = 0;
        
        kprint("IPC: Cleaned up process ");
        char pid_str[10];
        int_to_ascii(pid, pid_str);
        kprint(pid_str);
        kprint("\n");
    }
}

// System call handlers
u32 sys_ipc_send(u32 receiver_pid, u32 message_type, 
                  void *data, u32 data_size) {
    u32 sender_pid = get_current_pid();
    return ipc_send_message(sender_pid, receiver_pid, message_type, data, data_size);
}

u32 sys_ipc_receive(ipc_message_t *message) {
    u32 receiver_pid = get_current_pid();
    return ipc_receive_message(receiver_pid, message);
}

u32 sys_ipc_create_queue(u32 max_messages) {
    u32 pid = get_current_pid();
    return ipc_create_queue(pid, max_messages);
}

u32 sys_ipc_delete_queue(u32 queue_id) {
    return ipc_delete_queue(queue_id);
}

u32 sys_ipc_get_queue_status(u32 queue_id) {
    return ipc_get_queue_status(queue_id);
}

// Debug functions
void ipc_print_queue_status(u32 queue_id) {
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < IPC_MAX_QUEUES_PER_PROCESS; j++) {
            if (ipc_processes[i].queues[j].queue_id == queue_id) {
                ipc_queue_t *queue = &ipc_processes[i].queues[j];
                kprint("IPC Queue ");
                char queue_str[10];
                int_to_ascii(queue_id, queue_str);
                kprint(queue_str);
                kprint(" - Owner PID: ");
                char pid_str[10];
                int_to_ascii(queue->owner_pid, pid_str);
                kprint(pid_str);
                kprint(", Messages: ");
                int_to_ascii(queue->current_count, pid_str);
                kprint(pid_str);
                kprint("/");
                int_to_ascii(queue->max_messages, pid_str);
                kprint(pid_str);
                kprint("\n");
                return;
            }
        }
    }
    kprint("IPC Queue ");
    char queue_str[10];
    int_to_ascii(queue_id, queue_str);
    kprint(queue_str);
    kprint(" not found\n");
}

void ipc_print_process_stats(u32 pid) {
    ipc_process_t *ipc_proc = find_ipc_process(pid);
    if (ipc_proc) {
        kprint("IPC Process ");
        char pid_str[10];
        int_to_ascii(pid, pid_str);
        kprint(pid_str);
        kprint(" - Queues: ");
        int_to_ascii(ipc_proc->queue_count, pid_str);
        kprint(pid_str);
        kprint(", Pending: ");
        int_to_ascii(ipc_proc->pending_messages, pid_str);
        kprint(pid_str);
        kprint(", Sent: ");
        int_to_ascii(ipc_proc->total_messages_sent, pid_str);
        kprint(pid_str);
        kprint(", Received: ");
        int_to_ascii(ipc_proc->total_messages_received, pid_str);
        kprint(pid_str);
        kprint("\n");
    } else {
        kprint("IPC Process ");
        char pid_str[10];
        int_to_ascii(pid, pid_str);
        kprint(pid_str);
        kprint(" not found\n");
    }
}

void ipc_print_system_stats(void) {
    kprint("=== IPC System Statistics ===\n");
    kprint("Total queues created: ");
    char stat_str[10];
    int_to_ascii(total_queues_created, stat_str);
    kprint(stat_str);
    kprint("\n");
    kprint("Total messages sent: ");
    int_to_ascii(total_messages_sent, stat_str);
    kprint(stat_str);
    kprint("\n");
    kprint("Total messages received: ");
    int_to_ascii(total_messages_received, stat_str);
    kprint(stat_str);
    kprint("\n");
    kprint("Active processes: ");
    int active_count = 0;
    for (int i = 0; i < 32; i++) {
        if (ipc_processes[i].pid != 0) {
            active_count++;
        }
    }
    int_to_ascii(active_count, stat_str);
    kprint(stat_str);
    kprint("\n");
} 