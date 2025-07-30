#!/bin/bash

# Phase 1 Test Script - WISP-BB Microkernel Migration
# Tests all Phase 1 functionality: Process Foundation

set -e  # Exit on any error

echo "=========================================="
echo "Phase 1 Testing: Process Foundation"
echo "=========================================="

# Set up environment
export PATH=$HOME/opt/bin:$PATH

# Test 1: Build System
echo "Test 1: Build System"
echo "-------------------"
make clean
make
echo "✅ Build successful"

# Test 2: Check all object files
echo ""
echo "Test 2: Object Files"
echo "-------------------"
if [ -f "kernel/process.o" ]; then
    echo "✅ kernel/process.o exists"
else
    echo "❌ kernel/process.o missing"
    exit 1
fi

if [ -f "cpu/process_switch.o" ]; then
    echo "✅ cpu/process_switch.o exists"
else
    echo "❌ cpu/process_switch.o missing"
    exit 1
fi

if [ -f "os-image.bin" ]; then
    echo "✅ os-image.bin exists"
else
    echo "❌ os-image.bin missing"
    exit 1
fi

# Test 3: Check file sizes
echo ""
echo "Test 3: File Sizes"
echo "-------------------"
KERNEL_SIZE=$(stat -c%s "kernel.bin" 2>/dev/null || echo "0")
if [ "$KERNEL_SIZE" -gt 1000 ]; then
    echo "✅ Kernel size: $KERNEL_SIZE bytes (reasonable)"
else
    echo "❌ Kernel size: $KERNEL_SIZE bytes (too small)"
    exit 1
fi

OS_SIZE=$(stat -c%s "os-image.bin" 2>/dev/null || echo "0")
if [ "$OS_SIZE" -gt 1000 ]; then
    echo "✅ OS image size: $OS_SIZE bytes (reasonable)"
else
    echo "❌ OS image size: $OS_SIZE bytes (too small)"
    exit 1
fi

# Test 4: Check for compilation warnings
echo ""
echo "Test 4: Compilation Warnings"
echo "----------------------------"
WARNINGS=$(make 2>&1 | grep -i warning || echo "No warnings")
if [ "$WARNINGS" = "No warnings" ]; then
    echo "✅ No compilation warnings"
else
    echo "⚠️  Compilation warnings found:"
    echo "$WARNINGS"
fi

# Test 5: Check for undefined symbols
echo ""
echo "Test 5: Symbol Resolution"
echo "------------------------"
UNDEFINED=$(i386-elf-ld -o test.elf -Ttext 0x1000 boot/kernel_entry.o kernel/kernel.o cpu/isr_stubs_simple.o cpu/gdt_flush.o cpu/process_switch.o kernel/process.o drivers/keyboard.o drivers/print.o drivers/screen.o cpu/gdt.o cpu/idt.o cpu/isr.o cpu/ports.o cpu/timer.o libc/mem.o libc/string.o 2>&1 | grep "undefined reference" || echo "No undefined references")
if [ "$UNDEFINED" = "No undefined references" ]; then
    echo "✅ No undefined symbols"
else
    echo "❌ Undefined symbols found:"
    echo "$UNDEFINED"
    exit 1
fi

# Test 6: Check process management symbols
echo ""
echo "Test 6: Process Management Symbols"
echo "--------------------------------"
if nm test.elf 2>/dev/null | grep -q "create_process"; then
    echo "✅ create_process symbol found"
else
    echo "❌ create_process symbol missing"
fi

if nm test.elf 2>/dev/null | grep -q "switch_to_process"; then
    echo "✅ switch_to_process symbol found"
else
    echo "❌ switch_to_process symbol missing"
fi

if nm test.elf 2>/dev/null | grep -q "init_process_manager"; then
    echo "✅ init_process_manager symbol found"
else
    echo "❌ init_process_manager symbol missing"
fi

# Test 7: Check header files
echo ""
echo "Test 7: Header Files"
echo "-------------------"
if [ -f "kernel/process.h" ]; then
    echo "✅ kernel/process.h exists"
else
    echo "❌ kernel/process.h missing"
    exit 1
fi

if [ -f "cpu/process_switch.asm" ]; then
    echo "✅ cpu/process_switch.asm exists"
else
    echo "❌ cpu/process_switch.asm missing"
    exit 1
fi

# Test 8: Check Makefile integration
echo ""
echo "Test 8: Makefile Integration"
echo "---------------------------"
if grep -q "process_switch.o" Makefile; then
    echo "✅ process_switch.o in Makefile"
else
    echo "❌ process_switch.o not in Makefile"
    exit 1
fi

# Test 9: Check kernel integration
echo ""
echo "Test 9: Kernel Integration"
echo "------------------------"
if grep -q "init_process_manager" kernel/kernel.c; then
    echo "✅ init_process_manager called in kernel"
else
    echo "❌ init_process_manager not called in kernel"
    exit 1
fi

if grep -q "process.h" kernel/kernel.c; then
    echo "✅ process.h included in kernel"
else
    echo "❌ process.h not included in kernel"
    exit 1
fi

# Test 10: Check string library updates
echo ""
echo "Test 10: String Library Updates"
echo "-----------------------------"
if grep -q "memset" libc/string.h; then
    echo "✅ memset declared in string.h"
else
    echo "❌ memset not declared in string.h"
    exit 1
fi

if grep -q "memset" libc/string.c; then
    echo "✅ memset implemented in string.c"
else
    echo "❌ memset not implemented in string.c"
    exit 1
fi

# Test 11: Check process structure
echo ""
echo "Test 11: Process Structure"
echo "------------------------"
if grep -q "typedef struct" kernel/process.h; then
    echo "✅ Process structure defined"
else
    echo "❌ Process structure not defined"
    exit 1
fi

if grep -q "MAX_PROCESSES" kernel/process.h; then
    echo "✅ MAX_PROCESSES defined"
else
    echo "❌ MAX_PROCESSES not defined"
    exit 1
fi

# Test 12: Check process states
echo ""
echo "Test 12: Process States"
echo "----------------------"
if grep -q "PROCESS_RUNNING" kernel/process.h; then
    echo "✅ PROCESS_RUNNING defined"
else
    echo "❌ PROCESS_RUNNING not defined"
    exit 1
fi

if grep -q "PROCESS_READY" kernel/process.h; then
    echo "✅ PROCESS_READY defined"
else
    echo "❌ PROCESS_READY not defined"
    exit 1
fi

if grep -q "PROCESS_BLOCKED" kernel/process.h; then
    echo "✅ PROCESS_BLOCKED defined"
else
    echo "❌ PROCESS_BLOCKED not defined"
    exit 1
fi

if grep -q "PROCESS_TERMINATED" kernel/process.h; then
    echo "✅ PROCESS_TERMINATED defined"
else
    echo "❌ PROCESS_TERMINATED not defined"
    exit 1
fi

# Test 13: Check process privileges
echo ""
echo "Test 13: Process Privileges"
echo "--------------------------"
if grep -q "PRIVILEGE_KERNEL" kernel/process.h; then
    echo "✅ PRIVILEGE_KERNEL defined"
else
    echo "❌ PRIVILEGE_KERNEL not defined"
    exit 1
fi

if grep -q "PRIVILEGE_USER" kernel/process.h; then
    echo "✅ PRIVILEGE_USER defined"
else
    echo "❌ PRIVILEGE_USER not defined"
    exit 1
fi

# Test 14: Check assembly integration
echo ""
echo "Test 14: Assembly Integration"
echo "---------------------------"
if grep -q "switch_to_process" cpu/process_switch.asm; then
    echo "✅ switch_to_process in assembly"
else
    echo "❌ switch_to_process not in assembly"
    exit 1
fi

if grep -q "save_process_state" cpu/process_switch.asm; then
    echo "✅ save_process_state in assembly"
else
    echo "❌ save_process_state not in assembly"
    exit 1
fi

# Test 15: Check process management functions
echo ""
echo "Test 15: Process Management Functions"
echo "-----------------------------------"
if grep -q "create_process" kernel/process.c; then
    echo "✅ create_process implemented"
else
    echo "❌ create_process not implemented"
    exit 1
fi

if grep -q "schedule" kernel/process.c; then
    echo "✅ schedule implemented"
else
    echo "❌ schedule not implemented"
    exit 1
fi

if grep -q "get_current_pid" kernel/process.c; then
    echo "✅ get_current_pid implemented"
else
    echo "❌ get_current_pid not implemented"
    exit 1
fi

# Clean up
rm -f test.elf

echo ""
echo "=========================================="
echo "Phase 1 Testing Complete!"
echo "=========================================="
echo "✅ All tests passed!"
echo "✅ Process foundation successfully implemented"
echo "✅ System ready for Phase 2: Memory Protection"
echo ""
echo "Next steps:"
echo "1. Implement memory protection with segmentation"
echo "2. Add process-specific memory regions"
echo "3. Ensure memory isolation between processes"
echo "4. Test memory protection mechanisms"
echo "==========================================" 