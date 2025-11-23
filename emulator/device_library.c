/*
 * 6502 Device Library - Implementation
 *
 * Full simulation of ADC, DAC, PWM, and GPIO devices
 */

#include "device_library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ============================================
 * ADC IMPLEMENTATION
 * ============================================ */

void adc_init(ADC *adc) {
    memset(adc->channels, 0, sizeof(adc->channels));
    memset(adc->voltages, 0, sizeof(adc->voltages));

    adc->status = ADC_READY;
    adc->control = 0;
    adc->current_ch = 0;
    adc->converting = 0;
    adc->conversion_cycles = 0;

    // Set some default voltages for testing
    adc->voltages[0] = 2.5;   // Mid-range
    adc->voltages[1] = 1.0;   // Low
    adc->voltages[2] = 4.0;   // High
    adc->voltages[3] = 0.0;   // Ground
}

void adc_update(ADC *adc) {
    // Simulate conversion time
    if (adc->converting) {
        adc->conversion_cycles--;
        if (adc->conversion_cycles <= 0) {
            // Conversion complete
            uint8_t ch = adc->current_ch;
            adc->channels[ch] = voltage_to_adc(adc->voltages[ch]);
            adc->status = ADC_READY;
            adc->converting = 0;

            // Continuous mode?
            if (adc->control & ADC_CONTINUOUS) {
                adc->current_ch = (adc->current_ch + 1) % 8;
                adc->converting = 1;
                adc->conversion_cycles = 100;
                adc->status = ADC_BUSY;
            }
        } else {
            adc->status = ADC_BUSY;
        }
    }
}

uint8_t adc_read_channel(ADC *adc, uint8_t channel) {
    if (channel >= 8) return 0;

    // Start conversion if not busy
    if (!adc->converting) {
        adc->current_ch = channel;
        adc->converting = 1;
        adc->conversion_cycles = 100;  // ~100 cycles
        adc->status = ADC_BUSY;
    }

    return adc->channels[channel];
}

void adc_set_voltage(ADC *adc, uint8_t channel, float voltage) {
    if (channel >= 8) return;

    // Clamp to 0-5V range
    if (voltage < 0.0) voltage = 0.0;
    if (voltage > 5.0) voltage = 5.0;

    adc->voltages[channel] = voltage;
    adc->channels[channel] = voltage_to_adc(voltage);
}

float adc_get_voltage(ADC *adc, uint8_t channel) {
    if (channel >= 8) return 0.0;
    return adc->voltages[channel];
}


/* ============================================
 * DAC IMPLEMENTATION
 * ============================================ */

void dac_init(DAC *dac) {
    memset(dac->channels, 0, sizeof(dac->channels));
    memset(dac->voltages, 0, sizeof(dac->voltages));
    memset(dac->settling, 0, sizeof(dac->settling));

    dac->control = 0x0F;  // All channels enabled
}

void dac_update(DAC *dac) {
    // Update settling times
    for (int i = 0; i < 4; i++) {
        if (dac->settling[i] > 0) {
            dac->settling[i]--;
        }
    }

    // Update output voltages
    for (int i = 0; i < 4; i++) {
        if (dac->control & (1 << i)) {  // Channel enabled?
            dac->voltages[i] = dac_to_voltage(dac->channels[i]);
        } else {
            dac->voltages[i] = 0.0;  // Powered down
        }
    }
}

void dac_write_channel(DAC *dac, uint8_t channel, uint8_t value) {
    if (channel >= 4) return;

    dac->channels[channel] = value;
    dac->settling[channel] = 10;  // 10 cycles settling time
}

float dac_get_voltage(DAC *dac, uint8_t channel) {
    if (channel >= 4) return 0.0;
    return dac->voltages[channel];
}

void dac_display(DAC *dac) {
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║     DAC (Digital-to-Analog Converter)                ║\n");
    printf("╠══════════════════════════════════════════════════════╣\n");

    for (int i = 0; i < 4; i++) {
        printf("║  CH%d: 0x%02X (%3d) → %5.3fV  ", i, dac->channels[i],
               dac->channels[i], dac->voltages[i]);

        // Bar graph
        int bars = (int)(dac->voltages[i] * 10.0);
        printf("[");
        for (int j = 0; j < 10; j++) {
            if (j < bars) printf("█");
            else printf("░");
        }
        printf("]");

        if (dac->settling[i] > 0) {
            printf(" ⟳");  // Settling
        }
        printf(" ║\n");
    }

    printf("║  Control: 0x%02X  ", dac->control);
    for (int i = 0; i < 4; i++) {
        if (dac->control & (1 << i)) {
            printf("CH%d:ON ", i);
        } else {
            printf("CH%d:OFF ", i);
        }
    }
    printf("║\n");
    printf("╚══════════════════════════════════════════════════════╝\n");
}


/* ============================================
 * PWM IMPLEMENTATION
 * ============================================ */

void pwm_init(PWM *pwm) {
    memset(pwm->duty_cycle, 0, sizeof(pwm->duty_cycle));
    pwm->frequency = 1;
    pwm->counter = 0;
}

void pwm_update(PWM *pwm) {
    pwm->counter++;
    // PWM output updates would happen here
}

void pwm_set_duty(PWM *pwm, uint8_t channel, uint8_t duty) {
    if (channel >= 4) return;
    pwm->duty_cycle[channel] = duty;
}

void pwm_set_frequency(PWM *pwm, uint8_t freq_div) {
    pwm->frequency = freq_div;
}


/* ============================================
 * UTILITY FUNCTIONS
 * ============================================ */

uint8_t voltage_to_adc(float voltage) {
    if (voltage < 0.0) voltage = 0.0;
    if (voltage > 5.0) voltage = 5.0;
    return (uint8_t)((voltage / 5.0) * 255.0);
}

float adc_to_voltage(uint8_t adc_value) {
    return (float)adc_value * 5.0 / 255.0;
}

float dac_to_voltage(uint8_t dac_value) {
    return (float)dac_value * 5.0 / 255.0;
}

uint8_t voltage_to_dac(float voltage) {
    if (voltage < 0.0) voltage = 0.0;
    if (voltage > 5.0) voltage = 5.0;
    return (uint8_t)((voltage / 5.0) * 255.0);
}
