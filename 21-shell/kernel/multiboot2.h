#ifndef MULTIBOOT2_H
#define MULTIBOOT2_H

#include "../cpu/types.h"

// Multiboot2 header magic number
#define MULTIBOOT2_HEADER_MAGIC 0xE85250D6

// Multiboot2 architecture
#define MULTIBOOT2_ARCHITECTURE_I386 0x00000000

// Multiboot2 header flags
#define MULTIBOOT2_HEADER_FLAGS 0x00000000

// Multiboot2 header checksum
#define MULTIBOOT2_HEADER_CHECKSUM 0x100000000 - (MULTIBOOT2_HEADER_MAGIC + MULTIBOOT2_ARCHITECTURE_I386 + MULTIBOOT2_HEADER_FLAGS)

// Multiboot2 info structure
typedef struct {
    u32 total_size;
    u32 reserved;
} multiboot2_info_t;

// Multiboot2 tag types
#define MULTIBOOT2_TAG_TYPE_END 0
#define MULTIBOOT2_TAG_TYPE_CMDLINE 1
#define MULTIBOOT2_TAG_TYPE_BOOT_LOADER_NAME 2
#define MULTIBOOT2_TAG_TYPE_MODULE 3
#define MULTIBOOT2_TAG_TYPE_BASIC_MEMINFO 4
#define MULTIBOOT2_TAG_TYPE_BOOTDEV 5
#define MULTIBOOT2_TAG_TYPE_MMAP 6
#define MULTIBOOT2_TAG_TYPE_VBE 7
#define MULTIBOOT2_TAG_TYPE_FRAMEBUFFER 8
#define MULTIBOOT2_TAG_TYPE_ELF_SECTIONS 9
#define MULTIBOOT2_TAG_TYPE_APM 10
#define MULTIBOOT2_TAG_TYPE_EFI32 11
#define MULTIBOOT2_TAG_TYPE_EFI64 12
#define MULTIBOOT2_TAG_TYPE_SMBIOS 13
#define MULTIBOOT2_TAG_TYPE_ACPI_OLD 14
#define MULTIBOOT2_TAG_TYPE_ACPI_NEW 15
#define MULTIBOOT2_TAG_TYPE_NETWORK 16
#define MULTIBOOT2_TAG_TYPE_EFI_MMAP 17
#define MULTIBOOT2_TAG_TYPE_EFI_BS 18
#define MULTIBOOT2_TAG_TYPE_EFI32_IH 19
#define MULTIBOOT2_TAG_TYPE_EFI64_IH 20
#define MULTIBOOT2_TAG_TYPE_LOAD_BASE_ADDR 21

// Multiboot2 tag structure
typedef struct {
    u32 type;
    u32 size;
} multiboot2_tag_t;

// Multiboot2 memory map entry
typedef struct {
    u64 addr;
    u64 len;
    u32 type;
    u32 zero;
} multiboot2_memory_map_t;

// External multiboot2 info
extern u32 multiboot2_info;

#endif 