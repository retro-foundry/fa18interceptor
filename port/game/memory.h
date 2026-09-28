#ifndef FA18_GAME_MEMORY_H
#define FA18_GAME_MEMORY_H

/* Game memory access for the recreated source.
 *
 * While part of the game is still generated code, game state lives in the
 * original big-endian 68000 memory image (Chip RAM and Slow RAM) owned by the
 * machine layer, and C code addresses it with the original addresses. Tables
 * and records are therefore referenced as `gaddr` values with named field
 * offsets. When the last generated routine is gone, these become ordinary C
 * pointers and structs. */

#include <stdint.h>

#include "machine.h"

typedef uint32_t gaddr; /* an address in game memory */

static inline uint8_t rd_u8(gaddr a) { return fa18_bus_read8(a); }
static inline int8_t rd_s8(gaddr a) { return (int8_t)fa18_bus_read8(a); }
static inline uint16_t rd_u16(gaddr a) { return fa18_bus_read16(a); }
static inline int16_t rd_s16(gaddr a) { return (int16_t)fa18_bus_read16(a); }
static inline uint32_t rd_u32(gaddr a) { return fa18_bus_read32(a); }
static inline int32_t rd_s32(gaddr a) { return (int32_t)fa18_bus_read32(a); }

static inline void wr_u8(gaddr a, uint8_t v) { fa18_bus_write8(a, v); }
static inline void wr_u16(gaddr a, uint16_t v) { fa18_bus_write16(a, v); }
static inline void wr_s16(gaddr a, int16_t v) { fa18_bus_write16(a, (uint16_t)v); }
static inline void wr_u32(gaddr a, uint32_t v) { fa18_bus_write32(a, v); }
static inline void wr_s32(gaddr a, int32_t v) { fa18_bus_write32(a, (uint32_t)v); }

#endif
