#ifndef IPC_H
#define IPC_H

#include "../cpu/types.h"

// IPC Message Types
#define IPC_MSG_TYPE_DATA      0x01
#define IPC_MSG_TYPE_CONTROL   0x02
#define IPC_MSG_TYPE_SIGNAL   0x03
#define IPC_MSG_TYPE_REQUEST   0x04
#define IPC_MSG_TYPE_RESPONSE 0x05

// IPC Message Status
#define IPC_MSG_STATUS_PENDING   0x01
#define IPC_MSG_STATUS_DELIVERED 0x02
#define IPC_MSG_STATUS_READ      0x03
#define IPC_MSG_STATUS_ERROR     0x04

// IPC Queue Status
#define IPC_QUEUE_EMPTY     0x00
#define IPC_QUEUE_AVAILABLE 0x01
#define IPC_QUEUE_FULL      0x02

// IPC System Call Numbers
#define SYS_IPC_SEND        0x30
#define SYS_IPC_RECEIVE     0x31
#define SYS_IPC_CREATE_QUEUE 0x32
#define SYS_IPC_DELETE_QUEUE 0x33
#define SYS_IPC_GET_QUEUE_STATUS 0x34

// Maximum message size
#define IPC_MAX_MESSAGE_SIZE 256

// Maximum number of queues per process
#define IPC_MAX_QUEUES_PER_PROCESS 8

// IPC Message Structure
typedef struct {
    u32 sender_pid;
    u32 receiver_pid;
    u32 message_type;
    u32 message_id;
    u32 data_size;
    u8 data[IPC_MAX_MESSAGE_SIZE];
    u32 status;
    u32 timestamp;
} ipc_message_t;

// IPC Queue Structure
typedef struct {
    u32 queue_id;
    u32 owner_pid;
    u32 max_messages;
    u32 current_count;
    u32 head;
    u32 tail;
    ipc_message_t messages[16]; // Circular buffer
    u32 status;
} ipc_queue_t;

// IPC Process Structure
typedef struct {
    u32 pid;
    u32 queue_count;
    ipc_queue_t queues[IPC_MAX_QUEUES_PER_PROCESS];
    u32 pending_messages;
    u32 total_messages_sent;
    u32 total_messages_received;
} ipc_process_t;

// IPC System Functions
void init_ipc_system(void);
u32 ipc_create_queue(u32 pid, u32 max_messages);
u32 ipc_delete_queue(u32 queue_id);
u32 ipc_send_message(u32 sender_pid, u32 receiver_pid, 
                     u32 message_type, void *data, u32 data_size);
u32 ipc_receive_message(u32 receiver_pid, ipc_message_t *message);
u32 ipc_get_queue_status(u32 queue_id);
void ipc_cleanup_process(u32 pid);

// IPC System Call Handlers
u32 sys_ipc_send(u32 receiver_pid, u32 message_type, 
                  void *data, u32 data_size);
u32 sys_ipc_receive(ipc_message_t *message);
u32 sys_ipc_create_queue(u32 max_messages);
u32 sys_ipc_delete_queue(u32 queue_id);
u32 sys_ipc_get_queue_status(u32 queue_id);

// IPC Debug Functions
void ipc_print_queue_status(u32 queue_id);
void ipc_print_process_stats(u32 pid);
void ipc_print_system_stats(void);

#endif // IPC_H 