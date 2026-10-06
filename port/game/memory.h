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

#ifdef FA18_NATIVE
#include "native/storage.h"
#define GAME_READ8 native_data_read8
#define GAME_READ16 native_data_read16
#define GAME_READ32 native_data_read32
#define GAME_WRITE8 native_data_write8
#define GAME_WRITE16 native_data_write16
#define GAME_WRITE32 native_data_write32
#else
#include "machine.h"
#define GAME_READ8 fa18_bus_read8
#define GAME_READ16 fa18_bus_read16
#define GAME_READ32 fa18_bus_read32
#define GAME_WRITE8 fa18_bus_write8
#define GAME_WRITE16 fa18_bus_write16
#define GAME_WRITE32 fa18_bus_write32
#endif

typedef uint32_t gaddr; /* an address in game memory */

static inline uint8_t rd_u8(gaddr a) { return GAME_READ8(a); }
static inline int8_t rd_s8(gaddr a) { return (int8_t)GAME_READ8(a); }
static inline uint16_t rd_u16(gaddr a) { return GAME_READ16(a); }
static inline int16_t rd_s16(gaddr a) { return (int16_t)GAME_READ16(a); }
static inline uint32_t rd_u32(gaddr a) { return GAME_READ32(a); }
static inline int32_t rd_s32(gaddr a) { return (int32_t)GAME_READ32(a); }

static inline void wr_u8(gaddr a, uint8_t v) { GAME_WRITE8(a, v); }
static inline void wr_u16(gaddr a, uint16_t v) { GAME_WRITE16(a, v); }
static inline void wr_s16(gaddr a, int16_t v) { GAME_WRITE16(a, (uint16_t)v); }
static inline void wr_u32(gaddr a, uint32_t v) { GAME_WRITE32(a, v); }
static inline void wr_s32(gaddr a, int32_t v) { GAME_WRITE32(a, (uint32_t)v); }

#endif
