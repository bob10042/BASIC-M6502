# 6502 Unified Repository

A comprehensive collection of 6502 microprocessor resources including historical software, emulation, hardware simulation, games, and physical hardware projects.

**🎮 NEW: Play retro games, simulate hardware devices, control robot arms - all without physical hardware!**

## 📚 Repository Structure

This unified repository contains three distinct but complementary 6502 projects:

### 🏛️ [basic-original/](basic-original/) - Microsoft BASIC Historical Archive

The complete **original source code** for Microsoft BASIC Version 1.1 for the 6502 microprocessor (1976-1978). This historically significant code powered the personal computer revolution and established Microsoft as a software industry leader.

**Key Features:**
- Original 6502 assembly source (m6502.asm)
- Support for Apple II, Commodore PET, OSI, KIM-1
- Complete BASIC interpreter with floating-point arithmetic
- 8KB ROM version

[Read more →](basic-original/README.md)

### ⚙️ [emulator/](emulator/) - 6502 CPU Emulator (C)

A fast, accurate, and well-tested **C implementation** of the MOS Technology 6502 CPU. Perfect for software projects requiring 6502 emulation.

**Key Features:**
- Very accurate cycle-level emulation
- Small footprint (17KB compiled)
- Extensively tested and documented
- Easy to integrate into other projects
- Used in multiple machine emulators including NES emulators

[Read more →](emulator/README.md)

### 🔧 [hardware/](hardware/) - Breadboard Computer

A complete **physical 6502 system** built on breadboards following Ben Eater's tutorial series. Includes multiple working projects, from basic I/O to running Microsoft BASIC on physical hardware.

**Projects Include:**
- Hardware timers and LCD displays
- PS/2 keyboard interface
- RS-232 serial communication
- Wozmon monitor (Apple I software)
- Microsoft BASIC port (modular version)
- Arduino debugging tools
- Complete build toolchain (VASM, CA65, Docker)

[Read more →](hardware/README.md)

## 🚀 NEW: Complete Simulation Environment

### 🎮 [games/](games/) - Playable Retro Games

Three fully working graphical games that run in your terminal:

**Build and Play:**
```bash
cd games/

# Build all games
gcc -o snake snake.c
gcc -o pong pong.c
gcc -o pacman pacman.c

# Play Snake (classic arcade)
./snake
# Controls: WASD or Arrow Keys, Q to quit

# Play Pong (two player)
./pong
# Player 1: W/S, Player 2: Arrow Up/Down

# Play Pac-Man (full maze game)
./pacman
# Controls: WASD, collect all dots, eat ghosts with power pellets
```

### ⚙️ Hardware Simulation Suite

Complete device simulation without physical hardware - LEDs, sensors, motors, robot arms, and more!

**Quick Setup Tool:**
```bash
cd emulator/

# Build the interactive hardware configuration tool
gcc -o hardware_setup hardware_setup.c

# Run it to configure your simulation
./hardware_setup
```

**Configuration Tool Features:**
- ✅ Add devices interactively (LEDs, sensors, motors, pumps, servos)
- ✅ Load presets: Breadboard, Robot Arm, IoT Sensors
- ✅ Generate C code automatically
- ✅ Save/load configurations
- ✅ Visual device preview

**Build All Simulators:**
```bash
cd emulator/

# Basic 6502 system simulator
gcc -o example_system example_system.c sources/6502.c -I./API

# Complete hardware peripherals (LCD, VIA, UART)
gcc -o simulate_hardware simulate_hardware.c

# BBC Micro Model B computer with BASIC
gcc -o bbc_micro bbc_micro_simulator.c

# GPIO devices (LEDs, buttons, sensors, relays)
gcc -o gpio_devices gpio_devices.c device_library.c -lm

# ADC/DAC with PID temperature control
gcc -o adc_dac_demo adc_dac_demo.c device_library.c -lm

# 4-DOF robot arm with inverse kinematics
gcc -o robot_arm robot_arm.c device_library.c -lm
```

**Run Simulators:**
```bash
# GPIO demo - LEDs, buttons, sensors
./gpio_devices
# Watch LEDs blink, temperature sensors, relay control

# Temperature control system
./adc_dac_demo
# Real-time PID control with heater simulation

# Robot arm simulator
./robot_arm
# Interactive: WASD=move, RF=height, TG=gripper, QE=rotate
# Or auto mode: press A for automated movement

# BBC Micro computer
./bbc_micro
# Full computer with keyboard and display
```

### 📋 Memory Map Reference

All simulated devices use memory-mapped I/O:

| Address | Device | Function |
|---------|--------|----------|
| `0x6000` | VIA Port B | 8 input bits (buttons/switches) |
| `0x6001` | VIA Port A | 8 output bits (LEDs) |
| `0x6010-0x6017` | ADC | 8 analog input channels (sensors) |
| `0x6020-0x6023` | DAC | 4 analog output channels (motors) |
| `0x6030-0x6033` | PWM | 4 PWM channels (servos) |
| `0x6050` | UART | Serial I/O |

**Example: Control an LED from 6502 code**
```asm
LDA #$01       ; Bit 0 = 1
STA $6001      ; Write to Port A (LED array)
```

**Example: Read a sensor**
```asm
LDA $6010      ; Read ADC channel 0
; A now contains 0-255 (sensor value)
```

## 🎯 Use Cases

### For Historians & Educators
- Study the **original Microsoft BASIC source** that powered early personal computers
- Understand the historical context of 1970s software development
- See how Microsoft became a software industry leader

### For Software Developers
- Use the **C emulator** to run 6502 code in modern applications
- Build retro computing projects (NES, Apple II, etc.)
- Test 6502 assembly code without physical hardware
- **NEW**: Simulate complete systems with devices before building hardware

### For Hardware Enthusiasts
- Build a **working 6502 computer** on breadboards
- Learn computer architecture hands-on
- Run historical software on modern-built vintage hardware
- **NEW**: Prototype with simulated LEDs, sensors, motors before buying components

### For Makers & Robotics
- **NEW**: Control simulated robot arms with inverse kinematics
- **NEW**: Test PID control algorithms with temperature simulation
- **NEW**: Design IoT sensor systems in software first
- **NEW**: Learn embedded programming without hardware

### For Gamers & Retro Enthusiasts
- **NEW**: Play classic arcade games (Snake, Pong, Pac-Man)
- **NEW**: Experience the BBC Micro computer with BASIC
- **NEW**: Run games on authentic 6502 simulation

## 🔄 Cross-Project Connections

These three components work together beautifully:

```
┌─────────────────┐      ┌──────────────┐      ┌─────────────────┐
│ Original BASIC  │─────▶│  Emulator    │─────▶│  Hardware       │
│ (1978 source)   │      │  (Test code) │      │  (Run on real   │
│                 │      │              │      │   6502 chips)   │
└─────────────────┘      └──────────────┘      └─────────────────┘
```

**Example workflows:**
1. Study the original BASIC code → Port to modern BASIC variant → Run on emulator → Test on physical hardware
2. Write 6502 assembly → Test on emulator → Burn to EEPROM → Run on breadboard computer
3. Compare historical BASIC with modern port (hardware/assembly/msbasic/)

## 📖 Documentation

### 🚀 **START HERE: [QUICK_START.md](QUICK_START.md)** - Get running in 5 minutes!

### Complete Guides

- **[QUICK_START.md](QUICK_START.md)** - 5-minute quick start with games and simulators
- **[LINUX_GUIDE.md](LINUX_GUIDE.md)** - Complete Linux integration guide
- **[GAMES_GUIDE.md](GAMES_GUIDE.md)** - Classic 6502 games information
- **[emulator/SIMULATION_GUIDE.md](emulator/SIMULATION_GUIDE.md)** - How simulation works
- **[emulator/COMPLETE_SUMMARY.md](emulator/COMPLETE_SUMMARY.md)** - Everything explained in detail

### Project-Specific Documentation

- **[basic-original/README.md](basic-original/README.md)** - Historical context, supported systems, cultural impact
- **[emulator/README.md](emulator/README.md)** - API documentation, building instructions, integration guide
- **[hardware/README.md](hardware/README.md)** - Build instructions, project documentation, toolchain setup

## 🚀 Quick Start

### 🎮 Play Games (30 seconds)
```bash
cd games/
./snake       # Classic Snake game
./pong        # Two-player Pong
./pacman      # Full Pac-Man maze
```

### 🔧 Configure Hardware Simulation (2 minutes)
```bash
cd emulator/
./hardware_setup
# Interactive menu - try "Load Preset" → "Breadboard"
```

### 🤖 Run Advanced Simulators (3 minutes)
```bash
cd emulator/
./gpio_devices    # LEDs, buttons, sensors
./robot_arm       # 4-DOF robot with kinematics
./adc_dac_demo    # PID temperature control
```

### 🏗️ Build for Physical Hardware
```bash
cd hardware/
./scripts/build.sh assembly/hardware_timer/main.s
# See hardware/README.md for complete workflow
```

### 📚 Study Historical BASIC
```bash
cd basic-original/
# View m6502.asm - the original 1978 Microsoft BASIC source
```

## 🛠️ System Requirements

**All simulators and games:**
- Linux (any modern distribution)
- GCC compiler (`apt-get install build-essential`)
- Terminal with ANSI color support (standard on all Linux)
- No additional dependencies required!

**Physical hardware projects:**
- See [hardware/README.md](hardware/README.md) for VASM/CA65 assembler setup
- Docker optional (provides isolated build environment)

## 🎓 Learning Path

**Absolute Beginner (30 minutes):**
1. Read [QUICK_START.md](QUICK_START.md)
2. Play the games
3. Run `./hardware_setup` and explore the menu
4. Try `./gpio_devices` to see LEDs and sensors

**Intermediate (2-3 hours):**
1. Read [LINUX_GUIDE.md](LINUX_GUIDE.md)
2. Configure a custom hardware setup with `./hardware_setup`
3. Study the device library code (`device_library.c`)
4. Modify a game or simulator

**Advanced (ongoing):**
1. Read [emulator/SIMULATION_GUIDE.md](emulator/SIMULATION_GUIDE.md)
2. Write 6502 assembly code for the simulators
3. Create your own devices and peripherals
4. Build physical hardware from [hardware/](hardware/)
5. Port Microsoft BASIC to your configuration

## 📜 License

Each component maintains its original license:

- **basic-original/**: Original Microsoft copyright (1976-1978), preserved for historical reference
- **emulator/**: GNU Lesser General Public License v3.0
- **hardware/**: See hardware/LICENSE

## 🙏 Credits

- **Microsoft BASIC**: Original by Bill Gates, Paul Allen, and Microsoft Corporation (1976-1978)
- **6502 Emulator**: Manuel Sainz de Baranda y Goñi
- **Hardware Projects**: Inspired by [Ben Eater's 6502 tutorial](https://eater.net/6502)
- **Modern BASIC Port**: [mist64/msbasic](https://github.com/mist64/msbasic) by Michael Steil

## 🌟 Why This Repository Exists

The 6502 microprocessor was the heart of the personal computer revolution. This repository brings together:
- The **software** that made these computers useful (Microsoft BASIC)
- The **tools** to run that software today (C emulator)
- The **hardware** to experience it physically (breadboard computer)
- **NEW**: Complete **simulation environment** so anyone can learn without hardware

Together, these resources provide a complete view of 6502 computing from historical, theoretical, and practical perspectives.

## 📦 What's Included - Complete Feature List

### Games (All Fully Working)
- ✅ **Snake** - Classic arcade with ANSI graphics
- ✅ **Pong** - Two-player paddle game with physics
- ✅ **Pac-Man** - Full maze, 4 ghosts, power pellets
- ✅ **Guess the Number** - BASIC game for learning

### Simulators (6 Complete Programs)
- ✅ **hardware_setup** - Interactive device configuration tool with presets
- ✅ **gpio_devices** - LED arrays, buttons, sensors, relays with visual display
- ✅ **adc_dac_demo** - PID temperature control system with physics
- ✅ **robot_arm** - 4-DOF robot with inverse kinematics (interactive & auto)
- ✅ **bbc_micro** - Full BBC Micro Model B computer simulator
- ✅ **simulate_hardware** - Complete breadboard computer (LCD, VIA, UART)

### Device Library
- ✅ **8-channel ADC** - Analog input with voltage conversion
- ✅ **4-channel DAC** - Analog output for motor control
- ✅ **4-channel PWM** - Servo motor control
- ✅ **VIA ports** - 8-bit GPIO for LEDs and buttons
- ✅ **UART** - Serial communication
- ✅ **LCD** - HD44780-compatible display
- ✅ **Sensors** - Temperature, light, ultrasonic
- ✅ **Motors** - DC motors, servos, pumps
- ✅ **Relays** - Switching control

### Configuration Presets
- ✅ **Breadboard** - 8 LEDs, LCD, UART (Ben Eater style)
- ✅ **Robot Arm** - 4 servos, 4 encoders, gripper
- ✅ **IoT Sensors** - Temperature, humidity, light, 4 relays

### Documentation (5 Complete Guides)
- ✅ **QUICK_START.md** - 5-minute beginner guide
- ✅ **LINUX_GUIDE.md** - Complete Linux integration
- ✅ **GAMES_GUIDE.md** - Classic 6502 games
- ✅ **SIMULATION_GUIDE.md** - How simulation works
- ✅ **COMPLETE_SUMMARY.md** - Everything explained

### Historical Archive
- ✅ **Original Microsoft BASIC** - 1978 source code (m6502.asm)
- ✅ **6502 Emulator** - Cycle-accurate C implementation
- ✅ **Hardware Projects** - Physical breadboard computer

## 🔧 Troubleshooting

### Games won't compile
```bash
# Make sure you have GCC installed
sudo apt-get update
sudo apt-get install build-essential

# Try compiling with all warnings
gcc -Wall -o snake snake.c
```

### Terminal graphics look wrong
- Make sure your terminal supports ANSI colors (all modern Linux terminals do)
- Try: `echo -e "\033[31mRed\033[0m"` - should show "Red" in red
- Use a full-screen terminal for best experience

### Simulators aren't running
```bash
# Some need math library
gcc -o robot_arm robot_arm.c device_library.c -lm
#                                              ^^^^ important!

# Make sure files are executable
chmod +x hardware_setup gpio_devices robot_arm
```

### Hardware setup tool shows weird characters
- Your terminal needs UTF-8 support for box-drawing characters
- Try: `export LANG=en_US.UTF-8`
- Or set in your terminal preferences

### Need help?
1. Check [QUICK_START.md](QUICK_START.md) for step-by-step instructions
2. Read [emulator/SIMULATION_GUIDE.md](emulator/SIMULATION_GUIDE.md) for technical details
3. Look at the source code - everything is well-commented!

## 🎯 Project Ideas to Try

**Beginner (30 min each):**
- Modify Snake to be faster or add obstacles
- Change LED blink patterns in gpio_devices
- Adjust temperature setpoint in adc_dac_demo
- Create a new preset in hardware_setup

**Intermediate (2-3 hours each):**
- Add a new game (Tetris? Breakout?)
- Create a traffic light controller with GPIO
- Build a data logger for sensor readings
- Add a new device type to the library

**Advanced (full project):**
- Write 6502 assembly to control the devices
- Port a real BBC Micro BASIC program
- Create a complete IoT system simulation
- Build the physical hardware and run the same code

---

**Explore the history, emulate the present, build the future** ⚡

**🚀 Ready to start? Read [QUICK_START.md](QUICK_START.md) now!**
