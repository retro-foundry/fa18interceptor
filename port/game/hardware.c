#include "hardware.h"

#include "machine.h"

#define CUSTOM_BASE 0xDFF000u

void custom_write(unsigned reg, uint16_t value) { fa18_bus_write16(CUSTOM_BASE + reg, value); }

void custom_write_ptr(unsigned reg, uint32_t value) { fa18_bus_write32(CUSTOM_BASE + reg, value); }

uint16_t custom_read(unsigned reg) { return fa18_bus_read16(CUSTOM_BASE + reg); }

void wait_blitter(void) { fa18_machine_wait_blitter(); }
