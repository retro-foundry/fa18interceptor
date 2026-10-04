#ifndef FA18_ROMFREE_MEDIA_H
#define FA18_ROMFREE_MEDIA_H
#include "../amiga/ofs.h"
typedef struct {
    char disk_sha256[65],executable_sha256[65];
    const char *version;
    int ofs,supported;
} FA18MediaInfo;
void fa18_media_inspect(const AmigaOfs *,const void *executable,size_t size,FA18MediaInfo *);
int fa18_media_probe(const char *path,FA18MediaInfo *);
/* Search beside the actual executable, not the working directory. Distinct
 * supported images are ambiguous; identical copies use lexical path order. */
int fa18_media_discover(const char *argv0,char *path,size_t capacity,char *error,size_t error_size);
#endif
