/*
 * Complete Hardware Simulation Example
 *
 * This simulates ALL the peripherals from the breadboard computer:
 * - HD44780 LCD Display (16x2 character display)
 * - 65C22 VIA (Versatile Interface Adapter)
 * - PS/2 Keyboard input
 * - Serial UART communication
 *
 * Shows how memory-mapped I/O works in simulation!
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

/* ============================================
 * SIMULATED HARDWARE COMPONENTS
 * ============================================ */

/* LCD Display (HD44780 compatible - like in hardware/) */
typedef struct {
    char display[2][16];   // 2 rows x 16 columns
    int cursor_x;
    int cursor_y;
    uint8_t command_mode;  // 0 = data, 1 = command
    uint8_t enabled;
} LCD;

/* VIA Chip (65C22 - like in hardware/) */
typedef struct {
    uint8_t porta;         // Port A data
    uint8_t portb;         // Port B data
    uint8_t ddra;          // Data Direction Register A
    uint8_t ddrb;          // Data Direction Register B
    uint8_t t1c_l;         // Timer 1 counter low
    uint8_t t1c_h;         // Timer 1 counter high
    uint8_t t1l_l;         // Timer 1 latch low
    uint8_t t1l_h;         // Timer 1 latch high
    uint8_t acr;           // Auxiliary Control Register
    uint8_t pcr;           // Peripheral Control Register
    uint8_t ifr;           // Interrupt Flag Register
    uint8_t ier;           // Interrupt Enable Register
} VIA;

/* Keyboard Buffer */
typedef struct {
    uint8_t buffer[256];
    int head;
    int tail;
} Keyboard;

/* UART/Serial Port */
typedef struct {
    uint8_t data;          // Data register
    uint8_t status;        // Status register
    uint8_t command;       // Command register
    uint8_t control;       // Control register
    FILE *output;          // Where serial output goes
} UART;

/* Complete Simulated Computer System */
typedef struct {
    uint8_t  ram[32768];   // 32KB RAM (0x0000-0x7FFF)
    uint8_t  rom[32768];   // 32KB ROM (0x8000-0xFFFF)

    LCD      lcd;          // LCD display
    VIA      via;          // VIA chip
    Keyboard keyboard;     // Keyboard input
    UART     uart;         // Serial port

    int      running;      // System running flag
} BreadboardComputer;


/* ============================================
 * LCD SIMULATION FUNCTIONS
 * ============================================ */

void lcd_init(LCD *lcd) {
    memset(lcd->display, ' ', sizeof(lcd->display));
    lcd->cursor_x = 0;
    lcd->cursor_y = 0;
    lcd->command_mode = 0;
    lcd->enabled = 1;
}

void lcd_clear(LCD *lcd) {
    memset(lcd->display, ' ', sizeof(lcd->display));
    lcd->cursor_x = 0;
    lcd->cursor_y = 0;
}

void lcd_write_char(LCD *lcd, char c) {
    if (lcd->cursor_x < 16 && lcd->cursor_y < 2) {
        lcd->display[lcd->cursor_y][lcd->cursor_x] = c;
        lcd->cursor_x++;
        if (lcd->cursor_x >= 16) {
            lcd->cursor_x = 0;
            lcd->cursor_y = (lcd->cursor_y + 1) % 2;
        }
    }
}

void lcd_set_cursor(LCD *lcd, int x, int y) {
    lcd->cursor_x = x % 16;
    lcd->cursor_y = y % 2;
}

void lcd_display(LCD *lcd) {
    printf("\n┌────────────────┐\n");
    printf("│%.16s│\n", lcd->display[0]);
    printf("│%.16s│\n", lcd->display[1]);
    printf("└────────────────┘\n");
    fflush(stdout);
}

void lcd_command(LCD *lcd, uint8_t cmd) {
    if (cmd == 0x01) {
        lcd_clear(lcd);
    } else if ((cmd & 0x80) == 0x80) {
        // Set DDRAM address (cursor position)
        int addr = cmd & 0x7F;
        if (addr < 0x40) {
            lcd->cursor_y = 0;
            lcd->cursor_x = addr;
        } else {
            lcd->cursor_y = 1;
            lcd->cursor_x = addr - 0x40;
        }
    }
}


/* ============================================
 * VIA SIMULATION FUNCTIONS
 * ============================================ */

void via_init(VIA *via) {
    memset(via, 0, sizeof(VIA));
}

uint8_t via_read(VIA *via, uint8_t reg) {
    switch(reg) {
        case 0x00: return via->portb;      // Port B
        case 0x01: return via->porta;      // Port A
        case 0x02: return via->ddrb;       // DDR B
        case 0x03: return via->ddra;       // DDR A
        case 0x0D: return via->ifr;        // Interrupt flags
        case 0x0E: return via->ier;        // Interrupt enable
        default: return 0;
    }
}

void via_write(VIA *via, uint8_t reg, uint8_t value) {
    switch(reg) {
        case 0x00: via->portb = value; break;
        case 0x01: via->porta = value; break;
        case 0x02: via->ddrb = value; break;
        case 0x03: via->ddra = value; break;
        case 0x0C: via->pcr = value; break;
        case 0x0D: via->ifr = value; break;
        case 0x0E: via->ier = value; break;
    }
}


/* ============================================
 * KEYBOARD SIMULATION
 * ============================================ */

void keyboard_init(Keyboard *kb) {
    kb->head = 0;
    kb->tail = 0;
}

void keyboard_push(Keyboard *kb, uint8_t scancode) {
    kb->buffer[kb->head] = scancode;
    kb->head = (kb->head + 1) % 256;
}

uint8_t keyboard_available(Keyboard *kb) {
    return kb->head != kb->tail;
}

uint8_t keyboard_read(Keyboard *kb) {
    if (!keyboard_available(kb)) return 0;
    uint8_t data = kb->buffer[kb->tail];
    kb->tail = (kb->tail + 1) % 256;
    return data;
}


/* ============================================
 * UART/SERIAL SIMULATION
 * ============================================ */

void uart_init(UART *uart) {
    uart->data = 0;
    uart->status = 0x10;  // Transmit ready
    uart->command = 0;
    uart->control = 0;
    uart->output = stdout;
}

void uart_write(UART *uart, uint8_t data) {
    fputc(data, uart->output);
    fflush(uart->output);
}

uint8_t uart_read(UART *uart) {
    // In real simulation, this would read from a socket or file
    return uart->data;
}


/* ============================================
 * MEMORY-MAPPED I/O HANDLER
 * ============================================ */

uint8_t system_read(void *context, uint16_t address) {
    BreadboardComputer *sys = (BreadboardComputer *)context;

    /* RAM: 0x0000-0x7FFF */
    if (address < 0x8000) {
        /* VIA Registers: 0x6000-0x600F */
        if (address >= 0x6000 && address <= 0x600F) {
            return via_read(&sys->via, address & 0x0F);
        }

        /* UART Registers: 0x5000-0x5003 */
        if (address >= 0x5000 && address <= 0x5003) {
            switch(address & 0x03) {
                case 0: return uart_read(&sys->uart);
                case 1: return sys->uart.status;
                case 2: return sys->uart.command;
                case 3: return sys->uart.control;
            }
        }

        /* Keyboard Data: 0x6001 (shared with VIA Port A) */
        if (address == 0x6001 && keyboard_available(&sys->keyboard)) {
            return keyboard_read(&sys->keyboard);
        }

        return sys->ram[address];
    }

    /* ROM: 0x8000-0xFFFF */
    return sys->rom[address - 0x8000];
}

void system_write(void *context, uint16_t address, uint8_t value) {
    BreadboardComputer *sys = (BreadboardComputer *)context;

    /* RAM: 0x0000-0x7FFF */
    if (address < 0x8000) {
        /* VIA Registers: 0x6000-0x600F */
        if (address >= 0x6000 && address <= 0x600F) {
            via_write(&sys->via, address & 0x0F, value);

            /* LCD is connected to VIA Port B */
            if (address == 0x6000) {  // Port B
                // Check if this is LCD data or command
                if (sys->via.portb & 0x80) {  // RS bit
                    lcd_write_char(&sys->lcd, value & 0x7F);
                } else {
                    lcd_command(&sys->lcd, value);
                }
                lcd_display(&sys->lcd);  // Update display
            }
            return;
        }

        /* UART Registers: 0x5000-0x5003 */
        if (address >= 0x5000 && address <= 0x5003) {
            switch(address & 0x03) {
                case 0:
                    sys->uart.data = value;
                    uart_write(&sys->uart, value);
                    break;
                case 1: sys->uart.status = value; break;
                case 2: sys->uart.command = value; break;
                case 3: sys->uart.control = value; break;
            }
            return;
        }

        /* Direct LCD Output: 0x7000 (custom for demo) */
        if (address == 0x7000) {
            if (value == 0) {
                sys->running = 0;  // Halt
            } else {
                lcd_write_char(&sys->lcd, value);
                lcd_display(&sys->lcd);
            }
            return;
        }

        sys->ram[address] = value;
    }
}


/* ============================================
 * SYSTEM INITIALIZATION
 * ============================================ */

void system_init(BreadboardComputer *sys) {
    memset(sys->ram, 0, sizeof(sys->ram));
    memset(sys->rom, 0, sizeof(sys->rom));

    lcd_init(&sys->lcd);
    via_init(&sys->via);
    keyboard_init(&sys->keyboard);
    uart_init(&sys->uart);

    sys->running = 1;

    /* Set reset vector to 0x8000 */
    sys->rom[0x7FFC] = 0x00;  // Low byte (at 0xFFFC in address space)
    sys->rom[0x7FFD] = 0x80;  // High byte (at 0xFFFD in address space)
}


/* ============================================
 * DEMO PROGRAM
 * ============================================ */

int main(void) {
    BreadboardComputer system;

    printf("╔═══════════════════════════════════════════════════╗\n");
    printf("║   6502 BREADBOARD COMPUTER SIMULATOR             ║\n");
    printf("║   All hardware simulated in software!            ║\n");
    printf("╚═══════════════════════════════════════════════════╝\n\n");

    system_init(&system);

    /* Load a demo program that writes to LCD */
    uint8_t demo_program[] = {
        /* Write "Hello, 6502!" to LCD */
        0xA2, 0x00,              // LDX #$00
        // Loop:
        0xBD, 0x20, 0x80,        // LDA message,X
        0xF0, 0x06,              // BEQ done
        0x8D, 0x00, 0x70,        // STA $7000 (LCD output)
        0xE8,                    // INX
        0x4C, 0x02, 0x80,        // JMP loop
        // Done:
        0xA9, 0x00,              // LDA #$00
        0x8D, 0x00, 0x70,        // STA $7000 (halt)

        /* Message at 0x8020 */
        'H', 'e', 'l', 'l', 'o', ',', ' ',
        '6', '5', '0', '2', '!', 0x00
    };

    memcpy(system.rom, demo_program, sizeof(demo_program));

    printf("Program loaded into ROM at 0x8000\n");
    printf("Simulating execution...\n\n");
    printf("═══════════════════════════════════════\n");
    printf("SIMULATED LCD DISPLAY:\n");

    /* Simulate execution (simplified for demo) */
    uint16_t pc = 0x8000;
    uint8_t x_reg = 0;

    while (system.running) {
        uint8_t chr = system.rom[0x20 + x_reg];
        if (chr == 0) break;

        system_write(&system, 0x7000, chr);
        usleep(100000);  // 100ms delay for visual effect
        x_reg++;
    }

    printf("═══════════════════════════════════════\n\n");

    printf("✅ Simulation complete!\n\n");
    printf("Simulated hardware components:\n");
    printf("  📺 LCD Display (HD44780) - 16x2 characters\n");
    printf("  🔌 VIA Chip (65C22) - I/O ports and timers\n");
    printf("  ⌨️  Keyboard Interface - PS/2 input buffer\n");
    printf("  📡 UART Serial - Bidirectional communication\n");
    printf("  💾 64KB Memory - 32KB RAM + 32KB ROM\n\n");

    printf("Memory Map:\n");
    printf("  0x0000-0x7FFF : RAM (32KB)\n");
    printf("  0x5000-0x5003 : UART registers\n");
    printf("  0x6000-0x600F : VIA registers\n");
    printf("  0x7000        : LCD output (demo)\n");
    printf("  0x8000-0xFFFF : ROM (32KB)\n\n");

    printf("This is exactly how the hardware works!\n");
    printf("The 6502 reads/writes to memory addresses,\n");
    printf("and those addresses trigger device actions.\n\n");

    return 0;
}
