/*
 * GPIO Device Simulation - LEDs, Sensors, and Custom Peripherals
 *
 * Demonstrates how to simulate:
 * - LEDs connected to output pins
 * - Sensors connected to input pins
 * - Buttons and switches
 * - Temperature sensors
 * - Light sensors
 * - Motors and relays
 * - Any custom device!
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

/* ============================================
 * SIMULATED DEVICES
 * ============================================ */

/* LED Array (8 LEDs on Port A) */
typedef struct {
    uint8_t state;      // 8 bits = 8 LEDs
    char    symbols[8]; // Visual representation
} LEDArray;

/* Button/Switch Array (8 buttons on Port B) */
typedef struct {
    uint8_t state;      // Current button states
    uint8_t pressed;    // Which buttons were just pressed
} ButtonArray;

/* Temperature Sensor (ADC) */
typedef struct {
    float temperature;  // Current temperature in °C
    uint8_t adc_value;  // 8-bit ADC reading (0-255)
} TempSensor;

/* Light Sensor (Photoresistor) */
typedef struct {
    float light_level;  // 0.0 to 1.0
    uint8_t adc_value;  // 8-bit ADC reading
} LightSensor;

/* Relay/Motor Controller */
typedef struct {
    uint8_t relay_state;  // Which relays are on
    uint8_t motor_speed;  // Motor speed (PWM)
} RelayController;

/* Complete GPIO System */
typedef struct {
    uint8_t ram[65536];

    /* VIA Chip (65C22) */
    uint8_t porta;      // Port A data
    uint8_t portb;      // Port B data
    uint8_t ddra;       // Data Direction Register A (1=output, 0=input)
    uint8_t ddrb;       // Data Direction Register B

    /* Devices */
    LEDArray        leds;
    ButtonArray     buttons;
    TempSensor      temp_sensor;
    LightSensor     light_sensor;
    RelayController relays;

    int running;
} GPIOSystem;


/* ============================================
 * LED SIMULATION
 * ============================================ */

void led_init(LEDArray *leds) {
    leds->state = 0;
    for (int i = 0; i < 8; i++) {
        leds->symbols[i] = '○';  // Off
    }
}

void led_update(LEDArray *leds, uint8_t state) {
    leds->state = state;

    // Update visual representation
    for (int i = 0; i < 8; i++) {
        if (state & (1 << i)) {
            leds->symbols[i] = '●';  // On (filled circle)
        } else {
            leds->symbols[i] = '○';  // Off (empty circle)
        }
    }
}

void led_display(LEDArray *leds) {
    printf("\n╔══════════════════════════════════════╗\n");
    printf("║         LED ARRAY (Port A)           ║\n");
    printf("╠══════════════════════════════════════╣\n");
    printf("║ ");

    // Show LEDs with colors
    for (int i = 7; i >= 0; i--) {
        if (leds->state & (1 << i)) {
            printf("\033[1;31m%c\033[0m ", leds->symbols[i]);  // Red when on
        } else {
            printf("\033[0;37m%c\033[0m ", leds->symbols[i]);  // Gray when off
        }
    }
    printf("║\n");

    // Show bit numbers
    printf("║ 7 6 5 4 3 2 1 0                     ║\n");

    // Show binary state
    printf("║ ");
    for (int i = 7; i >= 0; i--) {
        printf("%d ", (leds->state >> i) & 1);
    }
    printf("(0x%02X)          ║\n", leds->state);
    printf("╚══════════════════════════════════════╝\n");
    fflush(stdout);
}


/* ============================================
 * BUTTON/SWITCH SIMULATION
 * ============================================ */

void button_init(ButtonArray *buttons) {
    buttons->state = 0;
    buttons->pressed = 0;
}

void button_press(ButtonArray *buttons, int button_num) {
    buttons->state |= (1 << button_num);
    buttons->pressed |= (1 << button_num);
}

void button_release(ButtonArray *buttons, int button_num) {
    buttons->state &= ~(1 << button_num);
}

uint8_t button_read(ButtonArray *buttons) {
    uint8_t result = buttons->state;
    buttons->pressed = 0;  // Clear edge detection
    return result;
}

void button_display(ButtonArray *buttons) {
    printf("\n╔══════════════════════════════════════╗\n");
    printf("║      BUTTON ARRAY (Port B)           ║\n");
    printf("╠══════════════════════════════════════╣\n");
    printf("║ ");

    for (int i = 7; i >= 0; i--) {
        if (buttons->state & (1 << i)) {
            printf("\033[1;32m▣\033[0m ");  // Green when pressed
        } else {
            printf("\033[0;37m▢\033[0m ");  // Gray when not pressed
        }
    }
    printf("║\n");
    printf("║ 7 6 5 4 3 2 1 0                     ║\n");
    printf("╚══════════════════════════════════════╝\n");
    fflush(stdout);
}


/* ============================================
 * TEMPERATURE SENSOR SIMULATION
 * ============================================ */

void temp_sensor_init(TempSensor *sensor) {
    sensor->temperature = 25.0;  // Start at room temp
    sensor->adc_value = 128;     // Mid-range
}

void temp_sensor_update(TempSensor *sensor) {
    // Simulate temperature variation
    sensor->temperature += ((rand() % 100) - 50) / 100.0;

    // Clamp to reasonable range
    if (sensor->temperature < -40.0) sensor->temperature = -40.0;
    if (sensor->temperature > 125.0) sensor->temperature = 125.0;

    // Convert to ADC value (0-255 for -40°C to 125°C)
    sensor->adc_value = (uint8_t)((sensor->temperature + 40.0) * 255.0 / 165.0);
}

uint8_t temp_sensor_read(TempSensor *sensor) {
    temp_sensor_update(sensor);
    return sensor->adc_value;
}

void temp_sensor_display(TempSensor *sensor) {
    printf("\n╔══════════════════════════════════════╗\n");
    printf("║      TEMPERATURE SENSOR (ADC)        ║\n");
    printf("╠══════════════════════════════════════╣\n");
    printf("║  Temperature: %6.2f°C               ║\n", sensor->temperature);
    printf("║  ADC Value:   0x%02X (%3d)             ║\n",
           sensor->adc_value, sensor->adc_value);

    // Show bar graph
    printf("║  ");
    int bars = (int)(sensor->temperature + 40) / 4;  // Scale for display
    for (int i = 0; i < 40; i++) {
        if (i < bars) printf("█");
        else printf("░");
    }
    printf(" ║\n");
    printf("║  -40°C                        125°C  ║\n");
    printf("╚══════════════════════════════════════╝\n");
    fflush(stdout);
}


/* ============================================
 * LIGHT SENSOR SIMULATION
 * ============================================ */

void light_sensor_init(LightSensor *sensor) {
    sensor->light_level = 0.5;  // Medium brightness
    sensor->adc_value = 128;
}

void light_sensor_update(LightSensor *sensor) {
    // Simulate light level changes
    sensor->light_level += ((rand() % 100) - 50) / 1000.0;

    if (sensor->light_level < 0.0) sensor->light_level = 0.0;
    if (sensor->light_level > 1.0) sensor->light_level = 1.0;

    sensor->adc_value = (uint8_t)(sensor->light_level * 255.0);
}

uint8_t light_sensor_read(LightSensor *sensor) {
    light_sensor_update(sensor);
    return sensor->adc_value;
}

void light_sensor_display(LightSensor *sensor) {
    printf("\n╔══════════════════════════════════════╗\n");
    printf("║        LIGHT SENSOR (ADC)            ║\n");
    printf("╠══════════════════════════════════════╣\n");
    printf("║  Light Level: %5.1f%%                 ║\n",
           sensor->light_level * 100.0);
    printf("║  ADC Value:   0x%02X (%3d)             ║\n",
           sensor->adc_value, sensor->adc_value);

    printf("║  ");
    int bars = (int)(sensor->light_level * 40);
    for (int i = 0; i < 40; i++) {
        if (i < bars) printf("█");
        else printf("░");
    }
    printf(" ║\n");
    printf("╚══════════════════════════════════════╝\n");
    fflush(stdout);
}


/* ============================================
 * RELAY/MOTOR CONTROLLER SIMULATION
 * ============================================ */

void relay_init(RelayController *relay) {
    relay->relay_state = 0;
    relay->motor_speed = 0;
}

void relay_update(RelayController *relay, uint8_t state) {
    relay->relay_state = state & 0x0F;  // 4 relays
    relay->motor_speed = (state >> 4) * 17;  // 4 bits = 0-255
}

void relay_display(RelayController *relay) {
    printf("\n╔══════════════════════════════════════╗\n");
    printf("║      RELAY CONTROLLER (Port C)       ║\n");
    printf("╠══════════════════════════════════════╣\n");

    for (int i = 0; i < 4; i++) {
        if (relay->relay_state & (1 << i)) {
            printf("║  Relay %d: \033[1;32mON\033[0m  ⚡                   ║\n", i + 1);
        } else {
            printf("║  Relay %d: \033[0;37mOFF\033[0m                        ║\n", i + 1);
        }
    }

    printf("╠══════════════════════════════════════╣\n");
    printf("║  Motor Speed: %3d%%                    ║\n",
           (relay->motor_speed * 100) / 255);
    printf("║  ");
    int bars = (relay->motor_speed * 38) / 255;
    for (int i = 0; i < 38; i++) {
        if (i < bars) printf("█");
        else printf("░");
    }
    printf(" ║\n");
    printf("╚══════════════════════════════════════╝\n");
    fflush(stdout);
}


/* ============================================
 * MEMORY-MAPPED I/O
 * ============================================ */

uint8_t gpio_read(void *context, uint16_t address) {
    GPIOSystem *sys = (GPIOSystem *)context;

    if (address < 0x8000) {
        /* VIA Registers: 0x6000-0x600F */
        if (address >= 0x6000 && address <= 0x600F) {
            switch(address & 0x0F) {
                case 0x00:  // Port B (input - buttons)
                    return button_read(&sys->buttons);

                case 0x01:  // Port A (output - reflected back for testing)
                    return sys->porta;

                case 0x02:  // DDRB
                    return sys->ddrb;

                case 0x03:  // DDRA
                    return sys->ddra;

                case 0x04:  // Temperature sensor ADC
                    return temp_sensor_read(&sys->temp_sensor);

                case 0x05:  // Light sensor ADC
                    return light_sensor_read(&sys->light_sensor);

                default:
                    return 0;
            }
        }

        return sys->ram[address];
    }

    return 0xFF;  // ROM area (not used in this demo)
}

void gpio_write(void *context, uint16_t address, uint8_t value) {
    GPIOSystem *sys = (GPIOSystem *)context;

    if (address < 0x8000) {
        /* VIA Registers: 0x6000-0x600F */
        if (address >= 0x6000 && address <= 0x600F) {
            switch(address & 0x0F) {
                case 0x00:  // Port B (input - ignored)
                    sys->portb = value;
                    break;

                case 0x01:  // Port A (output - LEDs)
                    sys->porta = value;
                    led_update(&sys->leds, value);
                    led_display(&sys->leds);
                    break;

                case 0x02:  // DDRB
                    sys->ddrb = value;
                    break;

                case 0x03:  // DDRA
                    sys->ddra = value;
                    break;

                case 0x08:  // Relay controller
                    relay_update(&sys->relays, value);
                    relay_display(&sys->relays);
                    break;
            }
            return;
        }

        sys->ram[address] = value;
    }
}


/* ============================================
 * SYSTEM INITIALIZATION & DEMO
 * ============================================ */

void gpio_system_init(GPIOSystem *sys) {
    memset(sys->ram, 0, sizeof(sys->ram));

    led_init(&sys->leds);
    button_init(&sys->buttons);
    temp_sensor_init(&sys->temp_sensor);
    light_sensor_init(&sys->light_sensor);
    relay_init(&sys->relays);

    sys->porta = 0;
    sys->portb = 0;
    sys->ddra = 0xFF;  // Port A = output (LEDs)
    sys->ddrb = 0x00;  // Port B = input (buttons)

    sys->running = 1;

    srand(time(NULL));
}

/* ============================================
 * DEMO PROGRAM
 * ============================================ */

int main(void) {
    GPIOSystem sys;

    printf("\n");
    printf("╔════════════════════════════════════════════════════╗\n");
    printf("║   6502 GPIO DEVICE SIMULATOR                       ║\n");
    printf("║   LEDs, Sensors, Buttons, Relays, and More!        ║\n");
    printf("╚════════════════════════════════════════════════════╝\n");

    gpio_system_init(&sys);

    printf("\n\nRunning automated demo...\n");
    printf("(In real 6502 code, you'd write to 0x6001 for LEDs,\n");
    printf(" read from 0x6000 for buttons, etc.)\n");
    printf("\nPress Ctrl+C to stop.\n\n");

    // Demo: Simulate 6502 controlling devices
    for (int step = 0; step < 50 && sys.running; step++) {
        usleep(500000);  // 500ms delay

        printf("\033[2J\033[H");  // Clear screen
        printf("╔════════════════════════════════════════════════════╗\n");
        printf("║   STEP %2d/50 - GPIO Device Simulation             ║\n", step + 1);
        printf("╚════════════════════════════════════════════════════╝\n");

        // 1. Update LEDs (simulate 6502 writing to Port A)
        uint8_t led_pattern = (1 << (step % 8));  // Rotating LED
        gpio_write(&sys, 0x6001, led_pattern);

        // 2. Simulate button presses
        if (step % 5 == 0) {
            button_press(&sys.buttons, step % 8);
        } else {
            button_release(&sys.buttons, (step - 1) % 8);
        }
        button_display(&sys.buttons);

        // 3. Update temperature sensor
        temp_sensor_display(&sys.temp_sensor);

        // 4. Update light sensor
        light_sensor_display(&sys.light_sensor);

        // 5. Update relay controller
        uint8_t relay_state = (step / 5) % 16;  // Cycle through relay patterns
        gpio_write(&sys, 0x6008, relay_state);

        printf("\n6502 Assembly Equivalent:\n");
        printf("  LDA #$%02X        ; Load LED pattern\n", led_pattern);
        printf("  STA $6001       ; Write to Port A (LEDs)\n");
        printf("  LDA $6000       ; Read Port B (buttons)\n");
        printf("  LDA $6004       ; Read temperature sensor\n");
        printf("  LDA $6005       ; Read light sensor\n");
        printf("  LDA #$%02X        ; Load relay pattern\n", relay_state);
        printf("  STA $6008       ; Write to relay controller\n\n");
    }

    printf("\n\n╔════════════════════════════════════════════════════╗\n");
    printf("║   Demo Complete!                                   ║\n");
    printf("╚════════════════════════════════════════════════════╝\n\n");

    printf("Device Summary:\n");
    printf("  📺 LEDs - Output on Port A (0x6001)\n");
    printf("  🔘 Buttons - Input on Port B (0x6000)\n");
    printf("  🌡️  Temperature - ADC input (0x6004)\n");
    printf("  💡 Light Sensor - ADC input (0x6005)\n");
    printf("  ⚡ Relays - Output (0x6008)\n\n");

    printf("Add your own devices by:\n");
    printf("  1. Define device structure\n");
    printf("  2. Add update functions\n");
    printf("  3. Map to memory address\n");
    printf("  4. Handle in gpio_read/write\n\n");

    return 0;
}
