/* Register bridge for $C2B05A. */
#include "glue.h"
#include "ports_glue.h"

#include "record_region_probe.h"


int glue_C2B05A(void) {
    RecordRegionProbeRegisters registers;
    unsigned i;
    for (i = 0; i < 8; ++i) registers.data[i] = D(i);
    for (i = 0; i < 6; ++i) registers.address[i] = A(i);
    probe_record_regions(&registers);
    for (i = 0; i < 8; ++i) D(i) = registers.data[i];
    for (i = 0; i < 6; ++i) A(i) = registers.address[i];
    return glue_return();
}
