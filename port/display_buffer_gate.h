#ifndef FA18_DISPLAY_BUFFER_GATE_H
#define FA18_DISPLAY_BUFFER_GATE_H
#include <stdint.h>
typedef int (*FA18DisplayBufferStage)(void *);
typedef struct { FA18DisplayBufferStage active_planes,alternate; void *context; } FA18DisplayBufferGateOps;
int fa18_prepare_display_buffer(uint16_t flags,const FA18DisplayBufferGateOps*);
#endif
