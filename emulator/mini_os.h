/*
 * Mini Operating System for 6502
 *
 * A simple but complete OS providing:
 * - File system (read, write, delete, rename)
 * - Directory operations (mkdir, rmdir, cd, ls)
 * - Disk management (format, mount, unmount)
 * - Memory management
 * - I/O system
 * - Shell/command interpreter
 * - BASIC integration
 *
 * Like Apple DOS, Commodore DOS, or CP/M for 6502!
 */

#ifndef MINI_OS_H
#define MINI_OS_H

#include <stdint.h>
#include <stdio.h>

/* ============================================
 * CONFIGURATION
 * ============================================ */

#define MAX_FILES        64     // Maximum files per disk
#define MAX_FILENAME     16     // Maximum filename length
#define MAX_PATH         128    // Maximum path length
#define SECTOR_SIZE      256    // Bytes per sector
#define MAX_OPEN_FILES   8      // Simultaneously open files
#define DISK_SIZE        (256 * 1024)  // 256KB disk

/* ============================================
 * FILE SYSTEM STRUCTURES
 * ============================================ */

typedef enum {
    FT_NONE = 0,
    FT_TEXT,        // Text file
    FT_BINARY,      // Binary file
    FT_BASIC,       // BASIC program
    FT_DATA,        // Data file
    FT_DIRECTORY    // Directory
} FileType;

typedef struct {
    char     name[MAX_FILENAME];
    FileType type;
    uint16_t start_sector;   // Starting sector on disk
    uint16_t num_sectors;    // Number of sectors used
    uint32_t size;           // File size in bytes
    uint8_t  attributes;     // Read-only, hidden, etc.
    uint16_t date;           // Creation date
    uint16_t time;           // Creation time
    uint8_t  used;           // Entry in use?
} FileEntry;

typedef struct {
    char     name[MAX_FILENAME];
    uint16_t parent;         // Parent directory
    uint16_t first_entry;    // First file entry
    uint16_t num_entries;    // Number of entries
} DirectoryEntry;

typedef struct {
    uint8_t  volume_name[16];
    uint16_t total_sectors;
    uint16_t free_sectors;
    uint16_t root_directory;
    uint8_t  format_version;
    FileEntry     files[MAX_FILES];
    DirectoryEntry directories[MAX_FILES / 4];
} DiskHeader;

/* ============================================
 * OPEN FILE HANDLE
 * ============================================ */

typedef struct {
    FileEntry *file;
    uint32_t  position;      // Current file position
    uint8_t   mode;          // 'r', 'w', 'a'
    uint8_t   in_use;
    uint8_t   *buffer;       // File buffer
} FileHandle;

/* ============================================
 * OPERATING SYSTEM STATE
 * ============================================ */

typedef struct {
    // Disk
    uint8_t   disk[DISK_SIZE];
    DiskHeader *header;
    int       disk_mounted;
    char      disk_file[256];  // Host filesystem path

    // Open files
    FileHandle open_files[MAX_OPEN_FILES];

    // Current directory
    uint16_t  current_directory;
    char      current_path[MAX_PATH];

    // System RAM
    uint8_t   ram[65536];

    // Memory allocation
    uint16_t  free_mem_start;
    uint16_t  free_mem_end;

    // System state
    int       running;
    char      command_buffer[256];

    // Statistics
    uint32_t  files_read;
    uint32_t  files_written;
    uint32_t  bytes_read;
    uint32_t  bytes_written;

} MiniOS;


/* ============================================
 * FILE OPERATIONS
 * ============================================ */

// Open a file
int os_open(MiniOS *os, const char *filename, char mode);

// Close a file
int os_close(MiniOS *os, int handle);

// Read from file
int os_read(MiniOS *os, int handle, uint8_t *buffer, int size);

// Write to file
int os_write(MiniOS *os, int handle, const uint8_t *data, int size);

// Seek in file
int os_seek(MiniOS *os, int handle, uint32_t position);

// Get file size
int os_filesize(MiniOS *os, const char *filename);

// Delete file
int os_delete(MiniOS *os, const char *filename);

// Rename file
int os_rename(MiniOS *os, const char *oldname, const char *newname);

// Copy file
int os_copy(MiniOS *os, const char *source, const char *dest);


/* ============================================
 * DIRECTORY OPERATIONS
 * ============================================ */

// List directory
void os_dir(MiniOS *os);

// Change directory
int os_cd(MiniOS *os, const char *path);

// Make directory
int os_mkdir(MiniOS *os, const char *name);

// Remove directory
int os_rmdir(MiniOS *os, const char *name);

// Get current directory
const char* os_pwd(MiniOS *os);


/* ============================================
 * DISK OPERATIONS
 * ============================================ */

// Format disk
int os_format(MiniOS *os, const char *volume_name);

// Mount disk from host filesystem
int os_mount(MiniOS *os, const char *disk_file);

// Unmount disk (save to host filesystem)
int os_unmount(MiniOS *os);

// Show disk info
void os_diskinfo(MiniOS *os);


/* ============================================
 * MEMORY MANAGEMENT
 * ============================================ */

// Allocate memory
uint16_t os_malloc(MiniOS *os, uint16_t size);

// Free memory
void os_free(MiniOS *os, uint16_t address);

// Get free memory
uint16_t os_freemem(MiniOS *os);


/* ============================================
 * SYSTEM CALLS (for 6502 programs)
 * ============================================ */

#define SYSCALL_BASE 0xFF00

// System call numbers
#define SYS_OPEN     0xFF00
#define SYS_CLOSE    0xFF01
#define SYS_READ     0xFF02
#define SYS_WRITE    0xFF03
#define SYS_DELETE   0xFF04
#define SYS_RENAME   0xFF05
#define SYS_MALLOC   0xFF06
#define SYS_FREE     0xFF07
#define SYS_PUTCHAR  0xFF08
#define SYS_GETCHAR  0xFF09
#define SYS_PRINT    0xFF0A
#define SYS_INPUT    0xFF0B

// Handle system call
uint8_t os_syscall(MiniOS *os, uint16_t syscall_num, uint8_t *params);


/* ============================================
 * SHELL / COMMAND INTERPRETER
 * ============================================ */

// Initialize OS
void os_init(MiniOS *os);

// Process command
void os_command(MiniOS *os, const char *command);

// Run shell
void os_shell(MiniOS *os);

// Boot sequence
void os_boot(MiniOS *os);


/* ============================================
 * BASIC INTEGRATION
 * ============================================ */

// Load BASIC program
int os_load_basic(MiniOS *os, const char *filename);

// Save BASIC program
int os_save_basic(MiniOS *os, const char *filename);

// Run BASIC program
int os_run_basic(MiniOS *os);


/* ============================================
 * UTILITY FUNCTIONS
 * ============================================ */

// Print file contents
void os_type(MiniOS *os, const char *filename);

// Get file info
void os_stat(MiniOS *os, const char *filename);

// Clear screen
void os_cls(void);

// Get system time
void os_time(char *buffer);

// Print banner
void os_banner(void);


#endif /* MINI_OS_H */
