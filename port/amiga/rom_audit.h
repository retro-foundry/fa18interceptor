#ifndef AMIGA_ROM_AUDIT_H
#define AMIGA_ROM_AUDIT_H
#include <stddef.h>
#include <stdint.h>
/* Reference-only observations, independent of any game, SDK or CPU core. */
typedef enum {
    AMIGA_AUDIT_INSTRUCTION, AMIGA_AUDIT_PROGRAM_READ, AMIGA_AUDIT_DATA_READ,
    AMIGA_AUDIT_DATA_WRITE, AMIGA_AUDIT_VECTOR_READ, AMIGA_AUDIT_VECTOR_WRITE,
    AMIGA_AUDIT_FLOW
} AmigaAuditKind;
typedef struct {
    uint32_t d[8], a[8], pc;
    uint16_t sr;
    uint64_t cycle, frame;
} AmigaAuditCpu;
typedef struct {
    uint32_t low, high; /* Exclusive high; ROM aliases and expansion ROMs are separate ranges. */
} AmigaAuditRange;
typedef struct AmigaRomAudit AmigaRomAudit;
AmigaRomAudit *amiga_rom_audit_create(const AmigaAuditRange *, size_t, size_t capacity);
void amiga_rom_audit_destroy(AmigaRomAudit *);
/* Zero means the inventory cannot represent an observation; callers must
 * fail rather than accepting incomplete coverage. No guest reads or writes. */
int amiga_rom_audit_instruction(AmigaRomAudit *, const AmigaAuditCpu *);
int amiga_rom_audit_access(AmigaRomAudit *, AmigaAuditKind, uint32_t pc,
    uint32_t address, unsigned size, uint32_t value, uint64_t cycle, uint64_t frame);
int amiga_rom_audit_write(const AmigaRomAudit *, const char *path);
#endif
