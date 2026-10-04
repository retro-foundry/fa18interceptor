#include "hunk_loader.h"
#include <stdio.h>
#include <string.h>
static int fail(char *error, size_t size, unsigned index, const char *why) {
    if (error && size) snprintf(error,size,"Hunk %u: %s",index,why);
    return 0;
}
int amiga_hunks_install(const AmigaHunks *image, const AmigaHunkPlacement *layout,
    size_t count, const AmigaGuestMemory *memory, uint32_t *segment_list,
    char *error, size_t error_size) {
    if (segment_list) *segment_list=0;
    if (!image || !image->segments || !layout || !image->count ||
        count!=image->count || !amiga_guest_memory_valid(memory))
        return fail(error,error_size,0,"invalid image, placement count or guest banks");
    /* Preflight every range and relocation before modifying guest memory. */
    for (uint32_t i=0; i<image->count; ++i) {
        const AmigaHunkPlacement *p=&layout[i];
        const AmigaHunkSegment *s=&image->segments[i];
        uint64_t end=(uint64_t)p->payload_base-8+p->allocation_size;
        if (p->payload_base<8 || (p->payload_base&3) ||
            (uint64_t)s->size+8>p->allocation_size || !s->data ||
            !amiga_guest_range(memory,p->payload_base-8,p->allocation_size))
            return fail(error,error_size,i,"allocation outside RAM, undersized or unaligned");
        if (s->memory_flags) {
            int matching=0;
            for (size_t b=0; b<memory->count; ++b)
                if (p->payload_base>=memory->banks[b].base &&
                    end<=(uint64_t)memory->banks[b].base+memory->banks[b].size &&
                    (memory->banks[b].attributes&s->memory_flags)==s->memory_flags) matching=1;
            if (!matching) return fail(error,error_size,i,"HUNK chip/fast requirement is not met");
        }
        for (uint32_t j=0; j<i; ++j)
            if (p->payload_base-8<(uint64_t)layout[j].payload_base-8+layout[j].allocation_size &&
                layout[j].payload_base-8<end)
                return fail(error,error_size,i,"segment allocations overlap");
        if (s->reloc_count && !s->relocs) return fail(error,error_size,i,"missing relocation table");
        for (uint32_t r=0; r<s->reloc_count; ++r) {
            const AmigaHunkReloc *rel=&s->relocs[r];
            if (rel->target>=count || (rel->offset&1) || s->size<4 || rel->offset>s->size-4)
                return fail(error,error_size,i,"relocation outside source or target image");
            if (r && rel->offset<s->relocs[r-1].offset+4)
                return fail(error,error_size,i,"relocation fields overlap or are unsorted");
        }
    }
    for (uint32_t i=0; i<image->count; ++i) {
        const AmigaHunkPlacement *p=&layout[i];
        const AmigaHunkSegment *s=&image->segments[i];
        uint8_t *allocation=amiga_guest_range(memory,p->payload_base-8,p->allocation_size);
        memset(allocation,0,p->allocation_size);
        amiga_store_be32(allocation,p->allocation_size);
        amiga_store_be32(allocation+4,i+1<count?(layout[i+1].payload_base-4)>>2:0);
        if (s->kind!=AMIGA_HUNK_BSS) memcpy(allocation+8,s->data,s->size);
        for (uint32_t r=0; r<s->reloc_count; ++r) {
            uint8_t *field=allocation+8+s->relocs[r].offset;
            amiga_store_be32(field,amiga_be32(field)+layout[s->relocs[r].target].payload_base);
        }
    }
    if (segment_list) *segment_list=(layout[0].payload_base-4)>>2;
    if (error && error_size) error[0]=0;
    return 1;
}
