#ifndef FA18_RECOMP_WRITE_INDEX_H
#define FA18_RECOMP_WRITE_INDEX_H
/* Proof bookkeeping only. Preserve exact raw-address keys and source order;
 * first/last indices are one-based so an empty slot can represent address 0. */
typedef struct { size_t first,last; } WriteIndexSlot;
typedef struct { const LogEntry *log; WriteIndexSlot *slots; size_t mask; } WriteIndex;
static WriteIndexSlot *write_index_slot(const WriteIndex *index,uint32_t address) {
    size_t slot=((size_t)address*2654435761u)&index->mask;
    while(index->slots[slot].first && index->log[index->slots[slot].first-1].address!=address)
        slot=(slot+1)&index->mask;
    return &index->slots[slot];
}
static WriteIndex write_index_build(const LogEntry *log,size_t count) {
    WriteIndex index; size_t capacity=16,i;
    if(count>SIZE_MAX/2) abort();
    while(capacity<count*2) { if(capacity>SIZE_MAX/2) abort(); capacity*=2; }
    index.log=log; index.mask=capacity-1;
    index.slots=calloc(capacity,sizeof *index.slots);
    if(!index.slots) { fputs("proof: cannot allocate write index\n",stderr); abort(); }
    for(i=0;i<count;++i) {
        WriteIndexSlot *slot=write_index_slot(&index,log[i].address);
        if(!slot->first) slot->first=i+1;
        slot->last=i+1;
    }
    return index;
}
static size_t write_index_first(const WriteIndex *index,uint32_t address) { return write_index_slot(index,address)->first; }
static size_t write_index_last(const WriteIndex *index,uint32_t address) { return write_index_slot(index,address)->last; }
static void write_index_free(WriteIndex *index) { free(index->slots); }
#endif
