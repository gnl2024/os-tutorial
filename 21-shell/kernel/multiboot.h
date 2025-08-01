#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include "../cpu/types.h"

// Multiboot header magic number
#define MULTIBOOT_HEADER_MAGIC 0x1BADB002

// Multiboot header flags
#define MULTIBOOT_HEADER_FLAGS 0x00000003

// Multiboot info structure
typedef struct {
    u32 flags;
    u32 mem_lower;
    u32 mem_upper;
    u32 boot_device;
    u32 cmdline;
    u32 mods_count;
    u32 mods_addr;
    u32 syms[4];
    u32 mmap_length;
    u32 mmap_addr;
    u32 drives_length;
    u32 drives_addr;
    u32 config_table;
    u32 boot_loader_name;
    u32 apm_table;
    u32 vbe_control_info;
    u32 vbe_mode_info;
    u16 vbe_mode;
    u16 vbe_interface_seg;
    u16 vbe_interface_off;
    u16 vbe_interface_len;
} multiboot_info_t;

// Multiboot info flags
#define MULTIBOOT_INFO_MEMORY 0x00000001
#define MULTIBOOT_INFO_BOOTDEV 0x00000002
#define MULTIBOOT_INFO_CMDLINE 0x00000004
#define MULTIBOOT_INFO_MODS 0x00000008
#define MULTIBOOT_INFO_SYMS 0x00000010
#define MULTIBOOT_INFO_MMAP 0x00000020
#define MULTIBOOT_INFO_DRIVES 0x00000040
#define MULTIBOOT_INFO_CONFIG_TABLE 0x00000080
#define MULTIBOOT_INFO_BOOT_LOADER_NAME 0x00000100
#define MULTIBOOT_INFO_APM_TABLE 0x00000200
#define MULTIBOOT_INFO_VBE 0x00000400

// External multiboot info
extern u32 multiboot_info;

#endif 