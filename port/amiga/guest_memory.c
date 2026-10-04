#include "guest_memory.h"
int amiga_guest_memory_valid(const AmigaGuestMemory *memory) {
    if (!memory || !memory->banks || !memory->count) return 0;
    for (size_t i=0; i<memory->count; ++i) {
        const AmigaGuestBank *bank=&memory->banks[i];
        uint64_t end=(uint64_t)bank->base+bank->size;
        if (!bank->bytes || !bank->size || end>0x1000000u) return 0;
        for (size_t j=0; j<i; ++j)
            if (bank->base<(uint64_t)memory->banks[j].base+memory->banks[j].size &&
                memory->banks[j].base<end) return 0;
    }
    return 1;
}
uint8_t *amiga_guest_range(const AmigaGuestMemory *memory, uint32_t address, uint32_t size) {
    if (!memory) return NULL;
    for (size_t i=0; i<memory->count; ++i) {
        AmigaGuestBank *bank=&memory->banks[i];
        if (address>=bank->base && address-bank->base<=bank->size &&
            size<=bank->size-(address-bank->base)) return bank->bytes+(address-bank->base);
    }
    return NULL;
}
void amiga_store_be32(uint8_t *p, uint32_t value) {
    p[0]=(uint8_t)(value>>24); p[1]=(uint8_t)(value>>16);
    p[2]=(uint8_t)(value>>8); p[3]=(uint8_t)value;
}
