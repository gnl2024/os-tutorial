#!/bin/bash

# QEMU Debug Run Script for WISP-BB Development
# Optimized for debugging and development

echo "=== QEMU Debug Run Script ==="
echo "Starting WISP-BB with debug-optimized QEMU configuration..."
echo

# Check if OS image exists
if [ ! -f "os-image.bin" ]; then
    echo "❌ Error: os-image.bin not found!"
    echo "Please run 'make' first to build the OS."
    exit 1
fi

# Check if kernel.elf exists for debugging
if [ ! -f "kernel.elf" ]; then
    echo "⚠️  Warning: kernel.elf not found!"
    echo "Run 'make kernel.elf' for debugging support."
fi

# QEMU debug flags
QEMU_FLAGS=""

# Basic configuration
QEMU_FLAGS="$QEMU_FLAGS -cpu 486"                       # Use 486 for compatibility
QEMU_FLAGS="$QEMU_FLAGS -m 64"                          # 64MB RAM (sufficient for debugging)
QEMU_FLAGS="$QEMU_FLAGS -smp 1"                         # Single CPU

# Display configuration
QEMU_FLAGS="$QEMU_FLAGS -display gtk"                   # GTK display
QEMU_FLAGS="$QEMU_FLAGS -vga std"                       # Standard VGA

# Input configuration
QEMU_FLAGS="$QEMU_FLAGS -k en-us"                       # US keyboard layout

# Storage configuration
QEMU_FLAGS="$QEMU_FLAGS -fda os-image.bin"             # Floppy disk image

# Debugging and monitoring
QEMU_FLAGS="$QEMU_FLAGS -serial stdio"                  # Serial output to terminal
QEMU_FLAGS="$QEMU_FLAGS -monitor stdio"                 # Monitor on stdio for debugging

# Debug-specific flags
QEMU_FLAGS="$QEMU_FLAGS -s"                             # Start GDB server on port 1234
QEMU_FLAGS="$QEMU_FLAGS -S"                             # Stop at startup (wait for GDB)

# Logging and debugging
QEMU_FLAGS="$QEMU_FLAGS -d guest_errors,int,unimp"      # Log errors, interrupts, unimplemented
QEMU_FLAGS="$QEMU_FLAGS -D qemu_debug.log"              # Debug log file

# Performance (debug mode)
QEMU_FLAGS="$QEMU_FLAGS -no-reboot"                     # Don't reboot on exit
QEMU_FLAGS="$QEMU_FLAGS -no-shutdown"                   # Don't shutdown on exit

echo "QEMU Debug Flags: $QEMU_FLAGS"
echo

echo "Starting QEMU in debug mode..."
echo "The system will pause at startup waiting for GDB connection."
echo
echo "To connect with GDB:"
echo "  gdb -ex 'target remote localhost:1234' -ex 'symbol-file kernel.elf'"
echo
echo "Or run the debug script: ./debug_qemu.sh"
echo

# Run QEMU with debug flags
qemu-system-i386 $QEMU_FLAGS

echo
echo "QEMU debug session ended."
echo "Check qemu_debug.log for detailed debugging information." 