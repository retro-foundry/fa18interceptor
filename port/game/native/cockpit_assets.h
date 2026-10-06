#ifndef FA18_NATIVE_COCKPIT_ASSETS_H
#define FA18_NATIVE_COCKPIT_ASSETS_H
#include "../../amiga/ofs.h"
#include "../memory.h"
int native_cockpit_load(const AmigaOfs *disk,char *error,size_t capacity);
/* C16982 caches the loaded planes and creates C16AD4's four-plane union. */
void native_cockpit_prepare(gaddr workspace,gaddr mask);
#endif
