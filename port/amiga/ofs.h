#ifndef AMIGA_OFS_H
#define AMIGA_OFS_H
#include <stddef.h>
#include <stdint.h>
/* Read-only OFS backend. Paths are root-relative with '/' components; a DOS
 * adapter owns volume prefixes, locks, current directories and timing. */
typedef struct { uint8_t *image; size_t size; } AmigaOfs;
typedef struct {
    uint32_t block,parent,size,protection,days,minutes,ticks;
    int32_t type; /* On-disk secondary type: volume 1, directory 2, file -3. */
    char name[31],comment[80];
} AmigaOfsEntry;
int amiga_ofs_open(AmigaOfs *,const char *path);
void amiga_ofs_close(AmigaOfs *);
int amiga_ofs_find(const AmigaOfs *,const char *path,AmigaOfsEntry *);
uint8_t *amiga_ofs_read(const AmigaOfs *,const char *path,size_t *size);
int amiga_ofs_read_range(const AmigaOfs *,const char *path,size_t offset,
                         void *data,size_t capacity,size_t *read_size);
/* Physical hash bucket/chain order, not a promise about DOS ExNext ordering.
 * Zero from the callback ends a successful scan. */
int amiga_ofs_list(const AmigaOfs *,const char *directory,
                   int (*visit)(void *,const AmigaOfsEntry *),void *context);
#endif
