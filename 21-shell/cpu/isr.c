#include "isr.h"
#include "idt.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../libc/string.h"
#include "timer.h"
#include "ports.h"
#include "isr_manager.h"
#include "../kernel/memory.h"
#include "../kernel/process.h"
#include "../kernel/mpu.h"

isr_t interrupt_handlers[256];

// Function declarations
void page_fault_handler(registers_t r);

/* Can't do this with a loop because we need the address
 * of the function names */
void isr_install() {
    set_idt_gate(0, (u32)isr0);
    set_idt_gate(1, (u32)isr1);
    set_idt_gate(2, (u32)isr2);
    set_idt_gate(3, (u32)isr3);
    set_idt_gate(4, (u32)isr4);
    set_idt_gate(5, (u32)isr5);
    set_idt_gate(6, (u32)isr6);
    set_idt_gate(7, (u32)isr7);
    set_idt_gate(8, (u32)isr8);
    set_idt_gate(9, (u32)isr9);
    set_idt_gate(10, (u32)isr10);
    set_idt_gate(11, (u32)isr11);
    set_idt_gate(12, (u32)isr12);
    set_idt_gate(13, (u32)isr13);
    set_idt_gate(14, (u32)isr14);
    set_idt_gate(15, (u32)isr15);
    
    // Register page fault handler
    register_interrupt_handler(14, page_fault_handler);
    set_idt_gate(16, (u32)isr16);
    set_idt_gate(17, (u32)isr17);
    set_idt_gate(18, (u32)isr18);
    set_idt_gate(19, (u32)isr19);
    set_idt_gate(20, (u32)isr20);
    set_idt_gate(21, (u32)isr21);
    set_idt_gate(22, (u32)isr22);
    set_idt_gate(23, (u32)isr23);
    set_idt_gate(24, (u32)isr24);
    set_idt_gate(25, (u32)isr25);
    set_idt_gate(26, (u32)isr26);
    set_idt_gate(27, (u32)isr27);
    set_idt_gate(28, (u32)isr28);
    set_idt_gate(29, (u32)isr29);
    set_idt_gate(30, (u32)isr30);
    set_idt_gate(31, (u32)isr31);

    // Remap the PIC
    port_byte_out(0x20, 0x11);
    port_byte_out(0xA0, 0x11);
    port_byte_out(0x21, 0x20);
    port_byte_out(0xA1, 0x28);
    port_byte_out(0x21, 0x04);
    port_byte_out(0xA1, 0x02);
    port_byte_out(0x21, 0x01);
    port_byte_out(0xA1, 0x01);
    port_byte_out(0x21, 0x0);
    port_byte_out(0xA1, 0x0); 

    // Install the IRQs
    set_idt_gate(32, (u32)irq0);
    set_idt_gate(33, (u32)irq1);
    set_idt_gate(34, (u32)irq2);
    set_idt_gate(35, (u32)irq3);
    set_idt_gate(36, (u32)irq4);
    set_idt_gate(37, (u32)irq5);
    set_idt_gate(38, (u32)irq6);
    set_idt_gate(39, (u32)irq7);
    set_idt_gate(40, (u32)irq8);
    set_idt_gate(41, (u32)irq9);
    set_idt_gate(42, (u32)irq10);
    set_idt_gate(43, (u32)irq11);
    set_idt_gate(44, (u32)irq12);
    set_idt_gate(45, (u32)irq13);
    set_idt_gate(46, (u32)irq14);
    set_idt_gate(47, (u32)irq15);

    set_idt(); // Load with ASM
}

/* To print the message which defines every exception */
char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void isr_handler(registers_t r) {
    kprint("received interrupt: ");
    char s[3];
    int_to_ascii(r.int_no, s);
    kprint(s);
    kprint("\n");
    kprint(exception_messages[r.int_no]);
    kprint("\n");
}

// Page fault handler (interrupt 14)
void page_fault_handler(registers_t r) {
    (void)r; // Suppress unused parameter warning
    u32 fault_address;
    asm volatile("mov %%cr2, %0" : "=r" (fault_address));
    
    kprint("Page Fault! ( ");
    char addr_str[10];
    int_to_ascii(fault_address, addr_str);
    kprint(addr_str);
    kprint(" )\n");
    
    // Check if this is a memory access violation
    int current_pid = get_current_pid();
    if (current_pid >= 0) {
        // First check MPU regions (hardware-like protection)
        int mpu_region_id = find_mpu_region(fault_address);
        if (mpu_region_id != -1) {
            // Address is in MPU region, check permissions
            mpu_region_t *mpu_region = &mpu_regions[mpu_region_id];
            if (mpu_region->process_id != current_pid) {
                kprint("MPU violation - wrong process\n");
                mpu_violation_handler(fault_address, current_pid, MPU_PERMISSION_READ);
                terminate_process(current_pid);
            } else {
                kprint("MPU violation - insufficient permissions\n");
                mpu_violation_handler(fault_address, current_pid, MPU_PERMISSION_READ);
                terminate_process(current_pid);
            }
        } else {
            // Check traditional memory regions
            int region_id = find_memory_region(fault_address);
            if (region_id == -1) {
                kprint("Memory access violation - address not in any region\n");
                terminate_process(current_pid);
            } else {
                // Check if process has permission to access this region
                memory_region_t *region = &memory_regions[region_id];
                if (region->process_id != current_pid) {
                    kprint("Memory access violation - wrong process\n");
                    terminate_process(current_pid);
                } else {
                    kprint("Memory access violation - insufficient permissions\n");
                    terminate_process(current_pid);
                }
            }
        }
    } else {
        kprint("Page fault in kernel mode\n");
    }
    
    kprint("System halted due to page fault\n");
    asm volatile("cli");
    asm volatile("hlt");
}

void register_interrupt_handler(u8 n, isr_t handler) {
    interrupt_handlers[n] = handler;
}

void irq_handler(registers_t r) {
    /* After every interrupt we need to send an EOI to the PICs
     * or they will not send another interrupt again */
    if (r.int_no >= 40) port_byte_out(0xA0, 0x20); /* slave */
    port_byte_out(0x20, 0x20); /* master */

    /* Handle the interrupt in a more modular way */
    if (interrupt_handlers[r.int_no] != 0) {
        isr_t handler = interrupt_handlers[r.int_no];
        handler(r);
    }
}

void irq_install() {
    /* Mask IRQ 0 (timer) in PIC to prevent timer interrupts */
    u8 mask = port_byte_in(0x21); /* Read current mask */
    mask |= 0x01; /* Set bit 0 to mask IRQ 0 */
    port_byte_out(0x21, mask); /* Write back mask */
    
    /* Enable interruptions */
    asm volatile("sti");
    /* IRQ0: timer - temporarily disabled */
    // init_timer(50);
    /* IRQ1: keyboard */
    init_keyboard();
}
