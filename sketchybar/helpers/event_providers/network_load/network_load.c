#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#include "network.h"
#include "../sketchybar.h"

#define EVENT_MESSAGE_SIZE 512
#define TRIGGER_MESSAGE_SIZE 512

// Define unit strings
static const char* unit_str[3][6] = { { " Bps" }, { "KBps" }, { "MBps" } };

// Global flag for graceful termination
volatile sig_atomic_t stop = 0;

// Signal handler for graceful shutdown
void handle_signal(int signum) {
    stop = 1;
}

// Function to validate input strings (interface and event name)
int validate_string(const char* str) {
    if (!str) return 0;
    for (size_t i = 0; i < strlen(str); ++i) {
        if (!isalnum(str[i]) && str[i] != '_' && str[i] != '-') {
            return 0;
        }
    }
    return 1;
}

// Function to setup the event in SketchyBar
int setup_event(const char* event_name) {
    char event_message[EVENT_MESSAGE_SIZE];
    if (snprintf(event_message, sizeof(event_message), "--add event '%s'", event_name) >= sizeof(event_message)) {
        fprintf(stderr, "[ERROR] Event message too long.\n");
        return -1;
    }
    sketchybar(event_message);
    return 0;
}

// Function to send trigger messages to SketchyBar
int send_trigger(const char* event_name, struct network* net) {
    char trigger_message[TRIGGER_MESSAGE_SIZE];
    if (snprintf(trigger_message,
                sizeof(trigger_message),
                "--trigger '%s' upload='%.2f%s' download='%.2f%s'",
                event_name,
                (double)net->up,
                unit_str[net->up_unit][0],
                (double)net->down,
                unit_str[net->down_unit][0]) >= sizeof(trigger_message)) {
        fprintf(stderr, "[ERROR] Trigger message too long.\n");
        return -1;
    }
    sketchybar(trigger_message);
    return 0;
}

int main(int argc, char** argv) {
    float update_freq;

    // Setup signal handling for graceful termination
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    // Validate command-line arguments
    if (argc < 4 || (sscanf(argv[3], "%f", &update_freq) != 1)) {
        fprintf(stderr, "Usage: %s \"<interface>\" \"<event-name>\" \"<event_freq>\"\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char* interface = argv[1];
    const char* event_name = argv[2];

    // Validate interface and event name
    if (!validate_string(interface)) {
        fprintf(stderr, "[ERROR] Invalid characters in interface name.\n");
        exit(EXIT_FAILURE);
    }

    if (!validate_string(event_name)) {
        fprintf(stderr, "[ERROR] Invalid characters in event name.\n");
        exit(EXIT_FAILURE);
    }

    // Disable any existing alarms
    alarm(0);

    // Setup the event in SketchyBar
    fprintf(stdout, "[INFO] Setting up event '%s' in SketchyBar.\n", event_name);
    if (setup_event(event_name) != 0) {
        exit(EXIT_FAILURE);
    }

    // Initialize network monitoring
    struct network network;
    if (network_init(&network, (char*)interface) != 0) {
        fprintf(stderr, "[ERROR] Failed to initialize network monitoring for interface %s.\n", interface);
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "[INFO] Starting network monitoring on interface '%s' with update frequency %.2f seconds.\n", interface, update_freq);

    // Infinite loop for monitoring and sending updates
    while (!stop) {
        // Acquire new network info
        network_update(&network);

        // Prepare and send the trigger message
        if (send_trigger(event_name, &network) != 0) {
            fprintf(stderr, "[ERROR] Failed to send trigger message to SketchyBar.\n");
            // Depending on requirements, you might choose to continue or exit
        }

        // Sleep for the specified update frequency
        usleep((useconds_t)(update_freq * 1e6));
    }

    fprintf(stdout, "\n[INFO] Received termination signal. Exiting gracefully.\n");
    return 0;
}
