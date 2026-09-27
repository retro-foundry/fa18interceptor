#include "disk.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { BLOCK = 512, ROOT_BLOCK = 880, HASH_SIZE = 72 };

static uint32_t be32(const uint8_t *p) {
    return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3];
}

static const uint8_t *block(const FA18Disk *disk, uint32_t number) {
    if ((size_t)(number + 1) * BLOCK > disk->size) return NULL;
    return disk->image + (size_t)number * BLOCK;
}

int fa18_disk_open(FA18Disk *disk, const char *path) {
    memset(disk, 0, sizeof *disk);
    FILE *file = fopen(path, "rb");
    if (!file) return 0;
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    disk->image = size > 0 ? malloc((size_t)size) : NULL;
    if (!disk->image || fread(disk->image, 1, (size_t)size, file) != (size_t)size) {
        fclose(file);
        free(disk->image);
        disk->image = NULL;
        return 0;
    }
    fclose(file);
    disk->size = (size_t)size;
    return memcmp(disk->image, "DOS", 3) == 0 && disk->image[3] == 0;
}

void fa18_disk_close(FA18Disk *disk) {
    free(disk->image);
    memset(disk, 0, sizeof *disk);
}

static uint32_t name_hash(const char *name, size_t length) {
    uint32_t hash = (uint32_t)length;
    for (size_t i = 0; i < length; ++i)
        hash = (hash * 13 + (uint32_t)toupper((unsigned char)name[i])) & 0x7ff;
    return hash % HASH_SIZE;
}

static int same_name(const uint8_t *header, const char *name, size_t length) {
    if (header[432] != length) return 0;
    for (size_t i = 0; i < length; ++i)
        if (toupper(header[433 + i]) != toupper((unsigned char)name[i])) return 0;
    return 1;
}

/* Find the header block of one path component inside directory `dir`. */
static uint32_t lookup(const FA18Disk *disk, uint32_t dir, const char *name, size_t length) {
    const uint8_t *directory = block(disk, dir);
    if (!directory) return 0;
    uint32_t key = be32(directory + 24 + 4 * name_hash(name, length));
    while (key) {
        const uint8_t *header = block(disk, key);
        if (!header) return 0;
        if (same_name(header, name, length)) return key;
        key = be32(header + 496);
    }
    return 0;
}

uint8_t *fa18_disk_read(const FA18Disk *disk, const char *path, size_t *size) {
    uint32_t key = ROOT_BLOCK;
    const char *part = path;
    while (*part) {
        const char *slash = strchr(part, '/');
        size_t length = slash ? (size_t)(slash - part) : strlen(part);
        key = lookup(disk, key, part, length);
        if (!key) return NULL;
        part += length + (slash ? 1 : 0);
    }
    const uint8_t *header = block(disk, key);
    if (!header || be32(header + 508) != (uint32_t)-3) return NULL;
    uint32_t bytes = be32(header + 324);
    uint8_t *data = malloc(bytes ? bytes : 1);
    if (!data) return NULL;
    uint32_t done = 0, next = be32(header + 16);
    while (done < bytes && next) {
        const uint8_t *chunk = block(disk, next);
        uint32_t length = chunk ? be32(chunk + 12) : 0;
        if (!chunk || length > BLOCK - 24 || length > bytes - done) {
            free(data);
            return NULL;
        }
        memcpy(data + done, chunk + 24, length);
        done += length;
        next = be32(chunk + 16);
    }
    if (done != bytes) {
        free(data);
        return NULL;
    }
    if (size) *size = bytes;
    return data;
}
