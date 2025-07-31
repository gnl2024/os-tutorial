#!/bin/bash

# QEMU Optimized Run Script for WISP-BB Development
# Optimized for development with better performance and debugging

echo "=== QEMU Optimized Run Script ==="
echo "Starting WISP-BB with optimized QEMU configuration..."
echo

# Check if OS image exists
if [ ! -f "os-image.bin" ]; then
    echo "❌ Error: os-image.bin not found!"
    echo "Please run 'make' first to build the OS."
    exit 1
fi

# QEMU optimization flags
QEMU_FLAGS=""

# Performance optimizations
QEMU_FLAGS="$QEMU_FLAGS -enable-kvm"                    # Use KVM if available (much faster)
QEMU_FLAGS="$QEMU_FLAGS -cpu host"                      # Use host CPU features
QEMU_FLAGS="$QEMU_FLAGS -smp 1"                         # Single CPU (sufficient for our OS)
QEMU_FLAGS="$QEMU_FLAGS -m 128"                         # 128MB RAM (plenty for development)

# Display optimizations
QEMU_FLAGS="$QEMU_FLAGS -display gtk"                   # GTK display (better than SDL)
QEMU_FLAGS="$QEMU_FLAGS -vga std"                       # Standard VGA
QEMU_FLAGS="$QEMU_FLAGS -full-screen"                   # Full screen mode

# Input optimizations
QEMU_FLAGS="$QEMU_FLAGS -k en-us"                       # US keyboard layout
QEMU_FLAGS="$QEMU_FLAGS -usb"                           # Enable USB support
QEMU_FLAGS="$QEMU_FLAGS -device usb-tablet"             # USB tablet for better mouse

# Storage optimizations
QEMU_FLAGS="$QEMU_FLAGS -fda os-image.bin"             # Floppy disk image

# Networking (for future development)
QEMU_FLAGS="$QEMU_FLAGS -net none"                      # No network for now (can be enabled later)

# Serial and debugging
QEMU_FLAGS="$QEMU_FLAGS -serial stdio"                  # Serial output to terminal
QEMU_FLAGS="$QEMU_FLAGS -monitor null"                  # Disable monitor (use serial instead)

# Performance tuning
QEMU_FLAGS="$QEMU_FLAGS -no-reboot"                     # Don't reboot on exit
QEMU_FLAGS="$QEMU_FLAGS -no-shutdown"                   # Don't shutdown on exit
# QEMU_FLAGS="$QEMU_FLAGS -daemonize"                     # Run in background (optional)

# Audio (for future development)
QEMU_FLAGS="$QEMU_FLAGS -audio none"                    # No audio for now

# Development-specific optimizations
QEMU_FLAGS="$QEMU_FLAGS -d guest_errors"                # Log guest errors
QEMU_FLAGS="$QEMU_FLAGS -D qemu.log"                    # Log to file

echo "QEMU Flags: $QEMU_FLAGS"
echo

# Check if KVM is available
if [ -e /dev/kvm ]; then
    echo "✅ KVM available - using hardware acceleration"
else
    echo "⚠️  KVM not available - using software emulation"
    # Remove KVM flag if not available
    QEMU_FLAGS=$(echo $QEMU_FLAGS | sed 's/-enable-kvm//')
fi

echo "Starting QEMU with optimized configuration..."
echo "Press Ctrl+C to exit"
echo

# Run QEMU with optimized flags
qemu-system-i386 $QEMU_FLAGS

echo
echo "QEMU session ended."
echo "Check qemu.log for any errors or debugging information." 