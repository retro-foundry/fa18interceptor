/* Structural-oracle-only build variant. Production port dispatch is included
 * unchanged; discard completed fixture bookkeeping before the next fixture.
 * These oracles compare full RAM snapshots, not write-log entries. */
#include "../../port/recomp/recomp_ports.c"

void fa18_structural_reset_write_log(void) {
    if(fa18_write_log_active || mode!=FA18_PORTS_OFF) abort();
    log_count=custom_count=dma_count=0;
    memset(dma_bits,0,sizeof dma_bits);
    fa18_write_log_hardware=0;
}

/* Dispatch-oracle assertion: reference modes must actually complete their
 * selected comparison, not merely continue on an unclassified reference. */
int fa18_structural_port_matched(uint32_t entry) {
    int i;
    for(i=0;i<fa18_port_count;++i) if(fa18_ports[i].entry==entry)
        return stats[i].matched==1 && !stats[i].mismatched && !stats[i].hardware && !stats[i].incomplete;
    return 0;
}

int fa18_structural_port_unused(uint32_t entry) {
    int i;
    for(i=0;i<fa18_port_count;++i) if(fa18_ports[i].entry==entry)
        return stats[i].calls==0 && stats[i].compared==0;
    return 0;
}

/* A hardware-bearing original child is retained by the reference dispatcher.
 * This assertion distinguishes its explicit hardware classification from a
 * completed C comparison, rather than treating it as a matched call. */
int fa18_structural_port_classified(uint32_t entry,int hardware) {
    int i;
    if(!hardware) return fa18_structural_port_matched(entry);
    for(i=0;i<fa18_port_count;++i) if(fa18_ports[i].entry==entry)
        return stats[i].calls==1 && stats[i].hardware==1 && !stats[i].matched &&
            !stats[i].mismatched && !stats[i].incomplete;
    return 0;
}
