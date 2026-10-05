#include "input_callback_registration.h"

int fa18_remove_native_input_callback(FA18InputCallbackRegistration *s) {
    if(!s || !s->descriptor || !s->consume) return 0;
    return s->consume(s->context, FA18_INPUT_CALLBACK_REMOVE, 5, s->descriptor);
}

int fa18_install_native_input_callback(FA18InputCallbackRegistration *s) {
    if(!s || !s->descriptor || !s->consume || !s->name || !s->callback) return 0;
    s->descriptor->type=2;
    s->descriptor->priority=0;
    s->descriptor->name=s->name;
    s->descriptor->callback=s->callback;
    s->descriptor->context=s->callback_context;
    return s->consume(s->context, FA18_INPUT_CALLBACK_ADD, 5, s->descriptor);
}
