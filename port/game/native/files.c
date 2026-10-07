/* C11478/C114D2 -> C162E4 startup refresh and C1643A config writes.
 * Game decisions stay in existing domain owners. DOS/Exec file services reuse
 * port/amiga's host overlay; neither the source ADF nor a CPU is written/run. */
#include "files.h"
#include "../globals.h"
#include "../menu_followup.h"
#include "../postflight_file_callers.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct { NativeFrontend *game;uint32_t handle,lock; } FileCalls;
static void filename(gaddr address,char path[256]) {
    for(unsigned i=0;i<256;++i) {
        path[i]=(char)rd_u8(address+i);if(!path[i]) return;
    }
    fputs("native file name is unterminated\n",stderr);abort();
}
static uint32_t open_file(FileCalls *calls,gaddr address,int32_t mode) {
    char path[256];filename(address,path);
    return amiga_host_open(&calls->game->files,path,mode);
}
static int32_t file_child(void *context,enum PostflightFileChild child) {
    FileCalls *calls=context;AmigaHostCompat *host=&calls->game->files;
    const PostflightFileHooks hooks={file_child,NULL,calls};
    switch(child) {
    case PFF_ALLOCATE_CHECK: return (int32_t)amiga_host_alloc(host,40,0x10001);
    case PFF_LOCK_CHECK: {
        char path[256];filename(0xc07fe4,path);
        calls->lock=amiga_host_lock(host,path);return (int32_t)calls->lock;
    }
    case PFF_INFO_CHECK:
        return amiga_host_info(host,calls->lock,native_storage_range(rd_u32(MODE_FILE_INFO_POINTER),36),36)?-1:0;
    case PFF_UNLOCK_CHECK: return amiga_host_unlock(host,calls->lock)?-1:0;
    case PFF_FREE_CHECK: return amiga_host_free(host,rd_u32(MODE_FILE_INFO_POINTER),40)?-1:0;
    case PFF_RELEASE_TABLE: case PFF_OWN_TABLE: return 0; /* Host raster ownership. */
    case PFF_CHECK_TABLE: return (int32_t)check_postflight_volume(&hooks);
    case PFF_LOAD_TABLE: return (int32_t)load_postflight_mode_file(&hooks);
    case PFF_SAVE_TABLE: return (int32_t)create_postflight_mode_file(&hooks);
    case PFF_OPEN_SAVE: calls->handle=open_file(calls,0xc08012,1006);return (int32_t)calls->handle;
    case PFF_OPEN_LOAD: calls->handle=open_file(calls,0xc0801d,1005);return (int32_t)calls->handle;
    case PFF_WRITE_SAVE:
        return amiga_host_write(host,calls->handle,native_storage_range(rd_u32(MODE_TABLE),78),78);
    case PFF_READ_LOAD:
        return amiga_host_read(host,calls->handle,native_storage_range(rd_u32(MODE_TABLE),78),78);
    case PFF_CLOSE_SAVE: case PFF_CLOSE_LOAD:
        return amiga_host_file_close(host,calls->handle)?-1:0;
    }
    abort();
}
static uint32_t write_child(void *context,enum MenuFollowupChild child,uint32_t value,gaddr address) {
    FileCalls *calls=context;AmigaHostCompat *host=&calls->game->files;
    switch(child) {
    case MF_FILE_RELEASE: case MF_FILE_OWN:
    case MF_FILE_YIELD_BEFORE_OPEN: case MF_FILE_YIELD_AFTER_OPEN:
    case MF_FILE_YIELD_AFTER_WRITE: case MF_FILE_YIELD_AFTER_CLOSE:
        return 0; /* Source Delay(0), and host raster ownership. */
    case MF_FILE_CHECK: {
        const PostflightFileHooks hooks={file_child,NULL,calls};
        return check_postflight_volume(&hooks);
    }
    case MF_FILE_OPEN: return open_file(calls,address,(int32_t)value);
    case MF_FILE_WRITE: return (uint32_t)amiga_host_write(host,value,native_storage_range(address,78),78);
    case MF_FILE_CLOSE: return amiga_host_file_close(host,value)?0xffffffffu:0;
    default: fprintf(stderr,"native config child unavailable: %u\n",(unsigned)child);abort();
    }
}
int native_files_open(NativeFrontend *game,const char *save_directory) {
    AmigaGuestBank bank={NATIVE_FILE_INFO,NATIVE_FILE_INFO_BYTES,AMIGA_MEMORY_CHIP,
        native_storage_range(NATIVE_FILE_INFO,NATIVE_FILE_INFO_BYTES)};
    const AmigaGuestMemory memory={&bank,1};
    return amiga_host_init(&game->files,&memory,&game->disk,save_directory,NULL,0);
}
void native_frontend_refresh_log(NativeFrontend *game) {
    FileCalls calls={game,0,0};const PostflightFileHooks hooks={file_child,NULL,&calls};
    refresh_postflight_mode_file(&hooks);
}
void native_frontend_save_log(NativeFrontend *game) {
    FileCalls calls={game,0,0};const MenuFollowupHooks hooks={write_child,NULL,&calls};
    write_menu_mode_file(&hooks);
}
