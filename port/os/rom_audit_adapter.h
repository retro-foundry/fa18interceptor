#ifndef FA18_ROM_AUDIT_ADAPTER_H
#define FA18_ROM_AUDIT_ADAPTER_H
#include "../amiga/rom_audit.h"
int fa18_rom_audit_open(void);
int fa18_rom_audit_finish(const char *path);
void fa18_rom_audit_instruction(uint32_t pc);
void fa18_rom_audit_access(AmigaAuditKind, uint32_t address, unsigned size, uint32_t value);
#endif
