#!/bin/bash

# Fast Development Run Script for WISP-BB
# Builds and runs the OS in one command

echo "=== Fast Development Run ==="
echo "Building and running WISP-BB..."
echo

# Build the OS
echo "🔨 Building OS..."
export PATH=$PATH:~/opt/bin
if make clean > /dev/null 2>&1 && make > /dev/null 2>&1; then
    echo "✅ Build successful"
else
    echo "❌ Build failed!"
    echo "Running make with verbose output..."
    make
    exit 1
fi

echo

# Check if OS image was created
if [ ! -f "os-image.bin" ]; then
    echo "❌ Error: os-image.bin not found after build!"
    exit 1
fi

echo "🚀 Starting QEMU with optimized configuration..."
echo

# QEMU optimized flags for development
QEMU_FLAGS="-cpu host -m 128 -smp 1"
QEMU_FLAGS="$QEMU_FLAGS -display gtk -vga std"
QEMU_FLAGS="$QEMU_FLAGS -k en-us -usb -device usb-tablet"
QEMU_FLAGS="$QEMU_FLAGS -fda os-image.bin"
QEMU_FLAGS="$QEMU_FLAGS -serial stdio -monitor null"
QEMU_FLAGS="$QEMU_FLAGS -no-reboot -no-shutdown"
QEMU_FLAGS="$QEMU_FLAGS -net none -audio none"
QEMU_FLAGS="$QEMU_FLAGS -d guest_errors -D qemu_dev.log"

# Check for KVM
if [ -e /dev/kvm ]; then
    QEMU_FLAGS="$QEMU_FLAGS -enable-kvm"
    echo "✅ Using KVM acceleration"
else
    echo "⚠️  KVM not available - using software emulation"
fi

echo "QEMU Flags: $QEMU_FLAGS"
echo
echo "Press Ctrl+C to exit"
echo

# Run QEMU
qemu-system-i386 $QEMU_FLAGS

echo
echo "Development session ended."
echo "Check qemu_dev.log for any errors." 