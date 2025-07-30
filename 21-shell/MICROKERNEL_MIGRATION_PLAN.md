# Microkernel Migration Plan - WISP-BB

## 🎯 Overview

This document outlines the step-by-step migration from our current monolithic kernel to a modular microkernel architecture, using MINIX 3 as our blueprint.

## 📊 Current System Status

### ✅ **Validated System**
- **Build**: ✅ Successful compilation
- **Run**: ✅ QEMU execution working
- **Architecture**: 32-bit x86 protected mode
- **Language**: 95% C, 5% Assembly
- **Features**: Complete OS with all chapters 00-24

### 🔧 **Current Architecture**
```
┌─────────────────┐
│   Boot Sector   │ ← Assembly (53 lines)
├─────────────────┤
│   Kernel Entry  │ ← Assembly (5 lines)
├─────────────────┤
│   GDT Setup     │ ← C (cpu/gdt.c)
├─────────────────┤
│   IDT Setup     │ ← C (cpu/idt.c)
├─────────────────┤
│   ISR System    │ ← C + Assembly stubs
├─────────────────┤
│   Drivers       │ ← C (screen, keyboard, print)
├─────────────────┤
│   Memory Mgmt   │ ← C (libc/mem.c)
├─────────────────┤
│   String Utils  │ ← C (libc/string.c)
└─────────────────┘
```

---

## 🚀 Migration Phases

### **Phase 1: Process Foundation (Week 1-2)**
**Goal**: Add basic process management to current system

#### **Step 1.1: Process Structure**
```c
// Add to kernel/process.h
typedef struct {
    int pid;
    void *stack;
    void *heap;
    int privileges;
    registers_t regs;
    int state;  // RUNNING, READY, BLOCKED, TERMINATED
} process_t;

// Process management
process_t *current_process;
process_t processes[MAX_PROCESSES];
int next_pid = 1;
```

#### **Step 1.2: Process Creation**
```c
// Add to kernel/process.c
process_t *create_process(void (*entry_point)(void), void *stack, int privileges) {
    process_t *proc = &processes[next_pid];
    proc->pid = next_pid++;
    proc->stack = stack;
    proc->heap = kmalloc(0x1000, 1, NULL);  // 4KB heap
    proc->privileges = privileges;
    proc->state = READY;
    
    // Initialize registers
    proc->regs.eip = (u32)entry_point;
    proc->regs.esp = (u32)stack + 0x1000;  // Stack grows down
    proc->regs.ebp = proc->regs.esp;
    
    return proc;
}
```

#### **Step 1.3: Basic Scheduler**
```c
// Add to kernel/scheduler.c
void schedule() {
    // Simple round-robin for now
    static int current_index = 0;
    
    // Find next ready process
    for (int i = 0; i < MAX_PROCESSES; i++) {
        int index = (current_index + i) % MAX_PROCESSES;
        if (processes[index].state == READY) {
            switch_to_process(&processes[index]);
            current_index = (index + 1) % MAX_PROCESSES;
            break;
        }
    }
}
```

#### **Step 1.4: Process Switching**
```c
// Add to cpu/process_switch.asm
[global switch_to_process]
switch_to_process:
    ; Save current process state
    mov eax, [esp + 4]  ; process_t pointer
    mov [current_process], eax
    
    ; Load new process state
    mov esp, [eax + 16]  ; regs.esp
    mov ebp, [eax + 20]  ; regs.ebp
    
    ; Jump to new process
    jmp [eax + 12]       ; regs.eip
```

### **Phase 2: Memory Protection (Week 3-4)**
**Goal**: Implement memory protection with segmentation

#### **Step 2.1: Process Segments**
```c
// Add to cpu/gdt.c
void setup_process_segments(process_t *proc) {
    int segment_index = 3 + (proc->pid * 2);  // Start after kernel segments
    
    // Code segment for process
    gdt_set_gate(segment_index, (u32)proc->heap, 0x1000, 0x9A, 0xCF);
    
    // Data segment for process
    gdt_set_gate(segment_index + 1, (u32)proc->heap, 0x1000, 0x92, 0xCF);
    
    proc->code_segment = segment_index * 8;
    proc->data_segment = (segment_index + 1) * 8;
}
```

#### **Step 2.2: Memory Regions**
```c
// Add to kernel/memory.h
typedef struct {
    u32 start;
    u32 end;
    int permissions;  // READ, WRITE, EXECUTE
    int process_id;   // Owner process
} memory_region_t;

memory_region_t memory_regions[MAX_REGIONS];
int region_count = 0;
```

#### **Step 2.3: Memory Protection**
```c
// Add to kernel/memory.c
int allocate_memory_region(u32 start, u32 size, int permissions, int process_id) {
    if (region_count >= MAX_REGIONS) return -1;
    
    memory_region_t *region = &memory_regions[region_count++];
    region->start = start;
    region->end = start + size;
    region->permissions = permissions;
    region->process_id = process_id;
    
    return region_count - 1;
}
```

### **Phase 3: IPC System (Week 5-6)**
**Goal**: Implement inter-process communication

#### **Step 3.1: Message Passing**
```c
// Add to kernel/ipc.h
typedef struct {
    int sender;
    int receiver;
    void *message;
    int size;
    int type;  // REQUEST, REPLY, NOTIFICATION
} ipc_message_t;

typedef struct {
    ipc_message_t messages[MAX_MESSAGES];
    int head, tail;
    int count;
} message_queue_t;
```

#### **Step 3.2: IPC Functions**
```c
// Add to kernel/ipc.c
int send_message(int to, void *msg, int size, int type) {
    if (to >= MAX_PROCESSES || processes[to].state == TERMINATED) {
        return -1;
    }
    
    ipc_message_t *message = &message_queues[to].messages[message_queues[to].tail];
    message->sender = current_process->pid;
    message->receiver = to;
    message->message = msg;
    message->size = size;
    message->type = type;
    
    message_queues[to].tail = (message_queues[to].tail + 1) % MAX_MESSAGES;
    message_queues[to].count++;
    
    return 0;
}

int receive_message(int from, void *msg, int size) {
    if (from >= MAX_PROCESSES) return -1;
    
    message_queue_t *queue = &message_queues[current_process->pid];
    if (queue->count == 0) return -1;
    
    ipc_message_t *message = &queue->messages[queue->head];
    if (from != -1 && message->sender != from) return -1;
    
    memcpy(msg, message->message, size);
    queue->head = (queue->head + 1) % MAX_MESSAGES;
    queue->count--;
    
    return 0;
}
```

### **Phase 4: User Space Services (Week 7-8)**
**Goal**: Move drivers to user space

#### **Step 4.1: Service Framework**
```c
// Add to kernel/services.h
typedef struct {
    int service_id;
    void (*handler)(ipc_message_t *msg);
    int process_id;
} service_t;

service_t services[MAX_SERVICES];
int service_count = 0;
```

#### **Step 4.2: Service Registration**
```c
// Add to kernel/services.c
int register_service(int id, void (*handler)(ipc_message_t *msg)) {
    if (service_count >= MAX_SERVICES) return -1;
    
    service_t *service = &services[service_count++];
    service->service_id = id;
    service->handler = handler;
    service->process_id = current_process->pid;
    
    return 0;
}

void handle_service_request(ipc_message_t *msg) {
    for (int i = 0; i < service_count; i++) {
        if (services[i].service_id == msg->type) {
            services[i].handler(msg);
            break;
        }
    }
}
```

#### **Step 4.3: Move Drivers to User Space**
```c
// Create user/drivers/screen_service.c
void screen_service_handler(ipc_message_t *msg) {
    switch (msg->type) {
        case SCREEN_PRINT:
            kprint((char*)msg->message);
            break;
        case SCREEN_CLEAR:
            clear_screen();
            break;
        case SCREEN_SCROLL:
            scroll_screen();
            break;
    }
}

// Create user/drivers/keyboard_service.c
void keyboard_service_handler(ipc_message_t *msg) {
    switch (msg->type) {
        case KEYBOARD_READ:
            // Read scancode and send back
            break;
    }
}
```

### **Phase 5: File System (Week 9-10)**
**Goal**: Implement basic file system

#### **Step 5.1: File System Structure**
```c
// Add to kernel/filesystem.h
typedef struct {
    char name[32];
    u32 size;
    u32 blocks[256];
    int permissions;
    int owner;
} file_t;

typedef struct {
    file_t files[MAX_FILES];
    int file_count;
} filesystem_t;
```

#### **Step 5.2: File Operations**
```c
// Add to kernel/filesystem.c
int open_file(const char *name) {
    for (int i = 0; i < filesystem.file_count; i++) {
        if (strcmp(filesystem.files[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int read_file(int fd, void *buf, int size) {
    if (fd < 0 || fd >= filesystem.file_count) return -1;
    
    file_t *file = &filesystem.files[fd];
    // Implement block reading
    return size;
}

int write_file(int fd, void *buf, int size) {
    if (fd < 0 || fd >= filesystem.file_count) return -1;
    
    file_t *file = &filesystem.files[fd];
    // Implement block writing
    return size;
}
```

---

## 🎯 Implementation Schedule

### **Week 1-2: Process Foundation**
- [ ] Day 1: Process structure and creation
- [ ] Day 2: Basic scheduler implementation
- [ ] Day 3: Process switching assembly
- [ ] Day 4: Testing and debugging
- [ ] Day 5: Integration with current system

### **Week 3-4: Memory Protection**
- [ ] Day 1: Process segments in GDT
- [ ] Day 2: Memory regions and protection
- [ ] Day 3: Memory allocation per process
- [ ] Day 4: Testing memory isolation
- [ ] Day 5: Integration testing

### **Week 5-6: IPC System**
- [ ] Day 1: Message passing structure
- [ ] Day 2: IPC send/receive functions
- [ ] Day 3: Service registration
- [ ] Day 4: Testing IPC communication
- [ ] Day 5: Integration with processes

### **Week 7-8: User Space Services**
- [ ] Day 1: Service framework
- [ ] Day 2: Move screen driver to user space
- [ ] Day 3: Move keyboard driver to user space
- [ ] Day 4: Testing user space services
- [ ] Day 5: Integration testing

### **Week 9-10: File System**
- [ ] Day 1: File system structure
- [ ] Day 2: File operations
- [ ] Day 3: Block device interface
- [ ] Day 4: Testing file system
- [ ] Day 5: Complete microkernel

---

## 🔧 Testing Strategy

### **Phase 1 Testing**
```bash
# Test process creation
make test_processes

# Test basic scheduling
make test_scheduler

# Test process switching
make test_switching
```

### **Phase 2 Testing**
```bash
# Test memory protection
make test_memory_protection

# Test process isolation
make test_isolation
```

### **Phase 3 Testing**
```bash
# Test IPC communication
make test_ipc

# Test service registration
make test_services
```

### **Phase 4 Testing**
```bash
# Test user space drivers
make test_user_drivers

# Test service communication
make test_service_communication
```

### **Phase 5 Testing**
```bash
# Test file system
make test_filesystem

# Test complete microkernel
make test_microkernel
```

---

## 📋 Success Criteria

### **Phase 1 Success**
- ✅ Process creation works
- ✅ Basic scheduling works
- ✅ Process switching works
- ✅ System remains stable

### **Phase 2 Success**
- ✅ Memory protection works
- ✅ Process isolation works
- ✅ No memory corruption
- ✅ System remains stable

### **Phase 3 Success**
- ✅ IPC communication works
- ✅ Service registration works
- ✅ Message passing works
- ✅ System remains stable

### **Phase 4 Success**
- ✅ Drivers work in user space
- ✅ Service communication works
- ✅ No kernel crashes
- ✅ System remains stable

### **Phase 5 Success**
- ✅ File system works
- ✅ Complete microkernel
- ✅ All features working
- ✅ System remains stable

---

## 🚀 Next Steps

### **Immediate Actions**
1. **Start Phase 1** - Process Foundation
2. **Implement process structure** in kernel
3. **Add basic scheduler** functionality
4. **Test incrementally** at each step
5. **Document progress** and lessons learned

### **Long-term Goals**
1. **Complete microkernel** architecture
2. **Process isolation** and memory protection
3. **User space services** and drivers
4. **File system** and applications
5. **Complete OS** with microkernel design

---

*Microkernel Migration Plan - WISP-BB v1.0*
*Status: Ready to Start Phase 1*
*Blueprint: MINIX 3 Architecture* 