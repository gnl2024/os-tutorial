#ifndef MPU_H
#define MPU_H

#include "../cpu/types.h"

// MPU region structure
typedef struct {
    u32 start;
    u32 end;
    u8 permissions;
    u8 process_id;
    u8 region_id;
    u8 active;
} mpu_region_t;

// MPU permissions
#define MPU_PERMISSION_READ    0x01
#define MPU_PERMISSION_WRITE   0x02
#define MPU_PERMISSION_EXECUTE 0x04

// MPU region types
#define MPU_REGION_CODE    0
#define MPU_REGION_DATA    1
#define MPU_REGION_STACK   2
#define MPU_REGION_HEAP    3

// MPU constants
#define MAX_MPU_REGIONS 8
#define MPU_REGION_SIZE 0x1000  // 4KB default size

// Global MPU management
extern mpu_region_t mpu_regions[MAX_MPU_REGIONS];
extern int mpu_region_count;

// Function declarations
int allocate_mpu_region(u32 start, u32 size, u8 permissions, u8 process_id, u8 type);
int check_mpu_access(u32 address, u8 process_id, u8 access_type);
void free_mpu_region(u8 region_id);
int find_mpu_region(u32 address);
void init_mpu(void);
void print_mpu_regions(void);
void mpu_violation_handler(u32 address, u8 process_id, u8 access_type);

#endif // MPU_H 