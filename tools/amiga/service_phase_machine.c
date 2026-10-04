/* Oracle adapter only: observe the same callbacks on both sides, with the
 * actual machine clock. ROM/program reads are reference evidence; candidate
 * phases are subject to the machine's strict guard instead. */
#define m68k_read_memory_8 oracle_read8
#define m68k_read_memory_16 oracle_read16
#define m68k_read_memory_32 oracle_read32
#define m68k_write_memory_8 oracle_write8
#define m68k_write_memory_16 oracle_write16
#define m68k_write_memory_32 oracle_write32
#include "../../port/machine/machine.c"
#undef m68k_read_memory_8
#undef m68k_read_memory_16
#undef m68k_read_memory_32
#undef m68k_write_memory_8
#undef m68k_write_memory_16
#undef m68k_write_memory_32

typedef struct {
    uint32_t address,value;
    int64_t cycle;
    unsigned size,write;
} Access;
static Access accesses[128],expected[128];
static size_t access_count,expected_count;
static int recording;
static void record_access(uint32_t address,unsigned size,uint32_t value,unsigned write) {
    if (!recording) return;
    if (access_count==sizeof accesses/sizeof accesses[0]) abort();
    Access *a=&accesses[access_count++];
    memset(a,0,sizeof *a);
    a->address=address&0xFFFFFFu; a->size=size; a->value=value; a->write=write;
    a->cycle=fa18_bus_now();
}
#define READ_WRAPPER(bits,bytes) \
    unsigned int m68k_read_memory_##bits(unsigned int a) { \
        unsigned int v=oracle_read##bits(a); record_access(a,bytes,v,0); return v; }
#define WRITE_WRAPPER(bits,bytes) \
    void m68k_write_memory_##bits(unsigned int a,unsigned int v) { \
        oracle_write##bits(a,v); record_access(a,bytes,v,1); }
READ_WRAPPER(8,1)
READ_WRAPPER(16,2)
READ_WRAPPER(32,4)
WRITE_WRAPPER(8,1)
WRITE_WRAPPER(16,2)
WRITE_WRAPPER(32,4)

void amiga_phase_observe_begin(void) {
    access_count=0; recording=1;
    in_execute=1;
    fa18_cycle_origin=fa18_machine->cycle+GET_CYCLES();
}
void amiga_phase_observe_reference(void) {
    expected_count=access_count;
    memcpy(expected,accesses,access_count*sizeof *accesses);
    recording=0;
}
int amiga_phase_observe_compare(void) {
    recording=0;
    if (access_count==expected_count && !memcmp(accesses,expected,access_count*sizeof *accesses)) return 1;
    fprintf(stderr,"service accesses: source=%zu C=%zu\n",expected_count,access_count);
    for (size_t i=0;i<access_count || i<expected_count;++i) {
        if (i<expected_count) fprintf(stderr," source %zu %c%u %06X=%08X cycle=%lld\n",i,
            expected[i].write?'W':'R',expected[i].size,expected[i].address,expected[i].value,(long long)expected[i].cycle);
        if (i<access_count) fprintf(stderr," C      %zu %c%u %06X=%08X cycle=%lld\n",i,
            accesses[i].write?'W':'R',accesses[i].size,accesses[i].address,accesses[i].value,(long long)accesses[i].cycle);
    }
    return 0;
}
