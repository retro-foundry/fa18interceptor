#ifndef FA18_HOST_COMPAT_ADAPTER_H
#define FA18_HOST_COMPAT_ADAPTER_H
#include "../amiga/host_compat.h"
#include "../amiga/exec_bootstrap.h"
/* Optional ROM-free profile, absent from the reference runner's registry. */
int fa18_os_host_compat_install(AmigaHostCompat *,const AmigaLibraryVector *,size_t count);
void fa18_os_host_compat_detach(void);
int fa18_os_host_exited(void);
int fa18_os_host_key(unsigned rawkey,int down);
void fa18_os_host_tick(uint64_t cycle);
#endif
