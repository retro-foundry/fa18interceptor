/* Test-only RAM read contracts: clear control bit 9 or record bit 6 after
 * the first word/byte read respectively. Both original CPU and normal C
 * facades publish after returning the same original value. Production memory
 * bodies are included with renamed exports; production files stay intact.
 * Long reads/fetches have no publisher, as these contracts concern the
 * source's explicit word/byte flag reloads. */
#define fa18_bus_read16 fa18_original_flight_read16
#define m68k_read_memory_16 fa18_original_flight_cpu_read16
#define fa18_bus_read8 fa18_original_flight_read8
#define m68k_read_memory_8 fa18_original_flight_cpu_read8
#include "../../port/machine/machine.c"
#undef m68k_read_memory_8
#undef fa18_bus_read8
#undef m68k_read_memory_16
#undef fa18_bus_read16
static unsigned contract_reads;
static int contract_active;
static int byte_contract_active;
static unsigned byte_contract_reads;
void fa18_flight_control_read_contract_reset(unsigned profile,uint32_t entry) {
    contract_reads=0; contract_active=entry==0xc149beu && (profile&8192u)!=0;
    byte_contract_reads=0; byte_contract_active=entry==0xc23a7eu && (profile&8192u)!=0 && profile%16u==8u;
}
static uint16_t publish_after_word_read(uint32_t address,uint16_t value) {
    if(contract_active && address==0xc46184u && contract_reads++==0)
        fa18_bus_write16(address,value&0xfdffu);
    return value;
}
uint16_t fa18_bus_read16(uint32_t address) {
    return publish_after_word_read(address,fa18_original_flight_read16(address));
}
unsigned int m68k_read_memory_16(unsigned int address) {
    return publish_after_word_read(address,(uint16_t)fa18_original_flight_cpu_read16(address));
}
static uint8_t publish_after_byte_read(uint32_t address,uint8_t value) {
    if(byte_contract_active && address==0xc60801u && byte_contract_reads++==0)
        fa18_bus_write8(address,value&0xbfu);
    return value;
}
uint8_t fa18_bus_read8(uint32_t address) {
    return publish_after_byte_read(address,fa18_original_flight_read8(address));
}
unsigned int m68k_read_memory_8(unsigned int address) {
    return publish_after_byte_read(address,(uint8_t)fa18_original_flight_cpu_read8(address));
}
