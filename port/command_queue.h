#ifndef FA18_COMMAND_QUEUE_H
#define FA18_COMMAND_QUEUE_H

#include "context_command_input.h"
#include "field_bytes.h"

enum { FA18_COMMAND_QUEUE_NEIGHBORS = 266, FA18_COMMAND_KEY_TABLE_SIZE = 128 };

/* The original signed indices also reach neighboring globals. Each slot
 * refers to its ordinary C owner; a word reference selects its high/low byte
 * by value, independent of the host's byte order. Unmodeled neighboring data
 * must be imported from the original game data, like the key translation table.
 * This bounded layout is not an address space or an instruction runtime. */
typedef PortFieldByte FA18CommandQueueByte;

typedef struct {
    FA18CommandInput *commands;
    uint8_t taken, count, write_index, translated_index;
    uint8_t raw[10], translated[11];
    uint8_t neighbors[FA18_COMMAND_QUEUE_NEIGHBORS];
    uint8_t key_table[FA18_COMMAND_KEY_TABLE_SIZE];
    FA18CommandQueueByte slots[FA18_COMMAND_QUEUE_NEIGHBORS];
} FA18CommandQueue;

/* Import the original 266-byte neighboring-global window (raw index -128
 * through translated index +127) and 128 translation bytes. Import includes
 * the currently modeled command/flight/view/context fields in this window.
 * context->key_taken is bound to queue->taken. Owners and queue must remain
 * at stable addresses; copying this structure does not rebind its pointers.
 * Returns 0 for missing data/owners, without changing them. */
int fa18_initialize_command_queue(FA18CommandQueue *queue,
                                  FA18ContextCommandState *context,
                                  const uint8_t *neighbors, size_t neighbor_count,
                                  const uint8_t *key_table, size_t key_count);

/* Attach a newly ported byte owner at a bounded neighboring-data offset.
 * Import the current canonical value before rebinding, including word bytes.
 * Returns 0 for an uninitialized queue or an invalid offset/owner. */
int fa18_bind_command_queue_byte(FA18CommandQueue *queue, unsigned offset,
                                 uint8_t *owner);
/* Import/rebind both big-endian bytes atomically with respect to owner
 * initialization, including when that same word already owns the slots. */
int fa18_bind_command_queue_word(FA18CommandQueue *queue, unsigned offset,
                                 int16_t *owner);

/* Complete $C1C23C-$C1C2B8 publication: signed count/index checks, original
 * write order, translation and unconditional modifier clearing. Queued events
 * retain the high word and replace the entire low word with the translation;
 * gated events retain their value. Returns 0 for invalid pointers/missing
 * field owners, retaining preceding stores and leaving the result unset. */
int fa18_publish_native_command(FA18CommandQueue *queue, uint32_t event,
                                uint32_t *published_event);
/* Same publication with the live carried axis used by record/view owners.
 * Signed queue indices replace its low word; the wrap-to-zero path clears
 * the whole value. NULL retains the event-only interface above. */
int fa18_publish_native_command_with_axis(FA18CommandQueue *queue,uint32_t event,
                                            uint32_t *published_event,uint32_t *axis);

#endif
