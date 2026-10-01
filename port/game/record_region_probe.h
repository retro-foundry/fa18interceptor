#ifndef FA18_GAME_RECORD_REGION_PROBE_H
#define FA18_GAME_RECORD_REGION_PROBE_H

#include <stdint.h>

/* Register state at $C2B05A. The source uses the registers as scratch across
 * two record walks; the generated caller observes their final values. */
typedef struct {
    uint32_t data[8];
    uint32_t address[6];
} RecordRegionProbeRegisters;

void probe_record_regions(RecordRegionProbeRegisters *registers);

#endif
