/* Test-only ordered publication after the first collision-class byte read.
 * Both CPU and normal C read the original value; RAM changes afterwards.
 * This proves the source's duplicate-read arm without claiming that the
 * recorded game's interrupt publishers have been reconstructed. */
#define fa18_bus_read8 fa18_original_dynamics_read8
#define m68k_read_memory_8 fa18_original_dynamics_cpu_read8
#include "../../port/machine/machine.c"
#undef fa18_bus_read8
#undef m68k_read_memory_8
static uint32_t class_address;
void flight_dynamics_publish_class(uint32_t address) { class_address=address; }
static uint8_t published_read(uint32_t address,uint8_t value) {
    if(class_address && address==class_address) {
        class_address=0; fa18_bus_write8(address,0);
    }
    return value;
}
uint8_t fa18_bus_read8(uint32_t address) { return published_read(address,fa18_original_dynamics_read8(address)); }
unsigned int m68k_read_memory_8(unsigned int address) { return published_read(address,(uint8_t)fa18_original_dynamics_cpu_read8(address)); }
