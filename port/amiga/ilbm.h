#ifndef AMIGA_ILBM_H
#define AMIGA_ILBM_H
#include <stdint.h>
#include <stddef.h>
typedef struct {
    unsigned width,height,planes;
    uint8_t header[20]; /* Original BMHD fields for source bitmap consumers. */
    uint16_t palette[32];
    uint8_t *indices;
} AmigaIlbm;
int amiga_ilbm_decode(AmigaIlbm *out,const uint8_t *bytes,size_t size,char *error,size_t capacity);
void amiga_ilbm_free(AmigaIlbm *image);
#endif
