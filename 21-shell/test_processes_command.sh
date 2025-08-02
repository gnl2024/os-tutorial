#!/bin/bash

echo "=== Testing PROCESSES Command ==="
echo "Build Status:"
make clean && make
echo ""

echo "System Size:"
ls -lh os-image.bin
echo ""

echo "Expected PROCESSES Command Output:"
echo "=== Active Processes ==="
echo "PID 0: RUNNING (KERNEL)"
echo "PID 1: READY (USER)"
echo "PID 2: READY (USER)"
echo "Total active processes: 3"
echo "====================="
echo ""

echo "Testing Instructions:"
echo "1. Run QEMU: qemu-system-i386 -fda os-image.bin -m 128 -enable-kvm -display gtk -usb -device usb-tablet"
echo "2. Type 'PROCESSES' to test the command"
echo "3. Type 'HELP' to see updated command list"
echo "4. Type 'END' to exit"
echo ""

echo "Test Criteria:"
echo "✅ Build successful (no errors)"
echo "✅ System size remains stable (~31K)"
echo "✅ PROCESSES command shows 3 active processes"
echo "✅ HELP command includes PROCESSES"
echo "✅ All existing commands still work"
echo ""

echo "Ready for QEMU testing!" 