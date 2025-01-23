#pragma once

#include <mach/mach.h>
#include <stdbool.h>
#include <stdint.h>

// Define CPU load units
typedef enum unit {
    UNIT_BPS,
    UNIT_KBPS,
    UNIT_MBPS,
    UNIT_GBPS // Added for scalability
} unit_t;

// CPU structure to hold load information
typedef struct cpu {
    host_t host;
    mach_msg_type_number_t count;
    host_cpu_load_info_data_t load;
    host_cpu_load_info_data_t prev_load;
    bool has_prev_load;

    double user_load;   // Changed to double for precision
    double sys_load;    // Changed to double for precision
    double total_load;  // Changed to double for precision
} cpu_t;

// Function prototypes
void cpu_init(cpu_t* cpu);
int cpu_update(cpu_t* cpu);
