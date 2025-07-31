#include "segment_protection.h"
#include "../drivers/screen.h"
#include "../libc/string.h"

// Global segment protection management
segment_protection_t segment_protections[MAX_SEGMENT_PROTECTIONS];
int segment_protection_count = 0;

// Initialize segment protection
void init_segment_protection(void) {
    // Clear all segment protections
    memset(segment_protections, 0, sizeof(segment_protections));
    segment_protection_count = 0;
    
    kprint("Segment protection initialized\n");
}

// Allocate a segment protection
int allocate_segment_protection(u16 selector, u32 base, u32 limit, u8 privilege, u8 permissions) {
    if (segment_protection_count >= MAX_SEGMENT_PROTECTIONS) {
        kprint("Error: Maximum segment protections reached\n");
        return -1;
    }
    
    // Check for overlapping selectors
    for (int i = 0; i < segment_protection_count; i++) {
        if (segment_protections[i].selector == selector) {
            kprint("Error: Segment selector already exists\n");
            return -1;
        }
    }
    
    segment_protection_t *protection = &segment_protections[segment_protection_count++];
    protection->selector = selector;
    protection->base = base;
    protection->limit = limit;
    protection->privilege = privilege;
    protection->permissions = permissions;
    
    kprint("Segment protection allocated: ");
    char sel_str[10];
    int_to_ascii(selector, sel_str);
    kprint(sel_str);
    kprint(" (Privilege: ");
    int_to_ascii(privilege, sel_str);
    kprint(sel_str);
    kprint(")\n");
    
    return segment_protection_count - 1;
}

// Check segment access permissions
int check_segment_access(u16 selector, u8 process_privilege, u8 access_type) {
    for (int i = 0; i < segment_protection_count; i++) {
        if (segment_protections[i].selector == selector) {
            // Check privilege level
            if (process_privilege < segment_protections[i].privilege) {
                return 0; // Access denied - insufficient privilege
            }
            
            // Check if access type is allowed
            return (segment_protections[i].permissions & access_type) == access_type;
        }
    }
    
    // Access denied - segment not found
    return 0;
}

// Find segment protection by selector
int find_segment_protection(u16 selector) {
    for (int i = 0; i < segment_protection_count; i++) {
        if (segment_protections[i].selector == selector) {
            return i;
        }
    }
    return -1;
}

// Free a segment protection
void free_segment_protection(u8 protection_id) {
    if (protection_id < segment_protection_count) {
        segment_protections[protection_id].selector = 0;
        kprint("Segment protection freed: ");
        char id_str[5];
        int_to_ascii(protection_id, id_str);
        kprint(id_str);
        kprint("\n");
    }
}

// Print all segment protections
void print_segment_protections(void) {
    kprint("Segment Protections:\n");
    for (int i = 0; i < segment_protection_count; i++) {
        if (segment_protections[i].selector != 0) {
            char info[50];
            int_to_ascii(i, info);
            kprint("Protection ");
            kprint(info);
            kprint(": Selector ");
            
            int_to_ascii(segment_protections[i].selector, info);
            kprint(info);
            kprint(" (Privilege: ");
            
            int_to_ascii(segment_protections[i].privilege, info);
            kprint(info);
            kprint(")\n");
        }
    }
}

// Handle segment violations
void segment_violation_handler(u16 selector, u8 process_privilege, u8 access_type) {
    kprint("Segment Violation! Selector: ");
    char sel_str[10];
    int_to_ascii(selector, sel_str);
    kprint(sel_str);
    kprint(" Process Privilege: ");
    int_to_ascii(process_privilege, sel_str);
    kprint(sel_str);
    kprint(" Access: ");
    int_to_ascii(access_type, sel_str);
    kprint(sel_str);
    kprint("\n");
    
    // Find the segment that caused the violation
    int protection_id = find_segment_protection(selector);
    if (protection_id != -1) {
        kprint("Violation in segment protection: ");
        int_to_ascii(protection_id, sel_str);
        kprint(sel_str);
        kprint("\n");
    } else {
        kprint("Selector not in any segment protection\n");
    }
}

// Switch segment privilege level
void switch_segment_privilege(u8 new_privilege) {
    kprint("Switching to privilege level: ");
    char priv_str[5];
    int_to_ascii(new_privilege, priv_str);
    kprint(priv_str);
    kprint("\n");
    
    // In a real implementation, this would modify the current privilege level
    // For now, we just log the switch
} 