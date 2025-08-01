#ifndef IPC_H
#define IPC_H

#include "../cpu/types.h"

// IPC message structure
typedef struct {
    u32 message_id;
    u32 sender_pid;
    u32 receiver_pid;
    u32 message_type;
    u32 data_size;
    u32 status;
    u32 priority;        // NEW: Priority levels
    u64 timestamp;       // NEW: Timestamp for ordering
    u8 data[256]; // Maximum message data size
} ipc_message_t;

// IPC queue structure
typedef struct {
    u32 queue_id;
    u32 owner_pid;
    u32 max_messages;
    u32 current_count;
    u32 head;
    u32 tail;
    u32 status;
    u32 total_messages_processed;  // NEW: Statistics
    u32 total_messages_dropped;    // NEW: Statistics
    ipc_message_t messages[16]; // Maximum 16 messages per queue
} ipc_queue_t;

// IPC process structure
typedef struct {
    u32 pid;
    u32 queue_count;
    u32 pending_messages;
    u32 total_messages_sent;
    u32 total_messages_received;
    u32 priority;        // NEW: Process priority
    ipc_queue_t queues[4]; // Maximum 4 queues per process
} ipc_process_t;

// Constants
#define IPC_MAX_QUEUES_PER_PROCESS 4
#define IPC_MAX_MESSAGE_SIZE 256
#define IPC_MAX_MESSAGES_PER_QUEUE 16

// Message status
#define IPC_MSG_STATUS_UNREAD 0
#define IPC_MSG_STATUS_READ 1
#define IPC_MSG_STATUS_TIMEOUT 2  // NEW: Timeout status

// Queue status
#define IPC_QUEUE_EMPTY 0
#define IPC_QUEUE_HAS_MESSAGES 1
#define IPC_QUEUE_FULL 2          // NEW: Full queue status

// Message types
#define IPC_MSG_TYPE_DATA 1
#define IPC_MSG_TYPE_COMMAND 2
#define IPC_MSG_TYPE_RESPONSE 3
#define IPC_MSG_TYPE_BROADCAST 4  // NEW: Broadcast messages
#define IPC_MSG_TYPE_PRIORITY 5   // NEW: Priority messages
#define IPC_MSG_TYPE_ASYNC 6      // NEW: Asynchronous messages

// Priority levels
#define IPC_PRIORITY_LOW      1
#define IPC_PRIORITY_NORMAL   2
#define IPC_PRIORITY_HIGH     3
#define IPC_PRIORITY_URGENT   4

// System call numbers for IPC
#define SYS_IPC_SEND 20
#define SYS_IPC_RECEIVE 21
#define SYS_IPC_CREATE_QUEUE 22
#define SYS_IPC_DELETE_QUEUE 23
#define SYS_IPC_SEND_PRIORITY 24    // NEW: Priority send
#define SYS_IPC_RECEIVE_TIMEOUT 25  // NEW: Timeout receive
#define SYS_IPC_BROADCAST 26        // NEW: Broadcast
#define SYS_IPC_GET_STATS 27        // NEW: Statistics

// IPC System Statistics
typedef struct {
    u32 total_queues_created;
    u32 total_messages_sent;
    u32 total_messages_received;
    u32 total_messages_dropped;
    u32 total_broadcasts;
    u32 average_message_size;
    u32 peak_queue_depth;
    u64 system_uptime;
} ipc_system_stats_t;

// Function declarations
void init_ipc_system(void);
u32 ipc_create_queue(u32 pid, u32 max_messages);
u32 ipc_delete_queue(u32 queue_id);
u32 ipc_send_message(u32 sender_pid, u32 receiver_pid, u32 message_type, void *data, u32 data_size);
u32 ipc_receive_message(u32 receiver_pid, ipc_message_t *message);
void ipc_cleanup_process(u32 pid);
void ipc_print_system_stats(void);

// NEW: Enhanced IPC functions
u32 ipc_send_with_priority(u32 sender_pid, u32 receiver_pid, u32 message_type, void *data, u32 data_size, u32 priority);
u32 ipc_receive_with_timeout(u32 receiver_pid, ipc_message_t *message, u32 timeout_ms);
u32 ipc_broadcast_message(u32 sender_pid, u32 message_type, void *data, u32 data_size);
u32 ipc_get_system_stats(ipc_system_stats_t *stats);
void ipc_set_process_priority(u32 pid, u32 priority);

// System call wrappers
u32 sys_ipc_send(u32 receiver_pid, u32 message_type, void *data, u32 data_size);
u32 sys_ipc_receive(ipc_message_t *message);
u32 sys_ipc_create_queue(u32 max_messages);
u32 sys_ipc_delete_queue(u32 queue_id);

// NEW: Enhanced system call wrappers
u32 sys_ipc_send_priority(u32 receiver_pid, u32 message_type, void *data, u32 data_size, u32 priority);
u32 sys_ipc_receive_timeout(ipc_message_t *message, u32 timeout);
u32 sys_ipc_broadcast(u32 message_type, void *data, u32 data_size);
u32 sys_ipc_get_stats(ipc_system_stats_t *stats);

#endif // IPC_H 