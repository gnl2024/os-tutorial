#include "mem.h"
#include "function.h"

void memory_copy(u8 *source, u8 *dest, int nbytes) {
    int i;
    for (i = 0; i < nbytes; i++) {
        *(dest + i) = *(source + i);
    }
}

void memory_set(u8 *dest, u8 val, u32 len) {
    u8 *temp = (u8 *)dest;
    for ( ; len != 0; len--) *temp++ = val;
}

/* This should be computed at link time, but a lot of newer systems
 * don't have a working "free" -- not even the kernel. We'll use
 * this as a simple (and non-unique) solution. */
u32 free_mem_addr = 0x30000;  /* Start after kernel regions */

/* Memory statistics tracking */
static u32 total_allocated = 0;
static u32 allocation_count = 0;
static u32 max_allocation = 0;

/* Implementation is just a pointer to some free memory which
 * keeps growing */
u32 kmalloc(u32 size, int align, u32 *phys_addr) {
    /* Pages are aligned to 4K, or 0x1000 */
    if (align == 1 && (free_mem_addr & 0xFFFFF000)) {
        free_mem_addr &= 0xFFFFF000;
        free_mem_addr += 0x1000;
    }
    /* Save also the physical address */
    if (phys_addr) *phys_addr = free_mem_addr;

    u32 ret = free_mem_addr;
    free_mem_addr += size; /* Remember to increment the pointer */

    /* Track memory statistics */
    total_allocated += size;
    allocation_count++;
    if (size > max_allocation) {
        max_allocation = size;
    }

    return ret;
}

/* Get memory statistics */
void get_memory_stats(u32 *total, u32 *count, u32 *max) {
    if (total) *total = total_allocated;
    if (count) *count = allocation_count;
    if (max) *max = max_allocation;
}

/* Reset memory statistics */
void reset_memory_stats(void) {
    total_allocated = 0;
    allocation_count = 0;
    max_allocation = 0;
}
