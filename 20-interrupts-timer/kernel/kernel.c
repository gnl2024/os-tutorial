#include "../cpu/isr.h"
#include "../cpu/timer.h"
#include "../drivers/keyboard.h"
#include "../drivers/screen.h"

void main() {
    isr_install();

    asm volatile("sti");
    init_timer(50);
    /* Comment out the timer IRQ handler to read
     * the keyboard IRQs easier */
    init_keyboard();
    
    /* Welcome message */
    kprint("Welcome to Meaty Skeleton OS!\n");
    kprint("=============================\n");
    kprint("System initialized successfully.\n");
    kprint("Timer: Active (50Hz)\n");
    kprint("Keyboard: Active\n");
    kprint("VGA: Active\n");
    kprint("\n");
    kprint("Try typing on your keyboard!\n");
    kprint("Each keypress will show the scancode and letter.\n");
    kprint("Press Ctrl+Alt+Del to exit QEMU.\n");
    kprint("\n");
}
