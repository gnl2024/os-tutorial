#include "ipc.h"
#include "../drivers/print.h"
#include "../drivers/screen.h"
#include "../libc/string.h"
#include "../libc/mem.h"
#include "process.h"
#include "memory.h"

// Global IPC system state
static ipc_process_t ipc_processes[32]; // Support up to 32 processes
static u32 next_queue_id = 1;
static u32 next_message_id = 1;
static u32 total_queues_created = 0;
static u32 total_messages_sent = 0;
static u32 total_messages_received = 0;
static u32 total_messages_dropped = 0;
static u32 total_broadcasts = 0;
static u64 system_start_time = 0;

// Initialize the IPC system
void init_ipc_system(void) {
    kprint("Initializing enhanced IPC system...\n");
    
    // Clear all IPC process structures
    for (int i = 0; i < 32; i++) {
        ipc_processes[i].pid = 0;
        ipc_processes[i].queue_count = 0;
        ipc_processes[i].pending_messages = 0;
        ipc_processes[i].total_messages_sent = 0;
        ipc_processes[i].total_messages_received = 0;
        ipc_processes[i].priority = IPC_PRIORITY_NORMAL;
        
        // Clear all queues for this process
        for (int j = 0; j < IPC_MAX_QUEUES_PER_PROCESS; j++) {
            ipc_processes[i].queues[j].queue_id = 0;
            ipc_processes[i].queues[j].owner_pid = 0;
            ipc_processes[i].queues[j].max_messages = 0;
            ipc_processes[i].queues[j].current_count = 0;
            ipc_processes[i].queues[j].head = 0;
            ipc_processes[i].queues[j].tail = 0;
            ipc_processes[i].queues[j].status = IPC_QUEUE_EMPTY;
            ipc_processes[i].queues[j].total_messages_processed = 0;
            ipc_processes[i].queues[j].total_messages_dropped = 0;
        }
    }
    
    next_queue_id = 1;
    next_message_id = 1;
    total_queues_created = 0;
    total_messages_sent = 0;
    total_messages_received = 0;
    total_messages_dropped = 0;
    total_broadcasts = 0;
    system_start_time = 0; // Will be set by timer
    
    kprint("Enhanced IPC system initialized successfully!\n");
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
            ipc_processes[i].priority = IPC_PRIORITY_NORMAL;
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
    
    // Find an empty queue slot
    for (int i = 0; i < IPC_MAX_QUEUES_PER_PROCESS; i++) {
        if (ipc_proc->queues[i].queue_id == 0) {
            ipc_proc->queues[i].queue_id = next_queue_id++;
            ipc_proc->queues[i].owner_pid = pid;
            ipc_proc->queues[i].max_messages = max_messages;
            ipc_proc->queues[i].current_count = 0;
            ipc_proc->queues[i].head = 0;
            ipc_proc->queues[i].tail = 0;
            ipc_proc->queues[i].status = IPC_QUEUE_EMPTY;
            ipc_proc->queues[i].total_messages_processed = 0;
            ipc_proc->queues[i].total_messages_dropped = 0;
            ipc_proc->queue_count++;
            total_queues_created++;
            
            kprint("IPC: Created queue ");
            char queue_str[10];
            int_to_ascii(ipc_proc->queues[i].queue_id, queue_str);
            kprint(queue_str);
            kprint(" for PID ");
            char pid_str[10];
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
                ipc_processes[i].queues[j].total_messages_processed = 0;
                ipc_processes[i].queues[j].total_messages_dropped = 0;
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
    return 0;
}

// Send a message to a process (basic version)
u32 ipc_send_message(u32 sender_pid, u32 receiver_pid, 
                     u32 message_type, void *data, u32 data_size) {
    return ipc_send_with_priority(sender_pid, receiver_pid, message_type, data, data_size, IPC_PRIORITY_NORMAL);
}

// Enhanced send message with priority
u32 ipc_send_with_priority(u32 sender_pid, u32 receiver_pid, 
                           u32 message_type, void *data, u32 data_size, u32 priority) {
    ipc_process_t *receiver = find_ipc_process(receiver_pid);
    
    if (!receiver) {
        kprint("IPC: Receiver PID ");
        char pid_str[10];
        int_to_ascii(receiver_pid, pid_str);
        kprint(pid_str);
        kprint(" not found\n");
        return 0;
    }
    
    // Find a queue for the receiver
    ipc_queue_t *queue = 0;
    for (int i = 0; i < IPC_MAX_QUEUES_PER_PROCESS; i++) {
        if (receiver->queues[i].queue_id != 0) {
            queue = &receiver->queues[i];
            break;
        }
    }
    
    if (!queue) {
        kprint("IPC: No queue found for receiver PID ");
        char pid_str[10];
        int_to_ascii(receiver_pid, pid_str);
        kprint(pid_str);
        kprint("\n");
        return 0;
    }
    
    if (queue->current_count >= queue->max_messages) {
        queue->total_messages_dropped++;
        total_messages_dropped++;
        kprint("IPC: Queue full for receiver PID ");
        char pid_str[10];
        int_to_ascii(receiver_pid, pid_str);
        kprint(pid_str);
        kprint(", message dropped\n");
        return 0;
    }
    
    // Add message to queue with priority ordering
    u32 insert_index = queue->tail;
    
    // Find the correct position based on priority
    for (u32 i = 0; i < queue->current_count; i++) {
        u32 idx = (queue->head + i) % queue->max_messages;
        if (queue->messages[idx].priority < priority) {
            // Move existing messages to make room
            for (u32 j = queue->current_count; j > i; j--) {
                u32 src_idx = (queue->head + j - 1) % queue->max_messages;
                u32 dst_idx = (queue->head + j) % queue->max_messages;
                memory_copy((u8*)&queue->messages[src_idx], (u8*)&queue->messages[dst_idx], sizeof(ipc_message_t));
            }
            insert_index = (queue->head + i) % queue->max_messages;
            break;
        }
    }
    
    // Create the message
    ipc_message_t *msg = &queue->messages[insert_index];
    msg->message_id = next_message_id++;
    msg->sender_pid = sender_pid;
    msg->receiver_pid = receiver_pid;
    msg->message_type = message_type;
    msg->data_size = data_size;
    msg->status = IPC_MSG_STATUS_UNREAD;
    msg->priority = priority;
    msg->timestamp = system_start_time; // Will be enhanced with real timer
    
    // Copy data if provided
    if (data && data_size > 0 && data_size <= IPC_MAX_MESSAGE_SIZE) {
        memory_copy((u8*)data, (u8*)msg->data, data_size);
    }
    
    // Update queue
    queue->tail = (queue->tail + 1) % queue->max_messages;
    queue->current_count++;
    queue->status = IPC_QUEUE_HAS_MESSAGES;
    queue->total_messages_processed++;
    receiver->pending_messages++;
    
    // Update sender stats
    ipc_process_t *sender = find_ipc_process(sender_pid);
    if (sender) {
        sender->total_messages_sent++;
    }
    
    total_messages_sent++;
    
    kprint("IPC: Priority message sent from PID ");
    char sender_str[10];
    int_to_ascii(sender_pid, sender_str);
    kprint(sender_str);
    kprint(" to PID ");
    char receiver_str[10];
    int_to_ascii(receiver_pid, receiver_str);
    kprint(receiver_str);
    kprint(" (Priority: ");
    char priority_str[10];
    int_to_ascii(priority, priority_str);
    kprint(priority_str);
    kprint(")\n");
    
    return msg->message_id;
}

// Receive a message for a process (basic version)
u32 ipc_receive_message(u32 receiver_pid, ipc_message_t *message) {
    return ipc_receive_with_timeout(receiver_pid, message, 0); // No timeout
}

// Enhanced receive message with timeout
u32 ipc_receive_with_timeout(u32 receiver_pid, ipc_message_t *message, u32 timeout_ms) {
    ipc_process_t *receiver = find_ipc_process(receiver_pid);
    
    if (!receiver) {
        kprint("IPC: Receiver PID ");
        char pid_str[10];
        int_to_ascii(receiver_pid, pid_str);
        kprint(pid_str);
        kprint(" not found\n");
        return 0;
    }
    
    // Find a queue with messages
    ipc_queue_t *queue = 0;
    for (int i = 0; i < IPC_MAX_QUEUES_PER_PROCESS; i++) {
        if (receiver->queues[i].queue_id != 0 && 
            receiver->queues[i].current_count > 0) {
            queue = &receiver->queues[i];
            break;
        }
    }
    
    if (!queue) {
        if (timeout_ms > 0) {
            kprint("IPC: No messages for PID ");
            char pid_str[10];
            int_to_ascii(receiver_pid, pid_str);
            kprint(pid_str);
            kprint(" (timeout)\n");
            message->status = IPC_MSG_STATUS_TIMEOUT;
        } else {
            kprint("IPC: No messages for PID ");
            char pid_str[10];
            int_to_ascii(receiver_pid, pid_str);
            kprint(pid_str);
            kprint("\n");
        }
        return 0;
    }
    
    // Get highest priority message
    u32 highest_priority = 0;
    u32 best_index = queue->head;
    
    for (u32 i = 0; i < queue->current_count; i++) {
        u32 idx = (queue->head + i) % queue->max_messages;
        if (queue->messages[idx].priority > highest_priority) {
            highest_priority = queue->messages[idx].priority;
            best_index = idx;
        }
    }
    
    // Copy message
    memory_copy((u8*)&queue->messages[best_index], (u8*)message, sizeof(ipc_message_t));
    message->status = IPC_MSG_STATUS_READ;
    
    // Remove message from queue
    queue->messages[best_index].status = IPC_MSG_STATUS_READ;
    queue->current_count--;
    receiver->pending_messages--;
    receiver->total_messages_received++;
    total_messages_received++;
    
    if (queue->current_count == 0) {
        queue->status = IPC_QUEUE_EMPTY;
    }
    
    kprint("IPC: Priority message received by PID ");
    char pid_str[10];
    int_to_ascii(receiver_pid, pid_str);
    kprint(pid_str);
    kprint(" (Priority: ");
    char priority_str[10];
    int_to_ascii(message->priority, priority_str);
    kprint(priority_str);
    kprint(")\n");
    
    return message->message_id;
}

// Broadcast message to all processes
u32 ipc_broadcast_message(u32 sender_pid, u32 message_type, void *data, u32 data_size) {
    u32 broadcast_count = 0;
    
    for (int i = 0; i < 32; i++) {
        if (ipc_processes[i].pid != 0 && ipc_processes[i].pid != sender_pid) {
            if (ipc_send_with_priority(sender_pid, ipc_processes[i].pid,
                                     message_type, data, data_size,
                                     IPC_PRIORITY_NORMAL)) {
                broadcast_count++;
            }
        }
    }
    
    total_broadcasts++;
    kprint("IPC: Broadcast sent to ");
    char count_str[10];
    int_to_ascii(broadcast_count, count_str);
    kprint(count_str);
    kprint(" processes\n");
    
    return broadcast_count;
}

// Set process priority
void ipc_set_process_priority(u32 pid, u32 priority) {
    ipc_process_t *proc = find_ipc_process(pid);
    if (proc) {
        proc->priority = priority;
        kprint("IPC: Set priority ");
        char priority_str[10];
        int_to_ascii(priority, priority_str);
        kprint(priority_str);
        kprint(" for PID ");
        char pid_str[10];
        int_to_ascii(pid, pid_str);
        kprint(pid_str);
        kprint("\n");
    }
}

// Get system statistics
u32 ipc_get_system_stats(ipc_system_stats_t *stats) {
    if (!stats) return 0;
    
    stats->total_queues_created = total_queues_created;
    stats->total_messages_sent = total_messages_sent;
    stats->total_messages_received = total_messages_received;
    stats->total_messages_dropped = total_messages_dropped;
    stats->total_broadcasts = total_broadcasts;
    stats->average_message_size = total_messages_sent > 0 ? 128 : 0; // Placeholder
    stats->peak_queue_depth = 0; // Would need to track this
    stats->system_uptime = system_start_time;
    
    return 1;
}

// Clean up IPC data for a process
void ipc_cleanup_process(u32 pid) {
    ipc_process_t *proc = find_ipc_process(pid);
    if (proc) {
        // Delete all queues for this process
        for (int i = 0; i < IPC_MAX_QUEUES_PER_PROCESS; i++) {
            if (proc->queues[i].queue_id != 0) {
                ipc_delete_queue(proc->queues[i].queue_id);
            }
        }
        
        // Clear process entry
        proc->pid = 0;
        proc->queue_count = 0;
        proc->pending_messages = 0;
        proc->total_messages_sent = 0;
        proc->total_messages_received = 0;
        proc->priority = IPC_PRIORITY_NORMAL;
        
        kprint("IPC: Cleaned up process ");
        char pid_str[10];
        int_to_ascii(pid, pid_str);
        kprint(pid_str);
        kprint("\n");
    }
}

// System call wrappers
u32 sys_ipc_send(u32 receiver_pid, u32 message_type, 
                  void *data, u32 data_size) {
    u32 current_pid = get_current_pid();
    return ipc_send_message(current_pid, receiver_pid, message_type, data, data_size);
}

u32 sys_ipc_receive(ipc_message_t *message) {
    u32 current_pid = get_current_pid();
    return ipc_receive_message(current_pid, message);
}

u32 sys_ipc_create_queue(u32 max_messages) {
    u32 current_pid = get_current_pid();
    return ipc_create_queue(current_pid, max_messages);
}

u32 sys_ipc_delete_queue(u32 queue_id) {
    return ipc_delete_queue(queue_id);
}

// NEW: Enhanced system call wrappers
u32 sys_ipc_send_priority(u32 receiver_pid, u32 message_type, void *data, u32 data_size, u32 priority) {
    u32 current_pid = get_current_pid();
    return ipc_send_with_priority(current_pid, receiver_pid, message_type, data, data_size, priority);
}

u32 sys_ipc_receive_timeout(ipc_message_t *message, u32 timeout) {
    u32 current_pid = get_current_pid();
    return ipc_receive_with_timeout(current_pid, message, timeout);
}

u32 sys_ipc_broadcast(u32 message_type, void *data, u32 data_size) {
    u32 current_pid = get_current_pid();
    return ipc_broadcast_message(current_pid, message_type, data, data_size);
}

u32 sys_ipc_get_stats(ipc_system_stats_t *stats) {
    return ipc_get_system_stats(stats);
}

// Print enhanced IPC statistics
void ipc_print_system_stats(void) {
    kprint("=== Enhanced IPC System Statistics ===\n");
    kprint("Total queues created: ");
    char queues_str[10];
    int_to_ascii(total_queues_created, queues_str);
    kprint(queues_str);
    kprint("\n");
    
    kprint("Total messages sent: ");
    char sent_str[10];
    int_to_ascii(total_messages_sent, sent_str);
    kprint(sent_str);
    kprint("\n");
    
    kprint("Total messages received: ");
    char received_str[10];
    int_to_ascii(total_messages_received, received_str);
    kprint(received_str);
    kprint("\n");
    
    kprint("Total messages dropped: ");
    char dropped_str[10];
    int_to_ascii(total_messages_dropped, dropped_str);
    kprint(dropped_str);
    kprint("\n");
    
    kprint("Total broadcasts: ");
    char broadcast_str[10];
    int_to_ascii(total_broadcasts, broadcast_str);
    kprint(broadcast_str);
    kprint("\n");
    
    kprint("Active processes:\n");
    for (int i = 0; i < 32; i++) {
        if (ipc_processes[i].pid != 0) {
            kprint("  PID ");
            char pid_str[10];
            int_to_ascii(ipc_processes[i].pid, pid_str);
            kprint(pid_str);
            kprint(": ");
            char queue_str[10];
            int_to_ascii(ipc_processes[i].queue_count, queue_str);
            kprint(queue_str);
            kprint(" queues, ");
            char pending_str[10];
            int_to_ascii(ipc_processes[i].pending_messages, pending_str);
            kprint(pending_str);
            kprint(" pending messages, Priority: ");
            char priority_str[10];
            int_to_ascii(ipc_processes[i].priority, priority_str);
            kprint(priority_str);
            kprint("\n");
        }
    }
    kprint("=====================================\n");
} 