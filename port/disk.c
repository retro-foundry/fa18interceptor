#include "amiga/ofs.h"
#include "amiga/hunk.h"
#include "disk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { BLOCK = 512, HASH_SIZE = 72 };

static uint32_t be32(const uint8_t *p) {
    return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3];
}

static const uint8_t *block(const AmigaOfs *disk, uint32_t number) {
    if (!disk || !disk->image || number >= disk->size / BLOCK) return NULL;
    return disk->image + (size_t)number * BLOCK;
}

static int checksum(const uint8_t *p) {
    uint32_t sum=0;
    if (!p) return 0;
    for (unsigned i=0;i<BLOCK;i+=4) sum+=be32(p+i);
    return sum==0;
}
static uint32_t root_block(const AmigaOfs *disk) { return (uint32_t)(disk->size/BLOCK/2); }
static unsigned upper(unsigned c) { return c>='a' && c<='z'?c-'a'+'A':c; }
static int entry(const AmigaOfs *disk,uint32_t key,AmigaOfsEntry *out) {
    const uint8_t *h=block(disk,key);
    if (!checksum(h) || be32(h)!=2 || h[432]>30) return 0;
    int32_t type=(int32_t)be32(h+508);
    if (type!=1 && type!=2 && type!=-3) return 0;
    if (type!=1 && be32(h+4)!=key) return 0;
    if (type!=1 && h[328]>79) return 0;
    if (out) {
        memset(out,0,sizeof *out); out->block=key; out->type=type;
        out->parent=type==1?0:be32(h+500); out->size=type==-3?be32(h+324):0;
        out->protection=type==1?0:be32(h+320);
        out->days=be32(h+420); out->minutes=be32(h+424); out->ticks=be32(h+428);
        memcpy(out->name,h+433,h[432]);
        if (type!=1) {
            memcpy(out->comment,h+329,h[328]);
        }
    }
    return 1;
}
int amiga_ofs_open(AmigaOfs *disk, const char *path) {
    if (!disk || !path) return 0;
    memset(disk, 0, sizeof *disk);
    FILE *file = fopen(path, "rb");
    if (!file) return 0;
    long size;
    if (fseek(file,0,SEEK_END) || (size=ftell(file))<2*BLOCK || size%BLOCK ||
        (uint64_t)size/BLOCK>UINT32_MAX || fseek(file,0,SEEK_SET)) { fclose(file); return 0; }
    disk->image = size > 0 ? malloc((size_t)size) : NULL;
    if (!disk->image || fread(disk->image, 1, (size_t)size, file) != (size_t)size) {
        fclose(file);
        amiga_ofs_close(disk);
        return 0;
    }
    int closed=fclose(file);
    disk->size = (size_t)size;
    AmigaOfsEntry root;
    if (closed || memcmp(disk->image,"DOS\0",4) || !entry(disk,root_block(disk),&root) || root.type!=1 ||
        be32(block(disk,root_block(disk))+12)!=HASH_SIZE) {
        amiga_ofs_close(disk); return 0;
    }
    return 1;
}

void amiga_ofs_close(AmigaOfs *disk) {
    if (!disk) return;
    free(disk->image);
    memset(disk, 0, sizeof *disk);
}

static uint32_t name_hash(const char *name, size_t length) {
    uint32_t hash = (uint32_t)length;
    for (size_t i = 0; i < length; ++i)
        hash = (hash * 13 + upper((unsigned char)name[i])) & 0x7ff;
    return hash % HASH_SIZE;
}

static int same_name(const uint8_t *header, const char *name, size_t length) {
    if (header[432] != length) return 0;
    for (size_t i = 0; i < length; ++i)
        if (upper(header[433 + i]) != upper((unsigned char)name[i])) return 0;
    return 1;
}

/* Find the header block of one path component inside directory `dir`. */
static uint32_t lookup(const AmigaOfs *disk, uint32_t dir, const char *name, size_t length) {
    AmigaOfsEntry parent;
    if (!length || length>30 || !entry(disk,dir,&parent) || parent.type<0) return 0;
    const uint8_t *directory = block(disk, dir);
    if (!directory) return 0;
    uint32_t key = be32(directory + 24 + 4 * name_hash(name, length));
    for (size_t remaining=disk->size/BLOCK;key && remaining;--remaining) {
        AmigaOfsEntry child;
        if (!entry(disk,key,&child) || child.parent!=dir) return 0;
        const uint8_t *header = block(disk, key);
        if (!header) return 0;
        if (same_name(header, name, length)) return key;
        key = be32(header + 496);
    }
    return 0;
}

int amiga_ofs_find(const AmigaOfs *disk,const char *path,AmigaOfsEntry *out) {
    if (!disk || !disk->image || !path) return 0;
    uint32_t key = root_block(disk);
    const char *part = path;
    while (*part=='/') ++part;
    while (*part) {
        const char *slash = strchr(part, '/');
        size_t length = slash ? (size_t)(slash - part) : strlen(part);
        key = lookup(disk, key, part, length);
        if (!key) return 0;
        part += length + (slash ? 1 : 0);
    }
    return entry(disk,key,out);
}
uint8_t *amiga_ofs_read(const AmigaOfs *disk, const char *path, size_t *size) {
    if (size) *size=0;
    AmigaOfsEntry file;
    if (!amiga_ofs_find(disk,path,&file) || file.type!=-3 || file.size>disk->size) return NULL;
    const uint8_t *header=block(disk,file.block);
    uint32_t bytes = file.size;
    uint8_t *data = malloc(bytes ? bytes : 1);
    if (!data) return NULL;
    uint32_t done = 0, next = be32(header + 16),sequence=1;
    for (size_t remaining=disk->size/BLOCK;done<bytes && next && remaining;--remaining) {
        const uint8_t *chunk = block(disk, next);
        uint32_t length = chunk ? be32(chunk + 12) : 0;
        if (!checksum(chunk) || be32(chunk)!=8 || be32(chunk+4)!=file.block ||
            be32(chunk+8)!=sequence++ || !length || length > BLOCK - 24 || length > bytes - done) {
            free(data);
            return NULL;
        }
        memcpy(data + done, chunk + 24, length);
        done += length;
        next = be32(chunk + 16);
    }
    if (done != bytes || next) {
        free(data);
        return NULL;
    }
    if (size) *size = bytes;
    return data;
}

int amiga_ofs_read_range(const AmigaOfs *disk,const char *path,size_t offset,
                         void *data,size_t capacity,size_t *read_size) {
    if (read_size) *read_size=0;
    if (capacity && !data) return 0;
    size_t size; uint8_t *file=amiga_ofs_read(disk,path,&size);
    if (!file) return 0;
    size_t bytes=offset<size?size-offset:0;
    if (bytes>capacity) bytes=capacity;
    if (bytes) memcpy(data,file+offset,bytes);
    if (read_size) *read_size=bytes;
    free(file); return 1;
}
int amiga_ofs_list(const AmigaOfs *disk,const char *directory,
                   int (*visit)(void *,const AmigaOfsEntry *),void *context) {
    AmigaOfsEntry dir;
    if (!visit || !amiga_ofs_find(disk,directory,&dir) || dir.type<0) return 0;
    const uint8_t *h=block(disk,dir.block);
    size_t remaining=disk->size/BLOCK;
    for (unsigned bucket=0;bucket<HASH_SIZE;++bucket) {
        uint32_t key=be32(h+24+4*bucket);
        while (key) {
            AmigaOfsEntry child;
            if (!remaining-- || !entry(disk,key,&child) || child.parent!=dir.block) return 0;
            if (!visit(context,&child)) return 1;
            key=be32(block(disk,key)+496);
        }
    }
    return 1;
}
/* Existing native-port API aliases the reusable read-only backend. */
int fa18_disk_open(FA18Disk *d,const char *p) { return amiga_ofs_open(d,p); }
void fa18_disk_close(FA18Disk *d) { amiga_ofs_close(d); }
uint8_t *fa18_disk_read(const FA18Disk *d,const char *p,size_t *n) { return amiga_ofs_read(d,p,n); }
