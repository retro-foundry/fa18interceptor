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

/* Live-continuation oracle: completion must release the retained C frame. */
size_t fa18_structural_native_pending(void) {
    size_t i,count=0;
    for(i=0;i<stepped_count;++i) if(stepped_calls[i].native_child) ++count;
    return count;
}

uint64_t fa18_structural_native_edge_calls(uint32_t caller,uint32_t callee) {
    size_t i;
    for(i=0;i<native_edge_count;++i)
        if(native_edges[i].caller==caller && native_edges[i].callee==callee) return native_edges[i].calls;
    return 0;
}

uint64_t fa18_structural_cpu_entry_calls(uint32_t entry) {
    int i;
    for(i=0;i<fa18_recomp_function_count;++i)
        if(fa18_recomp_functions[i].entry==entry) return profile[i];
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
