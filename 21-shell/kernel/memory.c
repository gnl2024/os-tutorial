#include "memory.h"
#include "../drivers/screen.h"
#include "../libc/string.h"

// Global memory region management
memory_region_t memory_regions[MAX_MEMORY_REGIONS];
int region_count = 0;

// Initialize memory regions
void init_memory_regions(void) {
    // Clear all memory regions
    memset(memory_regions, 0, sizeof(memory_regions));
    region_count = 0;
    
    // Allocate kernel memory regions
    allocate_memory_region(0x00000000, 0x00010000, 
                          PERMISSION_READ | PERMISSION_WRITE | PERMISSION_EXECUTE, 
                          0, MEMORY_TYPE_CODE);  // Kernel code (0x00000000-0x00010000)
    
    allocate_memory_region(0x00010000, 0x00010000, 
                          PERMISSION_READ | PERMISSION_WRITE, 
                          0, MEMORY_TYPE_STACK);  // Kernel stack (0x00010000-0x00020000)
    
    allocate_memory_region(0x00020000, 0x00010000, 
                          PERMISSION_READ | PERMISSION_WRITE, 
                          0, MEMORY_TYPE_HEAP);  // Kernel heap (0x00020000-0x00030000)
    
    kprint("Memory regions initialized\n");
}

// Allocate a memory region
int allocate_memory_region(u32 start, u32 size, int permissions, int process_id, int type) {
    if (region_count >= MAX_MEMORY_REGIONS) {
        kprint("Error: Maximum memory regions reached\n");
        return -1;
    }
    
    // Check for overlapping regions
    for (int i = 0; i < region_count; i++) {
        if (memory_regions[i].active) {
            if ((start < memory_regions[i].end) && 
                (start + size > memory_regions[i].start)) {
                kprint("Error: Memory region overlap\n");
                return -1;
            }
        }
    }
    
    memory_region_t *region = &memory_regions[region_count++];
    region->start = start;
    region->end = start + size;
    region->permissions = permissions;
    region->process_id = process_id;
    region->type = type;
    region->active = 1;
    
    kprint("Memory region allocated: ");
    char addr_str[10];
    int_to_ascii(start, addr_str);
    kprint(addr_str);
    kprint(" - ");
    int_to_ascii(start + size, addr_str);
    kprint(addr_str);
    kprint(" (PID: ");
    int_to_ascii(process_id, addr_str);
    kprint(addr_str);
    kprint(")\n");
    
    return region_count - 1;
}

// Check memory access permissions
int check_memory_access(u32 address, int process_id, int access_type) {
    for (int i = 0; i < region_count; i++) {
        if (memory_regions[i].active && 
            address >= memory_regions[i].start && 
            address < memory_regions[i].end) {
            
            // Check if process owns this region
            if (memory_regions[i].process_id == process_id) {
                // Check if access type is allowed
                return (memory_regions[i].permissions & access_type) == access_type;
            } else {
                // Access denied - wrong process
                return 0;
            }
        }
    }
    
    // Access denied - no region found
    return 0;
}

// Trigger a page fault for memory access violation
void trigger_page_fault(u32 address) {
    // This will cause a page fault by accessing invalid memory
    // The page fault handler will catch this and handle it appropriately
    volatile u32 *ptr = (u32*)address;
    *ptr = 0; // This will trigger a page fault if address is invalid
}

// Find memory region by address
int find_memory_region(u32 address) {
    for (int i = 0; i < region_count; i++) {
        if (memory_regions[i].active && 
            address >= memory_regions[i].start && 
            address < memory_regions[i].end) {
            return i;
        }
    }
    return -1;  // Region not found
}

// Free a memory region
void free_memory_region(int region_id) {
    if (region_id >= 0 && region_id < region_count) {
        memory_regions[region_id].active = 0;
        kprint("Memory region freed: ");
        char addr_str[10];
        int_to_ascii(memory_regions[region_id].start, addr_str);
        kprint(addr_str);
        kprint("\n");
    }
}

// Print all memory regions (for debugging)
void print_memory_regions(void) {
    kprint("Memory Regions:\n");
    for (int i = 0; i < region_count; i++) {
        if (memory_regions[i].active) {
            char addr_str[10];
            
            kprint("Region ");
            int_to_ascii(i, addr_str);
            kprint(addr_str);
            kprint(": ");
            
            int_to_ascii(memory_regions[i].start, addr_str);
            kprint(addr_str);
            kprint(" - ");
            
            int_to_ascii(memory_regions[i].end, addr_str);
            kprint(addr_str);
            kprint(" (PID: ");
            
            int_to_ascii(memory_regions[i].process_id, addr_str);
            kprint(addr_str);
            kprint(")\n");
        }
    }
} 