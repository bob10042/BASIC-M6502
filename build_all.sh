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
gcc -o bbc_micro bbc_micro_simulator.c && echo "✅ BBC Micro built"
gcc -o simulate_hardware simulate_hardware.c && echo "✅ Hardware simulator built"

echo ""
echo "======================================"
echo "✅ All builds complete!"
echo ""
echo "To run:"
echo "  Games:      cd games/ && ./snake"
echo "  Simulators: cd emulator/ && ./hardware_setup"
echo "  Robot:      cd emulator/ && ./robot_arm"
