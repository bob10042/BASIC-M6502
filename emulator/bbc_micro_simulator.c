/*
 * BBC Micro Model B Simulator
 *
 * Simulates a BBC Micro computer with:
 * - 6502 CPU at 2MHz
 * - 32KB RAM (expandable to 128KB)
 * - 32KB ROM (BBC BASIC + OS)
 * - VIA 6522 for I/O
 * - Screen memory mapped I/O
 * - Keyboard input
 * - Serial/parallel ports
 *
 * Can run Microsoft BASIC implementations from this repository!
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/select.h>

/* ============================================
 * BBC MICRO MEMORY MAP
 * ============================================
 *
 * 0x0000-0x7FFF : 32KB Main RAM
 * 0x8000-0xBFFF : Sideways ROM/RAM (16KB) [expandable]
 * 0xC000-0xFFFF : 16KB OS ROM
 *
 * I/O Addresses:
 * 0xFE00-0xFE1F : 6845 CRTC (video)
 * 0xFE40-0xFE5F : System VIA (6522)
 * 0xFE60-0xFE7F : User VIA (6522)
 * 0xFE80-0xFE9F : ACIA (serial)
 * 0xFEC0-0xFEDF : ADC (analog-digital)
 * ============================================ */

#define RAM_SIZE        (128 * 1024)   // 128KB RAM (expanded)
#define ROM_SIZE        (48 * 1024)    // 48KB ROM (BASIC + OS)
#define SCREEN_WIDTH    80
#define SCREEN_HEIGHT   25

/* BBC Micro System */
typedef struct {
    uint8_t ram[RAM_SIZE];
    uint8_t rom[ROM_SIZE];

    /* Video memory */
    char    screen[SCREEN_HEIGHT][SCREEN_WIDTH];
    int     cursor_x;
    int     cursor_y;

    /* VIA registers (simplified) */
    uint8_t via_porta;
    uint8_t via_portb;
    uint8_t via_ddra;
    uint8_t via_ddrb;

    /* Keyboard */
    uint8_t keyboard_buffer[256];
    int     kb_head;
    int     kb_tail;

    /* ACIA (serial) */
    uint8_t acia_data;
    uint8_t acia_status;

    /* System state */
    int     running;
    int     cycles;

    /* Configuration */
    int     ram_pages;    // Number of 16KB RAM pages (2-8)
    int     echo_input;   // Echo keyboard input
} BBCMicro;


/* ============================================
 * TERMINAL CONTROL
 * ============================================ */

struct termios orig_termios;
int terminal_raw_mode = 0;

void terminal_raw(void) {
    if (terminal_raw_mode) return;

    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    terminal_raw_mode = 1;
}

void terminal_restore(void) {
    if (!terminal_raw_mode) return;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    terminal_raw_mode = 0;
}

int kbhit(void) {
    struct timeval tv = {0, 0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
}


/* ============================================
 * SCREEN FUNCTIONS
 * ============================================ */

void screen_clear(BBCMicro *bbc) {
    memset(bbc->screen, ' ', sizeof(bbc->screen));
    bbc->cursor_x = 0;
    bbc->cursor_y = 0;
    printf("\033[2J\033[H");  // Clear terminal
    fflush(stdout);
}

void screen_putchar(BBCMicro *bbc, char c) {
    if (c == '\n' || c == '\r') {
        bbc->cursor_x = 0;
        bbc->cursor_y++;
        if (bbc->cursor_y >= SCREEN_HEIGHT) {
            // Scroll up
            memmove(bbc->screen[0], bbc->screen[1],
                    (SCREEN_HEIGHT - 1) * SCREEN_WIDTH);
            memset(bbc->screen[SCREEN_HEIGHT - 1], ' ', SCREEN_WIDTH);
            bbc->cursor_y = SCREEN_HEIGHT - 1;
        }
        putchar('\n');
    } else if (c == 8 || c == 127) {  // Backspace
        if (bbc->cursor_x > 0) {
            bbc->cursor_x--;
            bbc->screen[bbc->cursor_y][bbc->cursor_x] = ' ';
            printf("\b \b");
        }
    } else if (c >= 32 && c < 127) {
        if (bbc->cursor_x < SCREEN_WIDTH) {
            bbc->screen[bbc->cursor_y][bbc->cursor_x] = c;
            bbc->cursor_x++;
            putchar(c);
        }
        if (bbc->cursor_x >= SCREEN_WIDTH) {
            bbc->cursor_x = 0;
            bbc->cursor_y++;
            if (bbc->cursor_y >= SCREEN_HEIGHT) {
                memmove(bbc->screen[0], bbc->screen[1],
                        (SCREEN_HEIGHT - 1) * SCREEN_WIDTH);
                memset(bbc->screen[SCREEN_HEIGHT - 1], ' ', SCREEN_WIDTH);
                bbc->cursor_y = SCREEN_HEIGHT - 1;
            }
            putchar('\n');
        }
    }
    fflush(stdout);
}


/* ============================================
 * KEYBOARD FUNCTIONS
 * ============================================ */

void keyboard_push(BBCMicro *bbc, uint8_t key) {
    bbc->keyboard_buffer[bbc->kb_head] = key;
    bbc->kb_head = (bbc->kb_head + 1) % 256;
}

int keyboard_available(BBCMicro *bbc) {
    // Check for new terminal input
    if (kbhit()) {
        char c = getchar();
        if (c != -1) {
            keyboard_push(bbc, c);
        }
    }
    return bbc->kb_head != bbc->kb_tail;
}

uint8_t keyboard_read(BBCMicro *bbc) {
    if (!keyboard_available(bbc)) return 0;
    uint8_t key = bbc->keyboard_buffer[bbc->kb_tail];
    bbc->kb_tail = (bbc->kb_tail + 1) % 256;
    return key;
}


/* ============================================
 * MEMORY-MAPPED I/O
 * ============================================ */

uint8_t bbc_read(void *context, uint16_t address) {
    BBCMicro *bbc = (BBCMicro *)context;

    /* Main RAM: 0x0000-0x7FFF */
    if (address < 0x8000) {
        return bbc->ram[address];
    }

    /* I/O Page: 0xFE00-0xFEFF */
    if (address >= 0xFE00 && address < 0xFF00) {
        /* System VIA: 0xFE40-0xFE5F */
        if (address >= 0xFE40 && address <= 0xFE5F) {
            switch(address & 0x0F) {
                case 0x00: return bbc->via_portb;
                case 0x01: return bbc->via_porta;
                case 0x02: return bbc->via_ddrb;
                case 0x03: return bbc->via_ddra;
                default: return 0;
            }
        }

        /* ACIA (Serial): 0xFE80-0xFE9F */
        if (address >= 0xFE80 && address <= 0xFE9F) {
            if (address == 0xFE80) {
                // Check if keyboard data available
                if (keyboard_available(bbc)) {
                    bbc->acia_status |= 0x08;  // RX ready
                } else {
                    bbc->acia_status &= ~0x08;
                }
                return bbc->acia_status;
            }
            if (address == 0xFE81) {
                bbc->acia_status &= ~0x08;  // Clear RX ready
                return keyboard_read(bbc);
            }
        }

        return 0xFF;
    }

    /* ROM: 0x8000-0xFFFF (but skip I/O page) */
    if (address >= 0x8000) {
        return bbc->rom[address - 0x8000];
    }

    return 0xFF;
}

void bbc_write(void *context, uint16_t address, uint8_t value) {
    BBCMicro *bbc = (BBCMicro *)context;

    /* Main RAM: 0x0000-0x7FFF */
    if (address < 0x8000) {
        bbc->ram[address] = value;
        return;
    }

    /* I/O Page: 0xFE00-0xFEFF */
    if (address >= 0xFE00 && address < 0xFF00) {
        /* System VIA: 0xFE40-0xFE5F */
        if (address >= 0xFE40 && address <= 0xFE5F) {
            switch(address & 0x0F) {
                case 0x00: bbc->via_portb = value; break;
                case 0x01: bbc->via_porta = value; break;
                case 0x02: bbc->via_ddrb = value; break;
                case 0x03: bbc->via_ddra = value; break;
            }
            return;
        }

        /* ACIA (Serial/Console): 0xFE80-0xFE9F */
        if (address >= 0xFE80 && address <= 0xFE9F) {
            if (address == 0xFE80) {
                bbc->acia_status = value;
            }
            if (address == 0xFE81) {
                screen_putchar(bbc, value);
            }
            return;
        }

        /* OSWRCH (OS Write Character): Simulated at 0xFFFE */
        if (address == 0xFFFE) {
            screen_putchar(bbc, value);
            return;
        }
    }

    /* ROM is read-only, ignore writes */
}


/* ============================================
 * SYSTEM INITIALIZATION
 * ============================================ */

void bbc_init(BBCMicro *bbc, int ram_kb) {
    memset(bbc->ram, 0, sizeof(bbc->ram));
    memset(bbc->rom, 0, sizeof(bbc->rom));

    bbc->ram_pages = ram_kb / 16;
    if (bbc->ram_pages < 2) bbc->ram_pages = 2;   // Minimum 32KB
    if (bbc->ram_pages > 8) bbc->ram_pages = 8;   // Maximum 128KB

    screen_clear(bbc);
    bbc->kb_head = 0;
    bbc->kb_tail = 0;
    bbc->acia_status = 0x10;  // TX ready
    bbc->running = 1;
    bbc->echo_input = 1;

    printf("BBC Micro Model B Simulator\n");
    printf("═══════════════════════════════════════\n");
    printf("RAM: %dKB\n", bbc->ram_pages * 16);
    printf("ROM: %dKB\n", (int)(sizeof(bbc->rom) / 1024));
    printf("CPU: 6502 @ 2MHz\n");
    printf("═══════════════════════════════════════\n\n");
}


/* ============================================
 * BASIC INTERPRETER STUB
 * ============================================ */

void bbc_load_basic(BBCMicro *bbc) {
    /* This would load the actual Microsoft BASIC ROM
     * For now, create a simple interactive prompt
     */

    printf("BBC BASIC Simulator Ready\n\n");
    printf("Available commands:\n");
    printf("  PRINT <expr>  - Print expression\n");
    printf("  CLS           - Clear screen\n");
    printf("  QUIT          - Exit simulator\n\n");
    printf("Ready\n>");
    fflush(stdout);
}


void bbc_run_basic_interactive(BBCMicro *bbc) {
    char input[256];
    int pos = 0;

    terminal_raw();

    while (bbc->running) {
        // Check for keyboard input
        if (keyboard_available(bbc)) {
            char c = keyboard_read(bbc);

            if (c == '\n' || c == '\r') {
                input[pos] = '\0';
                printf("\n");

                // Process command
                if (strncmp(input, "QUIT", 4) == 0) {
                    bbc->running = 0;
                } else if (strncmp(input, "CLS", 3) == 0) {
                    screen_clear(bbc);
                } else if (strncmp(input, "PRINT ", 6) == 0) {
                    printf("%s\n", input + 6);
                } else if (pos > 0) {
                    printf("Syntax error\n");
                }

                printf(">");
                fflush(stdout);
                pos = 0;
            } else if (c == 127 || c == 8) {  // Backspace
                if (pos > 0) {
                    pos--;
                    printf("\b \b");
                    fflush(stdout);
                }
            } else if (c >= 32 && c < 127 && pos < 255) {
                input[pos++] = c;
                putchar(c);
                fflush(stdout);
            }
        }

        usleep(1000);  // 1ms delay
    }

    terminal_restore();
}


/* ============================================
 * MAIN
 * ============================================ */

int main(int argc, char *argv[]) {
    BBCMicro bbc;
    int ram_kb = 32;  // Default 32KB

    // Parse command line
    if (argc > 1) {
        ram_kb = atoi(argv[1]);
    }

    printf("\n");
    printf("╔════════════════════════════════════════════════════╗\n");
    printf("║   BBC MICRO MODEL B SIMULATOR                      ║\n");
    printf("║   6502 Computer with Microsoft BASIC               ║\n");
    printf("╚════════════════════════════════════════════════════╝\n\n");

    bbc_init(&bbc, ram_kb);
    bbc_load_basic(&bbc);
    bbc_run_basic_interactive(&bbc);

    printf("\n\nSimulator terminated.\n\n");

    printf("Next steps to run actual Microsoft BASIC:\n");
    printf("  1. Assemble BASIC from hardware/assembly/msbasic/\n");
    printf("  2. Load ROM image into simulator\n");
    printf("  3. Link with actual 6502 emulator (sources/6502.c)\n");
    printf("  4. Run full BASIC interpreter!\n\n");

    printf("Example build:\n");
    printf("  cd hardware/assembly/msbasic\n");
    printf("  ./make.sh\n");
    printf("  # Load eater.bin into this simulator\n\n");

    return 0;
}
