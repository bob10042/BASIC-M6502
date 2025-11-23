/*
 * ADC/DAC Complete Demonstration
 *
 * Shows a practical example: Temperature Control System
 * - ADC reads temperature sensor
 * - Software controls heater via DAC
 * - Simulates PID control loop
 * - Displays real-time graphs
 *
 * This demonstrates how 6502 programs can interface with analog I/O!
 */

#include "device_library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

/* ============================================
 * COMPLETE SYSTEM
 * ============================================ */

typedef struct {
    uint8_t ram[65536];

    // Analog devices
    ADC adc;
    DAC dac;
    PWM pwm;

    // GPIO
    uint8_t porta, portb;
    uint8_t ddra, ddrb;

    // System state
    int running;
    int cycles;

    // Simulation state
    float room_temp;      // Ambient temperature
    float object_temp;    // Object being heated
    float heater_power;   // Heater output (0-1)
    float setpoint;       // Desired temperature
} AnalogSystem;


/* ============================================
 * PHYSICS SIMULATION
 * ============================================ */

void simulate_physics(AnalogSystem *sys) {
    // Simple thermal model
    float heat_transfer_rate = 0.1;   // Heat loss to environment
    float heater_efficiency = 0.5;    // Heater effectiveness

    // Heat from heater (DAC channel 0 controls heater)
    sys->heater_power = dac_get_voltage(&sys->dac, 0) / 5.0;  // 0-1 range
    float heat_in = sys->heater_power * heater_efficiency * 50.0;  // Max 50°C/sec

    // Heat loss to environment
    float heat_out = (sys->object_temp - sys->room_temp) * heat_transfer_rate;

    // Update temperature
    sys->object_temp += (heat_in - heat_out) * 0.01;  // Time step

    // Clamp temperature
    if (sys->object_temp < sys->room_temp) sys->object_temp = sys->room_temp;
    if (sys->object_temp > 200.0) sys->object_temp = 200.0;

    // Update ADC with current temperature
    // Map 0-100°C to 0-5V
    float temp_voltage = (sys->object_temp / 100.0) * 5.0;
    adc_set_voltage(&sys->adc, 0, temp_voltage);

    // Add some noise to other ADC channels
    adc_set_voltage(&sys->adc, 1, 2.5 + (rand() % 100) / 1000.0);  // Noise
    adc_set_voltage(&sys->adc, 2, sys->heater_power * 5.0);        // Heater monitor
    adc_set_voltage(&sys->adc, 3, sys->room_temp / 100.0 * 5.0);   // Ambient
}


/* ============================================
 * MEMORY-MAPPED I/O
 * ============================================ */

uint8_t analog_read(void *context, uint16_t address) {
    AnalogSystem *sys = (AnalogSystem *)context;

    if (address < 0x8000) {
        // ADC channels: 0x6010-0x6017
        if (address >= ADC_BASE && address < ADC_BASE + 8) {
            return adc_read_channel(&sys->adc, address - ADC_BASE);
        }

        // ADC status: 0x6018
        if (address == ADC_STATUS) {
            return sys->adc.status;
        }

        // DAC readback
        if (address >= DAC_BASE && address < DAC_BASE + 4) {
            return sys->dac.channels[address - DAC_BASE];
        }

        // GPIO
        if (address == PORTA) return sys->porta;
        if (address == PORTB) return sys->portb;
        if (address == DDRA) return sys->ddra;
        if (address == DDRB) return sys->ddrb;

        return sys->ram[address];
    }

    return 0xFF;
}

void analog_write(void *context, uint16_t address, uint8_t value) {
    AnalogSystem *sys = (AnalogSystem *)context;

    if (address < 0x8000) {
        // DAC channels: 0x6020-0x6023
        if (address >= DAC_BASE && address < DAC_BASE + 4) {
            dac_write_channel(&sys->dac, address - DAC_BASE, value);
            return;
        }

        // DAC control: 0x6024
        if (address == DAC_CONTROL) {
            sys->dac.control = value;
            return;
        }

        // ADC control: 0x6019
        if (address == ADC_CONTROL) {
            sys->adc.control = value;
            return;
        }

        // PWM channels: 0x6030-0x6033
        if (address >= PWM_BASE && address < PWM_BASE + 4) {
            pwm_set_duty(&sys->pwm, address - PWM_BASE, value);
            return;
        }

        // GPIO
        if (address == PORTA) { sys->porta = value; return; }
        if (address == PORTB) { sys->portb = value; return; }
        if (address == DDRA) { sys->ddra = value; return; }
        if (address == DDRB) { sys->ddrb = value; return; }

        sys->ram[address] = value;
    }
}


/* ============================================
 * DISPLAY FUNCTIONS
 * ============================================ */

void display_temperature_graph(AnalogSystem *sys) {
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║        TEMPERATURE CONTROL SYSTEM                    ║\n");
    printf("╠══════════════════════════════════════════════════════╣\n");

    printf("║  Current Temp:  %6.2f°C  ", sys->object_temp);
    int temp_bar = (int)(sys->object_temp / 2.0);
    printf("[");
    for (int i = 0; i < 20; i++) {
        if (i < temp_bar) printf("█");
        else printf("░");
    }
    printf("]  ║\n");

    printf("║  Setpoint:      %6.2f°C                            ║\n", sys->setpoint);
    printf("║  Ambient:       %6.2f°C                            ║\n", sys->room_temp);
    printf("║  Heater Power:  %6.1f%%                             ║\n",
           sys->heater_power * 100.0);
    printf("╚══════════════════════════════════════════════════════╝\n");
}

void display_adc_channels(AnalogSystem *sys) {
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║     ADC CHANNELS (8-bit, 0-5V range)                ║\n");
    printf("╠══════════════════════════════════════════════════════╣\n");

    const char *ch_names[] = {
        "Temperature Sensor",
        "Noise Generator  ",
        "Heater Monitor   ",
        "Ambient Sensor   ",
        "Analog Input 4   ",
        "Analog Input 5   ",
        "Analog Input 6   ",
        "Analog Input 7   "
    };

    for (int i = 0; i < 8; i++) {
        uint8_t val = sys->adc.channels[i];
        float volt = adc_get_voltage(&sys->adc, i);

        printf("║  CH%d: %s  %5.3fV  0x%02X (%3d)  ║\n",
               i, ch_names[i], volt, val, val);
    }

    printf("║  Status: ");
    if (sys->adc.status & ADC_BUSY) printf("BUSY ");
    if (sys->adc.status & ADC_READY) printf("READY ");
    printf("                             ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n");
}


/* ============================================
 * SIMPLE PID CONTROLLER (Software)
 * ============================================ */

float pid_control(float setpoint, float current, float *integral, float *prev_error) {
    float kp = 2.0;    // Proportional gain
    float ki = 0.1;    // Integral gain
    float kd = 0.5;    // Derivative gain

    float error = setpoint - current;
    *integral += error;

    // Anti-windup
    if (*integral > 100.0) *integral = 100.0;
    if (*integral < -100.0) *integral = -100.0;

    float derivative = error - *prev_error;
    *prev_error = error;

    float output = kp * error + ki * (*integral) + kd * derivative;

    // Clamp to 0-255 (DAC range)
    if (output < 0) output = 0;
    if (output > 255) output = 255;

    return output;
}


/* ============================================
 * MAIN SIMULATION
 * ============================================ */

void analog_system_init(AnalogSystem *sys) {
    memset(sys->ram, 0, sizeof(sys->ram));

    adc_init(&sys->adc);
    dac_init(&sys->dac);
    pwm_init(&sys->pwm);

    sys->porta = 0;
    sys->portb = 0;
    sys->ddra = 0xFF;
    sys->ddrb = 0x00;

    sys->running = 1;
    sys->cycles = 0;

    sys->room_temp = 25.0;    // Room temperature
    sys->object_temp = 25.0;  // Start at room temp
    sys->heater_power = 0.0;
    sys->setpoint = 60.0;     // Target: 60°C

    srand(time(NULL));
}

int main(void) {
    AnalogSystem sys;
    float integral = 0.0;
    float prev_error = 0.0;

    printf("\n");
    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║   ADC/DAC TEMPERATURE CONTROL DEMONSTRATION            ║\n");
    printf("║   Simulates a 6502-based temperature controller        ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n");

    analog_system_init(&sys);

    printf("\n\nSystem Configuration:\n");
    printf("  ADC: 8 channels, 8-bit, 0-5V range\n");
    printf("  DAC: 4 channels, 8-bit, 0-5V range\n");
    printf("  PWM: 4 channels, 8-bit resolution\n");
    printf("  Control: PID algorithm running in software\n\n");

    printf("Scenario: Heating an object from 25°C to 60°C\n");
    printf("The 6502 reads temperature via ADC and controls heater via DAC\n\n");
    printf("Press Ctrl+C to stop.\n\n");

    sleep(2);

    // Main control loop
    for (int step = 0; step < 100 && sys.running; step++) {
        // Clear screen
        printf("\033[2J\033[H");

        printf("╔════════════════════════════════════════════════════════╗\n");
        printf("║   STEP %3d/100 - Real-time Control Loop               ║\n", step + 1);
        printf("╚════════════════════════════════════════════════════════╝\n");

        // 1. Read temperature from ADC (simulate 6502 code)
        uint8_t temp_adc = analog_read(&sys, ADC_CH0);
        float temp_celsius = (adc_to_voltage(temp_adc) / 5.0) * 100.0;

        // 2. Run PID controller (simulate 6502 algorithm)
        float control_output = pid_control(sys.setpoint, temp_celsius,
                                          &integral, &prev_error);

        // 3. Write to DAC (simulate 6502 code)
        analog_write(&sys, DAC_CH0, (uint8_t)control_output);

        // 4. Update hardware simulation
        adc_update(&sys.adc);
        dac_update(&sys.dac);
        simulate_physics(&sys);

        // 5. Display
        display_temperature_graph(&sys);
        display_adc_channels(&sys);
        dac_display(&sys.dac);

        // 6502 Assembly equivalent
        printf("\n6502 Code Equivalent:\n");
        printf("  ; Read temperature\n");
        printf("  LDA $%04X       ; Read ADC channel 0\n", ADC_CH0);
        printf("  STA temp       ; Store in variable\n\n");
        printf("  ; Calculate control output (PID algorithm)\n");
        printf("  JSR pid_control\n\n");
        printf("  ; Output to heater DAC\n");
        printf("  LDA control_out\n");
        printf("  STA $%04X       ; Write to DAC channel 0\n", DAC_CH0);
        printf("\n");

        // Check if at setpoint
        if (fabs(sys.object_temp - sys.setpoint) < 1.0) {
            printf("✅ Target temperature reached!\n");
        }

        usleep(200000);  // 200ms delay

        sys.cycles += 1000;  // Simulate 1000 CPU cycles per step
    }

    printf("\n\n╔════════════════════════════════════════════════════════╗\n");
    printf("║   Simulation Complete!                                 ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n\n");

    printf("Summary:\n");
    printf("  Total Cycles: %d\n", sys.cycles);
    printf("  Final Temperature: %.2f°C\n", sys.object_temp);
    printf("  Setpoint: %.2f°C\n", sys.setpoint);
    printf("  Error: %.2f°C\n\n", sys.setpoint - sys.object_temp);

    printf("This demonstrates:\n");
    printf("  ✓ ADC reading analog sensors\n");
    printf("  ✓ DAC controlling analog outputs\n");
    printf("  ✓ Software control algorithms (PID)\n");
    printf("  ✓ Real-time control systems\n");
    printf("  ✓ Memory-mapped analog I/O\n\n");

    printf("You can use these devices in your 6502 programs!\n");
    printf("See device_library.h for the complete API.\n\n");

    return 0;
}
