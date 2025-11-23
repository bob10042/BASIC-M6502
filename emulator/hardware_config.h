/*
 * Hardware Configuration System
 *
 * Easy interface for setting up simulated hardware devices
 * without writing C code. Just define your devices in a config
 * file and load them!
 *
 * Example:
 *   [LED0]
 *   type = LED
 *   address = 0x6001
 *   bit = 0
 *   color = red
 *
 *   [TempSensor]
 *   type = ADC
 *   address = 0x6010
 *   channel = 0
 *   min = 0
 *   max = 100
 */

#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

#include <stdint.h>

/* ============================================
 * DEVICE TYPES
 * ============================================ */

typedef enum {
    DEV_LED,              // Single LED
    DEV_LED_ARRAY,        // Array of LEDs
    DEV_BUTTON,           // Push button
    DEV_SWITCH,           // Toggle switch
    DEV_ADC_SENSOR,       // Generic ADC sensor
    DEV_TEMP_SENSOR,      // Temperature sensor
    DEV_LIGHT_SENSOR,     // Light sensor
    DEV_POT,              // Potentiometer
    DEV_DAC_OUTPUT,       // Generic DAC output
    DEV_SERVO,            // Servo motor
    DEV_DC_MOTOR,         // DC motor
    DEV_STEPPER,          // Stepper motor
    DEV_RELAY,            // Relay
    DEV_PUMP,             // Pump/valve
    DEV_HEATER,           // Heating element
    DEV_LCD,              // LCD display
    DEV_SEVEN_SEG,        // 7-segment display
    DEV_BUZZER,           // Piezo buzzer
    DEV_RGB_LED,          // RGB LED
    DEV_ULTRASONIC,       // Ultrasonic distance sensor
    DEV_CUSTOM            // Custom device
} DeviceType;

/* ============================================
 * DEVICE DEFINITION
 * ============================================ */

typedef struct {
    char name[32];        // Device name
    DeviceType type;      // Device type
    uint16_t address;     // Memory address
    uint8_t  channel;     // ADC/DAC channel
    uint8_t  bit;         // Bit number for GPIO
    float    min_value;   // Min value
    float    max_value;   // Max value
    char     color[16];   // Color (for LEDs)
    char     units[16];   // Units (°C, %, etc.)
    int      enabled;     // Is device active?
    void     *user_data;  // Custom data
} DeviceConfig;

/* ============================================
 * HARDWARE SETUP
 * ============================================ */

#define MAX_DEVICES 64

typedef struct {
    DeviceConfig devices[MAX_DEVICES];
    int num_devices;
    char config_file[256];
} HardwareSetup;

/* ============================================
 * CONFIGURATION FUNCTIONS
 * ============================================ */

// Initialize hardware setup
void hw_init(HardwareSetup *hw);

// Load configuration from file
int hw_load_config(HardwareSetup *hw, const char *filename);

// Save configuration to file
int hw_save_config(HardwareSetup *hw, const char *filename);

// Add a device
int hw_add_device(HardwareSetup *hw, DeviceConfig *dev);

// Remove a device
int hw_remove_device(HardwareSetup *hw, const char *name);

// Find device by name
DeviceConfig* hw_find_device(HardwareSetup *hw, const char *name);

// List all devices
void hw_list_devices(HardwareSetup *hw);

/* ============================================
 * QUICK SETUP HELPERS
 * ============================================ */

// Add LED
int hw_add_led(HardwareSetup *hw, const char *name, uint16_t addr,
               uint8_t bit, const char *color);

// Add button
int hw_add_button(HardwareSetup *hw, const char *name, uint16_t addr,
                  uint8_t bit);

// Add ADC sensor
int hw_add_adc_sensor(HardwareSetup *hw, const char *name, uint8_t channel,
                     float min, float max, const char *units);

// Add DAC output
int hw_add_dac_output(HardwareSetup *hw, const char *name, uint8_t channel,
                     float min, float max);

// Add servo
int hw_add_servo(HardwareSetup *hw, const char *name, uint8_t channel,
                float min_angle, float max_angle);

// Add motor
int hw_add_motor(HardwareSetup *hw, const char *name, uint8_t channel);

// Add relay
int hw_add_relay(HardwareSetup *hw, const char *name, uint16_t addr,
                uint8_t bit);

// Add pump
int hw_add_pump(HardwareSetup *hw, const char *name, uint8_t channel);

/* ============================================
 * INTERACTIVE SETUP
 * ============================================ */

// Interactive configuration wizard
void hw_interactive_setup(HardwareSetup *hw);

// Display setup menu
void hw_display_menu(HardwareSetup *hw);

// Wizard for adding device
void hw_add_device_wizard(HardwareSetup *hw);

/* ============================================
 * EXAMPLE CONFIGURATIONS
 * ============================================ */

// Load a preset configuration
int hw_load_preset(HardwareSetup *hw, const char *preset_name);

// Available presets:
//   "breadboard"    - Ben Eater style setup
//   "bbc_micro"     - BBC Micro peripherals
//   "robot_arm"     - 4-DOF robot configuration
//   "lab_bench"     - Lab equipment setup
//   "iot"           - IoT sensors and actuators
//   "automation"    - Home automation
//   "arcade"        - Arcade controls

/* ============================================
 * RUNTIME DEVICE ACCESS
 * ============================================ */

// Read device value
float hw_read_device(HardwareSetup *hw, const char *name);

// Write device value
void hw_write_device(HardwareSetup *hw, const char *name, float value);

// Get device status
const char* hw_get_status(HardwareSetup *hw, const char *name);


#endif /* HARDWARE_CONFIG_H */
