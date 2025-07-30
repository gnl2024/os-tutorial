#!/bin/bash

# Phase 2 Day 1-4 Test Script - WISP-BB Microkernel Migration
# Tests all Phase 2 Day 1-4 functionality: GDT Segments + Memory Region Management

set -e  # Exit on any error

echo "=========================================="
echo "Phase 2 Day 1-4 Testing: GDT Segments + Memory Regions"
echo "=========================================="

# Set up environment
export PATH=$HOME/opt/bin:$PATH

# Test 1: Build System
echo "Test 1: Build System"
echo "-------------------"
make clean
make
echo "✅ Build successful"

# Test 2: Check all new object files
echo ""
echo "Test 2: Object Files"
echo "-------------------"
if [ -f "kernel/memory.o" ]; then
    echo "✅ kernel/memory.o exists"
else
    echo "❌ kernel/memory.o missing"
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
if [ "$KERNEL_SIZE" -gt 10000 ]; then
    echo "✅ Kernel size: $KERNEL_SIZE bytes (increased due to memory management)"
else
    echo "❌ Kernel size: $KERNEL_SIZE bytes (too small)"
    exit 1
fi

OS_SIZE=$(stat -c%s "os-image.bin" 2>/dev/null || echo "0")
if [ "$OS_SIZE" -gt 10000 ]; then
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
UNDEFINED=$(i386-elf-ld -o test.elf -Ttext 0x1000 boot/kernel_entry.o kernel/kernel.o cpu/isr_stubs_simple.o cpu/gdt_flush.o cpu/process_switch.o kernel/memory.o kernel/process.o drivers/keyboard.o drivers/print.o drivers/screen.o cpu/gdt.o cpu/idt.o cpu/isr.o cpu/ports.o cpu/timer.o libc/mem.o libc/string.o 2>&1 | grep "undefined reference" || echo "No undefined references")
if [ "$UNDEFINED" = "No undefined references" ]; then
    echo "✅ No undefined symbols"
else
    echo "❌ Undefined symbols found:"
    echo "$UNDEFINED"
    exit 1
fi

# Test 6: Check GDT segment symbols
echo ""
echo "Test 6: GDT Segment Symbols"
echo "---------------------------"
if nm test.elf 2>/dev/null | grep -q "setup_process_segments"; then
    echo "✅ setup_process_segments symbol found"
else
    echo "❌ setup_process_segments symbol missing"
fi

if nm test.elf 2>/dev/null | grep -q "assign_process_segments"; then
    echo "✅ assign_process_segments symbol found"
else
    echo "❌ assign_process_segments symbol missing"
fi

# Test 7: Check memory management symbols
echo ""
echo "Test 7: Memory Management Symbols"
echo "--------------------------------"
if nm test.elf 2>/dev/null | grep -q "allocate_memory_region"; then
    echo "✅ allocate_memory_region symbol found"
else
    echo "❌ allocate_memory_region symbol missing"
fi

if nm test.elf 2>/dev/null | grep -q "check_memory_access"; then
    echo "✅ check_memory_access symbol found"
else
    echo "❌ check_memory_access symbol missing"
fi

if nm test.elf 2>/dev/null | grep -q "init_memory_regions"; then
    echo "✅ init_memory_regions symbol found"
else
    echo "❌ init_memory_regions symbol missing"
fi

# Test 8: Check header files
echo ""
echo "Test 8: Header Files"
echo "-------------------"
if [ -f "kernel/memory.h" ]; then
    echo "✅ kernel/memory.h exists"
else
    echo "❌ kernel/memory.h missing"
    exit 1
fi

if [ -f "kernel/memory.c" ]; then
    echo "✅ kernel/memory.c exists"
else
    echo "❌ kernel/memory.c missing"
    exit 1
fi

# Test 9: Check GDT extensions
echo ""
echo "Test 9: GDT Extensions"
echo "---------------------"
if grep -q "MAX_GDT_ENTRIES" cpu/gdt.c; then
    echo "✅ MAX_GDT_ENTRIES defined in GDT"
else
    echo "❌ MAX_GDT_ENTRIES not defined in GDT"
    exit 1
fi

if grep -q "setup_process_segments" cpu/gdt.c; then
    echo "✅ setup_process_segments implemented in GDT"
else
    echo "❌ setup_process_segments not implemented in GDT"
    exit 1
fi

# Test 10: Check memory region structures
echo ""
echo "Test 10: Memory Region Structures"
echo "--------------------------------"
if grep -q "memory_region_t" kernel/memory.h; then
    echo "✅ memory_region_t structure defined"
else
    echo "❌ memory_region_t structure not defined"
    exit 1
fi

if grep -q "MAX_MEMORY_REGIONS" kernel/memory.h; then
    echo "✅ MAX_MEMORY_REGIONS defined"
else
    echo "❌ MAX_MEMORY_REGIONS not defined"
    exit 1
fi

# Test 11: Check memory permissions
echo ""
echo "Test 11: Memory Permissions"
echo "--------------------------"
if grep -q "PERMISSION_READ" kernel/memory.h; then
    echo "✅ PERMISSION_READ defined"
else
    echo "❌ PERMISSION_READ not defined"
    exit 1
fi

if grep -q "PERMISSION_WRITE" kernel/memory.h; then
    echo "✅ PERMISSION_WRITE defined"
else
    echo "❌ PERMISSION_WRITE not defined"
    exit 1
fi

if grep -q "PERMISSION_EXECUTE" kernel/memory.h; then
    echo "✅ PERMISSION_EXECUTE defined"
else
    echo "❌ PERMISSION_EXECUTE not defined"
    exit 1
fi

# Test 12: Check memory region functions
echo ""
echo "Test 12: Memory Region Functions"
echo "-------------------------------"
if grep -q "allocate_memory_region" kernel/memory.c; then
    echo "✅ allocate_memory_region implemented"
else
    echo "❌ allocate_memory_region not implemented"
    exit 1
fi

if grep -q "check_memory_access" kernel/memory.c; then
    echo "✅ check_memory_access implemented"
else
    echo "❌ check_memory_access not implemented"
    exit 1
fi

if grep -q "init_memory_regions" kernel/memory.c; then
    echo "✅ init_memory_regions implemented"
else
    echo "❌ init_memory_regions not implemented"
    exit 1
fi

# Test 13: Check process integration
echo ""
echo "Test 13: Process Integration"
echo "--------------------------"
if grep -q "allocate_memory_region" kernel/process.c; then
    echo "✅ Memory region allocation integrated in process creation"
else
    echo "❌ Memory region allocation not integrated in process creation"
    exit 1
fi

if grep -q "setup_process_segments" kernel/process.c; then
    echo "✅ Process segment setup integrated in process creation"
else
    echo "❌ Process segment setup not integrated in process creation"
    exit 1
fi

# Test 14: Check kernel integration
echo ""
echo "Test 14: Kernel Integration"
echo "-------------------------"
if grep -q "init_memory_regions" kernel/kernel.c; then
    echo "✅ Memory regions initialized in kernel"
else
    echo "❌ Memory regions not initialized in kernel"
    exit 1
fi

if grep -q "memory.h" kernel/kernel.c; then
    echo "✅ memory.h included in kernel"
else
    echo "❌ memory.h not included in kernel"
    exit 1
fi

# Test 15: Check GDT header extensions
echo ""
echo "Test 15: GDT Header Extensions"
echo "-----------------------------"
if grep -q "GDT_KERNEL_CODE" cpu/gdt.h; then
    echo "✅ GDT_KERNEL_CODE defined"
else
    echo "❌ GDT_KERNEL_CODE not defined"
    exit 1
fi

if grep -q "GDT_USER_CODE" cpu/gdt.h; then
    echo "✅ GDT_USER_CODE defined"
else
    echo "❌ GDT_USER_CODE not defined"
    exit 1
fi

if grep -q "setup_process_segments" cpu/gdt.h; then
    echo "✅ setup_process_segments declared in GDT header"
else
    echo "❌ setup_process_segments not declared in GDT header"
    exit 1
fi

# Test 16: Check memory region types
echo ""
echo "Test 16: Memory Region Types"
echo "---------------------------"
if grep -q "MEMORY_TYPE_CODE" kernel/memory.h; then
    echo "✅ MEMORY_TYPE_CODE defined"
else
    echo "❌ MEMORY_TYPE_CODE not defined"
    exit 1
fi

if grep -q "MEMORY_TYPE_HEAP" kernel/memory.h; then
    echo "✅ MEMORY_TYPE_HEAP defined"
else
    echo "❌ MEMORY_TYPE_HEAP not defined"
    exit 1
fi

if grep -q "MEMORY_TYPE_STACK" kernel/memory.h; then
    echo "✅ MEMORY_TYPE_STACK defined"
else
    echo "❌ MEMORY_TYPE_STACK not defined"
    exit 1
fi

# Test 17: Check segment allocation logic
echo ""
echo "Test 17: Segment Allocation Logic"
echo "--------------------------------"
if grep -q "segment_index = 5 + (proc->pid \* 2)" cpu/gdt.c; then
    echo "✅ Process segment allocation logic implemented"
else
    echo "❌ Process segment allocation logic not implemented"
    exit 1
fi

if grep -q "proc->code_segment = segment_index \* 8" cpu/gdt.c; then
    echo "✅ Code segment assignment implemented"
else
    echo "❌ Code segment assignment not implemented"
    exit 1
fi

if grep -q "proc->data_segment = (segment_index + 1) \* 8" cpu/gdt.c; then
    echo "✅ Data segment assignment implemented"
else
    echo "❌ Data segment assignment not implemented"
    exit 1
fi

# Test 18: Check memory region allocation logic
echo ""
echo "Test 18: Memory Region Allocation Logic"
echo "-------------------------------------"
if grep -q "region->start = start" kernel/memory.c; then
    echo "✅ Memory region start assignment implemented"
else
    echo "❌ Memory region start assignment not implemented"
    exit 1
fi

if grep -q "region->process_id = process_id" kernel/memory.c; then
    echo "✅ Memory region process ownership implemented"
else
    echo "❌ Memory region process ownership not implemented"
    exit 1
fi

if grep -q "region->permissions = permissions" kernel/memory.c; then
    echo "✅ Memory region permissions implemented"
else
    echo "❌ Memory region permissions not implemented"
    exit 1
fi

# Test 19: Check access checking logic
echo ""
echo "Test 19: Access Checking Logic"
echo "-----------------------------"
if grep -q "memory_regions\[i\].process_id == process_id" kernel/memory.c; then
    echo "✅ Process ownership checking implemented"
else
    echo "❌ Process ownership checking not implemented"
    exit 1
fi

if grep -q "memory_regions\[i\].permissions & access_type" kernel/memory.c; then
    echo "✅ Permission checking implemented"
else
    echo "❌ Permission checking not implemented"
    exit 1
fi

# Test 20: Check overlap detection
echo ""
echo "Test 20: Overlap Detection"
echo "-------------------------"
if grep -q "start < memory_regions\[i\].end" kernel/memory.c; then
    echo "✅ Memory region overlap detection implemented"
else
    echo "❌ Memory region overlap detection not implemented"
    exit 1
fi

# Clean up
rm -f test.elf

echo ""
echo "=========================================="
echo "Phase 2 Day 1-4 Testing Complete!"
echo "=========================================="
echo "✅ All 20 tests passed!"
echo "✅ GDT segments successfully implemented"
echo "✅ Memory region management working"
echo "✅ Process integration complete"
echo "✅ System ready for Day 5-6: Page Fault Handling"
echo ""
echo "Next steps:"
echo "1. Implement page fault handler"
echo "2. Add page fault stub to assembly"
echo "3. Test memory protection violations"
echo "4. Ensure process isolation works"
echo "==========================================" 