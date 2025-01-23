#include <Carbon/Carbon.h>
#include <CoreFoundation/CoreFoundation.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

// Macro for safely releasing CF objects
#define CFReleaseSafe(cf) \
    if (cf) { CFRelease(cf); cf = NULL; }

// Logging macro for easier debugging
#define LOG_ERROR(fmt, ...) fprintf(stderr, "ERROR: " fmt "\n", ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) fprintf(stdout, "INFO: " fmt "\n", ##__VA_ARGS__)

// Initialize accessibility and ensure the process is trusted
void ax_init() {
    const void *keys[] = { kAXTrustedCheckOptionPrompt };
    const void *values[] = { kCFBooleanTrue };
    
    CFDictionaryRef options = CFDictionaryCreate(kCFAllocatorDefault,
                                                 keys,
                                                 values,
                                                 sizeof(keys) / sizeof(*keys),
                                                 &kCFCopyStringDictionaryKeyCallBacks,
                                                 &kCFTypeDictionaryValueCallBacks);
    if (!options) {
        LOG_ERROR("Failed to create CFDictionary for accessibility options.");
        exit(EXIT_FAILURE);
    }
    
    bool trusted = AXIsProcessTrustedWithOptions(options);
    CFReleaseSafe(options);
    
    if (!trusted) {
        LOG_ERROR("Accessibility permissions not granted. Exiting.");
        exit(EXIT_FAILURE);
    }
}

// Perform a click action on an AXUIElement
void ax_perform_click(AXUIElementRef element) {
    if (!element) {
        LOG_ERROR("AXUIElementRef is NULL. Cannot perform click.");
        return;
    }

    AXError error;
    
    error = AXUIElementPerformAction(element, kAXPressAction);
    if (error != kAXErrorSuccess) {
        LOG_ERROR("Failed to perform press action: %d", error);
        return;
    }
    
    // Short delay to allow the action to take effect
    usleep(150000);
}

// Retrieve the title attribute of an AXUIElement
CFStringRef ax_get_title(AXUIElementRef element) {
    if (!element) {
        LOG_ERROR("AXUIElementRef is NULL. Cannot get title.");
        return NULL;
    }
    
    CFTypeRef title = NULL;
    AXError error = AXUIElementCopyAttributeValue(element, kAXTitleAttribute, &title);
    
    if (error != kAXErrorSuccess) {
        LOG_ERROR("Failed to copy title attribute: %d", error);
        return NULL;
    }
    
    if (CFGetTypeID(title) != CFStringGetTypeID()) {
        LOG_ERROR("Title attribute is not a CFString.");
        CFReleaseSafe(title);
        return NULL;
    }
    
    return (CFStringRef)title;
}

// Select a menu option by its ID
bool ax_select_menu_option(AXUIElementRef app, int id) {
    if (!app) {
        LOG_ERROR("AXUIElementRef app is NULL. Cannot select menu option.");
        return false;
    }

    AXUIElementRef menubar = NULL;
    AXError error = AXUIElementCopyAttributeValue(app, kAXMenuBarAttribute, (CFTypeRef *)&menubar);
    if (error != kAXErrorSuccess || !menubar) {
        LOG_ERROR("Failed to copy menu bar attribute: %d", error);
        return false;
    }
    
    CFArrayRef children = NULL;
    error = AXUIElementCopyAttributeValue(menubar, kAXVisibleChildrenAttribute, (CFTypeRef *)&children);
    CFReleaseSafe(menubar);
    
    if (error != kAXErrorSuccess || !children) {
        LOG_ERROR("Failed to copy visible children: %d", error);
        return false;
    }
    
    CFIndex count = CFArrayGetCount(children);
    if (id < 0 || id >= count) {
        LOG_ERROR("Menu option ID %d is out of bounds. Total options: %ld", id, count);
        CFReleaseSafe(children);
        return false;
    }
    
    AXUIElementRef item = (AXUIElementRef)CFArrayGetValueAtIndex(children, id);
    if (!item) {
        LOG_ERROR("Menu item at index %d is NULL.", id);
        CFReleaseSafe(children);
        return false;
    }
    
    ax_perform_click(item);
    CFReleaseSafe(children);
    return true;
}

// Print all visible menu options
bool ax_print_menu_options(AXUIElementRef app) {
    if (!app) {
        LOG_ERROR("AXUIElementRef app is NULL. Cannot print menu options.");
        return false;
    }

    AXUIElementRef menubar = NULL;
    AXError error = AXUIElementCopyAttributeValue(app, kAXMenuBarAttribute, (CFTypeRef *)&menubar);
    if (error != kAXErrorSuccess || !menubar) {
        LOG_ERROR("Failed to copy menu bar attribute: %d", error);
        return false;
    }
    
    CFArrayRef children = NULL;
    error = AXUIElementCopyAttributeValue(menubar, kAXVisibleChildrenAttribute, (CFTypeRef *)&children);
    CFReleaseSafe(menubar);
    
    if (error != kAXErrorSuccess || !children) {
        LOG_ERROR("Failed to copy visible children: %d", error);
        return false;
    }
    
    CFIndex count = CFArrayGetCount(children);
    for (CFIndex i = 0; i < count; i++) {
        AXUIElementRef item = (AXUIElementRef)CFArrayGetValueAtIndex(children, i);
        if (!item) {
            LOG_ERROR("Menu item at index %ld is NULL.", i);
            continue;
        }
        
        CFStringRef title = ax_get_title(item);
        if (title) {
            char buffer[256];
            if (CFStringGetCString(title, buffer, sizeof(buffer), kCFStringEncodingUTF8)) {
                printf("[%ld] %s\n", i, buffer);
            } else {
                LOG_ERROR("Failed to convert CFStringRef to C string.");
            }
            CFReleaseSafe(title);
        }
    }
    
    CFReleaseSafe(children);
    return true;
}

// Retrieve an extra menu item based on an alias
AXUIElementRef ax_get_extra_menu_item(const char* alias) {
    if (!alias) {
        LOG_ERROR("Alias is NULL. Cannot retrieve extra menu item.");
        return NULL;
    }

    CFArrayRef window_list = CGWindowListCopyWindowInfo(kCGWindowListOptionAll, kCGNullWindowID);
    if (!window_list) {
        LOG_ERROR("Failed to copy window list.");
        return NULL;
    }

    char owner_buffer[256] = {0};
    char name_buffer[256] = {0};
    char buffer[512] = {0};
    pid_t pid = 0;
    CGRect bounds = CGRectNull;
    
    CFIndex window_count = CFArrayGetCount(window_list);
    for (CFIndex i = 0; i < window_count; ++i) {
        CFDictionaryRef dictionary = (CFDictionaryRef)CFArrayGetValueAtIndex(window_list, i);
        if (!dictionary) continue;
        
        CFStringRef owner_ref = CFDictionaryGetValue(dictionary, kCGWindowOwnerName);
        CFNumberRef owner_pid_ref = CFDictionaryGetValue(dictionary, kCGWindowOwnerPID);
        CFStringRef name_ref = CFDictionaryGetValue(dictionary, kCGWindowName);
        CFNumberRef layer_ref = CFDictionaryGetValue(dictionary, kCGWindowLayer);
        CFDictionaryRef bounds_ref = CFDictionaryGetValue(dictionary, kCGWindowBounds);
        
        if (!owner_ref || !owner_pid_ref || !name_ref || !layer_ref || !bounds_ref)
            continue;
        
        long long int layer = 0;
        if (!CFNumberGetValue(layer_ref, kCFNumberLongLongType, &layer) || layer != 25) { // 25 is typically the layer for normal windows
            continue;
        }
        
        if (!CGRectMakeWithDictionaryRepresentation(bounds_ref, &bounds))
            continue;
        
        if (!CFStringGetCString(owner_ref, owner_buffer, sizeof(owner_buffer), kCFStringEncodingUTF8))
            continue;
        if (!CFStringGetCString(name_ref, name_buffer, sizeof(name_buffer), kCFStringEncodingUTF8))
            continue;
        
        snprintf(buffer, sizeof(buffer), "%s,%s", owner_buffer, name_buffer);
        if (strcmp(buffer, alias) == 0) {
            if (!CFNumberGetValue(owner_pid_ref, kCFNumberSInt32Type, &pid)) {
                LOG_ERROR("Failed to get PID from CFNumberRef.");
                CFReleaseSafe(window_list);
                return NULL;
            }
            break;
        }
    }
    
    CFReleaseSafe(window_list);
    
    if (!pid) {
        LOG_ERROR("No matching window found for alias '%s'.", alias);
        return NULL;
    }
    
    AXUIElementRef app = AXUIElementCreateApplication(pid);
    if (!app) {
        LOG_ERROR("Failed to create AXUIElementRef for PID %d.", pid);
        return NULL;
    }
    
    AXUIElementRef extras = NULL;
    AXError error = AXUIElementCopyAttributeValue(app, kAXExtrasMenuBarAttribute, (CFTypeRef *)&extras);
    if (error != kAXErrorSuccess || !extras) {
        LOG_ERROR("Failed to copy extras menu bar attribute: %d", error);
        CFReleaseSafe(app);
        return NULL;
    }
    
    AXUIElementRef result = NULL;
    CFArrayRef children = NULL;
    error = AXUIElementCopyAttributeValue(extras, kAXVisibleChildrenAttribute, (CFTypeRef *)&children);
    CFReleaseSafe(extras);
    
    if (error != kAXErrorSuccess || !children) {
        LOG_ERROR("Failed to copy visible children from extras: %d", error);
        CFReleaseSafe(app);
        return NULL;
    }
    
    CFIndex count = CFArrayGetCount(children);
    for (CFIndex i = 0; i < count; i++) {
        AXUIElementRef item = (AXUIElementRef)CFArrayGetValueAtIndex(children, i);
        if (!item) continue;
        
        CGPoint position = CGPointZero;
        CGSize size = CGSizeZero;
        AXValueRef position_ref = NULL;
        AXValueRef size_ref = NULL;
        
        AXError pos_error = AXUIElementCopyAttributeValue(item, kAXPositionAttribute, (CFTypeRef *)&position_ref);
        AXError size_error = AXUIElementCopyAttributeValue(item, kAXSizeAttribute, (CFTypeRef *)&size_ref);
        
        if (pos_error != kAXErrorSuccess || size_error != kAXErrorSuccess || !position_ref || !size_ref) {
            CFReleaseSafe(position_ref);
            CFReleaseSafe(size_ref);
            continue;
        }
        
        if (!AXValueGetValue(position_ref, kAXValueCGPointType, &position) ||
            !AXValueGetValue(size_ref, kAXValueCGSizeType, &size)) {
            LOG_ERROR("Failed to get CGPoint or CGSize from AXValueRef.");
            CFReleaseSafe(position_ref);
            CFReleaseSafe(size_ref);
            continue;
        }
        
        CFReleaseSafe(position_ref);
        CFReleaseSafe(size_ref);
        
        // Adjust the threshold as needed
        if (fabs(position.x - bounds.origin.x) <= 10.0) {
            result = item;
            break;
        }
    }
    
    CFReleaseSafe(children);
    CFReleaseSafe(app);
    return result;
}

// Select an extra menu option by alias
bool ax_select_menu_extra(const char* alias) {
    if (!alias) {
        LOG_ERROR("Alias is NULL. Cannot select extra menu.");
        return false;
    }

    AXUIElementRef item = ax_get_extra_menu_item(alias);
    if (!item) {
        LOG_ERROR("Extra menu item for alias '%s' not found.", alias);
        return false;
    }

    // Hide the menu bar before performing the click
    extern int SLSMainConnectionID();
    extern void SLSSetMenuBarVisibilityOverrideOnDisplay(int cid, int did, bool enabled);
    extern void SLSSetMenuBarInsetAndAlpha(int cid, double u1, double u2, float alpha);
    
    int cid = SLSMainConnectionID();
    SLSSetMenuBarInsetAndAlpha(cid, 0, 1, 0.0f);
    SLSSetMenuBarVisibilityOverrideOnDisplay(cid, 0, true);
    SLSSetMenuBarInsetAndAlpha(cid, 0, 1, 0.0f);
    
    ax_perform_click(item);
    
    SLSSetMenuBarVisibilityOverrideOnDisplay(cid, 0, false);
    SLSSetMenuBarInsetAndAlpha(cid, 0, 1, 1.0f);
    
    CFReleaseSafe(item);
    return true;
}

// Retrieve the frontmost application as an AXUIElementRef
AXUIElementRef ax_get_front_app() {
    extern void _SLPSGetFrontProcess(ProcessSerialNumber* psn);
    extern void SLSGetConnectionIDForPSN(int cid, ProcessSerialNumber* psn, int* cid_out);
    extern void SLSConnectionGetPID(int cid, pid_t* pid_out);
    
    ProcessSerialNumber psn;
    _SLPSGetFrontProcess(&psn);
    
    int target_cid = 0;
    SLSGetConnectionIDForPSN(SLSMainConnectionID(), &psn, &target_cid);
    
    pid_t pid = 0;
    SLSConnectionGetPID(target_cid, &pid);
    if (!pid) {
        LOG_ERROR("Failed to get PID for front process.");
        return NULL;
    }
    
    AXUIElementRef app = AXUIElementCreateApplication(pid);
    if (!app) {
        LOG_ERROR("Failed to create AXUIElementRef for PID %d.", pid);
        return NULL;
    }
    
    return app;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s [-l | -s id | -s alias]\n", argv[0]);
        return EXIT_SUCCESS;
    }
    
    ax_init();
    
    if (strcmp(argv[1], "-l") == 0) {
        AXUIElementRef app = ax_get_front_app();
        if (!app) {
            LOG_ERROR("Failed to get front application.");
            return EXIT_FAILURE;
        }
        if (!ax_print_menu_options(app)) {
            LOG_ERROR("Failed to print menu options.");
            CFReleaseSafe(app);
            return EXIT_FAILURE;
        }
        CFReleaseSafe(app);
    }
    else if (strcmp(argv[1], "-s") == 0) {
        if (argc < 3) {
            LOG_ERROR("Missing argument for -s option.");
            return EXIT_FAILURE;
        }
        
        int id = -1;
        if (sscanf(argv[2], "%d", &id) == 1) {
            if (id < 0) {
                LOG_ERROR("Invalid menu option ID: %d", id);
                return EXIT_FAILURE;
            }
            AXUIElementRef app = ax_get_front_app();
            if (!app) {
                LOG_ERROR("Failed to get front application.");
                return EXIT_FAILURE;
            }
            if (!ax_select_menu_option(app, id)) {
                LOG_ERROR("Failed to select menu option with ID %d.", id);
                CFReleaseSafe(app);
                return EXIT_FAILURE;
            }
            CFReleaseSafe(app);
        }
        else {
            // Treat argv[2] as an alias
            if (!ax_select_menu_extra(argv[2])) {
                LOG_ERROR("Failed to select extra menu with alias '%s'.", argv[2]);
                return EXIT_FAILURE;
            }
        }
    }
    else {
        LOG_ERROR("Unknown option '%s'.", argv[1]);
        printf("Usage: %s [-l | -s id | -s alias]\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}