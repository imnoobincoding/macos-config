#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>

// Define number of CPU states
#define NUM_CPU_STATES 4

/**
 * @brief Initializes the CPU structure.
 *
 * @param cpu Pointer to the CPU structure to initialize.
 */
void cpu_init(cpu_t* cpu) {
    if (!cpu) {
        fprintf(stderr, "[ERROR] NULL pointer passed to cpu_init.\n");
        exit(EXIT_FAILURE);
    }

    memset(cpu, 0, sizeof(cpu_t));
    cpu->host = mach_host_self();
    cpu->count = HOST_CPU_LOAD_INFO_COUNT;
    cpu->has_prev_load = false;
}

/**
 * @brief Updates the CPU load information.
 *
 * @param cpu Pointer to the CPU structure to update.
 * @return int Returns 0 on success, -1 on failure.
 */
int cpu_update(cpu_t* cpu) {
    if (!cpu) {
        fprintf(stderr, "[ERROR] NULL pointer passed to cpu_update.\n");
        return -1;
    }

    kern_return_t error = host_statistics(cpu->host,
                                          HOST_CPU_LOAD_INFO,
                                          (host_info_t)&cpu->load,
                                          &cpu->count);
    if (error != KERN_SUCCESS) {
        fprintf(stderr, "[ERROR] Could not read CPU host statistics. Error code: %d\n", error);
        return -1;
    }

    if (cpu->has_prev_load) {
        uint32_t delta_user = cpu->load.cpu_ticks[CPU_STATE_USER] - cpu->prev_load.cpu_ticks[CPU_STATE_USER];
        uint32_t delta_system = cpu->load.cpu_ticks[CPU_STATE_SYSTEM] - cpu->prev_load.cpu_ticks[CPU_STATE_SYSTEM];
        uint32_t delta_idle = cpu->load.cpu_ticks[CPU_STATE_IDLE] - cpu->prev_load.cpu_ticks[CPU_STATE_IDLE];
        uint32_t delta_nice = cpu->load.cpu_ticks[CPU_STATE_NICE] - cpu->prev_load.cpu_ticks[CPU_STATE_NICE];

        uint32_t total_ticks = delta_user + delta_system + delta_idle + delta_nice;

        if (total_ticks == 0) {
            fprintf(stderr, "[WARN] Total CPU ticks is zero. Skipping update.\n");
            return 0;
        }

        // Calculate load percentages
        cpu->user_load = ((double)delta_user / (double)total_ticks) * 100.0;
        cpu->sys_load = ((double)delta_system / (double)total_ticks) * 100.0;
        cpu->total_load = cpu->user_load + cpu->sys_load;
    }

    // Update previous load data
    cpu->prev_load = cpu->load;
    cpu->has_prev_load = true;

    return 0;
}
