#include "rom_audit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
    int used;
    AmigaAuditKind kind;
    uint32_t pc, address, value;
    unsigned size;
    uint64_t count, first_cycle, last_cycle, first_frame, last_frame;
    AmigaAuditCpu first_cpu;
} Row;
struct AmigaRomAudit {
    AmigaAuditRange *ranges;
    size_t range_count, capacity, used;
    Row *rows;
    uint32_t previous_pc;
    int previous_valid, failed;
};
static int in_rom(const AmigaRomAudit *audit, uint32_t address, unsigned size) {
    uint64_t end=(uint64_t)address+size;
    for (size_t i=0; i<audit->range_count; ++i)
        if (address<audit->ranges[i].high && end>audit->ranges[i].low) return 1;
    return 0;
}
AmigaRomAudit *amiga_rom_audit_create(const AmigaAuditRange *ranges, size_t n, size_t capacity) {
    if (!ranges || !n || !capacity || (capacity&(capacity-1))) return NULL;
    for (size_t i=0; i<n; ++i)
        if (ranges[i].low>=ranges[i].high || ranges[i].high>0x1000000u) return NULL;
    if (n>SIZE_MAX/sizeof *ranges || capacity>SIZE_MAX/sizeof(Row)) return NULL;
    AmigaRomAudit *audit=calloc(1,sizeof *audit);
    if (!audit) return NULL;
    audit->ranges=malloc(n*sizeof *ranges); audit->rows=calloc(capacity,sizeof *audit->rows);
    if (!audit->ranges || !audit->rows) { amiga_rom_audit_destroy(audit); return NULL; }
    memcpy(audit->ranges,ranges,n*sizeof *ranges);
    audit->range_count=n; audit->capacity=capacity;
    return audit;
}
void amiga_rom_audit_destroy(AmigaRomAudit *audit) {
    if (audit) { free(audit->rows); free(audit->ranges); free(audit); }
}
static Row *observe(AmigaRomAudit *audit, AmigaAuditKind kind, uint32_t pc,
    uint32_t address, unsigned size, uint32_t value, uint64_t cycle, uint64_t frame) {
    uint64_t key=((uint64_t)pc<<32)|address;
    key^=(uint64_t)(kind*17u+size)*0x9E3779B97F4A7C15ull;
    key^=key>>33; key*=0xFF51AFD7ED558CCDull; key^=key>>33;
    size_t slot=(size_t)key&(audit->capacity-1);
    if (audit->failed) return NULL;
    for (size_t probes=0; probes<audit->capacity; ++probes) {
        Row *row=&audit->rows[slot];
        if (!row->used) {
            row->used=1; row->kind=kind; row->pc=pc; row->address=address;
            row->size=size; row->value=value; row->first_cycle=cycle; row->first_frame=frame;
            audit->used++;
        }
        if (row->kind==kind && row->pc==pc && row->address==address && row->size==size) {
            if (row->count==UINT64_MAX) { audit->failed=1; return NULL; }
            row->count++; row->last_cycle=cycle; row->last_frame=frame;
            return row;
        }
        slot=(slot+1)&(audit->capacity-1);
    }
    audit->failed=1; return NULL;
}
int amiga_rom_audit_instruction(AmigaRomAudit *audit, const AmigaAuditCpu *cpu) {
    if (!audit || !cpu || audit->failed) return 0;
    if (audit->previous_valid &&
        (in_rom(audit,cpu->pc,1) || in_rom(audit,audit->previous_pc,1))) {
        Row *flow=observe(audit,AMIGA_AUDIT_FLOW,audit->previous_pc,cpu->pc,0,0,cpu->cycle,cpu->frame);
        if (!flow) return 0;
        if (flow->count==1) flow->first_cpu=*cpu;
    }
    audit->previous_pc=cpu->pc; audit->previous_valid=1;
    if (in_rom(audit,cpu->pc,1)) {
        Row *row=observe(audit,AMIGA_AUDIT_INSTRUCTION,cpu->pc,cpu->pc,0,0,cpu->cycle,cpu->frame);
        if (!row) return 0;
        if (row->count==1) row->first_cpu=*cpu;
    }
    return 1;
}
int amiga_rom_audit_access(AmigaRomAudit *audit, AmigaAuditKind kind, uint32_t pc,
    uint32_t address, unsigned size, uint32_t value, uint64_t cycle, uint64_t frame) {
    if (!audit || audit->failed || kind<AMIGA_AUDIT_PROGRAM_READ || kind>AMIGA_AUDIT_VECTOR_WRITE ||
        (size!=1 && size!=2 && size!=4)) return 0;
    if (!in_rom(audit,address,size) && !in_rom(audit,pc,1) &&
        kind!=AMIGA_AUDIT_VECTOR_READ && kind!=AMIGA_AUDIT_VECTOR_WRITE) return 1;
    return observe(audit,kind,pc,address,size,value,cycle,frame)!=NULL;
}
static const char *name(AmigaAuditKind kind) {
    static const char *names[]={"instruction","program_read","data_read","data_write","vector_read","vector_write","flow"};
    return names[kind];
}
int amiga_rom_audit_write(const AmigaRomAudit *audit, const char *path) {
    if (!audit || audit->failed || !path) return 0;
    FILE *out=fopen(path,"w");
    if (!out) return 0;
    fputs("{\"schema\":\"amiga.rom_audit.v1\",\"complete\":true,\"ranges\":[",out);
    for (size_t i=0; i<audit->range_count; ++i)
        fprintf(out,"%s{\"low\":%u,\"high\":%u}",i?",":"",audit->ranges[i].low,audit->ranges[i].high);
    fputs("],\"observations\":[\n",out);
    int first=1;
    for (size_t i=0; i<audit->capacity; ++i) {
        const Row *r=&audit->rows[i];
        if (!r->used) continue;
        fprintf(out,"%s{\"kind\":\"%s\",\"pc\":\"%06X\",\"address\":\"%06X\",\"size\":%u,\"first_value\":%u,"
            "\"count\":%llu,\"first_cycle\":%llu,\"last_cycle\":%llu,\"first_frame\":%llu,\"last_frame\":%llu",
            first?"":",\n",name(r->kind),r->pc,r->address,r->size,r->value,
            (unsigned long long)r->count,(unsigned long long)r->first_cycle,(unsigned long long)r->last_cycle,
            (unsigned long long)r->first_frame,(unsigned long long)r->last_frame);
        if (r->kind==AMIGA_AUDIT_INSTRUCTION || r->kind==AMIGA_AUDIT_FLOW) {
            fputs(",\"first_cpu\":{\"d\":[",out);
            for (unsigned j=0; j<8; ++j) fprintf(out,"%s%u",j?",":"",r->first_cpu.d[j]);
            fputs("],\"a\":[",out);
            for (unsigned j=0; j<8; ++j) fprintf(out,"%s%u",j?",":"",r->first_cpu.a[j]);
            fprintf(out,"],\"pc\":%u,\"sr\":%u}",r->first_cpu.pc,r->first_cpu.sr);
        }
        fputs("}",out); first=0;
    }
    fputs("\n]}\n",out);
    int ok=!ferror(out);
    if (fclose(out)) ok=0;
    if (!ok) remove(path);
    return ok;
}
