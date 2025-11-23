/*
 * Hardware Setup Tool - Visual Configuration
 *
 * Easy way to set up simulated hardware:
 * - LEDs, buttons, sensors, motors, etc.
 * - Drag-and-drop style configuration
 * - Live preview
 * - Save/load configurations
 *
 * Usage: ./hardware_setup [config_file]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PORTS 256

typedef enum {
    DEV_NONE = 0,
    DEV_LED,
    DEV_BUTTON,
    DEV_TEMP_SENSOR,
    DEV_LIGHT_SENSOR,
    DEV_SERVO,
    DEV_MOTOR,
    DEV_RELAY,
    DEV_PUMP,
    DEV_LCD,
    DEV_BUZZER,
    DEV_ADC,
    DEV_DAC,
    DEV_UART
} DeviceType;

typedef struct {
    DeviceType type;
    char name[32];
    int port;
    int channel;
    float min_val;
    float max_val;
    char units[16];
    int enabled;
} Device;

typedef struct {
    Device devices[MAX_PORTS];
    int num_devices;
} HardwareConfig;

const char* device_names[] = {
    "NONE", "LED", "BUTTON", "TEMP_SENSOR", "LIGHT_SENSOR",
    "SERVO", "MOTOR", "RELAY", "PUMP", "LCD", "BUZZER",
    "ADC", "DAC", "UART"
};

void init_config(HardwareConfig *cfg) {
    cfg->num_devices = 0;
    memset(cfg->devices, 0, sizeof(cfg->devices));
}

void display_banner() {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║       6502 HARDWARE SETUP - SIMULATION CONFIGURATOR        ║\n");
    printf("║       Easy device configuration for your 6502 system       ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

void display_menu() {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║                      MAIN MENU                             ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  1. Add Device                                             ║\n");
    printf("║  2. Remove Device                                          ║\n");
    printf("║  3. List All Devices                                       ║\n");
    printf("║  4. Load Preset Configuration                              ║\n");
    printf("║  5. Save Configuration                                     ║\n");
    printf("║  6. Load Configuration                                     ║\n");
    printf("║  7. Test Hardware (Live Demo)                              ║\n");
    printf("║  8. Generate C Code                                        ║\n");
    printf("║  9. Exit                                                   ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\nChoice: ");
}

void display_device_menu() {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║                  AVAILABLE DEVICES                         ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  1. LED              - Single LED light                    ║\n");
    printf("║  2. Button           - Push button input                   ║\n");
    printf("║  3. Temperature      - Temperature sensor (ADC)            ║\n");
    printf("║  4. Light Sensor     - Light level sensor (ADC)            ║\n");
    printf("║  5. Servo Motor      - Position servo (PWM/DAC)            ║\n");
    printf("║  6. DC Motor         - Speed-controlled motor (PWM)        ║\n");
    printf("║  7. Relay            - On/Off relay switch                 ║\n");
    printf("║  8. Pump/Valve       - Fluid control (PWM)                 ║\n");
    printf("║  9. LCD Display      - Character display                   ║\n");
    printf("║ 10. Buzzer           - Sound output                        ║\n");
    printf("║ 11. Generic ADC      - Custom analog input                ║\n");
    printf("║ 12. Generic DAC      - Custom analog output               ║\n");
    printf("║ 13. UART Serial      - Serial communication                ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\nDevice type (1-13): ");
}

void add_device_wizard(HardwareConfig *cfg) {
    Device dev = {0};
    int choice;

    display_device_menu();
    scanf("%d", &choice);
    getchar();

    switch(choice) {
        case 1: dev.type = DEV_LED; break;
        case 2: dev.type = DEV_BUTTON; break;
        case 3: dev.type = DEV_TEMP_SENSOR; break;
        case 4: dev.type = DEV_LIGHT_SENSOR; break;
        case 5: dev.type = DEV_SERVO; break;
        case 6: dev.type = DEV_MOTOR; break;
        case 7: dev.type = DEV_RELAY; break;
        case 8: dev.type = DEV_PUMP; break;
        case 9: dev.type = DEV_LCD; break;
        case 10: dev.type = DEV_BUZZER; break;
        case 11: dev.type = DEV_ADC; break;
        case 12: dev.type = DEV_DAC; break;
        case 13: dev.type = DEV_UART; break;
        default:
            printf("Invalid choice!\n");
            return;
    }

    printf("\nDevice name: ");
    fgets(dev.name, sizeof(dev.name), stdin);
    dev.name[strcspn(dev.name, "\n")] = 0;

    printf("Memory address (hex, e.g., 0x6001): ");
    scanf("%x", &dev.port);

    if (dev.type == DEV_ADC || dev.type == DEV_DAC ||
        dev.type == DEV_SERVO || dev.type == DEV_MOTOR ||
        dev.type == DEV_TEMP_SENSOR || dev.type == DEV_LIGHT_SENSOR) {
        printf("Channel number (0-7): ");
        scanf("%d", &dev.channel);
    }

    if (dev.type == DEV_TEMP_SENSOR || dev.type == DEV_LIGHT_SENSOR ||
        dev.type == DEV_ADC || dev.type == DEV_DAC) {
        printf("Minimum value: ");
        scanf("%f", &dev.min_val);
        printf("Maximum value: ");
        scanf("%f", &dev.max_val);
        printf("Units (e.g., °C, %, V): ");
        scanf("%s", dev.units);
    }

    dev.enabled = 1;

    cfg->devices[cfg->num_devices++] = dev;

    printf("\n✓ Device '%s' added successfully!\n", dev.name);
}

void list_devices(HardwareConfig *cfg) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║                  CONFIGURED DEVICES                        ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");

    if (cfg->num_devices == 0) {
        printf("║  No devices configured yet.                                ║\n");
    } else {
        for (int i = 0; i < cfg->num_devices; i++) {
            Device *d = &cfg->devices[i];
            printf("║  %2d. %-15s %-12s 0x%04X CH%-2d %s        ║\n",
                   i + 1, d->name, device_names[d->type],
                   d->port, d->channel,
                   d->enabled ? "✓" : "✗");
        }
    }

    printf("╚════════════════════════════════════════════════════════════╝\n");
}

void load_preset(HardwareConfig *cfg, const char *preset) {
    init_config(cfg);

    if (strcmp(preset, "breadboard") == 0) {
        printf("\nLoading 'Ben Eater Breadboard' preset...\n");

        // 8 LEDs on Port A
        for (int i = 0; i < 8; i++) {
            sprintf(cfg->devices[cfg->num_devices].name, "LED%d", i);
            cfg->devices[cfg->num_devices].type = DEV_LED;
            cfg->devices[cfg->num_devices].port = 0x6001;
            cfg->devices[cfg->num_devices].channel = i;
            cfg->devices[cfg->num_devices].enabled = 1;
            cfg->num_devices++;
        }

        // LCD Display
        strcpy(cfg->devices[cfg->num_devices].name, "LCD");
        cfg->devices[cfg->num_devices].type = DEV_LCD;
        cfg->devices[cfg->num_devices].port = 0x6000;
        cfg->devices[cfg->num_devices].enabled = 1;
        cfg->num_devices++;

        // UART
        strcpy(cfg->devices[cfg->num_devices].name, "Serial");
        cfg->devices[cfg->num_devices].type = DEV_UART;
        cfg->devices[cfg->num_devices].port = 0x6050;
        cfg->devices[cfg->num_devices].enabled = 1;
        cfg->num_devices++;

    } else if (strcmp(preset, "robot") == 0) {
        printf("\nLoading 'Robot Arm' preset...\n");

        const char *joints[] = {"Base", "Shoulder", "Elbow", "Wrist"};
        for (int i = 0; i < 4; i++) {
            // Servo
            sprintf(cfg->devices[cfg->num_devices].name, "%sServo", joints[i]);
            cfg->devices[cfg->num_devices].type = DEV_SERVO;
            cfg->devices[cfg->num_devices].port = 0x6020 + i;
            cfg->devices[cfg->num_devices].channel = i;
            cfg->devices[cfg->num_devices].min_val = -180;
            cfg->devices[cfg->num_devices].max_val = 180;
            strcpy(cfg->devices[cfg->num_devices].units, "deg");
            cfg->devices[cfg->num_devices].enabled = 1;
            cfg->num_devices++;

            // Encoder
            sprintf(cfg->devices[cfg->num_devices].name, "%sEncoder", joints[i]);
            cfg->devices[cfg->num_devices].type = DEV_ADC;
            cfg->devices[cfg->num_devices].port = 0x6010 + i;
            cfg->devices[cfg->num_devices].channel = i;
            cfg->devices[cfg->num_devices].min_val = -180;
            cfg->devices[cfg->num_devices].max_val = 180;
            strcpy(cfg->devices[cfg->num_devices].units, "deg");
            cfg->devices[cfg->num_devices].enabled = 1;
            cfg->num_devices++;
        }

    } else if (strcmp(preset, "iot") == 0) {
        printf("\nLoading 'IoT Sensors' preset...\n");

        // Temperature
        strcpy(cfg->devices[cfg->num_devices].name, "Temperature");
        cfg->devices[cfg->num_devices].type = DEV_TEMP_SENSOR;
        cfg->devices[cfg->num_devices].port = 0x6010;
        cfg->devices[cfg->num_devices].channel = 0;
        cfg->devices[cfg->num_devices].min_val = -40;
        cfg->devices[cfg->num_devices].max_val = 125;
        strcpy(cfg->devices[cfg->num_devices].units, "°C");
        cfg->devices[cfg->num_devices].enabled = 1;
        cfg->num_devices++;

        // Humidity (simulated as ADC)
        strcpy(cfg->devices[cfg->num_devices].name, "Humidity");
        cfg->devices[cfg->num_devices].type = DEV_ADC;
        cfg->devices[cfg->num_devices].port = 0x6011;
        cfg->devices[cfg->num_devices].channel = 1;
        cfg->devices[cfg->num_devices].min_val = 0;
        cfg->devices[cfg->num_devices].max_val = 100;
        strcpy(cfg->devices[cfg->num_devices].units, "%");
        cfg->devices[cfg->num_devices].enabled = 1;
        cfg->num_devices++;

        // Light
        strcpy(cfg->devices[cfg->num_devices].name, "Light");
        cfg->devices[cfg->num_devices].type = DEV_LIGHT_SENSOR;
        cfg->devices[cfg->num_devices].port = 0x6012;
        cfg->devices[cfg->num_devices].channel = 2;
        cfg->devices[cfg->num_devices].min_val = 0;
        cfg->devices[cfg->num_devices].max_val = 100;
        strcpy(cfg->devices[cfg->num_devices].units, "%");
        cfg->devices[cfg->num_devices].enabled = 1;
        cfg->num_devices++;

        // Relays
        for (int i = 0; i < 4; i++) {
            sprintf(cfg->devices[cfg->num_devices].name, "Relay%d", i);
            cfg->devices[cfg->num_devices].type = DEV_RELAY;
            cfg->devices[cfg->num_devices].port = 0x6001;
            cfg->devices[cfg->num_devices].channel = i;
            cfg->devices[cfg->num_devices].enabled = 1;
            cfg->num_devices++;
        }
    }

    printf("✓ Loaded %d devices\n", cfg->num_devices);
}

void save_config(HardwareConfig *cfg, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        printf("Error: Could not save configuration!\n");
        return;
    }

    fprintf(f, "# 6502 Hardware Configuration\n");
    fprintf(f, "# Generated by hardware_setup tool\n\n");
    fprintf(f, "[System]\n");
    fprintf(f, "devices = %d\n\n", cfg->num_devices);

    for (int i = 0; i < cfg->num_devices; i++) {
        Device *d = &cfg->devices[i];
        fprintf(f, "[%s]\n", d->name);
        fprintf(f, "type = %s\n", device_names[d->type]);
        fprintf(f, "address = 0x%04X\n", d->port);
        fprintf(f, "channel = %d\n", d->channel);
        fprintf(f, "min = %.2f\n", d->min_val);
        fprintf(f, "max = %.2f\n", d->max_val);
        fprintf(f, "units = %s\n", d->units);
        fprintf(f, "enabled = %s\n\n", d->enabled ? "yes" : "no");
    }

    fclose(f);
    printf("\n✓ Configuration saved to '%s'\n", filename);
}

void generate_code(HardwareConfig *cfg) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║              GENERATED C CODE                              ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");

    printf("// Device addresses\n");
    for (int i = 0; i < cfg->num_devices; i++) {
        Device *d = &cfg->devices[i];
        char upper[32];
        for (int j = 0; d->name[j]; j++) {
            upper[j] = toupper(d->name[j]);
        }
        upper[strlen(d->name)] = 0;

        printf("#define %s_ADDR   0x%04X\n", upper, d->port);
        if (d->channel >= 0) {
            printf("#define %s_CH     %d\n", upper, d->channel);
        }
    }

    printf("\n// Usage example:\n");
    printf("void setup_hardware() {\n");
    for (int i = 0; i < cfg->num_devices; i++) {
        Device *d = &cfg->devices[i];
        if (d->type == DEV_LED) {
            printf("    // Turn on %s\n", d->name);
            printf("    sys->porta |= (1 << %d);\n", d->channel);
        }
    }
    printf("}\n");
}

int main(int argc, char *argv[]) {
    HardwareConfig cfg;
    init_config(&cfg);

    display_banner();

    if (argc > 1) {
        printf("Loading configuration from '%s'...\n", argv[1]);
        // Would load from file here
    }

    int running = 1;
    while (running) {
        display_menu();

        int choice;
        scanf("%d", &choice);
        getchar();

        switch(choice) {
            case 1:
                add_device_wizard(&cfg);
                break;

            case 2:
                printf("Feature coming soon!\n");
                break;

            case 3:
                list_devices(&cfg);
                break;

            case 4:
                printf("\nAvailable presets:\n");
                printf("  1. breadboard - Ben Eater style\n");
                printf("  2. robot      - Robot arm\n");
                printf("  3. iot        - IoT sensors\n");
                printf("\nChoice: ");
                int preset;
                scanf("%d", &preset);
                if (preset == 1) load_preset(&cfg, "breadboard");
                else if (preset == 2) load_preset(&cfg, "robot");
                else if (preset == 3) load_preset(&cfg, "iot");
                break;

            case 5:
                printf("\nFilename: ");
                char filename[256];
                scanf("%s", filename);
                save_config(&cfg, filename);
                break;

            case 6:
                printf("Feature coming soon!\n");
                break;

            case 7:
                printf("\n");
                printf("Test mode would show live hardware visualization here.\n");
                printf("Press Enter to continue...");
                getchar();
                break;

            case 8:
                generate_code(&cfg);
                printf("\nPress Enter to continue...");
                getchar();
                break;

            case 9:
                running = 0;
                break;

            default:
                printf("Invalid choice!\n");
        }
    }

    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║           Hardware setup complete!                         ║\n");
    printf("║  %2d devices configured and ready to use                   ║\n", cfg.num_devices);
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");

    return 0;
}
