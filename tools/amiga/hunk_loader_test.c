/* SDK-independent loader contract tests. Real disk/oracle parity is driven
 * by check_hunk_loader.py; malformed inputs must not modify guest RAM. */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hunk_loader.h"
#include "ofs.h"
static uint8_t *read_file(const char *path,size_t *size) {
    FILE *f=fopen(path,"rb"); assert(f);
    assert(!fseek(f,0,SEEK_END)); long n=ftell(f); assert(n>=0);
    assert(!fseek(f,0,SEEK_SET)); uint8_t *data=malloc(n?n:1); assert(data);
    assert(fread(data,1,(size_t)n,f)==(size_t)n); assert(!fclose(f)); *size=(size_t)n; return data;
}
static void negative_contracts(void) {
    uint8_t bytes[128],before[128],data[8]={0};
    AmigaGuestBank bank={0,128,AMIGA_MEMORY_CHIP,bytes};
    AmigaGuestMemory memory={&bank,1};
    AmigaHunkSegment segments[2]={{AMIGA_HUNK_CODE,data,8,NULL,0,0},{AMIGA_HUNK_BSS,data,8,NULL,0,0}};
    AmigaHunks image={segments,2};
    AmigaHunkPlacement layout[2]={{32,16},{40,16}};
    char error[128]; uint32_t seglist=99;
    memset(bytes,0xA5,sizeof bytes); memcpy(before,bytes,sizeof bytes);
    assert(!amiga_hunks_install(&image,layout,2,&memory,&seglist,error,sizeof error));
    assert(!seglist && strstr(error,"overlap") && !memcmp(bytes,before,sizeof bytes));
    layout[1].payload_base=64; segments[1].memory_flags=AMIGA_MEMORY_FAST;
    assert(!amiga_hunks_install(&image,layout,2,&memory,&seglist,error,sizeof error));
    assert(strstr(error,"requirement") && !memcmp(bytes,before,sizeof bytes));
    segments[1].memory_flags=0;
    AmigaHunkReloc bad={7,0}; segments[0].relocs=&bad; segments[0].reloc_count=1;
    assert(!amiga_hunks_install(&image,layout,2,&memory,&seglist,error,sizeof error));
    assert(!memcmp(bytes,before,sizeof bytes));
    bad.offset=4; bad.target=2;
    assert(!amiga_hunks_install(&image,layout,2,&memory,&seglist,error,sizeof error));
    assert(!memcmp(bytes,before,sizeof bytes));
    segments[0].reloc_count=0;
    assert(amiga_hunks_install(&image,layout,2,&memory,&seglist,error,sizeof error));
    assert(seglist==7 && amiga_be32(bytes+24)==16 && amiga_be32(bytes+28)==15);
    assert(amiga_be32(bytes+56)==16 && amiga_be32(bytes+60)==0);
    for (unsigned i=64;i<72;++i) assert(bytes[i]==0);
    AmigaHunks parsed;
    const uint8_t malformed[]={0,0,3,0xF3,0,0,0,0,0xff,0xff,0xff,0xff};
    assert(!amiga_hunks_parse(&parsed,malformed,sizeof malformed));
    /* Resident names contain a word count, not a list of name words ending
     * at an incidental zero inside the text. */
    const uint32_t named_words[]={0x3F3,2,0x74657374,0x2E6C6962,0,1,0,0,1,0x3E9,1,0x4E750000,0x3F2};
    uint8_t named[sizeof named_words];
    for (unsigned i=0;i<sizeof named_words/sizeof *named_words;++i)
        amiga_store_be32(named+i*4,named_words[i]);
    assert(amiga_hunks_parse(&parsed,named,sizeof named));
    assert(parsed.count==1 && amiga_be32(parsed.segments[0].data)==0x4E750000);
    amiga_hunks_free(&parsed);
    for (size_t n=0;n<sizeof named;++n) {
        assert(!amiga_hunks_parse(&parsed,named,n));
        assert(!parsed.count && !parsed.segments);
    }
}
int main(int argc,char **argv) {
    negative_contracts();
    if (argc==1) { puts("Hunk loader negative/atomicity contracts pass"); return 0; }
    assert(argc==4 || (argc==6 && !strcmp(argv[1],"--adf"))); size_t size,layout_size;
    uint8_t *exe;
    const char *layout_path,*output_path;
    if (argc==6) {
        AmigaOfs disk; assert(amiga_ofs_open(&disk,argv[2]));
        exe=amiga_ofs_read(&disk,argv[3],&size); assert(exe); amiga_ofs_close(&disk);
        layout_path=argv[4]; output_path=argv[5];
    } else {
        exe=read_file(argv[1],&size); layout_path=argv[2]; output_path=argv[3];
    }
    uint8_t *raw_layout=read_file(layout_path,&layout_size);
    AmigaHunks image; assert(amiga_hunks_parse(&image,exe,size));
    assert(layout_size>=4 && amiga_be32(raw_layout)==image.count && layout_size==4+8*image.count);
    AmigaHunkPlacement *layout=calloc(image.count,sizeof *layout); assert(layout);
    for (unsigned i=0;i<image.count;++i) {
        layout[i].payload_base=amiga_be32(raw_layout+4+8*i);
        layout[i].allocation_size=amiga_be32(raw_layout+8+8*i);
    }
    uint8_t *chip=calloc(0x80000,1),*slow=calloc(0x80000,1); assert(chip&&slow);
    AmigaGuestBank banks[]={{0,0x80000,AMIGA_MEMORY_CHIP,chip},{0xC00000,0x80000,AMIGA_MEMORY_FAST,slow}};
    AmigaGuestMemory memory={banks,2}; char error[256]; uint32_t seglist;
    if (!amiga_hunks_install(&image,layout,image.count,&memory,&seglist,error,sizeof error)) {
        fprintf(stderr,"%s\n",error); return 1;
    }
    FILE *out=fopen(output_path,"wb"); assert(out);
    assert(fwrite(chip,1,0x80000,out)==0x80000 && fwrite(slow,1,0x80000,out)==0x80000 && !fclose(out));
    uint32_t relocations=0;
    for (unsigned i=0;i<image.count;++i) relocations+=image.segments[i].reloc_count;
    printf("%u disk hunks installed; %u relocations; segment-list BPTR=%08X\n",image.count,relocations,seglist);
    amiga_hunks_free(&image); free(exe); free(raw_layout); free(layout); free(chip); free(slow);
    return 0;
}
