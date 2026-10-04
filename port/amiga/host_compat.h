#ifndef AMIGA_HOST_COMPAT_H
#define AMIGA_HOST_COMPAT_H
#include "guest_memory.h"
#include "ofs.h"
#include <stdio.h>
/* Behavior-level compatibility. Host bookkeeping replaces OS task/packet work;
 * all pointers visible to the guest still refer to its packed address space.
 * No CPU, SDK, machine, ROM, savestate or game dependency. */
enum { AMIGA_HOST_EXEC,AMIGA_HOST_DOS,AMIGA_HOST_GRAPHICS,AMIGA_HOST_INTUITION,
       AMIGA_HOST_POTGO,AMIGA_HOST_CIAA,AMIGA_HOST_CIAB,AMIGA_HOST_TIMER,
       AMIGA_HOST_INPUT,AMIGA_HOST_GAMEPORT,AMIGA_HOST_KEYBOARD,AMIGA_HOST_AUDIO,
       AMIGA_HOST_LIBRARY_COUNT,AMIGA_HOST_SERVICE_BASE=0xEF0000 };
typedef struct { uint32_t base,size,attributes; } AmigaHostRegion;
typedef struct { uint8_t *data; size_t size,position; FILE *file; int active; char overlay_path[1024]; } AmigaHostFile;
typedef struct {
    char path[256]; int active,enumerated;
    AmigaOfsEntry *entries; size_t entry_count,entry_next;
} AmigaHostLock;
typedef struct {
    AmigaGuestBank banks[8]; AmigaGuestMemory memory; const AmigaOfs *disk;
    AmigaHostRegion free[2048],used[1024]; size_t free_count,used_count;
    uint32_t libraries[AMIGA_HOST_LIBRARY_COUNT],current_directory;
    AmigaHostFile files[64]; AmigaHostLock locks[64];
    char save_directory[512]; int32_t error;
    int exited; int32_t exit_code;
    uint32_t input_handlers[16]; size_t input_handler_count;
    uint8_t keyboard_matrix[16],gameport_type[2],gameport_trigger[2][8];
    uint8_t sprite_allocated;
    struct { uint32_t request; uint64_t deadline; unsigned device; } pending[64];
    size_t pending_count;
    uint32_t synchronous_request;
    unsigned wait_kind; uint32_t wait_value;
    uint8_t keys[128]; unsigned key_head,key_tail;
    uint16_t qualifiers;
    int desktop_hidden;
} AmigaHostCompat;
int amiga_host_init(AmigaHostCompat *,const AmigaGuestMemory *,const AmigaOfs *,
                    const char *save_directory,const AmigaHostRegion *reserved,size_t count);
/* Flush/close files, free directory scans, and discard pending guest work.
 * Returns zero if any file could not be closed; cleanup still completes. */
int amiga_host_close(AmigaHostCompat *);
uint32_t amiga_host_alloc(AmigaHostCompat *,uint32_t size,uint32_t flags);
int amiga_host_free(AmigaHostCompat *,uint32_t address,uint32_t size);
uint32_t amiga_host_available(const AmigaHostCompat *,uint32_t flags);
uint32_t amiga_host_library(AmigaHostCompat *,const char *name,uint32_t version);
const char *amiga_host_library_name(unsigned id);
int amiga_host_string(const AmigaHostCompat *,uint32_t address,char *out,size_t capacity);
uint32_t amiga_host_open(AmigaHostCompat *,const char *path,int32_t mode);
int amiga_host_file_close(AmigaHostCompat *,uint32_t handle);
int32_t amiga_host_read(AmigaHostCompat *,uint32_t handle,void *,int32_t length);
int32_t amiga_host_write(AmigaHostCompat *,uint32_t handle,const void *,int32_t length);
int32_t amiga_host_seek(AmigaHostCompat *,uint32_t handle,int32_t position,int32_t mode);
uint32_t amiga_host_lock(AmigaHostCompat *,const char *path);
int amiga_host_unlock(AmigaHostCompat *,uint32_t lock);
int amiga_host_examine(AmigaHostCompat *,uint32_t lock,uint8_t *fib,size_t size);
int amiga_host_exnext(AmigaHostCompat *,uint32_t lock,uint8_t *fib,size_t size);
int amiga_host_info(AmigaHostCompat *,uint32_t lock,uint8_t *info,size_t size);
int amiga_host_add_tail(AmigaHostCompat *,uint32_t list,uint32_t node);
int amiga_host_remove(AmigaHostCompat *,uint32_t node);
int amiga_host_queue_key(AmigaHostCompat *,unsigned rawkey,int down);
#endif
