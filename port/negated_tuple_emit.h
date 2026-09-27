#ifndef FA18_NEGATED_TUPLE_EMIT_H
#define FA18_NEGATED_TUPLE_EMIT_H

#include <stddef.h>
#include <stdint.h>

typedef struct { int16_t x, y, z; } FA18ClipTuple;

/* Direct (non-interpolating) output path in `$C249E6-$C24A7C`: cache one
 * tuple and append it only when the source's negated-X plane comparison holds. */
int fa18_emit_negated_tuple_direct(FA18ClipTuple tuple, FA18ClipTuple *cache,
                                   uint8_t *cache_seen, uint8_t *emitted_count,
                                   FA18ClipTuple *output,
                                   size_t capacity, uint16_t *count,
                                   uint8_t *emitted_flag);

#endif
