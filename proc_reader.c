#include "proc_reader.h"

int list_process_directories(void) {
    // Open /proc directory
    DIR *proc_dir = opendir("/proc");
    if (proc_dir == NULL) {
        perror("opendir failed");
        return -1;
    }

    struct dirent *entry;
    int process_count = 0;

    printf("Process directories in /proc:\n");
    printf("%-8s %-20s\n", "PID", "Type");
    printf("%-8s %-20s\n", "---", "----");

    // Read directory entries
    while ((entry = readdir(proc_dir)) != NULL) {
        if (is_number(entry->d_name)) {
            printf("%-8s %-20s\n", entry->d_name, "Process");
            process_count++;
        }
    }

    // Close directory
    if (closedir(proc_dir) == -1) {
        perror("closedir failed");
        return -1;
    }

    printf("\nTotal process directories: %d\n", process_count);

    return 0;
}



int read_process_info(const char* pid) {
    char filepath[256];

    // Create path to /proc/[pid]/status
    snprintf(filepath, sizeof(filepath), "/proc/%s/status", pid);

    printf("\n--- Process Information for PID %s ---\n", pid);

    // Read status file
    if (read_file_with_syscalls(filepath) != 0) {
        fprintf(stderr, "Failed to read %s\n", filepath);
        return -1;
    }

    // Create path to /proc/[pid]/cmdline
    snprintf(filepath, sizeof(filepath), "/proc/%s/cmdline", pid);

    printf("\n--- Command Line ---\n");

    // Read cmdline file
    if (read_file_with_syscalls(filepath) != 0) {
        fprintf(stderr, "Failed to read %s\n", filepath);
        return -1;
    }

    printf("\n"); // Extra newline for readability

    return 0;

}

int show_system_info(void) {
     int line_count = 0;
    const int MAX_LINES = 10;
    char buffer[256];
    FILE *file;

    printf("\n--- CPU Information (first %d lines) ---\n", MAX_LINES);

    // Open /proc/cpuinfo
    file = fopen("/proc/cpuinfo", "r");
    if (file == NULL) {
        perror("fopen cpuinfo failed");
        return -1;
    }

    // Read and print lines
    line_count = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL && line_count < MAX_LINES) {
        printf("%s", buffer);
        line_count++;
    }

    // Close file
    if (fclose(file) != 0) {
        perror("fclose cpuinfo failed");
        return -1;
    }

    printf("\n--- Memory Information (first %d lines) ---\n", MAX_LINES);

    // Open /proc/meminfo
    file = fopen("/proc/meminfo", "r");
    if (file == NULL) {
        perror("fopen meminfo failed");
        return -1;
    }

    // Read and print lines
    line_count = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL && line_count < MAX_LINES) {
        printf("%s", buffer);
        line_count++;
    }

    // Close file
    if (fclose(file) != 0) {
        perror("fclose meminfo failed");
        return -1;
    }

    return 0;
}

void compare_file_methods(void) {
    const char* test_file = "/proc/version";

    printf("Comparing file reading methods for: %s\n\n", test_file);

    printf("=== Method 1: Using System Calls ===\n");
    read_file_with_syscalls(test_file);

    printf("\n=== Method 2: Using Library Functions ===\n");
    read_file_with_library(test_file);

    printf("\nNOTE: Run this program with strace to see the difference!\n");
    printf("Example: strace -e trace=openat,read,write,close ./lab2\n");
}

int read_file_with_syscalls(const char* filename) {
     int fd;
    char buffer[256];
    ssize_t bytes_read;

    // Open file
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open failed");
        return -1;
    }

    // Read file in chunks
    while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';  // null terminate
        printf("%s", buffer);
    }

    // Handle read error
    if (bytes_read == -1) {
        perror("read failed");
        close(fd);
        return -1;
    }

    // Close file
    if (close(fd) == -1) {
        perror("close failed");
        return -1;
    }

    return 0;
}

int read_file_with_library(const char* filename) {
     FILE *file;
    char buffer[256];

    // Open file
    file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen failed");
        return -1;
    }

    // Read file line by line
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s", buffer);
    }

    // Close file
    if (fclose(file) != 0) {
        perror("fclose failed");
        return -1;
    }

    return 0;

}

int is_number(const char* str) {
       // Handle NULL or empty string
    if (str == NULL || *str == '\0') {
        return 0;
    }

    // Check each character
    while (*str) {
        if (!isdigit((unsigned char)*str)) {
            return 0;
        }
        str++;
    }

    // All characters were digits
    return 1;
}
