# 6502 Unified Repository

A comprehensive collection of 6502 microprocessor resources including historical software, emulation, and physical hardware projects.

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

## 🎯 Use Cases

### For Historians & Educators
- Study the **original Microsoft BASIC source** that powered early personal computers
- Understand the historical context of 1970s software development
- See how Microsoft became a software industry leader

### For Software Developers
- Use the **C emulator** to run 6502 code in modern applications
- Build retro computing projects (NES, Apple II, etc.)
- Test 6502 assembly code without physical hardware

### For Hardware Enthusiasts
- Build a **working 6502 computer** on breadboards
- Learn computer architecture hands-on
- Run historical software on modern-built vintage hardware

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

Each subdirectory contains its own detailed README with specific documentation:

- **[basic-original/README.md](basic-original/README.md)** - Historical context, supported systems, cultural impact
- **[emulator/README.md](emulator/README.md)** - API documentation, building instructions, integration guide
- **[hardware/README.md](hardware/README.md)** - Build instructions, project documentation, toolchain setup

## 🚀 Quick Start

### Test code with the emulator
```bash
cd emulator/
# See emulator/README.md for build instructions
```

### Build for physical hardware
```bash
cd hardware/
./scripts/build.sh assembly/hardware_timer/main.s
# See hardware/README.md for complete workflow
```

### Study historical BASIC
```bash
cd basic-original/
# View m6502.asm in your favorite editor
```

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

Together, these resources provide a complete view of 6502 computing from historical, theoretical, and practical perspectives.

---

**Explore the history, emulate the present, build the future** ⚡
