#ifndef SEGMENT_PROTECTION_H
#define SEGMENT_PROTECTION_H

#include "../cpu/types.h"

// Segment protection structure
typedef struct {
    u16 selector;
    u32 base;
    u32 limit;
    u8 privilege;
    u8 permissions;
} segment_protection_t;

// Privilege levels
#define PRIVILEGE_RING_0 0  // Kernel mode
#define PRIVILEGE_RING_3 3  // User mode

// Segment permissions
#define SEGMENT_PERMISSION_READ    0x01
#define SEGMENT_PERMISSION_WRITE   0x02
#define SEGMENT_PERMISSION_EXECUTE 0x04

// Segment types
#define SEGMENT_TYPE_CODE 0
#define SEGMENT_TYPE_DATA 1
#define SEGMENT_TYPE_STACK 2

// Segment protection constants
#define MAX_SEGMENT_PROTECTIONS 16
#define SEGMENT_PROTECTION_SIZE 0x1000  // 4KB default size

// Global segment protection management
extern segment_protection_t segment_protections[MAX_SEGMENT_PROTECTIONS];
extern int segment_protection_count;

// Function declarations
int allocate_segment_protection(u16 selector, u32 base, u32 limit, u8 privilege, u8 permissions);
int check_segment_access(u16 selector, u8 process_privilege, u8 access_type);
void free_segment_protection(u8 protection_id);
int find_segment_protection(u16 selector);
void init_segment_protection(void);
void print_segment_protections(void);
void segment_violation_handler(u16 selector, u8 process_privilege, u8 access_type);
void switch_segment_privilege(u8 new_privilege);

#endif // SEGMENT_PROTECTION_H 