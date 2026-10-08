#ifndef FA18_NATIVE_STORAGE_H
#define FA18_NATIVE_STORAGE_H
#include <stdint.h>
#include <stddef.h>
/* Source data addresses identify ordinary host buffers. No bus, CPU, MMIO,
 * instruction fetch, chipset state or event clock exists in this backend. */
typedef struct { uint8_t source[0x80000], buffers[0x80000]; } NativeStorage;
void native_storage_bind(NativeStorage *storage);
/* Explicit owner for retained host views; unaffected by later global binds. */
uint8_t *native_storage_span(NativeStorage *storage,uint32_t address,size_t bytes);
uint8_t *native_storage_range(uint32_t address,size_t bytes);
uint8_t native_data_read8(uint32_t address);
uint16_t native_data_read16(uint32_t address);
uint32_t native_data_read32(uint32_t address);
void native_data_write8(uint32_t address,uint8_t value);
void native_data_write16(uint32_t address,uint16_t value);
void native_data_write32(uint32_t address,uint32_t value);
#endif
