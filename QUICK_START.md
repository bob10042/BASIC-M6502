# 🚀 QUICK START GUIDE

Get up and running with 6502 simulation in 5 minutes!

---

## ✅ What You Have

A complete 6502 simulation environment with:
- ✅ Working games (Snake, Pong, Pac-Man)
- ✅ Hardware simulators (LEDs, sensors, motors, displays)
- ✅ Easy configuration tool
- ✅ Microsoft BASIC
- ✅ Robot arm simulator
- ✅ All running on Linux

---

## 🎮 Try Games Right Now!

```bash
cd /home/user/BASIC-M6502/games

# Snake - Classic arcade
./snake

# Pong - Two player
./pong

# Pac-Man - Full maze game
./pacman
```

**Controls:**
- WASD or Arrow Keys
- Q to quit

---

## 🔧 Configure Hardware

### Interactive Setup Tool
```bash
cd /home/user/BASIC-M6502/emulator
./hardware_setup
```

**Menu Options:**
1. **Add Device** - Choose from LEDs, sensors, motors, etc.
2. **Load Preset** - Quick setups:
   - Breadboard (Ben Eater style)
   - Robot Arm (4-DOF)
   - IoT Sensors
3. **Generate Code** - Get C code for your setup
4. **Save Config** - Export your configuration

### Quick Presets

**Load Breadboard Computer:**
```
Menu → 4 (Load Preset) → 1 (Breadboard)
```
Adds: 8 LEDs, LCD, UART serial

**Load Robot Arm:**
```
Menu → 4 (Load Preset) → 2 (Robot)
```
Adds: 4 servos, 4 encoders, gripper

**Load IoT Setup:**
```
Menu → 4 (Load Preset) → 3 (IoT)
```
Adds: Temp/humidity/light sensors, 4 relays

---

## 🤖 Run Simulators

### Hardware Simulator
```bash
cd /home/user/BASIC-M6502/emulator

# GPIO devices (LEDs, buttons, sensors)
./gpio_devices

# ADC/DAC demo (temperature control)
./adc_dac_demo

# Robot arm with inverse kinematics
./robot_arm

# BBC Micro computer
./bbc_micro
```

---

## 📝 Device Address Map

| Address | Device | What It Does |
|---------|--------|--------------|
| `0x6000` | VIA Port B | 8 input bits (buttons) |
| `0x6001` | VIA Port A | 8 output bits (LEDs) |
| `0x6010-0x6017` | ADC | 8 analog inputs (sensors) |
| `0x6020-0x6023` | DAC | 4 analog outputs (motors) |
| `0x6030-0x6033` | PWM | 4 PWM channels (servos) |
| `0x6050` | UART | Serial I/O |

---

## 💡 Quick Examples

### Turn on LED 0
```asm
LDA #$01       ; Bit 0 = 1
STA $6001      ; Write to Port A
```

### Read Temperature Sensor
```asm
LDA $6010      ; Read ADC channel 0
; A now contains 0-255 (0-100°C)
```

### Control Servo
```asm
LDA #$80       ; Mid position
STA $6020      ; Write to DAC channel 0
```

---

## 🎯 Your First Project

### Step 1: Configure Hardware
```bash
./hardware_setup
> 4  # Load preset
> 1  # Breadboard
> 5  # Save
> mysetup.conf
> 9  # Exit
```

### Step 2: Write Code
```c
// Turn on all LEDs in sequence
for (int i = 0; i < 8; i++) {
    analog_write(0x6001, 1 << i);
    usleep(100000);  // 100ms
}
```

### Step 3: Run
```bash
gcc my_program.c -o myprogram
./myprogram
```

---

## 📚 Full Documentation

- **[COMPLETE_SUMMARY.md](emulator/COMPLETE_SUMMARY.md)** - Everything explained
- **[LINUX_GUIDE.md](LINUX_GUIDE.md)** - Linux integration
- **[SIMULATION_GUIDE.md](emulator/SIMULATION_GUIDE.md)** - How simulation works
- **[GAMES_GUIDE.md](GAMES_GUIDE.md)** - Classic games info

---

## 🎓 Learning Path

### Beginner
1. ✅ Play the games
2. ✅ Use hardware_setup tool
3. ✅ Run the demos

### Intermediate
4. Load a preset configuration
5. Modify example code
6. Create simple LED patterns

### Advanced
7. Write your own 6502 assembly
8. Create custom devices
9. Build a complete project

---

## 🏆 Project Ideas

- **Blinky** - Flashing LEDs (5 min)
- **Temperature Monitor** - Read sensor, display on LCD (15 min)
- **Servo Tester** - Control servo with potentiometer (20 min)
- **Data Logger** - Log sensor data to file (30 min)
- **Robot Arm Pick-and-Place** - Automated task (1 hour)

---

## 💾 Save Your Work

```bash
# In hardware_setup tool:
Menu → 5 (Save Configuration) → myproject.conf

# Generate C code:
Menu → 8 (Generate Code)

# Copy to your project
```

---

## ❓ Quick Help

### "How do I add an LED?"
```
./hardware_setup → 1 (Add Device) → 1 (LED)
→ Name: "MyLED"
→ Address: 0x6001
→ Bit: 0
```

### "How do I read a sensor?"
```
./hardware_setup → 1 (Add Device) → 3 (Temperature)
→ Name: "TempSensor"
→ Channel: 0
→ Min: 0, Max: 100
```

### "How do I control a motor?"
```
./hardware_setup → 1 (Add Device) → 6 (DC Motor)
→ Name: "Motor1"
→ Channel: 0
```

---

## 🚀 You're Ready!

Everything is set up and ready to use. Start with:

1. **Play a game** - `./games/snake`
2. **Configure hardware** - `./emulator/hardware_setup`
3. **Run a demo** - `./emulator/gpio_devices`

Have fun building with the 6502! 🎉

---

**Questions? Check the full documentation in the repo!**
