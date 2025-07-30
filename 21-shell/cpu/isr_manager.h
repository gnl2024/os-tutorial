#ifndef ISR_MANAGER_H
#define ISR_MANAGER_H

#include "../cpu/types.h"

// ISR handler function pointer type
typedef void (*isr_handler_t)(u32 interrupt_number, u32 error_code);

// ISR manager functions
void isr_manager_init(void);
void register_isr_handler(u8 interrupt_number, isr_handler_t handler);
isr_handler_t get_isr_handler(u8 interrupt_number);
void isr_dispatcher(u32 interrupt_number, u32 error_code);
void irq_dispatcher(u32 irq_number);

// Specific handlers
void timer_isr_handler(u32 interrupt_number, u32 error_code);
void keyboard_isr_handler(u32 interrupt_number, u32 error_code);
void init_specific_handlers(void);

#endif // ISR_MANAGER_H 