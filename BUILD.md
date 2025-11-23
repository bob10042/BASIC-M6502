# 🔨 BUILD INSTRUCTIONS

Complete step-by-step instructions to build and run everything in this repository.

---

## ✅ Prerequisites

### Check if you have GCC installed:
```bash
gcc --version
```

If not installed:
```bash
sudo apt-get update
sudo apt-get install build-essential
```

That's it! No other dependencies needed for simulation and games.

---

## 🎮 Build Games (1 minute)

### Navigate to games directory:
```bash
cd /home/user/BASIC-M6502/games
```

### Build all three games:
```bash
# Snake
gcc -o snake snake.c
# Output: ./snake executable

# Pong
gcc -o pong pong.c
# Output: ./pong executable

# Pac-Man
gcc -o pacman pacman.c
# Output: ./pacman executable
```

### Run the games:
```bash
./snake     # Use WASD or arrows, Q to quit
./pong      # Player 1: W/S, Player 2: Up/Down
./pacman    # Use WASD to move, eat all dots
```

**Expected output**: Terminal clears and shows colorful game graphics. All controls are responsive.

---

## 🔧 Build Hardware Configuration Tool (30 seconds)

### Navigate to emulator directory:
```bash
cd /home/user/BASIC-M6502/emulator
```

### Build the configuration tool:
```bash
gcc -o hardware_setup hardware_setup.c
# Output: ./hardware_setup executable (about 25KB)
```

### Run it:
```bash
./hardware_setup
```

**Expected output**: Visual menu with options:
```
╔════════════════════════════════════════════╗
║      6502 Hardware Configuration Tool      ║
╠════════════════════════════════════════════╣
║ 1. Add Device                              ║
║ 2. Remove Device                           ║
║ 3. List Devices                            ║
║ 4. Load Preset                             ║
...
```

---

## ⚙️ Build Device Library (1 minute)

The device library provides ADC, DAC, PWM, and sensor simulation.

### Navigate to emulator directory:
```bash
cd /home/user/BASIC-M6502/emulator
```

### Build GPIO devices demo:
```bash
gcc -o gpio_devices gpio_devices.c device_library.c -lm
#                                                    ^^^
#                            Math library (needed for calculations)
```

### Run it:
```bash
./gpio_devices
```

**Expected output**: Real-time display showing:
- LED array: `●○●○●○●○` (blinking LEDs)
- Temperature: `[25.3°C] ████████░░░░░░░░░░░░`
- Light sensor: `[652 lux] ██████████░░░░░░░░░░`
- Relay status: `Motor: ●ON  Speed: 127`

---

## 🌡️ Build Temperature Control Demo (1 minute)

PID controller with heater simulation and real-time graphing.

### Build:
```bash
cd /home/user/BASIC-M6502/emulator
gcc -o adc_dac_demo adc_dac_demo.c device_library.c -lm
```

### Run:
```bash
./adc_dac_demo
```

**Expected output**: Live temperature control system:
```
Temperature Control System (PID)
Setpoint: 50.0°C

Time: 12.5s
Temp: [48.2°C] ████████████████░░░░░░ Target: 50.0°C
Heat: [ 75%  ] ███████████████░░░░░░░░
Error: +1.8°C
```

Temperature climbs to setpoint and stabilizes with PID control.

---

## 🤖 Build Robot Arm Simulator (1 minute)

4-DOF robot arm with inverse kinematics.

### Build:
```bash
cd /home/user/BASIC-M6502/emulator
gcc -o robot_arm robot_arm.c device_library.c -lm
```

### Run:
```bash
./robot_arm
```

**Expected output**: Interactive robot display:
```
4-DOF Robot Arm Simulator

Controls:
  W/S - Move forward/back    R/F - Up/Down
  A/D - Move left/right      Q/E - Rotate base
  T/G - Gripper open/close   X - Auto demo
  ESC - Quit

Position: X:15.0 Y:0.0 Z:10.0
Joint 0 (Base):     [  45°] ████████░░░░░░
Joint 1 (Shoulder): [  30°] ██████░░░░░░░░
Joint 2 (Elbow):    [ -20°] ░░░░██████░░░░
Joint 3 (Wrist):    [  10°] ░░░░░██░░░░░░░
Gripper: CLOSED [100%] ████████████████████
```

**Controls**:
- WASD: Move arm in X/Y plane
- R/F: Move up/down (Z axis)
- Q/E: Rotate base
- T/G: Open/close gripper
- X: Run automatic demonstration
- ESC: Exit

---

## 🖥️ Build BBC Micro Simulator (1 minute)

Full BBC Micro Model B computer with 32KB RAM.

### Build:
```bash
cd /home/user/BASIC-M6502/emulator
gcc -o bbc_micro bbc_micro_simulator.c
```

### Run:
```bash
./bbc_micro
```

**Expected output**: BBC Micro boot screen:
```
BBC Micro Model B
32K RAM

6502 CPU @ 2MHz
READY
```

(Note: Full BASIC interpreter integration pending)

---

## 🛠️ Build Complete Hardware Simulator (2 minutes)

Full breadboard computer with LCD, VIA, UART, keyboard.

### Build:
```bash
cd /home/user/BASIC-M6502/emulator
gcc -o simulate_hardware simulate_hardware.c
```

### Run:
```bash
./simulate_hardware
```

**Expected output**: Simulated hardware system with:
- LCD display (2x16 characters)
- VIA port status (8-bit I/O)
- UART communication
- Memory map

---

## 🔨 Build Everything at Once (One Command)

### Create a build script:
```bash
cd /home/user/BASIC-M6502
cat > build_all.sh << 'EOF'
#!/bin/bash
set -e  # Exit on error

echo "Building 6502 Unified Repository..."
echo "======================================"

# Build games
echo ""
echo "Building games..."
cd games/
gcc -o snake snake.c && echo "✅ Snake built"
gcc -o pong pong.c && echo "✅ Pong built"
gcc -o pacman pacman.c && echo "✅ Pac-Man built"

# Build simulators
echo ""
echo "Building simulators..."
cd ../emulator/
gcc -o hardware_setup hardware_setup.c && echo "✅ Hardware setup built"
gcc -o gpio_devices gpio_devices.c device_library.c -lm && echo "✅ GPIO devices built"
gcc -o adc_dac_demo adc_dac_demo.c device_library.c -lm && echo "✅ ADC/DAC demo built"
gcc -o robot_arm robot_arm.c device_library.c -lm && echo "✅ Robot arm built"
gcc -o bbc_micro bbc_micro_simulator.c sources/6502.c -I./ && echo "✅ BBC Micro built"
gcc -o simulate_hardware simulate_hardware.c sources/6502.c -I./ && echo "✅ Hardware simulator built"

echo ""
echo "======================================"
echo "✅ All builds complete!"
echo ""
echo "To run:"
echo "  Games:      cd games/ && ./snake"
echo "  Simulators: cd emulator/ && ./hardware_setup"
echo "  Robot:      cd emulator/ && ./robot_arm"
EOF

chmod +x build_all.sh
```

### Run the build script:
```bash
./build_all.sh
```

**Expected output**: All programs build successfully with checkmarks.

---

## 📏 Verify Everything Built Correctly

### Check games:
```bash
ls -lh games/snake games/pong games/pacman
```
Should show three executables, each 15-25KB.

### Check simulators:
```bash
ls -lh emulator/hardware_setup emulator/gpio_devices emulator/robot_arm
```
Should show executables, ranging from 20-35KB.

### Quick test:
```bash
# Test hardware_setup menu loads
cd emulator/
echo "9" | ./hardware_setup   # Menu option 9 = Exit
echo $?   # Should print 0 (success)
```

---

## 🐛 Troubleshooting

### Problem: `gcc: command not found`
**Solution**: Install GCC
```bash
sudo apt-get update
sudo apt-get install build-essential
```

### Problem: `undefined reference to 'sqrt'`
**Solution**: Add math library flag `-lm`
```bash
gcc -o robot_arm robot_arm.c device_library.c -lm
#                                              ^^^
```

### Problem: Warning about multi-character constants
**Example**: `warning: multi-character character constant`
**Solution**: This is normal - just warnings about Unicode box-drawing characters. Programs still work fine.

### Problem: Games show weird characters instead of graphics
**Solution**: Your terminal needs UTF-8 and ANSI color support
```bash
export LANG=en_US.UTF-8
export TERM=xterm-256color
```

### Problem: Can't find sources/6502.c
**Solution**: Make sure you're in the emulator directory
```bash
cd /home/user/BASIC-M6502/emulator
ls sources/6502.c   # Should exist
```

---

## 🎓 Next Steps After Building

1. **Play the games** - Start with Snake: `cd games/ && ./snake`

2. **Configure hardware** - Interactive tool: `cd emulator/ && ./hardware_setup`

3. **Try robot arm** - Cool kinematics demo: `cd emulator/ && ./robot_arm` then press X for auto mode

4. **Read the guides**:
   - [QUICK_START.md](QUICK_START.md) - 5-minute intro
   - [LINUX_GUIDE.md](LINUX_GUIDE.md) - Deep dive into Linux integration
   - [emulator/SIMULATION_GUIDE.md](emulator/SIMULATION_GUIDE.md) - How everything works

5. **Modify something** - All source code is heavily commented. Try:
   - Change Snake speed in `games/snake.c`
   - Add an LED in `emulator/gpio_devices.c`
   - Change temperature setpoint in `emulator/adc_dac_demo.c`

---

## 📦 Clean Build (Start Fresh)

To remove all compiled files and rebuild:

```bash
cd /home/user/BASIC-M6502

# Remove all executables
rm -f games/snake games/pong games/pacman
rm -f emulator/hardware_setup emulator/gpio_devices
rm -f emulator/adc_dac_demo emulator/robot_arm
rm -f emulator/bbc_micro emulator/simulate_hardware

# Rebuild everything
./build_all.sh
```

---

## ✅ Success Checklist

After building, you should have:

- ✅ 3 game executables in `games/`
- ✅ 6 simulator executables in `emulator/`
- ✅ All programs run without errors
- ✅ Graphics display correctly in terminal
- ✅ No "command not found" errors

**If all checkmarks are green, you're ready to go! 🎉**

---

**Need help? Check [QUICK_START.md](QUICK_START.md) or read the source code - it's well commented!**
