#include "../amiga/hunk.h"
#include "hunk.h"

#include <stdlib.h>
#include <string.h>

enum {
    HUNK_HEADER = 0x3F3, HUNK_CODE = 0x3E9, HUNK_DATA = 0x3EA, HUNK_BSS = 0x3EB,
    HUNK_RELOC32 = 0x3EC, HUNK_SYMBOL = 0x3F0, HUNK_DEBUG = 0x3F1, HUNK_END = 0x3F2
};

typedef struct {
    const uint8_t *file;
    size_t size, pos;
    int bad;
} Reader;

static uint32_t next(Reader *r) {
    if (r->pos > r->size || r->size - r->pos < 4) {
        r->bad = 1;
        return 0;
    }
    uint32_t value = amiga_be32(r->file + r->pos);
    r->pos += 4;
    return value;
}

static int by_offset(const void *a, const void *b) {
    uint32_t x = ((const AmigaHunkReloc *)a)->offset, y = ((const AmigaHunkReloc *)b)->offset;
    return x < y ? -1 : x > y;
}

int amiga_hunks_parse(AmigaHunks *hunks, const uint8_t *file, size_t size) {
    if (!hunks) return 0;
    memset(hunks, 0, sizeof *hunks);
    if (!file) return 0;
    Reader r = {file, size, 0, 0};
    if (next(&r) != HUNK_HEADER) return 0;
    /* Each resident-library name is a length followed by that many longs. */
    for (uint32_t n=next(&r); n && !r.bad; n=next(&r)) {
        if (n>(size-r.pos)/4) return 0;
        r.pos+=(size_t)n*4;
    }
    uint32_t count = next(&r), first = next(&r), last = next(&r);
    if (r.bad || !count || count > 65536 || first != 0 || last < first ||
        last - first + 1 != count || count > (size - r.pos) / 4) return 0;
    hunks->segments = calloc(count, sizeof *hunks->segments);
    if (!hunks->segments) return 0;
    hunks->count = count;
    for (uint32_t i = 0; i < count; ++i) {
        uint32_t allocation = next(&r);
        /* Extended allocation attributes require another header word. They
         * are not used by this executable; reject instead of misparsing. */
        if ((allocation >> 30) == 3 || (allocation & 0x3FFFFFFF) > UINT32_MAX / 4)
            return amiga_hunks_free(hunks), 0;
        hunks->segments[i].size = (allocation & 0x3FFFFFFF) * 4;
        hunks->segments[i].memory_flags = allocation >> 30;
    }
    AmigaHunkSegment *current = NULL;
    uint32_t loaded = 0;
    while (r.pos < size && !r.bad) {
        uint32_t kind = next(&r) & 0x3FFFFFFF;
        if (kind == HUNK_CODE || kind == HUNK_DATA || kind == HUNK_BSS) {
            if (current || loaded >= count) return amiga_hunks_free(hunks), 0;
            current = &hunks->segments[loaded++];
            current->kind = kind == HUNK_CODE ? AMIGA_HUNK_CODE
                          : kind == HUNK_DATA ? AMIGA_HUNK_DATA : AMIGA_HUNK_BSS;
            uint32_t longs = next(&r) & 0x3FFFFFFF;
            if (longs > UINT32_MAX / 4) return amiga_hunks_free(hunks), 0;
            uint32_t bytes = longs * 4;
            if (bytes > current->size) return amiga_hunks_free(hunks), 0;
            current->data = calloc(current->size ? current->size : 1, 1);
            if (!current->data) return amiga_hunks_free(hunks), 0;
            if (kind != HUNK_BSS) {
                if (bytes > size-r.pos) return amiga_hunks_free(hunks), 0;
                memcpy(current->data, file + r.pos, bytes);
                r.pos += bytes;
            }
        } else if (kind == HUNK_RELOC32) {
            if (!current) return amiga_hunks_free(hunks), 0;
            for (uint32_t n = next(&r); n && !r.bad; n = next(&r)) {
                uint32_t target = next(&r);
                if (r.bad || target >= count || n > (size - r.pos) / 4 ||
                    n > UINT32_MAX - current->reloc_count ||
                    (uint64_t)current->reloc_count+n > SIZE_MAX/sizeof(AmigaHunkReloc))
                    return amiga_hunks_free(hunks), 0;
                AmigaHunkReloc *grown = realloc(current->relocs,
                    (current->reloc_count + n) * sizeof *grown);
                if (!grown) return amiga_hunks_free(hunks), 0;
                current->relocs = grown;
                for (uint32_t i = 0; i < n; ++i) {
                    uint32_t offset = next(&r);
                    if ((offset & 1) || current->size < 4 || offset > current->size - 4)
                        return amiga_hunks_free(hunks), 0;
                    grown[current->reloc_count].offset = offset;
                    grown[current->reloc_count++].target = (uint16_t)target;
                }
            }
        } else if (kind == HUNK_SYMBOL) {
            for (uint32_t n = next(&r); n && !r.bad; n = next(&r)) {
                if (size - r.pos < 4 || n > (size - r.pos - 4) / 4)
                    return amiga_hunks_free(hunks), 0;
                r.pos += (size_t)n * 4 + 4;
            }
        } else if (kind == HUNK_DEBUG) {
            uint32_t n = next(&r);
            if (r.bad || n > (size - r.pos) / 4) return amiga_hunks_free(hunks), 0;
            r.pos += (size_t)n * 4;
        } else if (kind == HUNK_END) {
            if (!current) return amiga_hunks_free(hunks), 0;
            current = NULL;
        } else {
            return amiga_hunks_free(hunks), 0;
        }
    }
    if (r.bad || current || loaded != count) return amiga_hunks_free(hunks), 0;
    for (uint32_t i = 0; i < count; ++i) {
        qsort(hunks->segments[i].relocs, hunks->segments[i].reloc_count,
              sizeof(AmigaHunkReloc), by_offset);
        for (uint32_t j = 1; j < hunks->segments[i].reloc_count; ++j)
            if (hunks->segments[i].relocs[j].offset < hunks->segments[i].relocs[j-1].offset + 4)
                return amiga_hunks_free(hunks), 0;
    }
    return 1;
}

void amiga_hunks_free(AmigaHunks *hunks) {
    if (!hunks) return;
    for (uint32_t i = 0; i < hunks->count; ++i) {
        free(hunks->segments[i].data);
        free(hunks->segments[i].relocs);
    }
    free(hunks->segments);
    memset(hunks, 0, sizeof *hunks);
}

int amiga_hunk_pointer(const AmigaHunks *hunks, uint32_t seg, uint32_t offset,
                      uint32_t *target_seg, uint32_t *target_offset) {
    if (seg >= hunks->count) return 0;
    const AmigaHunkSegment *s = &hunks->segments[seg];
    AmigaHunkReloc key = {offset, 0};
    const AmigaHunkReloc *hit = bsearch(&key, s->relocs, s->reloc_count, sizeof key, by_offset);
    if (!hit || s->size < 4 || offset > s->size - 4) return 0;
    *target_seg = hit->target;
    *target_offset = amiga_be32(s->data + offset);
    return 1;
}

/* Existing native-port API; parser semantics are shared with the guest loader. */
int fa18_hunks_load(FA18Hunks *h, const uint8_t *p, size_t n) { return amiga_hunks_parse(h,p,n); }
void fa18_hunks_free(FA18Hunks *h) { amiga_hunks_free(h); }
int fa18_hunk_pointer(const FA18Hunks *h, uint32_t s, uint32_t o, uint32_t *t, uint32_t *v) { return amiga_hunk_pointer(h,s,o,t,v); }
