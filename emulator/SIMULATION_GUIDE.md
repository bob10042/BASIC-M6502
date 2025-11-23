# Complete 6502 System Simulation Guide

**Yes, you can simulate the ENTIRE breadboard computer in software!**

No need for:
- ❌ Physical breadboards
- ❌ 6502 CPU chips
- ❌ EEPROM chips
- ❌ LCD displays
- ❌ Keyboard interfaces
- ❌ Arduino debuggers

Everything runs in software on your Unix/Linux system!

---

## What Can Be Simulated

### 1. **CPU Execution**
The emulator provides cycle-accurate 6502 CPU emulation:
- All opcodes and addressing modes
- Exact timing (2-7 cycles per instruction)
- IRQ and NMI interrupts
- Full register state (PC, A, X, Y, S, P)

### 2. **Memory**
Simulate the full 64KB address space:
```
0x0000-0x00FF : Zero Page (fast memory)
0x0100-0x01FF : Stack
0x0200-0x5FFF : RAM
0x6000-0x7FFF : Memory-mapped I/O
0x8000-0xFFFF : ROM (program code)
```

### 3. **Peripherals**
Simulate hardware devices in software:
- **LCD Display**: Output to terminal
- **Keyboard**: Read from stdin
- **UART/Serial**: Socket or pipe I/O
- **VIA Timer**: Software timers
- **LEDs**: Visual indicators in terminal

### 4. **Programs**
Run any 6502 program:
- Hardware projects from `hardware/assembly/`
- Microsoft BASIC interpreter
- Wozmon monitor
- Your own assembly code

---

## Architecture: How It Works

```
┌─────────────────────────────────────────────┐
│          Your Unix/Linux System             │
│                                             │
│  ┌───────────────────────────────────────┐ │
│  │     6502 Emulator (sources/6502.c)    │ │
│  │  • Executes instructions              │ │
│  │  • Manages registers                  │ │
│  │  • Calls read/write callbacks         │ │
│  └──────────────┬────────────────────────┘ │
│                 │                           │
│  ┌──────────────▼────────────────────────┐ │
│  │     Your System Wrapper               │ │
│  │  • Simulated 64KB RAM array           │ │
│  │  • I/O device emulation               │ │
│  │  • Peripheral simulation              │ │
│  └──────────────┬────────────────────────┘ │
│                 │                           │
│  ┌──────────────▼────────────────────────┐ │
│  │     Unix I/O                          │ │
│  │  • Terminal (stdout/stdin)            │ │
│  │  • Files                              │ │
│  │  • Network sockets                    │ │
│  └───────────────────────────────────────┘ │
└─────────────────────────────────────────────┘
```

---

## Quick Start: Simulate "Hello World"

### Step 1: Install Dependencies
```bash
# Install Z library (header-only, simple)
cd /tmp
git clone https://github.com/redcode/Z.git
sudo cp -r Z/API /usr/local/include/

# Alternative: Download just the headers you need
```

### Step 2: Write Your Simulator
```c
#include <emulation/CPU/6502.h>

typedef struct {
    uint8_t ram[65536];
} MyComputer;

uint8_t my_read(void *ctx, uint16_t addr) {
    return ((MyComputer*)ctx)->ram[addr];
}

void my_write(void *ctx, uint16_t addr, uint8_t val) {
    MyComputer *sys = (MyComputer*)ctx;
    if (addr == 0x6000) putchar(val);  // LCD output
    else sys->ram[addr] = val;
}

int main() {
    MyComputer computer = {0};
    M6502 cpu;

    // Setup CPU
    cpu.context = &computer;
    cpu.read = my_read;
    cpu.write = my_write;

    // Load program into RAM
    computer.ram[0x8000] = 0xA9;  // LDA #'H'
    computer.ram[0x8001] = 'H';
    computer.ram[0x8002] = 0x8D;  // STA $6000
    computer.ram[0x8003] = 0x00;
    computer.ram[0x8004] = 0x60;

    // Set reset vector
    computer.ram[0xFFFC] = 0x00;
    computer.ram[0xFFFD] = 0x80;

    // Run!
    m6502_reset(&cpu);
    m6502_run(&cpu, 1000000);  // Run 1M cycles
}
```

### Step 3: Compile and Run
```bash
cd emulator/
gcc -o simulator my_simulator.c sources/6502.c \
    -I. -DCPU_6502_STATIC -DCPU_6502_USE_LOCAL_HEADER
./simulator
```

---

## Simulating Hardware Projects

### Option 1: Direct Simulation
Take any program from `hardware/assembly/` and run it in the emulator:

```bash
# 1. Assemble the program (to get binary)
cd hardware/assembly/keyboard
../../scripts/assemble.sh main_ps2.s

# 2. Load binary into emulator (write a loader)
# 3. Simulate keyboard input via stdin
# 4. Simulate LCD output to terminal
```

### Option 2: Create a Complete Breadboard Simulator
```c
typedef struct {
    // Memory
    uint8_t ram[32768];   // 32KB RAM
    uint8_t rom[32768];   // 32KB ROM

    // Peripherals
    struct {
        char display[32];  // LCD 16x2
        int cursor;
    } lcd;

    struct {
        uint8_t buffer[16];
        int head, tail;
    } keyboard;

    struct {
        uint8_t ddra, ddrb;
        uint8_t ora, orb;
        uint8_t ira, irb;
    } via;  // 65C22 VIA chip

} BreadboardComputer;

// Implement read/write to map addresses:
// 0x0000-0x7FFF : RAM
// 0x6000-0x600F : VIA registers
// 0x5000        : LCD data
// 0x5001        : LCD control
// 0x8000-0xFFFF : ROM
```

---

## Advanced: Simulating Complete Systems

### Apple II Simulation
```c
// Simulate Apple II with:
// - 48KB RAM + 16KB ROM
// - Video memory ($400-$7FF)
// - Keyboard input
// - Speaker output
// Run the original Microsoft BASIC!
```

### Ben Eater Breadboard Computer
```c
// Simulate the exact hardware setup:
// - W65C02S CPU
// - AT28C256 EEPROM
// - 62256 RAM chip
// - 65C22 VIA
// - HD44780 LCD
// - PS/2 keyboard
// Run all projects from hardware/assembly/
```

### Test Environment
```c
// Unit test your 6502 programs:
// 1. Load program
// 2. Set initial state
// 3. Run N cycles
// 4. Assert expected results
// Perfect for CI/CD!
```

---

## Existing Simulators You Can Use

### 1. **py65** (Python-based)
```bash
pip install py65
py65mon
# Interactive 6502 monitor with memory viewer
```

### 2. **lib6502** (Ruby/C)
```bash
# Full 6502 simulation library
# Can load and run binary programs
```

### 3. **Visual 6502**
Web-based transistor-level simulation:
http://visual6502.org/

### 4. **VICE** (Commodore 64 emulator)
```bash
sudo apt install vice
# Full C64 with 6502, runs Commodore BASIC
```

---

## Benefits of Simulation

### ✅ Development
- **Instant compile-test cycle** (no EEPROM burning)
- **Easy debugging** (inspect any memory/register)
- **Automated testing** (CI/CD integration)
- **Fast execution** (can run at any speed)

### ✅ Learning
- **See exactly what's happening** (cycle-by-cycle)
- **No hardware requirements** (learn anywhere)
- **Safe experimentation** (can't damage anything)
- **Instant reset** (start over immediately)

### ✅ Portability
- **Run anywhere** (Linux, Mac, Windows via WSL)
- **Share easily** (just send the binary)
- **Version control** (git everything)
- **Reproducible** (same results every time)

---

## Next Steps

1. **Try the example**: `./example_system` (already built!)

2. **Build a simple simulator**:
   - Start with 64KB RAM
   - Add console I/O at 0x6000
   - Load a program
   - Watch it run!

3. **Simulate a hardware project**:
   - Pick one from `hardware/assembly/`
   - Assemble it
   - Create appropriate I/O simulation
   - Run in emulator

4. **Run Microsoft BASIC**:
   - Load BASIC interpreter ROM
   - Simulate keyboard and display
   - Write BASIC programs!

---

## Resources

- **This emulator**: `emulator/sources/6502.c`
- **Example code**: `emulator/example_system.c`
- **Programs to run**: `hardware/assembly/`
- **Original BASIC**: `basic-original/m6502.asm`
- **Z library**: https://github.com/redcode/Z
- **6502 opcodes**: http://www.6502.org/tutorials/6502opcodes.html

---

**The hardware is optional - simulation gives you everything!** 🚀
