/* Original disk/Hunk/decoded palette checks. No CPU or machine linkage. */
#include "../../port/display_palette_assets.h"
#include "../../port/postflight_text.h"
#include "../../port/menu_record.h"
#include "../../port/disk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int compare_words(const uint16_t *words,const uint8_t *expected,size_t count) {
    size_t i;
    for(i=0;i<count;++i) if(words[i]!=fa18_be16(expected+2*i)) return 0;
    return 1;
}
int main(int argc,char **argv) {
    FA18Disk disk;
    FA18Hunks exe;
    FA18DisplayPaletteAssets assets,previous;
    size_t bytes,i;
    uint8_t *file,*image,expected[672];
    FILE *seal;
    const char *paths[]={"pix/inst5","pix/frnt5"};
    if(argc!=3 || !fa18_disk_open(&disk,argv[1])) return 1;
    seal=fopen(argv[2],"rb");
    if(!seal || fread(expected,1,sizeof expected,seal)!=sizeof expected || fclose(seal)) return 1;
    image=fa18_disk_read(&disk,"F-18 Interceptor",&bytes);
    if(!image || !fa18_hunks_load(&exe,image,bytes)) return 1;
    free(image);
    for(i=0;i<2;++i) {
        size_t mode;
        file=fa18_disk_read(&disk,paths[i],&bytes);
        if(!file || !fa18_import_display_palette_assets(&assets,&exe,file,bytes) ||
           !compare_words(assets.initial,expected,32) || !compare_words(assets.static_words,expected+64,32)) return 1;
        for(mode=0;mode<16;++mode) if(assets.modes[mode]!=assets.mode_words+(15-mode)*16 ||
            !compare_words(assets.modes[mode],expected+128+32*mode,16)) return 1;
        if(!compare_words(assets.mode_words,expected+128+32*15,16) ||
           !compare_words(assets.mode_words+16,expected+128+32*14,16)) return 1;
        {
            FA18NativePostflightText text={0};
            FA18MenuRecord record;
            uint8_t old;
            if(!fa18_bind_native_postflight_text_assets(&text,&exe,&assets) ||
               text.palette_seed!=assets.mode_words || text.text_bytes<32 ||
               memcmp(text.text_descriptor,expected+640,32)) return 1;
            old=text.text_descriptor[21]; text.text_descriptor[21]='1';
            if(fa18_menu_select_message_record(&exe,97,&record) || record.text[17]!='1') return 1;
            text.text_descriptor[21]=old;
        }
        previous=assets;
        if(fa18_import_display_palette_assets(&assets,&exe,file,bytes-1) || memcmp(&assets,&previous,sizeof assets)) return 1;
        /* Imported Hunk words remain raw, even if an owner modifies their
         * upper bits. Only the real display backend masks output. */
        {
            uint8_t *word=exe.segments[21].data+0x80;
            uint8_t old[2]={word[0],word[1]};
            word[0]=0xab; word[1]=0xcd;
            if(!fa18_import_display_palette_assets(&assets,&exe,file,bytes) || assets.mode_words[0]!=0xabcd) return 1;
            word[0]=old[0]; word[1]=old[1];
        }
        free(file);
    }
    fa18_hunks_free(&exe); fa18_disk_close(&disk);
    puts("native display assets: inst5 and frnt5 imports each match all 320 decoded/static/mode words; contiguous 32-word seed, actual mutable selector-97 text, raw high bits, owned pointers and truncated-resource rejection pass");
    return 0;
}
