#include "host_compat.h"
#include "hunk.h"
#include "abi_13.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#define make_directory(path) _mkdir(path)
#else
#include <unistd.h>
#include <dirent.h>
#define make_directory(path) mkdir(path,0777)
#endif
static void word(uint8_t *p,uint16_t v) { p[0]=(uint8_t)(v>>8); p[1]=(uint8_t)v; }
static uint32_t aligned(uint32_t n) { return (n+7u)&~7u; }
static int add_free(AmigaHostCompat *c,AmigaHostRegion r) {
    if (!r.size) return 1;
    if (c->free_count==sizeof c->free/sizeof *c->free) return 0;
    size_t i=c->free_count++;
    while (i && c->free[i-1].base>r.base) { c->free[i]=c->free[i-1]; --i; }
    c->free[i]=r; return 1;
}
int amiga_host_init(AmigaHostCompat *c,const AmigaGuestMemory *m,const AmigaOfs *disk,
                    const char *save,const AmigaHostRegion *reserved,size_t count) {
    if (!c || !amiga_guest_memory_valid(m) || m->count>8 || !disk || !save ||
        strlen(save)>=sizeof c->save_directory || (!reserved && count)) return 0;
    memset(c,0,sizeof *c); memcpy(c->banks,m->banks,m->count*sizeof *m->banks);
    c->memory=(AmigaGuestMemory){c->banks,m->count}; c->disk=disk; strcpy(c->save_directory,save);
    for (size_t b=0;b<m->count;++b) {
        const AmigaGuestBank *bank=&m->banks[b];
        uint32_t start=aligned(bank->base),end=(bank->base+bank->size)&~7u;
        while (start<end) {
            uint32_t next=end,after=start;
            for (size_t i=0;i<count;++i) {
                uint64_t re=(uint64_t)reserved[i].base+reserved[i].size;
                if (re>0x1000000) return 0;
                uint32_t low=reserved[i].base&~7u;
                if (!reserved[i].size) continue;
                if (low<=start && re>start) { if (re>after) after=aligned((uint32_t)re); }
                else if (low>start && low<next) next=low;
            }
            if (after>start) { start=after; continue; }
            uint32_t attrs=1u|((bank->attributes&AMIGA_MEMORY_CHIP)?2u:4u);
            if (!add_free(c,(AmigaHostRegion){start,next-start,attrs})) return 0;
            start=next;
        }
    }
    return 1;
}
uint32_t amiga_host_alloc(AmigaHostCompat *c,uint32_t size,uint32_t flags) {
    if (!size || size>0xFFFFFFF8 || c->used_count==sizeof c->used/sizeof *c->used) return 0;
    uint32_t n=aligned(size),need=flags&6;
    for (unsigned pass=0;pass<2;++pass) for (size_t i=0;i<c->free_count;++i) {
        AmigaHostRegion *r=&c->free[i];
        if ((r->attributes&need)!=need || r->size<n || (!need && ((r->attributes&4)!=0)!=(pass==0))) continue;
        uint32_t address=r->base; c->used[c->used_count++]=(AmigaHostRegion){address,n,r->attributes};
        r->base+=n; r->size-=n;
        if (!r->size) { memmove(r,r+1,(c->free_count-i-1)*sizeof *r); --c->free_count; }
        if (flags&0x10000) memset(amiga_guest_range(&c->memory,address,n),0,n);
        return address;
    }
    c->error=103; return 0;
}
int amiga_host_free(AmigaHostCompat *c,uint32_t address,uint32_t size) {
    if (!address && !size) return 1;
    for (size_t i=0;i<c->used_count;++i) if (c->used[i].base==address && c->used[i].size==aligned(size)) {
        if (!add_free(c,c->used[i])) return 0;
        c->used[i]=c->used[--c->used_count];
        for (size_t j=1;j<c->free_count;) {
            AmigaHostRegion *left=&c->free[j-1],*right=&c->free[j];
            if (left->base+left->size==right->base && left->attributes==right->attributes) {
                left->size+=right->size; memmove(right,right+1,(c->free_count-j-1)*sizeof *right); --c->free_count;
            } else ++j;
        }
        return 1;
    }
    return 0;
}
uint32_t amiga_host_available(const AmigaHostCompat *c,uint32_t flags) {
    uint32_t result=0;
    for (size_t i=0;i<c->free_count;++i) if ((c->free[i].attributes&(flags&6))==(flags&6)) {
        if (flags&0x20000) { if (c->free[i].size>result) result=c->free[i].size; }
        else result+=c->free[i].size;
    }
    return result;
}
int amiga_host_string(const AmigaHostCompat *c,uint32_t a,char *out,size_t capacity) {
    for (size_t i=0;i<capacity;++i) {
        const uint8_t *p=amiga_guest_range(&c->memory,a+(uint32_t)i,1);
        if (!p) return 0;
        out[i]=(char)*p; if (!out[i]) return 1;
    }
    return 0;
}
static const char *library_names[]={"exec.library","dos.library","graphics.library","intuition.library",
    "potgo.resource","ciaa.resource","ciab.resource","timer.device","input.device","gameport.device",
    "keyboard.device","audio.device"};
const char *amiga_host_library_name(unsigned id) { return id<AMIGA_HOST_LIBRARY_COUNT?library_names[id]:NULL; }
uint32_t amiga_host_library(AmigaHostCompat *c,const char *name,uint32_t version) {
    if (version>34) return 0;
    unsigned id;
    for (id=0;id<AMIGA_HOST_LIBRARY_COUNT;++id) if (!strcmp(name,library_names[id])) break;
    if (id==AMIGA_HOST_LIBRARY_COUNT) return 0;
    if (!c->libraries[id]) {
        uint32_t allocation=amiga_host_alloc(c,768+512+32,0x10004);
        if (!allocation) return 0;
        uint32_t base=allocation+768; uint8_t *p=amiga_guest_range(&c->memory,base,544);
        c->libraries[id]=base; p[AMIGA_NODE_TYPE]=9;
        amiga_store_be32(p+AMIGA_NODE_NAME,base+512); strcpy((char *)p+512,name);
        word(p+16,768); word(p+18,512); word(p+20,34); word(p+22,2);
        for (unsigned offset=6;offset<=768;offset+=6) {
            uint8_t *v=amiga_guest_range(&c->memory,base-offset,6);
            word(v,0x4EF9); amiga_store_be32(v+2,AMIGA_HOST_SERVICE_BASE+id*0x400+(offset/6)*2);
        }
    }
    uint8_t *p=amiga_guest_range(&c->memory,c->libraries[id]+32,2);
    word(p,(uint16_t)(amiga_be16(p)+1)); return c->libraries[id];
}
/* One mounted volume and a writable overlay. Normalize ASCII names so Amiga
 * case-insensitive save lookups work on both Windows and GNU hosts. */
static int path_name(AmigaHostCompat *c,const char *input,char *out) {
    const char *colon=strchr(input,':');
    if (colon) input=colon+1;
    char joined[512]; const char *directory="";
    if (!colon && c->current_directory) {
        unsigned i=c->current_directory-1;
        if (i>=64 || !c->locks[i].active) { c->error=205; return 0; }
        directory=c->locks[i].path;
    }
    int n=snprintf(joined,sizeof joined,"%s%s%s",directory,*directory?"/":"",input);
    if (n<0 || n>=256 || *joined=='/' || *joined=='\\') { c->error=205; return 0; }
    size_t component=0;
    for (int i=0;i<=n;++i) {
        unsigned char ch=(unsigned char)joined[i];
        if (ch=='\\' || ch==':') { c->error=205; return 0; }
        if (ch=='/' || !ch) {
            size_t length=(size_t)i-component;
            if ((length==2 && joined[component]=='.' && joined[component+1]=='.') ||
                (length==1 && joined[component]=='.') || (!length && ch)) { c->error=205; return 0; }
            component=(size_t)i+1;
        }
        out[i]=(char)(ch>='A' && ch<='Z'?ch+32:ch);
    }
    return 1;
}
static int host_path(AmigaHostCompat *c,const char *name,char *out,size_t size) {
    int n=snprintf(out,size,"%s/%s",c->save_directory,name); return n>=0 && (size_t)n<size;
}
static int directories(char *path) {
    for (size_t i=1;path[i];++i) if (path[i]=='/' || path[i]=='\\') {
        char ch=path[i]; path[i]=0; make_directory(path); path[i]=ch;
    }
    return 1;
}
static AmigaHostFile *file_handle(AmigaHostCompat *c,uint32_t handle) {
    if (!handle || handle>64 || !c->files[handle-1].active) { c->error=209; return NULL; }
    return &c->files[handle-1];
}
uint32_t amiga_host_open(AmigaHostCompat *c,const char *input,int32_t mode) {
    char name[256],host[1024];
    if (!path_name(c,input,name) || !*name || !host_path(c,name,host,sizeof host)) { c->error=205; return 0; }
    unsigned index; for (index=0;index<64 && c->files[index].active;++index) {}
    if (index==64) { c->error=103; return 0; }
    if (mode!=1004 && mode!=1005 && mode!=1006) { c->error=115; return 0; }
    AmigaHostFile *f=&c->files[index]; memset(f,0,sizeof *f);
    if (mode==1006) { directories(host); f->file=fopen(host,"wb+"); }
    else {
        f->file=fopen(host,mode==1004?"rb+":"rb");
        if (!f->file) f->data=amiga_ofs_read(c->disk,name,&f->size);
        if (mode==1004 && f->data) {
            directories(host); f->file=fopen(host,"wb+");
            if (f->file && fwrite(f->data,1,f->size,f->file)==f->size && !fseek(f->file,0,SEEK_SET)) {
                free(f->data); f->data=NULL;
            } else { if (f->file) fclose(f->file); f->file=NULL; free(f->data); f->data=NULL; }
        }
        else if (mode==1004 && !f->file) { directories(host); f->file=fopen(host,"wb+"); }
    }
    if (!f->file && !f->data) { c->error=205; return 0; }
    f->active=1; c->error=0; return index+1;
}
int amiga_host_file_close(AmigaHostCompat *c,uint32_t handle) {
    AmigaHostFile *f=file_handle(c,handle); if (!f) return 0;
    int ok=!f->file || fclose(f->file)==0; free(f->data); memset(f,0,sizeof *f); c->error=ok?0:209; return ok;
}
int32_t amiga_host_read(AmigaHostCompat *c,uint32_t h,void *buffer,int32_t length) {
    AmigaHostFile *f=file_handle(c,h); if (!f || length<0) { c->error=115; return -1; }
    if (f->file) { size_t n=fread(buffer,1,(size_t)length,f->file); if (ferror(f->file)) { c->error=209; return -1; } return (int32_t)n; }
    size_t n=(size_t)length; if (n>f->size-f->position) n=f->size-f->position;
    if (n) memcpy(buffer,f->data+f->position,n);
    f->position+=n; return (int32_t)n;
}
int32_t amiga_host_write(AmigaHostCompat *c,uint32_t h,const void *buffer,int32_t length) {
    AmigaHostFile *f=file_handle(c,h); if (!f || length<0 || !f->file) { c->error=209; return -1; }
    size_t n=fwrite(buffer,1,(size_t)length,f->file); if (ferror(f->file)) { c->error=209; return -1; } return (int32_t)n;
}
int32_t amiga_host_seek(AmigaHostCompat *c,uint32_t h,int32_t position,int32_t mode) {
    AmigaHostFile *f=file_handle(c,h); if (!f || mode<-1 || mode>1) { c->error=219; return -1; }
    if (f->file) { long old=ftell(f->file); if (old<0 || fseek(f->file,position,mode==-1?SEEK_SET:mode?SEEK_END:SEEK_CUR)) { c->error=219; return -1; } return (int32_t)old; }
    int64_t next=(mode==-1?0:mode?f->size:f->position)+(int64_t)position;
    if (next<0 || (uint64_t)next>f->size) { c->error=219; return -1; }
    int32_t old=(int32_t)f->position; f->position=(size_t)next; return old;
}
uint32_t amiga_host_lock(AmigaHostCompat *c,const char *input) {
    char name[256],host[1024]; AmigaOfsEntry entry; struct stat info;
    if (!path_name(c,input,name) || !host_path(c,name,host,sizeof host)) return 0;
    if (stat(host,&info) && !amiga_ofs_find(c->disk,name,&entry)) { c->error=205; return 0; }
    for (unsigned i=0;i<64;++i) if (!c->locks[i].active) {
        c->locks[i].active=1; strcpy(c->locks[i].path,name); c->error=0; return i+1;
    }
    c->error=103; return 0;
}
int amiga_host_unlock(AmigaHostCompat *c,uint32_t lock) {
    if (!lock) return 1;
    if (lock>64 || !c->locks[lock-1].active) { c->error=205; return 0; }
    free(c->locks[lock-1].entries); memset(&c->locks[lock-1],0,sizeof c->locks[lock-1]); return 1;
}
static void fill_fib(uint8_t *fib,const AmigaOfsEntry *entry) {
    memset(fib,0,260);
    amiga_store_be32(fib,entry->block); amiga_store_be32(fib+4,(uint32_t)entry->type);
    snprintf((char *)fib+8,108,"%s",entry->name);
    amiga_store_be32(fib+116,entry->protection); amiga_store_be32(fib+120,(uint32_t)entry->type);
    amiga_store_be32(fib+124,entry->size); amiga_store_be32(fib+128,(entry->size+487)/488);
    amiga_store_be32(fib+132,entry->days); amiga_store_be32(fib+136,entry->minutes);
    amiga_store_be32(fib+140,entry->ticks); snprintf((char *)fib+144,80,"%s",entry->comment);
}
static int add_directory_entry(void *context,const AmigaOfsEntry *entry) {
    AmigaHostLock *lock=context;
    for (size_t i=0;i<lock->entry_count;++i) if (!strcmp(lock->entries[i].name,entry->name)) {
        lock->entries[i]=*entry; return 1;
    }
    AmigaOfsEntry *entries=realloc(lock->entries,(lock->entry_count+1)*sizeof *entries);
    if (!entries) return 0;
    lock->entries=entries; entries[lock->entry_count++]=*entry; return 1;
}
static int add_host_entry(AmigaHostLock *lock,const char *directory,const char *name) {
    if (!strcmp(name,".") || !strcmp(name,"..")) return 1;
    if (strlen(name)>=sizeof ((AmigaOfsEntry *)0)->name) return 0;
    char path[1024]; struct stat info;
    int n=snprintf(path,sizeof path,"%s/%s",directory,name);
    if (n<0 || (size_t)n>=sizeof path || stat(path,&info)) return 0;
    AmigaOfsEntry entry={0}; entry.type=(info.st_mode&S_IFDIR)?2:-3; entry.size=(uint32_t)info.st_size;
    strcpy(entry.name,name);
    for (char *p=entry.name;*p;++p) if (*p>='A' && *p<='Z') *p=(char)(*p+32);
    return add_directory_entry(lock,&entry);
}
int amiga_host_exnext(AmigaHostCompat *c,uint32_t index,uint8_t *fib,size_t size) {
    if (!index || index>64 || !c->locks[index-1].active || size<260) { c->error=205; return 0; }
    AmigaHostLock *lock=&c->locks[index-1];
    if (!lock->enumerated) {
        AmigaOfsEntry directory;
        if (amiga_ofs_find(c->disk,lock->path,&directory) && directory.type>0 &&
            !amiga_ofs_list(c->disk,lock->path,add_directory_entry,lock)) { c->error=103; return 0; }
        char path[1024]; if (!host_path(c,lock->path,path,sizeof path)) return 0;
#ifdef _WIN32
        char pattern[1028]; snprintf(pattern,sizeof pattern,"%s/*",path);
        WIN32_FIND_DATAA data; HANDLE handle=FindFirstFileA(pattern,&data);
        if (handle!=INVALID_HANDLE_VALUE) {
            int ok=1;
            do { if (!add_host_entry(lock,path,data.cFileName)) { ok=0; break; } } while (FindNextFileA(handle,&data));
            FindClose(handle); if (!ok) { c->error=103; return 0; }
        }
#else
        DIR *directory_handle=opendir(path);
        if (directory_handle) {
            struct dirent *entry; int ok=1;
            while ((entry=readdir(directory_handle))) if (!add_host_entry(lock,path,entry->d_name)) { ok=0; break; }
            closedir(directory_handle); if (!ok) { c->error=103; return 0; }
        }
#endif
        lock->enumerated=1;
    }
    if (lock->entry_next==lock->entry_count) { c->error=232; return 0; }
    fill_fib(fib,&lock->entries[lock->entry_next++]); c->error=0; return 1;
}
int amiga_host_examine(AmigaHostCompat *c,uint32_t lock,uint8_t *fib,size_t size) {
    if (!lock || lock>64 || !c->locks[lock-1].active || size<260) { c->error=205; return 0; }
    char host[1024]; const char *name=c->locks[lock-1].path; struct stat info; AmigaOfsEntry entry;
    free(c->locks[lock-1].entries); c->locks[lock-1].entries=NULL;
    c->locks[lock-1].entry_count=c->locks[lock-1].entry_next=0; c->locks[lock-1].enumerated=0;
    memset(fib,0,size);
    if (!host_path(c,name,host,sizeof host)) return 0;
    if (!stat(host,&info)) {
        int32_t type=(info.st_mode&S_IFDIR)?2:-3; amiga_store_be32(fib+4,(uint32_t)type); amiga_store_be32(fib+120,(uint32_t)type);
        amiga_store_be32(fib+124,(uint32_t)info.st_size); const char *leaf=strrchr(name,'/'); leaf=leaf?leaf+1:name;
        snprintf((char *)fib+8,108,"%s",leaf);
    } else if (amiga_ofs_find(c->disk,name,&entry)) {
        amiga_store_be32(fib,entry.block); amiga_store_be32(fib+4,(uint32_t)entry.type); strcpy((char *)fib+8,entry.name);
        amiga_store_be32(fib+116,entry.protection); amiga_store_be32(fib+120,(uint32_t)entry.type); amiga_store_be32(fib+124,entry.size);
        amiga_store_be32(fib+128,(entry.size+487)/488); amiga_store_be32(fib+132,entry.days);
        amiga_store_be32(fib+136,entry.minutes); amiga_store_be32(fib+140,entry.ticks); strcpy((char *)fib+144,entry.comment);
    } else { c->error=205; return 0; }
    c->error=0; return 1;
}
void amiga_host_close(AmigaHostCompat *c) {
    if (!c) return;
    for (unsigned i=0;i<64;++i) if (c->files[i].active) amiga_host_file_close(c,i+1);
    for (unsigned i=0;i<64;++i) if (c->locks[i].active) amiga_host_unlock(c,i+1);
    memset(c,0,sizeof *c);
}
int amiga_host_add_tail(AmigaHostCompat *c,uint32_t list,uint32_t node) {
    uint8_t *l=amiga_guest_range(&c->memory,list,12),*n=amiga_guest_range(&c->memory,node,8);
    if (!l || !n) return 0;
    uint32_t predecessor=amiga_be32(l+8); uint8_t *p=amiga_guest_range(&c->memory,predecessor,4);
    if (!p) return 0;
    amiga_store_be32(n,list+4); amiga_store_be32(n+4,predecessor);
    amiga_store_be32(p,node); amiga_store_be32(l+8,node); return 1;
}
int amiga_host_remove(AmigaHostCompat *c,uint32_t node) {
    uint8_t *n=amiga_guest_range(&c->memory,node,8); if (!n) return 0;
    uint8_t *next=amiga_guest_range(&c->memory,amiga_be32(n)+4,4),*previous=amiga_guest_range(&c->memory,amiga_be32(n+4),4);
    if (!next || !previous) return 0;
    amiga_store_be32(next,amiga_be32(n+4)); amiga_store_be32(previous,amiga_be32(n)); return 1;
}
int amiga_host_queue_key(AmigaHostCompat *c,unsigned rawkey,int down) {
    if (!c || rawkey>=128) return 0;
    unsigned next=(c->key_tail+1)%sizeof c->keys;
    if (next==c->key_head) return 0;
    if (down) c->keyboard_matrix[rawkey/8]|=(uint8_t)(1u<<(rawkey&7));
    else c->keyboard_matrix[rawkey/8]&=(uint8_t)~(1u<<(rawkey&7));
    c->keys[c->key_tail]=(uint8_t)(rawkey|(down?0:128)); c->key_tail=next;
    return 1;
}
