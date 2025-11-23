/*
 * 6502 Device Library - Header File
 *
 * Provides drivers and functions for simulated peripherals:
 * - GPIO (LEDs, buttons, switches)
 * - ADC (Analog-to-Digital Converter - 8 channels)
 * - DAC (Digital-to-Analog Converter - 4 channels)
 * - PWM (Pulse Width Modulation)
 * - Timers
 * - Serial I/O
 *
 * Include this in your 6502 programs!
 */

#ifndef DEVICE_LIBRARY_H
#define DEVICE_LIBRARY_H

#include <stdint.h>

/* ============================================
 * MEMORY MAP - Where devices are located
 * ============================================ */

// GPIO Ports (VIA 6522)
#define PORTB       0x6000  // Port B (8-bit input/output)
#define PORTA       0x6001  // Port A (8-bit input/output)
#define DDRB        0x6002  // Data Direction Register B
#define DDRA        0x6003  // Data Direction Register A

// ADC (Analog-to-Digital Converter)
#define ADC_BASE    0x6010  // ADC base address
#define ADC_CH0     0x6010  // ADC Channel 0
#define ADC_CH1     0x6011  // ADC Channel 1
#define ADC_CH2     0x6012  // ADC Channel 2
#define ADC_CH3     0x6013  // ADC Channel 3
#define ADC_CH4     0x6014  // ADC Channel 4
#define ADC_CH5     0x6015  // ADC Channel 5
#define ADC_CH6     0x6016  // ADC Channel 6
#define ADC_CH7     0x6017  // ADC Channel 7
#define ADC_STATUS  0x6018  // ADC Status Register
#define ADC_CONTROL 0x6019  // ADC Control Register

// DAC (Digital-to-Analog Converter)
#define DAC_BASE    0x6020  // DAC base address
#define DAC_CH0     0x6020  // DAC Channel 0
#define DAC_CH1     0x6021  // DAC Channel 1
#define DAC_CH2     0x6022  // DAC Channel 2
#define DAC_CH3     0x6023  // DAC Channel 3
#define DAC_CONTROL 0x6024  // DAC Control Register

// PWM Controller
#define PWM_BASE    0x6030  // PWM base address
#define PWM_CH0     0x6030  // PWM Channel 0 duty cycle
#define PWM_CH1     0x6031  // PWM Channel 1 duty cycle
#define PWM_CH2     0x6032  // PWM Channel 2 duty cycle
#define PWM_CH3     0x6033  // PWM Channel 3 duty cycle
#define PWM_FREQ    0x6034  // PWM frequency divider

// Timer/Counter
#define TIMER_LOW   0x6040  // Timer counter low byte
#define TIMER_HIGH  0x6041  // Timer counter high byte
#define TIMER_CTRL  0x6042  // Timer control

// Serial I/O (UART)
#define UART_DATA   0x6050  // UART data register
#define UART_STATUS 0x6051  // UART status register
#define UART_CTRL   0x6052  // UART control register


/* ============================================
 * ADC (Analog-to-Digital Converter)
 * ============================================
 *
 * 8-bit ADC with 8 channels
 * Input range: 0.0V to 5.0V
 * Output: 0x00 to 0xFF (0-255)
 * Conversion time: ~100 CPU cycles
 */

typedef struct {
    uint8_t  channels[8];    // Current ADC values (0-255)
    float    voltages[8];    // Actual voltages (0.0-5.0V)
    uint8_t  status;         // Status register
    uint8_t  control;        // Control register
    uint8_t  current_ch;     // Currently selected channel
    int      converting;     // Conversion in progress
    int      conversion_cycles; // Cycles remaining
} ADC;

// ADC Status Register Bits
#define ADC_READY      0x01  // Conversion complete
#define ADC_BUSY       0x02  // Conversion in progress
#define ADC_OVERRUN    0x04  // Data overrun error

// ADC Control Register Bits
#define ADC_START      0x01  // Start conversion
#define ADC_CONTINUOUS 0x02  // Continuous mode
#define ADC_CHANNEL_MASK 0x07  // Channel select (bits 0-2)

void adc_init(ADC *adc);
void adc_update(ADC *adc);
uint8_t adc_read_channel(ADC *adc, uint8_t channel);
void adc_set_voltage(ADC *adc, uint8_t channel, float voltage);
float adc_get_voltage(ADC *adc, uint8_t channel);


/* ============================================
 * DAC (Digital-to-Analog Converter)
 * ============================================
 *
 * 8-bit DAC with 4 channels
 * Input: 0x00 to 0xFF (0-255)
 * Output range: 0.0V to 5.0V
 * Settling time: ~10 CPU cycles
 */

typedef struct {
    uint8_t  channels[4];    // DAC input values (0-255)
    float    voltages[4];    // Output voltages (0.0-5.0V)
    uint8_t  control;        // Control register
    int      settling[4];    // Settling time counters
} DAC;

// DAC Control Register Bits
#define DAC_ENABLE_CH0 0x01
#define DAC_ENABLE_CH1 0x02
#define DAC_ENABLE_CH2 0x04
#define DAC_ENABLE_CH3 0x08
#define DAC_POWER_DOWN 0x10

void dac_init(DAC *dac);
void dac_update(DAC *dac);
void dac_write_channel(DAC *dac, uint8_t channel, uint8_t value);
float dac_get_voltage(DAC *dac, uint8_t channel);
void dac_display(DAC *dac);


/* ============================================
 * PWM (Pulse Width Modulation)
 * ============================================
 *
 * 4-channel PWM generator
 * Duty cycle: 0-255 (0-100%)
 * Frequency: Configurable
 */

typedef struct {
    uint8_t  duty_cycle[4];  // PWM duty cycles (0-255)
    uint8_t  frequency;      // Frequency divider
    uint16_t counter;        // Internal counter
} PWM;

void pwm_init(PWM *pwm);
void pwm_update(PWM *pwm);
void pwm_set_duty(PWM *pwm, uint8_t channel, uint8_t duty);
void pwm_set_frequency(PWM *pwm, uint8_t freq_div);


/* ============================================
 * Common Device Operations
 * ============================================ */

// Read from device
uint8_t device_read(void *context, uint16_t address);

// Write to device
void device_write(void *context, uint16_t address, uint8_t value);

// Initialize all devices
void device_init_all(void *system);

// Update all devices (call every cycle)
void device_update_all(void *system);

// Display device states
void device_display_all(void *system);


/* ============================================
 * Utility Functions
 * ============================================ */

// Convert voltage to ADC value (0.0-5.0V → 0-255)
uint8_t voltage_to_adc(float voltage);

// Convert ADC value to voltage (0-255 → 0.0-5.0V)
float adc_to_voltage(uint8_t adc_value);

// Convert DAC value to voltage (0-255 → 0.0-5.0V)
float dac_to_voltage(uint8_t dac_value);

// Convert voltage to DAC value (0.0-5.0V → 0-255)
uint8_t voltage_to_dac(float voltage);


#endif /* DEVICE_LIBRARY_H */
