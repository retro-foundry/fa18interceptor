#ifndef AMIGA_GUEST_MEMORY_H
#define AMIGA_GUEST_MEMORY_H
#include <stddef.h>
#include <stdint.h>
enum { AMIGA_MEMORY_CHIP = 1, AMIGA_MEMORY_FAST = 2 };
typedef struct {
    uint32_t base, size, attributes;
    uint8_t *bytes;
} AmigaGuestBank;
typedef struct { AmigaGuestBank *banks; size_t count; } AmigaGuestMemory;
/* Validate all banks once before use. No mirrored or implicit memory banks. */
int amiga_guest_memory_valid(const AmigaGuestMemory *);
uint8_t *amiga_guest_range(const AmigaGuestMemory *, uint32_t, uint32_t);
void amiga_store_be32(uint8_t *, uint32_t);
void amiga_store_be16(uint8_t *, uint16_t);
#endif
