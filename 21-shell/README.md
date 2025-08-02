# Mini32OS - Enhanced Microkernel with Command Interface

## 🎯 **System Overview**

Mini32OS is a 32-bit microkernel operating system featuring enhanced command interface, process management, memory protection, and inter-process communication (IPC). The system provides a professional command-line interface with real-time system monitoring capabilities.

## ✅ **Current Features**

### **🎯 Command Interface**
- **END** - Stop the CPU and exit
- **MEMORY** - Display memory statistics (real data)
- **STATS** - Display IPC system statistics (real data)
- **HELP** - Show available commands

### **🔧 Core Systems**
- **Process Management**: Multi-process support with user/kernel modes
- **Memory Management**: Allocation tracking with comprehensive statistics
- **IPC System**: Message passing with detailed activity monitoring
- **Interrupt Handling**: Timer and keyboard interrupts
- **Hardware Drivers**: Screen, keyboard, timer drivers
- **System Calls**: Kernel interface for user processes

### **📊 System Capabilities**
- **Real-time Monitoring**: Commands show actual system state
- **Professional Interface**: Clean, usable command prompt
- **Memory Tracking**: Real allocation statistics
- **IPC Activity**: Comprehensive message passing statistics
- **Process Isolation**: Complete user/kernel mode separation

## 🚀 **Quick Start**

### **Build and Run**
```bash
# Build the system
make clean && make

# Run in QEMU
qemu-system-i386 -fda os-image.bin -m 128 -enable-kvm -display gtk -usb -device usb-tablet
```

### **Available Commands**
```
END       - Stop the CPU and exit
MEMORY    - Display memory statistics
STATS     - Display IPC system statistics
HELP      - Show this help message
```

### **Example Output**

**MEMORY Command:**
```
Memory Statistics:
Total allocated: 3584 bytes
Allocation count: 3
Max allocation: 2048 bytes
>
```

**STATS Command:**
```
=== Enhanced IPC System Statistics ===
Total queues created: 2
Total messages sent: 2
Total messages received: 0
Total messages dropped: 0
Total broadcasts: 1
Active processes:
  PID 0: 1 queues, 0 pending messages, Priority: 1
  PID 1: 1 queues, 2 pending messages, Priority: 1
=====================================
>
```

## 🔧 **Technical Architecture**

### **Kernel Components**
- **Process Manager**: Multi-process scheduling and management
- **Memory Manager**: Allocation tracking and statistics
- **IPC System**: Message passing between processes
- **Interrupt Handler**: Timer and keyboard interrupt management
- **System Calls**: Kernel interface for user processes

### **Hardware Support**
- **x86 Architecture**: 32-bit protected mode
- **Memory Protection**: MPU simulation and segment protection
- **Interrupt Handling**: PIC and timer interrupts
- **I/O Drivers**: Screen, keyboard, and timer drivers

### **Development Features**
- **Incremental Development**: Step-by-step command addition
- **Build System**: Clean compilation with size monitoring
- **Testing Framework**: QEMU-based testing environment
- **Documentation**: Comprehensive system documentation

## 📈 **Performance Metrics**

### **System Size**
- **Current Size**: ~26,000 bytes
- **Growth**: Minimal (compiler optimization)
- **Efficiency**: High (zero size impact for most additions)

### **Memory Usage**
- **Test Allocations**: 3 allocations (1024, 2048, 512 bytes)
- **Total Allocated**: 3584 bytes
- **Tracking**: Real-time allocation statistics

### **IPC Activity**
- **Queues Created**: 2 test queues
- **Messages Sent**: 2 test messages
- **Broadcasts**: 1 test broadcast
- **Process Activity**: 3 processes (kernel + 2 user)

## 🧪 **Testing**

### **Build Testing**
- **Compilation**: No errors or warnings
- **Linking**: Successful binary generation
- **Size Impact**: Minimal (compiler optimization)

### **QEMU Testing**
- **Boot Process**: System loads successfully
- **Keyboard Input**: Functional command input
- **Command Execution**: All commands work correctly
- **Data Display**: Shows real system data

## 🚀 **Development Status**

### **✅ Completed Features**
- **Process Management**: Multi-process support with user/kernel modes
- **Memory Management**: Allocation tracking with statistics
- **IPC System**: Message passing with comprehensive statistics
- **Command Interface**: Professional command-line interface
- **Real-time Monitoring**: Commands show actual system state

### **🔄 Current Development**
- **Incremental Command Addition**: Step-by-step command development
- **System Enhancement**: Continuous improvement of existing features
- **Testing and Validation**: Comprehensive QEMU-based testing

### **📋 Planned Features**
- **PROCESSES Command**: Display all active processes
- **CLEAR Command**: Clear the screen
- **TIME Command**: Show system uptime
- **VERSION Command**: Show OS version information

## 📚 **Documentation**

### **System Documentation**
- `CURRENT_SYSTEM_STATUS.md` - Comprehensive system status
- `MEMORY_STATS_CORRECTIONS.md` - Command corrections and fixes
- `FIRST_INCREMENT_RESULTS.md` - Development progress tracking

### **Technical Documentation**
- **Memory Management**: `libc/mem.c` and `libc/mem.h`
- **IPC System**: `kernel/ipc.c` and `kernel/ipc.h`
- **Process Management**: `kernel/process.c` and `kernel/process.h`
- **Kernel Interface**: `kernel/kernel.c`

## 🎯 **Architecture Goals**

### **Microkernel Design**
- **Minimal Kernel**: Core functionality in kernel space
- **User Space Services**: Drivers and services in user space
- **Process Isolation**: Complete separation between processes
- **System Stability**: Robust error handling and recovery

### **Development Efficiency**
- **Incremental Development**: Step-by-step feature addition
- **Testing Framework**: QEMU-based testing environment
- **Documentation**: Comprehensive system documentation
- **Build System**: Clean, efficient compilation

## 📊 **System Status**

**Status**: ✅ **Stable with Enhanced Commands**
**Build**: ✅ **Clean and Working**
**Testing**: ✅ **QEMU Ready**
**Documentation**: ✅ **Comprehensive and Up-to-Date**
**Development**: ✅ **Ready for Next Increment**

**The system is stable, well-documented, and ready for continued development!** 🚀

## 🤝 **Contributing**

This is an educational operating system project. The system is designed for learning and experimentation with microkernel architecture, process management, and system programming concepts.

## 📄 **License**

This project is for educational purposes. Feel free to use, modify, and learn from the code.
