#include "mpu.h"
#include "../drivers/screen.h"
#include "../libc/string.h"

// Global MPU management
mpu_region_t mpu_regions[MAX_MPU_REGIONS];
int mpu_region_count = 0;

// Initialize MPU
void init_mpu(void) {
    // Clear all MPU regions
    memset(mpu_regions, 0, sizeof(mpu_regions));
    mpu_region_count = 0;
    
    kprint("MPU initialized\n");
}

// Allocate an MPU region
int allocate_mpu_region(u32 start, u32 size, u8 permissions, u8 process_id, u8 type) {
    (void)type; // Suppress unused parameter warning
    if (mpu_region_count >= MAX_MPU_REGIONS) {
        kprint("Error: Maximum MPU regions reached\n");
        return -1;
    }
    
    // Check for overlapping regions
    for (int i = 0; i < mpu_region_count; i++) {
        if (mpu_regions[i].active) {
            if ((start < mpu_regions[i].end) &&
                (start + size > mpu_regions[i].start)) {
                kprint("Error: MPU region overlap\n");
                return -1;
            }
        }
    }
    
    mpu_region_t *region = &mpu_regions[mpu_region_count++];
    region->start = start;
    region->end = start + size;
    region->permissions = permissions;
    region->process_id = process_id;
    region->region_id = mpu_region_count - 1;
    region->active = 1;
    
    kprint("MPU region allocated: ");
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
    
    return region->region_id;
}

// Check MPU access permissions
int check_mpu_access(u32 address, u8 process_id, u8 access_type) {
    for (int i = 0; i < mpu_region_count; i++) {
        if (mpu_regions[i].active && 
            address >= mpu_regions[i].start && 
            address < mpu_regions[i].end) {
            
            // Check if process owns this region
            if (mpu_regions[i].process_id == process_id) {
                // Check if access type is allowed
                return (mpu_regions[i].permissions & access_type) == access_type;
            } else {
                // Access denied - wrong process
                return 0;
            }
        }
    }
    
    // Access denied - no region found
    return 0;
}

// Find MPU region by address
int find_mpu_region(u32 address) {
    for (int i = 0; i < mpu_region_count; i++) {
        if (mpu_regions[i].active && 
            address >= mpu_regions[i].start && 
            address < mpu_regions[i].end) {
            return i;
        }
    }
    return -1;
}

// Free an MPU region
void free_mpu_region(u8 region_id) {
    if (region_id < mpu_region_count && mpu_regions[region_id].active) {
        mpu_regions[region_id].active = 0;
        kprint("MPU region freed: ");
        char id_str[5];
        int_to_ascii(region_id, id_str);
        kprint(id_str);
        kprint("\n");
    }
}

// Print all MPU regions
void print_mpu_regions(void) {
    kprint("MPU Regions:\n");
    for (int i = 0; i < mpu_region_count; i++) {
        if (mpu_regions[i].active) {
            char info[50];
            int_to_ascii(i, info);
            kprint("Region ");
            kprint(info);
            kprint(": ");
            
            int_to_ascii(mpu_regions[i].start, info);
            kprint(info);
            kprint(" - ");
            
            int_to_ascii(mpu_regions[i].end, info);
            kprint(info);
            kprint(" (PID: ");
            
            int_to_ascii(mpu_regions[i].process_id, info);
            kprint(info);
            kprint(")\n");
        }
    }
}

// Handle MPU violations
void mpu_violation_handler(u32 address, u8 process_id, u8 access_type) {
    kprint("MPU Violation! Address: ");
    char addr_str[10];
    int_to_ascii(address, addr_str);
    kprint(addr_str);
    kprint(" PID: ");
    int_to_ascii(process_id, addr_str);
    kprint(addr_str);
    kprint(" Access: ");
    int_to_ascii(access_type, addr_str);
    kprint(addr_str);
    kprint("\n");
    
    // Find the region that caused the violation
    int region_id = find_mpu_region(address);
    if (region_id != -1) {
        kprint("Violation in MPU region: ");
        int_to_ascii(region_id, addr_str);
        kprint(addr_str);
        kprint("\n");
    } else {
        kprint("Address not in any MPU region\n");
    }
} 