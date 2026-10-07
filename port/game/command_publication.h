#ifndef FA18_COMMAND_PUBLICATION_H
#define FA18_COMMAND_PUBLICATION_H
#include "memory.h"
/* C1C23C..C1C2B8 shared command exit, including its signed queue indices. */
enum CommandPublicationPhase {
    COMMAND_PUBLICATION_TAKEN, COMMAND_PUBLICATION_RELEASE,
    COMMAND_PUBLICATION_CLAIM, COMMAND_PUBLICATION_COUNT,
    COMMAND_PUBLICATION_WRITE_INDEX, COMMAND_PUBLICATION_RESET_INDEX,
    COMMAND_PUBLICATION_RAW, COMMAND_PUBLICATION_TRANSLATED,
    COMMAND_PUBLICATION_ADVANCE_INDEX, COMMAND_PUBLICATION_ADVANCE_COUNT,
    COMMAND_PUBLICATION_TRANSLATED_INDEX, COMMAND_PUBLICATION_TRANSLATED_WRITE,
    COMMAND_PUBLICATION_CLEAR
};
typedef struct {
    void (*observe)(void *context,enum CommandPublicationPhase phase,
                    uint32_t value,gaddr address);
    void *context;
} CommandPublicationHooks;
/* Returns the translated byte when queued; otherwise the incoming byte.
 * The CPU adapter preserves upper bytes and all source flags independently. */
uint8_t publish_command_event(uint8_t raw,const CommandPublicationHooks *hooks);
/* The actual translated-store index is an output of C1C298/C1C29E, including
 * signed indices and raw-store aliases. No store leaves queued false. */
typedef struct { uint8_t event; int queued; int8_t translated_index; } CommandPublicationResult;
CommandPublicationResult publish_command_event_result(uint8_t raw,const CommandPublicationHooks *hooks);
#endif
