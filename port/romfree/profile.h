#ifndef FA18_ROMFREE_PROFILE_H
#define FA18_ROMFREE_PROFILE_H
#include "../machine/startup.h"
#include "../amiga/ofs.h"
#include "../amiga/hunk.h"
#include "../amiga/host_compat.h"
typedef struct {
    AmigaOfs adf;
    AmigaHunks image;
    uint32_t segment_list;
    const char *save_directory;
    AmigaHostCompat *compat;
} FA18RomFreeProfile;
/* Original disk image -> checked Hunk placement -> explicit process handoff.
 * Does not read ROMs, savestates, extracted assets or captured RAM. Unknown
 * services remain fatal, including original OS wrappers residing in RAM. */
int fa18_romfree_load(FA18RomFreeProfile *,FA18Machine *,const char *adf_path,
                      const char *save_directory,int use_recomp,char *,size_t);
int fa18_romfree_close(FA18RomFreeProfile *);
#endif
