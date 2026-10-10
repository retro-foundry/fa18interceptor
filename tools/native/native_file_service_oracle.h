/* Execute complete original file owners; intercept only actual OS services.
 * The writable overlay and volume metadata use the same port/amiga backend.
 * No game routine (including C0EF08, C162E4 or C1643A) is bypassed. */
#include "../../../fa18-interceptor-decomp/src/menu/files.h"
#include "../../port/amiga/hunk.h"
#include <sys/stat.h>
#ifdef _WIN32
#include <process.h>
#include <direct.h>
#define oracle_process_id _getpid
#define oracle_remove_directory _rmdir
#else
#include <unistd.h>
#define oracle_process_id getpid
#define oracle_remove_directory rmdir
#endif
static NativeFrontend *file_oracle_game;
static char file_oracle_directory[256],file_oracle_path[272];
static unsigned file_oracle_writes;
#ifdef FA18_CONFIG_NATIVE_RUNTIME
uint8_t *native_storage_range(uint32_t address,size_t bytes) {
    if(address<0x80000 && bytes<=0x80000-address) return fa18_machine->chip+address;
    if(address>=0xc00000 && address<0xc80000 && bytes<=0xc80000-address)
        return fa18_machine->slow+address-0xc00000;
    abort();
}
#include "../../../fa18-interceptor-decomp/src/menu/files.c"
#endif
static int file_oracle_reset(NativeFrontend *game) {
    file_oracle_game=game;file_oracle_writes=0;
    snprintf(file_oracle_directory,sizeof file_oracle_directory,"build/native-file-services-%u",(unsigned)oracle_process_id());
    snprintf(file_oracle_path,sizeof file_oracle_path,"%s/config",file_oracle_directory);
#ifdef _WIN32
    _mkdir(file_oracle_directory);
#else
    mkdir(file_oracle_directory,0755);
#endif
    if(!game->disk.image && !amiga_ofs_open(&game->disk,"local/media/fa18.adf")) return 0;
    if(!amiga_host_close(&game->files)) return 0;
    AmigaGuestBank bank={NATIVE_FILE_INFO,NATIVE_FILE_INFO_BYTES,AMIGA_MEMORY_CHIP,
        fa18_machine->chip+NATIVE_FILE_INFO};
    const AmigaGuestMemory memory={&bank,1};
    if(!amiga_host_init(&game->files,&memory,&game->disk,file_oracle_directory,NULL,0)) return 0;
    FILE *file=fopen(file_oracle_path,"wb");if(!file) return 0;
    const gaddr log=rd_u32(MODE_TABLE);
    for(unsigned i=0;i<78;++i) if(fputc(rd_u8(log+i),file)==EOF) {fclose(file);return 0;}
    return fclose(file)==0;
}
static int file_oracle_read_log(uint8_t log[78]) {
    FILE *file=fopen(file_oracle_path,"rb");if(!file) return 0;
    int result=fread(log,1,78,file)==78 && fgetc(file)==EOF;
    return fclose(file)==0 && result;
}
static void file_oracle_close(NativeFrontend *game) {
    amiga_host_close(&game->files);amiga_ofs_close(&game->disk);
    remove(file_oracle_path);oracle_remove_directory(file_oracle_directory);
}
static int file_oracle_service(void) {
    AmigaHostCompat *host=&file_oracle_game->files;
    const gaddr stack=REG_A[7];uint32_t result=0;
    char path[256];
    switch(REG_PC) {
    case 0xc53b30:
        if(rd_u32(stack+4)!=40 || rd_u32(stack+8)!=0x10001) abort();
        result=amiga_host_alloc(host,40,0x10001);break;
    case 0xc53b48:
        if(rd_u32(stack+8)!=40) abort();
        result=amiga_host_free(host,rd_u32(stack+4),40)?0xffffffffu:0;break;
    case 0xc53a7c: case 0xc539a8: {
        gaddr address=rd_u32(stack+4);unsigned i;
        for(i=0;i<sizeof path;++i) {path[i]=(char)rd_u8(address+i);if(!path[i]) break;}
        if(i==sizeof path) abort();
        if(REG_PC==0xc53a7c) {
            if(rd_u32(stack+8)!=0xfffffffeu) abort();
            result=amiga_host_lock(host,path);
        } else result=amiga_host_open(host,path,(int32_t)rd_u32(stack+8));
        break;
    }
    case 0xc53ac8: {
        const gaddr address=rd_u32(stack+8);
        if(address!=NATIVE_FILE_INFO) abort();
        result=amiga_host_info(host,rd_u32(stack+4),fa18_machine->chip+address,36)?0xffffffffu:0;break;
    }
    case 0xc53a98: result=amiga_host_unlock(host,rd_u32(stack+4))?0xffffffffu:0;break;
    case 0xc539d8: case 0xc539f4: {
        const gaddr address=rd_u32(stack+8);
        uint8_t *bytes=address<0x80000?fa18_machine->chip+address:fa18_machine->slow+address-0xc00000;
        if(rd_u32(stack+12)!=78 || address!=rd_u32(MODE_TABLE)) abort();
        if(REG_PC==0xc539d8) result=(uint32_t)amiga_host_read(host,rd_u32(stack+4),bytes,78);
        else {++file_oracle_writes;result=(uint32_t)amiga_host_write(host,rd_u32(stack+4),bytes,78);}
        break;
    }
    case 0xc539c4: result=amiga_host_file_close(host,rd_u32(stack+4))?0xffffffffu:0;break;
    case 0xc53f9c: if(rd_u32(stack+4)) return 0;break; /* Delay(0) only. */
    case 0xc53fb0: case 0xc53fc0: break; /* Host raster ownership. */
    default: return 0;
    }
    REG_D[0]=result;REG_PC=rd_u32(stack);REG_A[7]+=4;return 1;
}
