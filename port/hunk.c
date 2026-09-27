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
    if (r->pos + 4 > r->size) {
        r->bad = 1;
        return 0;
    }
    uint32_t value = fa18_be32(r->file + r->pos);
    r->pos += 4;
    return value;
}

static int by_offset(const void *a, const void *b) {
    uint32_t x = ((const FA18HunkReloc *)a)->offset, y = ((const FA18HunkReloc *)b)->offset;
    return x < y ? -1 : x > y;
}

int fa18_hunks_load(FA18Hunks *hunks, const uint8_t *file, size_t size) {
    memset(hunks, 0, sizeof *hunks);
    Reader r = {file, size, 0, 0};
    if (next(&r) != HUNK_HEADER) return 0;
    while (next(&r) && !r.bad) {}
    uint32_t count = next(&r), first = next(&r), last = next(&r);
    if (r.bad || last < first || last - first + 1 != count) return 0;
    hunks->segments = calloc(count, sizeof *hunks->segments);
    if (!hunks->segments) return 0;
    hunks->count = count;
    for (uint32_t i = 0; i < count; ++i) hunks->segments[i].size = (next(&r) & 0x3FFFFFFF) * 4;
    FA18HunkSegment *current = NULL;
    uint32_t loaded = 0;
    while (r.pos < size && !r.bad) {
        uint32_t kind = next(&r) & 0x3FFFFFFF;
        if (kind == HUNK_CODE || kind == HUNK_DATA || kind == HUNK_BSS) {
            if (loaded >= count) return fa18_hunks_free(hunks), 0;
            current = &hunks->segments[loaded++];
            current->kind = kind == HUNK_CODE ? FA18_HUNK_CODE
                          : kind == HUNK_DATA ? FA18_HUNK_DATA : FA18_HUNK_BSS;
            uint32_t bytes = (next(&r) & 0x3FFFFFFF) * 4;
            if (bytes > current->size) current->size = bytes;
            current->data = calloc(current->size ? current->size : 1, 1);
            if (!current->data) return fa18_hunks_free(hunks), 0;
            if (kind != HUNK_BSS) {
                if (r.pos + bytes > size) return fa18_hunks_free(hunks), 0;
                memcpy(current->data, file + r.pos, bytes);
                r.pos += bytes;
            }
        } else if (kind == HUNK_RELOC32) {
            if (!current) return fa18_hunks_free(hunks), 0;
            for (uint32_t n = next(&r); n && !r.bad; n = next(&r)) {
                uint32_t target = next(&r);
                FA18HunkReloc *grown = realloc(current->relocs,
                    (current->reloc_count + n) * sizeof *grown);
                if (!grown || target >= count) return fa18_hunks_free(hunks), 0;
                current->relocs = grown;
                for (uint32_t i = 0; i < n; ++i) {
                    grown[current->reloc_count].offset = next(&r);
                    grown[current->reloc_count++].target = (uint16_t)target;
                }
            }
        } else if (kind == HUNK_SYMBOL) {
            for (uint32_t n = next(&r); n && !r.bad; n = next(&r)) r.pos += (size_t)n * 4 + 4;
        } else if (kind == HUNK_DEBUG) {
            r.pos += (size_t)next(&r) * 4;
        } else if (kind != HUNK_END) {
            return fa18_hunks_free(hunks), 0;
        }
    }
    if (r.bad || loaded != count) return fa18_hunks_free(hunks), 0;
    for (uint32_t i = 0; i < count; ++i)
        qsort(hunks->segments[i].relocs, hunks->segments[i].reloc_count,
              sizeof(FA18HunkReloc), by_offset);
    return 1;
}

void fa18_hunks_free(FA18Hunks *hunks) {
    for (uint32_t i = 0; i < hunks->count; ++i) {
        free(hunks->segments[i].data);
        free(hunks->segments[i].relocs);
    }
    free(hunks->segments);
    memset(hunks, 0, sizeof *hunks);
}

int fa18_hunk_pointer(const FA18Hunks *hunks, uint32_t seg, uint32_t offset,
                      uint32_t *target_seg, uint32_t *target_offset) {
    if (seg >= hunks->count) return 0;
    const FA18HunkSegment *s = &hunks->segments[seg];
    FA18HunkReloc key = {offset, 0};
    const FA18HunkReloc *hit = bsearch(&key, s->relocs, s->reloc_count, sizeof key, by_offset);
    if (!hit || offset + 4 > s->size) return 0;
    *target_seg = hit->target;
    *target_offset = fa18_be32(s->data + offset);
    return 1;
}
