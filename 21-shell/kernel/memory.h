#ifndef MEMORY_H
#define MEMORY_H

#include "../cpu/types.h"

// Memory region types
#define MEMORY_TYPE_CODE  0
#define MEMORY_TYPE_DATA  1
#define MEMORY_TYPE_STACK 2
#define MEMORY_TYPE_HEAP  3

// Memory permissions
#define PERMISSION_READ    0x01
#define PERMISSION_WRITE   0x02
#define PERMISSION_EXECUTE 0x04

// Memory region structure
typedef struct {
    u32 start;
    u32 end;
    int permissions;  // READ, WRITE, EXECUTE
    int process_id;   // Owner process
    int type;         // CODE, DATA, STACK, HEAP
    int active;       // 1 if active, 0 if freed
} memory_region_t;

// Memory management constants
#define MAX_MEMORY_REGIONS 64
#define MEMORY_REGION_SIZE 0x1000  // 4KB default size

// Global memory region management
extern memory_region_t memory_regions[MAX_MEMORY_REGIONS];
extern int region_count;

// Function declarations
int allocate_memory_region(u32 start, u32 size, int permissions, int process_id, int type);
int check_memory_access(u32 address, int process_id, int access_type);
void free_memory_region(int region_id);
int find_memory_region(u32 address);
void init_memory_regions(void);
void print_memory_regions(void);

#endif // MEMORY_H 