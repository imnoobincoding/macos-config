#pragma once

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <mach/arm/kern_return.h>
#include <mach/mach.h>
#include <mach/mach_port.h>
#include <mach/message.h>
#include <bootstrap.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/time.h>

typedef char* env;

#define MACH_HANDLER(name) void name(env env)
typedef MACH_HANDLER(mach_handler);

#define EVENT_MESSAGE_SIZE 512
#define FORMATTED_MESSAGE_SIZE 1024

struct mach_message {
    mach_msg_header_t header;
    mach_msg_size_t msgh_descriptor_count;
    mach_msg_ool_descriptor_t descriptor;
};

struct mach_buffer {
    struct mach_message message;
    mach_msg_trailer_t trailer;
};

static mach_port_t g_mach_port = 0;

/**
 * @brief Retrieves the bootstrap port for SketchyBar.
 *
 * @return mach_port_t The Mach port for SketchyBar, or 0 on failure.
 */
static inline mach_port_t mach_get_bs_port() {
    mach_port_name_t task = mach_task_self();

    mach_port_t bs_port;
    if (task_get_special_port(task, TASK_BOOTSTRAP_PORT, &bs_port) != KERN_SUCCESS) {
        fprintf(stderr, "[ERROR] Failed to get bootstrap port.\n");
        return 0;
    }

    const char* name = getenv("BAR_NAME");
    if (!name) {
        name = "sketchybar";
    } else {
        // Validate that 'name' contains only alphanumerics and underscores
        for (size_t i = 0; i < strlen(name); ++i) {
            if (!isalnum(name[i]) && name[i] != '_') {
                fprintf(stderr, "[ERROR] Invalid characters in BAR_NAME environment variable.\n");
                return 0;
            }
        }
    }

    size_t lookup_len = 16 + strlen(name) + 1; // +1 for null terminator
    char* buffer = malloc(lookup_len);
    if (!buffer) {
        perror("[ERROR] Failed to allocate memory for buffer");
        return 0;
    }
    snprintf(buffer, lookup_len, "git.felix.%s", name);

    mach_port_t port;
    if (bootstrap_look_up(bs_port, buffer, &port) != KERN_SUCCESS) {
        fprintf(stderr, "[ERROR] Failed to look up bootstrap service for '%s'.\n", buffer);
        free(buffer);
        return 0;
    }

    free(buffer);
    return port;
}

/**
 * @brief Sends a Mach message to the specified port.
 *
 * @param port The Mach port to send the message to.
 * @param message The message to send.
 * @param len The length of the message.
 * @return true If the message was sent successfully.
 * @return false If the message failed to send.
 */
static inline bool mach_send_message(mach_port_t port, const char* message, uint32_t len) {
    if (!message || !port) {
        fprintf(stderr, "[ERROR] Invalid message or port.\n");
        return false;
    }

    struct mach_message msg = {0};
    msg.header.msgh_remote_port = port;
    msg.header.msgh_local_port = MACH_PORT_NULL;
    msg.header.msgh_id = 0;
    msg.header.msgh_bits = MACH_MSGH_BITS_SET(MACH_MSG_TYPE_COPY_SEND,
                                              MACH_MSG_TYPE_MAKE_SEND,
                                              0,
                                              MACH_MSGH_BITS_COMPLEX);

    msg.header.msgh_size = sizeof(struct mach_message);
    msg.msgh_descriptor_count = 1;

    msg.descriptor.address = (void*)message;
    msg.descriptor.size = len * sizeof(char);
    msg.descriptor.copy = MACH_MSG_VIRTUAL_COPY;
    msg.descriptor.deallocate = false;
    msg.descriptor.type = MACH_MSG_OOL_DESCRIPTOR;

    kern_return_t err = mach_msg(&msg.header,
                                 MACH_SEND_MSG,
                                 sizeof(struct mach_message),
                                 0,
                                 MACH_PORT_NULL,
                                 MACH_MSG_TIMEOUT_NONE,
                                 MACH_PORT_NULL);

    if (err != KERN_SUCCESS) {
        fprintf(stderr, "[ERROR] mach_msg failed with error: %d\n", err);
        return false;
    }

    return true;
}

/**
 * @brief Formats a command message by removing quotes and splitting on spaces not enclosed in quotes.
 *
 * @param message The original message string.
 * @param formatted_message The buffer to store the formatted message.
 * @param max_len The maximum length of the formatted message buffer.
 * @return uint32_t The length of the formatted message.
 */
static inline uint32_t format_message(const char* message, char* formatted_message, size_t max_len) {
    if (!message || !formatted_message) return 0;

    char outer_quote = 0;
    uint32_t caret = 0;
    size_t message_length = strlen(message);

    for (size_t i = 0; i < message_length; ++i) {
        if (message[i] == '"' || message[i] == '\'') {
            if (outer_quote && outer_quote == message[i]) {
                outer_quote = 0;
            } else if (!outer_quote) {
                outer_quote = message[i];
            }
            continue;
        }
        if (caret >= max_len - 1) {
            fprintf(stderr, "[ERROR] Formatted message exceeds buffer size.\n");
            break;
        }
        formatted_message[caret] = message[i];
        if (message[i] == ' ' && !outer_quote) {
            formatted_message[caret] = '\0';
        }
        caret++;
    }

    // Ensure null termination
    if (caret < max_len) {
        formatted_message[caret] = '\0';
    } else {
        formatted_message[max_len - 1] = '\0';
    }

    return caret + 1;
}

/**
 * @brief Sends a formatted message to SketchyBar.
 *
 * @param message The message to send.
 */
static inline void sketchybar(const char* message) {
    if (!message) return;

    char formatted_message[FORMATTED_MESSAGE_SIZE] = {0};
    uint32_t length = format_message(message, formatted_message, sizeof(formatted_message));
    if (length == 0) {
        fprintf(stderr, "[ERROR] Failed to format message.\n");
        return;
    }

    if (!g_mach_port) {
        g_mach_port = mach_get_bs_port();
        if (!g_mach_port) {
            fprintf(stderr, "[ERROR] Failed to obtain Mach port.\n");
            return;
        }
    }

    if (!mach_send_message(g_mach_port, formatted_message, length)) {
        fprintf(stderr, "[WARN] First attempt to send message failed. Reconnecting...\n");
        g_mach_port = mach_get_bs_port();
        if (!g_mach_port) {
            fprintf(stderr, "[ERROR] Failed to obtain Mach port on retry.\n");
            return;
        }
        if (!mach_send_message(g_mach_port, formatted_message, length)) {
            fprintf(stderr, "[ERROR] Second attempt to send message failed. Exiting.\n");
            exit(EXIT_FAILURE);
        }
    }
}
