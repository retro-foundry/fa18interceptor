#ifndef FA18_EXEC_SERVICE_STATE_H
#define FA18_EXEC_SERVICE_STATE_H
#include "../amiga/exec_task_services.h"
#include "service_phase.h"
#include <string.h>
/* Shared CPU/bus adapter only. A memory-to-memory MOVE may fetch its
 * destination extensions after reading its source. The callback context
 * contains that pending word count; it is consumed once, after the read. */
static inline void fa18_exec_service_trailing(void *c) {
    if (c && *(unsigned *)c) {
        unsigned n=*(unsigned *)c; *(unsigned *)c=0; fa18_service_extension_words(n);
    }
}
static inline uint8_t fa18_exec_service_read8(void *c,uint32_t a) {
    uint8_t v=(uint8_t)m68k_read_memory_8(a); fa18_exec_service_trailing(c); return v;
}
static inline uint16_t fa18_exec_service_read16(void *c,uint32_t a) {
    uint16_t v=(uint16_t)m68k_read_memory_16(a); fa18_exec_service_trailing(c); return v;
}
static inline uint32_t fa18_exec_service_read32(void *c,uint32_t a) {
    uint32_t v=m68k_read_memory_32(a); fa18_exec_service_trailing(c); return v;
}
static inline void fa18_exec_service_write8(void *c,uint32_t a,uint8_t v) { (void)c; m68k_write_memory_8(a,v); }
static inline void fa18_exec_service_write16(void *c,uint32_t a,uint16_t v) { (void)c; m68k_write_memory_16(a,v); }
static inline void fa18_exec_service_write32(void *c,uint32_t a,uint32_t v) { (void)c; m68k_write_memory_32(a,v); }
static inline AmigaExecTaskBus fa18_exec_service_bus(void *context) {
    AmigaExecTaskBus bus={context,fa18_exec_service_read8,fa18_exec_service_read16,fa18_exec_service_read32,
        fa18_exec_service_write8,fa18_exec_service_write16,fa18_exec_service_write32};
    return bus;
}
static inline void fa18_exec_service_load(AmigaExecTaskState *s) {
    memcpy(s->d,REG_D,sizeof s->d); memcpy(s->a,REG_A,sizeof s->a); s->ccr=(uint8_t)m68ki_get_ccr();
}
static inline void fa18_exec_service_store(const AmigaExecTaskState *s) {
    memcpy(REG_D,s->d,sizeof s->d); memcpy(REG_A,s->a,sizeof s->a); m68ki_set_ccr(s->ccr);
}
#endif
