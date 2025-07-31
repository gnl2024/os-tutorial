# QEMU Optimization Guide for WISP-BB Development

## 🎯 **Overview**

This guide provides optimized QEMU configurations for different development scenarios, ensuring the best performance and debugging capabilities throughout the microkernel migration process.

## 🚀 **Available Run Scripts**

### **1. Fast Development Run** (`./dev_run.sh`)
**Best for**: Daily development and testing
```bash
./dev_run.sh
```
**Features**:
- ✅ Automatic build and run
- ✅ KVM acceleration (if available)
- ✅ Optimized performance flags
- ✅ GTK display with USB input
- ✅ Error logging to `qemu_dev.log`

### **2. Optimized Run** (`./run_qemu_optimized.sh`)
**Best for**: Performance-focused testing
```bash
./run_qemu_optimized.sh
```
**Features**:
- ✅ Full performance optimization
- ✅ Hardware acceleration
- ✅ Full-screen mode
- ✅ Comprehensive logging

### **3. Debug Run** (`./run_qemu_debug.sh`)
**Best for**: Debugging and troubleshooting
```bash
./run_qemu_debug.sh
```
**Features**:
- ✅ GDB server on port 1234
- ✅ Pause at startup for debugging
- ✅ Detailed error logging
- ✅ Monitor interface

### **4. GDB Debug** (`./debug_qemu.sh`)
**Best for**: Step-by-step debugging
```bash
./debug_qemu.sh
```
**Features**:
- ✅ Automatic GDB connection
- ✅ Pre-configured breakpoints
- ✅ Symbol loading
- ✅ Interactive debugging

## 🔧 **Makefile Integration**

### **Quick Commands**:
```bash
make dev          # Fast development run
make run-optimized # Optimized performance run
make run-debug    # Debug mode run
make debug        # GDB debugging session
```

### **Traditional Commands**:
```bash
make              # Build only
make run          # Basic QEMU run
make debug        # GDB debugging
```

## 📊 **Performance Optimizations**

### **Hardware Acceleration**
- **KVM**: `-enable-kvm` (much faster if available)
- **CPU**: `-cpu host` (use host CPU features)
- **Memory**: `-m 128` (128MB RAM, sufficient for development)

### **Display Optimizations**
- **Display**: `-display gtk` (better than SDL)
- **VGA**: `-vga std` (standard VGA mode)
- **Full-screen**: `-full-screen` (immersive development)

### **Input Optimizations**
- **Keyboard**: `-k en-us` (US keyboard layout)
- **USB**: `-usb -device usb-tablet` (better mouse support)

### **Storage Optimizations**
- **Floppy**: `-fda os-image.bin` (direct floppy access)
- **Format**: `-drive file=os-image.bin,if=floppy,format=raw`

## 🐛 **Debugging Features**

### **GDB Integration**
```bash
# Start QEMU in debug mode
./run_qemu_debug.sh

# In another terminal, connect with GDB
./debug_qemu.sh
```

### **Pre-configured Breakpoints**
- `kernel_main` - Kernel entry point
- `gdt_init` - GDT initialization
- `init_memory_regions` - Memory setup
- `init_process_manager` - Process management

### **Logging and Monitoring**
- **Error logging**: `-d guest_errors`
- **Interrupt logging**: `-d int`
- **Unimplemented logging**: `-d unimp`
- **Log file**: `-D qemu_debug.log`

## 📈 **Performance Comparison**

### **Standard QEMU**
```bash
qemu-system-i386 -fda os-image.bin
```
- ⚠️ Basic performance
- ⚠️ No acceleration
- ⚠️ Limited debugging

### **Optimized QEMU**
```bash
./dev_run.sh
```
- ✅ KVM acceleration
- ✅ Optimized CPU usage
- ✅ Better display performance
- ✅ USB input support
- ✅ Error logging

### **Debug QEMU**
```bash
./run_qemu_debug.sh
```
- ✅ GDB server
- ✅ Step-by-step debugging
- ✅ Detailed logging
- ✅ Monitor interface

## 🔍 **Troubleshooting**

### **KVM Not Available**
```bash
# Check KVM availability
ls -l /dev/kvm

# If not available, scripts automatically fall back to software emulation
```

### **Display Issues**
```bash
# Try different display backends
qemu-system-i386 -display sdl -fda os-image.bin
qemu-system-i386 -display gtk -fda os-image.bin
qemu-system-i386 -display curses -fda os-image.bin
```

### **Debugging Issues**
```bash
# Check if GDB server is running
netstat -tlnp | grep 1234

# Manual GDB connection
i386-elf-gdb -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"
```

## 📋 **Development Workflow**

### **Daily Development**
```bash
# 1. Make changes to code
# 2. Run fast development
./dev_run.sh

# 3. Test functionality
# 4. Repeat
```

### **Debugging Session**
```bash
# 1. Start debug mode
./run_qemu_debug.sh

# 2. In another terminal, connect GDB
./debug_qemu.sh

# 3. Set breakpoints and debug
# 4. Continue development
```

### **Performance Testing**
```bash
# 1. Run optimized version
./run_qemu_optimized.sh

# 2. Test performance
# 3. Monitor resource usage
```

## 🎯 **Benefits for Microkernel Development**

### **Phase 3: IPC Development**
- Fast iteration with `./dev_run.sh`
- Debug IPC issues with `./run_qemu_debug.sh`
- Monitor message passing in real-time

### **Phase 4: User Space Services**
- Test service communication
- Debug user/kernel transitions
- Monitor process isolation

### **Phase 5: File System**
- Test file operations
- Debug storage access
- Monitor I/O performance

### **Future Phases**
- GUI development with optimized display
- Network testing with QEMU networking
- Audio support when needed

## 🚀 **Ready for Development**

The optimized QEMU setup provides:
- ✅ **Fast development cycles** with automatic build and run
- ✅ **Comprehensive debugging** with GDB integration
- ✅ **Performance optimization** with KVM acceleration
- ✅ **Error monitoring** with detailed logging
- ✅ **Flexible configuration** for different development needs

**Start developing with**: `./dev_run.sh` 