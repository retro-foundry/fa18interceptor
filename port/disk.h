#ifndef FA18_DISK_H
#define FA18_DISK_H

#include <stddef.h>
#include <stdint.h>

/* An AmigaDOS OFS floppy image (ADF). The game reads its executable, pictures
 * and text files from it. */
typedef struct {
    uint8_t *image;
    size_t size;
} FA18Disk;

int fa18_disk_open(FA18Disk *disk, const char *path);
void fa18_disk_close(FA18Disk *disk);

/* Read a file such as "pix/inst5". Returns a malloc'd buffer or NULL. */
uint8_t *fa18_disk_read(const FA18Disk *disk, const char *path, size_t *size);

#endif
