#!/bin/bash

# GDB Debug Script for WISP-BB Development
# Automatically connects to QEMU for debugging

echo "=== GDB Debug Script ==="
echo "Connecting to QEMU for debugging..."
echo

# Check if kernel.elf exists
if [ ! -f "kernel.elf" ]; then
    echo "❌ Error: kernel.elf not found!"
    echo "Please run 'make kernel.elf' first."
    exit 1
fi

# Check if QEMU is running
echo "Checking if QEMU is running on port 1234..."
if ! nc -z localhost 1234 2>/dev/null; then
    echo "❌ Error: QEMU is not running on port 1234!"
    echo "Please start QEMU in debug mode first:"
    echo "  ./run_qemu_debug.sh"
    echo
    echo "Or run QEMU with debug flags:"
    echo "  qemu-system-i386 -s -S -fda os-image.bin -serial stdio"
    exit 1
fi

echo "✅ QEMU found on port 1234"
echo

# GDB configuration
GDB_COMMANDS=""

# Connect to QEMU
GDB_COMMANDS="$GDB_COMMANDS target remote localhost:1234"

# Load symbols
GDB_COMMANDS="$GDB_COMMANDS symbol-file kernel.elf"

# Set architecture
GDB_COMMANDS="$GDB_COMMANDS set architecture i386"

# Useful breakpoints for development
GDB_COMMANDS="$GDB_COMMANDS break kernel_main"
GDB_COMMANDS="$GDB_COMMANDS break gdt_init"
GDB_COMMANDS="$GDB_COMMANDS break init_memory_regions"
GDB_COMMANDS="$GDB_COMMANDS break init_process_manager"

# Continue execution
GDB_COMMANDS="$GDB_COMMANDS continue"

echo "Starting GDB with commands:"
echo "$GDB_COMMANDS"
echo

# Start GDB
i386-elf-gdb -ex "$GDB_COMMANDS"

echo
echo "GDB session ended." 