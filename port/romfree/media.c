#include "media.h"
#include "../amiga/sha256.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <unistd.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif
#endif
typedef struct { const char *sha256,*name; } DiskVersion;
static const DiskVersion versions[]={
#include "adf_versions.inc"
};
void fa18_media_inspect(const AmigaOfs *disk,const void *executable,size_t size,FA18MediaInfo *info) {
    memset(info,0,sizeof *info); info->ofs=1; info->version="unlisted disk image";
    amiga_sha256_hex(disk->image,disk->size,info->disk_sha256);
    if (executable) amiga_sha256_hex(executable,size,info->executable_sha256);
    info->supported=executable && size==331232 &&
        !strcmp(info->executable_sha256,"d7301e20958548f9b5f1ba744f8ff91fef476306c220eb886e7c90bac2802ffa");
    for (size_t i=0;i<sizeof versions/sizeof versions[0];++i)
        if (!strcmp(info->disk_sha256,versions[i].sha256)) { info->version=versions[i].name; break; }
}
int fa18_media_probe(const char *path,FA18MediaInfo *info) {
    AmigaOfs disk={0};
    if (amiga_ofs_open(&disk,path)) {
        size_t size=0; uint8_t *exe=amiga_ofs_read(&disk,"F-18 Interceptor",&size);
        fa18_media_inspect(&disk,exe,size,info); free(exe); amiga_ofs_close(&disk); return 1;
    }
    /* Retain identity for recognized images that this OFS reader cannot open. */
    FILE *f=fopen(path,"rb"); if (!f) return 0;
    long size; uint8_t *data=NULL; int ok=0;
    if (!fseek(f,0,SEEK_END) && (size=ftell(f))>=0 && size<=16*1024*1024 &&
        !fseek(f,0,SEEK_SET) && (data=malloc((size_t)size+1)) && fread(data,1,(size_t)size,f)==(size_t)size) {
        AmigaOfs raw={data,(size_t)size}; fa18_media_inspect(&raw,NULL,0,info); info->ofs=0; ok=1;
    }
    free(data); if (fclose(f)) ok=0; return ok;
}
static int consider(const char *directory,const char *name,char *selected,size_t capacity,char hash[65]) {
    size_t n=strlen(name); if (n<4 || name[n-4]!='.' ||
        tolower((unsigned char)name[n-3])!='a' || tolower((unsigned char)name[n-2])!='d' ||
        tolower((unsigned char)name[n-1])!='f') return 1;
    char candidate[4096]; int written=snprintf(candidate,sizeof candidate,"%s/%s",directory,name);
    if (written<0 || (size_t)written>=sizeof candidate) return 0;
    FA18MediaInfo info; if (!fa18_media_probe(candidate,&info) || !info.supported) return 1;
    if (*selected && strcmp(hash,info.disk_sha256)) return 0;
    if (!*selected || strcmp(candidate,selected)<0) {
        if (strlen(candidate)>=capacity) return 0;
        strcpy(selected,candidate); strcpy(hash,info.disk_sha256);
    }
    return 1;
}
int fa18_media_discover(const char *argv0,char *path,size_t capacity,char *error,size_t error_size) {
    char directory[4096],hash[65]={0}; int ok=1;
    if (!path || !capacity) return 0;
    *path=0;
#ifdef _WIN32
    (void)argv0;
    DWORD length=GetModuleFileNameA(NULL,directory,sizeof directory);
    if (!length || length>=sizeof directory) goto location_failed;
#elif defined(__APPLE__)
    uint32_t length=sizeof directory;
    if (_NSGetExecutablePath(directory,&length)) goto location_failed;
#elif defined(__linux__)
    (void)argv0;
    ssize_t length=readlink("/proc/self/exe",directory,sizeof directory-1);
    if (length<0 || (size_t)length>=sizeof directory-1) goto location_failed;
    directory[length]=0;
#else
    if (!argv0 || !realpath(argv0,directory)) goto location_failed;
#endif
    char *slash=strrchr(directory,'/'),*backslash=strrchr(directory,'\\');
    if (backslash && (!slash || backslash>slash)) slash=backslash;
    if (!slash) goto location_failed;
    if (slash==directory) slash[1]=0; else *slash=0;
#ifdef _WIN32
    char pattern[4096];
    if (snprintf(pattern,sizeof pattern,"%s/*",directory)>=(int)sizeof pattern) goto location_failed;
    WIN32_FIND_DATAA entry; HANDLE scan=FindFirstFileA(pattern,&entry);
    if (scan==INVALID_HANDLE_VALUE) goto location_failed;
    do { if (!(entry.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) &&
              !consider(directory,entry.cFileName,path,capacity,hash)) { ok=0; break; } } while (FindNextFileA(scan,&entry));
    if (ok && GetLastError()!=ERROR_NO_MORE_FILES) ok=0;
    FindClose(scan);
#else
    DIR *scan=opendir(directory); if (!scan) goto location_failed;
    struct dirent *entry;
    while ((entry=readdir(scan))) if (!consider(directory,entry->d_name,path,capacity,hash)) { ok=0; break; }
    closedir(scan);
#endif
    if (!ok) { snprintf(error,error_size,"Multiple distinct supported ADFs or a directory error beside the executable; select one with --adf PATH"); *path=0; return 0; }
    if (!*path) { snprintf(error,error_size,"No supported ADF beside the executable (%s). Add a supported game ADF there or use --adf PATH",directory); return 0; }
    return 1;
location_failed:
    snprintf(error,error_size,"Cannot scan executable directory; use --adf PATH"); return 0;
}
