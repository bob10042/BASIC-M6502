/*
 * Voltage Source and Signal Generator Library
 *
 * Simulates various voltage sources that can be connected to ADC/DAC:
 * - DC voltage sources (power supplies)
 * - AC signal generators (sine, square, triangle, sawtooth)
 * - Noise generators (white noise, random)
 * - Function generators (sweeps, bursts)
 * - Sensor simulators (temperature, light, pressure)
 * - Interactive voltage control
 *
 * These can be connected to ADC inputs or controlled by DAC outputs!
 */

#ifndef VOLTAGE_SOURCE_H
#define VOLTAGE_SOURCE_H

#include <stdint.h>
#include <math.h>

/* ============================================
 * VOLTAGE SOURCE TYPES
 * ============================================ */

typedef enum {
    VS_DC,              // Constant DC voltage
    VS_SINE,            // Sine wave
    VS_SQUARE,          // Square wave
    VS_TRIANGLE,        // Triangle wave
    VS_SAWTOOTH,        // Sawtooth wave
    VS_NOISE,           // Random noise
    VS_PULSE,           // Pulse train
    VS_SWEEP,           // Frequency sweep
    VS_SENSOR,          // Sensor simulation
    VS_INTERACTIVE      // User-controlled
} VoltageSourceType;

/* ============================================
 * VOLTAGE SOURCE STRUCTURE
 * ============================================ */

typedef struct {
    VoltageSourceType type;
    float voltage;          // Current output voltage (0-5V)
    float amplitude;        // Signal amplitude
    float offset;           // DC offset
    float frequency;        // Frequency (Hz)
    float phase;            // Phase (radians)
    float duty_cycle;       // Duty cycle for pulse/square (0-1)

    // Internal state
    float time;             // Current time
    float time_step;        // Time increment per update
    float sweep_start_freq; // Sweep start frequency
    float sweep_end_freq;   // Sweep end frequency
    float sweep_rate;       // Sweep rate

    // Sensor simulation
    float sensor_min;       // Minimum sensor value
    float sensor_max;       // Maximum sensor value
    float sensor_rate;      // Change rate

    // Connection
    int connected_to_adc;   // Connected to ADC channel (-1 if not)
    int connected_to_dac;   // Controlled by DAC channel (-1 if not)

    // Statistics
    float min_voltage;      // Recorded minimum
    float max_voltage;      // Recorded maximum
    float avg_voltage;      // Average voltage
    int   samples;          // Number of samples
} VoltageSource;


/* ============================================
 * SIGNAL GENERATION FUNCTIONS
 * ============================================ */

// Initialize a voltage source
void vs_init(VoltageSource *vs, VoltageSourceType type);

// Update voltage source (call each cycle)
void vs_update(VoltageSource *vs);

// Get current voltage
float vs_get_voltage(VoltageSource *vs);

// Set voltage (for DC and interactive sources)
void vs_set_voltage(VoltageSource *vs, float voltage);

// Configure signal parameters
void vs_set_frequency(VoltageSource *vs, float frequency);
void vs_set_amplitude(VoltageSource *vs, float amplitude);
void vs_set_offset(VoltageSource *vs, float offset);
void vs_set_duty_cycle(VoltageSource *vs, float duty);

// Configure sweep
void vs_set_sweep(VoltageSource *vs, float start_freq, float end_freq, float rate);

// Configure sensor simulation
void vs_set_sensor_range(VoltageSource *vs, float min, float max, float rate);

// Reset source
void vs_reset(VoltageSource *vs);

// Display source info
void vs_display(VoltageSource *vs, const char *name);


/* ============================================
 * CIRCUIT CONNECTIONS
 * ============================================ */

// Connect voltage source to ADC channel
void vs_connect_to_adc(VoltageSource *vs, void *adc, int channel);

// Connect voltage source to be controlled by DAC channel
void vs_connect_to_dac(VoltageSource *vs, void *dac, int channel);

// Disconnect source
void vs_disconnect(VoltageSource *vs);


/* ============================================
 * MULTI-SOURCE MANAGER
 * ============================================ */

#define MAX_SOURCES 16

typedef struct {
    VoltageSource sources[MAX_SOURCES];
    int num_sources;
} VoltageSourceBank;

// Initialize source bank
void vsb_init(VoltageSourceBank *bank);

// Add a source
int vsb_add_source(VoltageSourceBank *bank, VoltageSourceType type);

// Update all sources
void vsb_update_all(VoltageSourceBank *bank);

// Display all sources
void vsb_display_all(VoltageSourceBank *bank);

// Get source by index
VoltageSource* vsb_get_source(VoltageSourceBank *bank, int index);


/* ============================================
 * UTILITY FUNCTIONS
 * ============================================ */

// Generate specific waveforms
float generate_sine(float time, float freq, float amp, float offset);
float generate_square(float time, float freq, float amp, float offset, float duty);
float generate_triangle(float time, float freq, float amp, float offset);
float generate_sawtooth(float time, float freq, float amp, float offset);
float generate_noise(float amp, float offset);

// Clamp voltage to 0-5V range
float clamp_voltage(float voltage);


#endif /* VOLTAGE_SOURCE_H */
