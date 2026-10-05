#ifndef FA18_INPUT_CALLBACK_REGISTRATION_H
#define FA18_INPUT_CALLBACK_REGISTRATION_H

#include <stdint.h>

typedef void (*FA18NativeInputCallback)(void *context);
typedef struct {
    uint8_t type;
    int8_t priority;
    const char *name;
    FA18NativeInputCallback callback;
    void *context;
} FA18NativeInputDescriptor;

typedef enum { FA18_INPUT_CALLBACK_REMOVE, FA18_INPUT_CALLBACK_ADD } FA18InputCallbackOperation;
typedef struct {
    FA18NativeInputDescriptor *descriptor;
    /* Actual original name and actual native input callback, supplied by
     * their owners. Registration does not invent a callback implementation. */
    const char *name;
    FA18NativeInputCallback callback;
    void *callback_context;
    int (*consume)(void *context, FA18InputCallbackOperation operation,
                    unsigned kind, FA18NativeInputDescriptor *descriptor);
    void *context;
} FA18InputCallbackRegistration;

/* Actual $C1748C/$C17456 game-side bodies. Host registration/removal (kind 5)
 * remains a required service. Installation writes the descriptor first;
 * those writes remain if the service fails. */
int fa18_remove_native_input_callback(FA18InputCallbackRegistration *state);
int fa18_install_native_input_callback(FA18InputCallbackRegistration *state);

#endif
