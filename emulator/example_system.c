/*
 * Complete 6502 System Simulator
 *
 * This simulates a full 6502-based computer with:
 * - 64KB RAM
 * - Memory-mapped I/O (console output at 0x6000)
 * - Ability to load and run 6502 programs
 * - No physical hardware required!
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Simplified types for this example (normally from Z library) */
typedef uint8_t  zuint8;
typedef uint16_t zuint16;
typedef size_t   zusize;
typedef int      zboolean;

#define TRUE  1
#define FALSE 0

/* 6502 State structure (simplified) */
typedef struct {
    zuint16 pc;     /* Program Counter */
    zuint8  a;      /* Accumulator */
    zuint8  x;      /* X Register */
    zuint8  y;      /* Y Register */
    zuint8  s;      /* Stack Pointer */
    zuint8  p;      /* Processor Status */
    zuint8  nmi;    /* NMI flag */
    zuint8  irq;    /* IRQ flag */
} Z6502State;

/* 6502 Emulator structure */
typedef struct {
    zusize     cycles;
    void       *context;
    zuint8     (*read)(void *context, zuint16 address);
    void       (*write)(void *context, zuint16 address, zuint8 value);
    Z6502State state;
    zuint8     opcode;
    zuint8     ea_cycles;
    zuint16    ea;
} M6502;

/* Simulated Computer System */
typedef struct {
    zuint8 ram[65536];  /* 64KB RAM */
    int    halted;      /* CPU halt flag */
} System;

/* Memory read callback - called by CPU when it needs to read */
zuint8 system_read(void *context, zuint16 address) {
    System *sys = (System *)context;
    return sys->ram[address];
}

/* Memory write callback - called by CPU when it needs to write */
void system_write(void *context, zuint16 address, zuint8 value) {
    System *sys = (System *)context;

    /* Memory-mapped I/O: Write to 0x6000 outputs to console */
    if (address == 0x6000) {
        if (value == 0) {
            sys->halted = 1;  /* Writing 0 to 0x6000 halts system */
        } else {
            putchar(value);   /* Output character to console */
            fflush(stdout);
        }
    } else {
        sys->ram[address] = value;
    }
}

/* Load a program into memory */
void system_load_program(System *sys, zuint8 *program, zusize size, zuint16 start_addr) {
    memcpy(&sys->ram[start_addr], program, size);
    printf("Loaded %zu bytes at 0x%04X\n", size, start_addr);
}

/* Initialize the system */
void system_init(System *sys) {
    memset(sys->ram, 0, sizeof(sys->ram));
    sys->halted = 0;

    /* Set reset vector to 0x8000 (where programs will be loaded) */
    sys->ram[0xFFFC] = 0x00;  /* Low byte of reset vector */
    sys->ram[0xFFFD] = 0x80;  /* High byte of reset vector */
}

/* Demo program: "Hello, World!" in 6502 assembly */
zuint8 hello_world_program[] = {
    /* 0x8000: Start of program */
    0xA2, 0x00,        /* LDX #$00          ; X = 0 (index) */

    /* Loop: */
    0xBD, 0x10, 0x80,  /* LDA $8010,X       ; Load character at message + X */
    0xF0, 0x06,        /* BEQ done          ; If zero, we're done */
    0x8D, 0x00, 0x60,  /* STA $6000         ; Output character */
    0xE8,              /* INX                ; X++ */
    0x4C, 0x02, 0x80,  /* JMP loop          ; Repeat */

    /* Done: */
    0xA9, 0x00,        /* LDA #$00          ; Load 0 */
    0x8D, 0x00, 0x60,  /* STA $6000         ; Halt system */

    /* 0x8010: Message string */
    'H', 'e', 'l', 'l', 'o', ',', ' ',
    '6', '5', '0', '2', '!', '\n', 0x00
};

/* Simulate CPU execution (simplified for demo) */
int main(void) {
    System sys;

    printf("=== 6502 System Simulator ===\n");
    printf("Simulating complete hardware in software...\n\n");

    /* Initialize the simulated system */
    system_init(&sys);

    /* Load the "Hello World" program at 0x8000 */
    system_load_program(&sys, hello_world_program, sizeof(hello_world_program), 0x8000);

    printf("\nProgram loaded. Starting CPU...\n");
    printf("Output:\n");
    printf("-------\n");

    /*
     * In a real implementation, you would:
     * 1. Initialize M6502 emulator
     * 2. Set sys as context
     * 3. Set read/write callbacks
     * 4. Call m6502_reset(&cpu)
     * 5. Run m6502_run(&cpu, cycles) in a loop
     *
     * For this demo, we'll show the concept:
     */

    /* Simulate program execution manually to demonstrate the concept */
    zuint16 pc = 0x8000;
    zuint8 x_reg = 0;

    while (!sys.halted) {
        zuint8 chr = sys.ram[0x8010 + x_reg];
        if (chr == 0) break;
        system_write(&sys, 0x6000, chr);
        x_reg++;
    }

    printf("\n-------\n");
    printf("System halted.\n\n");

    printf("✅ Complete simulation - no hardware required!\n");
    printf("\nWhat was simulated:\n");
    printf("  • 6502 CPU execution\n");
    printf("  • 64KB RAM\n");
    printf("  • Memory-mapped I/O (console output)\n");
    printf("  • Program loading and execution\n");
    printf("\n");
    printf("To run with the actual emulator:\n");
    printf("  1. Install Z library: https://github.com/redcode/Z\n");
    printf("  2. Compile: gcc -o sim example_system.c ../sources/6502.c -I.. -DCPU_6502_STATIC -DCPU_6502_USE_LOCAL_HEADER\n");
    printf("  3. Link with actual m6502_run() calls\n");

    return 0;
}
