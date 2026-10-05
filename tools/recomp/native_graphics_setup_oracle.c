/* Original memory/CPU and native-object serialization exist only in validation. */
#define main publication_validation_main
#include "command_publication_oracle.c"
#undef main
#include "../../port/graphics_setup.c"
#include "../../port/five_plane_chip_binding.c"
#include "../../port/five_plane_page.c"
#include "../../port/copper_page.c"
#include "../../port/graphics_storage.c"
#include "../../port/amiga/host_graphics.h"
#include "../../port/amiga/hunk.h"
#include "../../build/recomp/native_graphics_setup_source.h"

enum { PLANE_BASE=0xc60000, MAP_BASE=0xc6b800, DYNAMIC_BASE=0xc6b900,
       SERVICE_BASE=0xc6bc00, PREVIOUS_VIEW=0xc6bc80, STACK_FIRST=0xc7fd00,STACK_END=0xc7ff00 };
typedef struct {
    FA18NativeGraphicsSetup setup;
    FA18NativeInputDisplay display;
    FA18NativeGraphicsStorage storage;
    FA18NativeGraphicsSetupOps backend;
    uint16_t palette[32];
    unsigned initialized,made,merged,allocations;
    int previous_view;
} NativeGraphics;
typedef struct { unsigned kind; uint32_t value; } GraphicsEvent;
static GraphicsEvent graphics_events[16];
static uint8_t graphics_ram[16][FA18_CHIP_SIZE+FA18_SLOW_SIZE];
static unsigned graphics_visited[sizeof graphics_source_bytes/sizeof graphics_source_bytes[0]];
static unsigned graphics_case,source_count,native_count,source_allocations,source_planes,total_boundaries,normal_calls,terminal_calls,second_calls;
static AmigaHostCompat graphics_host;
static uint32_t list_address(unsigned slot) { return slot?0xc6a418:0xc6a000; }
static uint32_t view_address(unsigned slot) { return slot?0xc6a748:0xc6a330; }
static uint32_t plane_value(FA18NativeGraphicsPlane *p) { return p?PLANE_BASE+p->payload:0; }
static uint32_t bitmap_value(NativeGraphics *s,FA18NativeGraphicsBitmap *p) {
    if(!p) return 0;
    if(p==&s->setup.bitmap_storage[0]) return 0xc1826a;
    if(p==&s->setup.bitmap_storage[1]) return 0xc18292;
    abort();
}
static uint32_t pair_value(NativeGraphics *s,void *p,int view) {
    unsigned i;
    if(!p) return 0;
    for(i=0;i<2;++i) if(p==(view?(void*)&s->storage.lists[i].view_list:(void*)&s->storage.lists[i].display_list))
        return view?view_address(i):list_address(i);
    abort();
}
static int equal_graphics_ram(const uint8_t *expected) {
    unsigned i;
    if(!memcmp(expected,fa18_machine->chip,FA18_CHIP_SIZE) &&
       !memcmp(expected+FA18_CHIP_SIZE,fa18_machine->slow,STACK_FIRST-FA18_SLOW_BASE) &&
       !memcmp(expected+FA18_CHIP_SIZE+STACK_END-FA18_SLOW_BASE,
          fa18_machine->slow+STACK_END-FA18_SLOW_BASE,FA18_SLOW_SIZE-(STACK_END-FA18_SLOW_BASE))) return 1;
    for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
        uint32_t a=i<FA18_CHIP_SIZE?i:FA18_SLOW_BASE+i-FA18_CHIP_SIZE;
        uint8_t got=i<FA18_CHIP_SIZE?fa18_machine->chip[i]:fa18_machine->slow[i-FA18_CHIP_SIZE];
        if(a>=STACK_FIRST && a<STACK_END) continue;
        if(got!=expected[i]) { fprintf(stderr,"graphics case %u event %u RAM %06X original %02X native %02X\n",graphics_case,native_count,a,expected[i],got); return 0; }
    }
    return 0;
}
static uint16_t translated_plane_word(uint16_t reg,uint16_t value) {
    /* Resolve private offsets back to validation-only pointer payloads. */
    if(reg>=0xe0 && reg<=0xf2 && !(reg&1)) {
        if((reg&3)==0) return (uint16_t)(value+(PLANE_BASE>>16));
        return value; /* Plane base is 64 KiB aligned. */
    }
    return value;
}
static void export_lists(NativeGraphics *s,unsigned slot) {
    AmigaNativeViewportLists *lists=&s->storage.lists[slot];
    unsigned i;
    uint32_t a=list_address(slot),v=view_address(slot);
    if(s->made&(1u<<slot)) {
        memset(fa18_machine->slow+a-FA18_SLOW_BASE,0,48+128*6);
        wr_u32(a+8,0xc1822a); wr_u32(a+12,a+48); wr_u32(a+16,a+48+6*(uint32_t)lists->display_list.instruction_count);
        wr_u16(a+28,(uint16_t)lists->display_list.instruction_count); wr_u16(a+30,128);
        if(s->merged&(1u<<slot)) { wr_u32(a+20,v+16); wr_u32(a+24,v+16); }
        for(i=0;i<lists->display_list.instruction_count;++i) {
            const uint8_t *p=lists->records+6*i;
            uint16_t op=amiga_be16(p),reg=amiga_be16(p+2),value=amiga_be16(p+4);
            wr_u16(a+48+6*i,op); wr_u16(a+50+6*i,reg);
            wr_u16(a+52+6*i,op?value:translated_plane_word(reg,value));
        }
    }
    if(s->merged&(1u<<slot)) {
        size_t records=lists->view_list.byte_count/4;
        memset(fa18_machine->slow+v-FA18_SLOW_BASE,0,(16+records*4+7u)&~7u);
        wr_u32(v+4,v+16); wr_u16(v+8,(uint16_t)records);
        for(i=0;i<records;++i) {
            const uint8_t *p=lists->merged+4*i;
            uint16_t reg=amiga_be16(p),value=amiga_be16(p+2);
            wr_u16(v+16+4*i,reg); wr_u16(v+18+4*i,(reg&1)?value:translated_plane_word(reg,value));
        }
    }
}
static void store_graphics(NativeGraphics *s) {
    unsigned i,j;
    FA18NativeGraphicsSetup *g=&s->setup;
    wr_u32(0xc182ca,g->service?SERVICE_BASE:0); wr_u32(0xc182ce,g->previous_view?PREVIOUS_VIEW:0);
    wr_u16(DRAW_PAGE,s->display.draw_page);
    for(i=0;i<9;++i) wr_u32(0xc456be +4*i,plane_value(g->source[i]));
    for(i=0;i<8;++i) wr_u32(0xc4566e +4*i,plane_value(g->table_a[i]));
    for(i=0;i<10;++i) wr_u32(0xc4568e +4*i,plane_value(g->table_b[i]));
    for(i=0;i<2;++i) {
        wr_u32(0xc182ba+4*i,pair_value(s,g->pairs[i].view,1));
        wr_u32(0xc182c2+4*i,pair_value(s,g->pairs[i].display_list,0));
        export_lists(s,i);
    }
    if(s->initialized) {
        memset(fa18_machine->slow+0xc18218-FA18_SLOW_BASE,0,18);
        wr_u32(0xc18218,0xc1822a); wr_u32(0xc1821c,pair_value(s,s->display.saved_pair.view,1));
        wr_u32(0xc18220,pair_value(s,g->short_view,1));
        wr_u16(0xc18224,(uint16_t)g->view_y); wr_u16(0xc18226,(uint16_t)g->view_x);
        memset(fa18_machine->slow+0xc1822a-FA18_SLOW_BASE,0,40);
        wr_u32(0xc1822e,g->viewport_color_map?MAP_BASE:0); wr_u32(0xc18232,pair_value(s,s->display.saved_pair.display_list,0));
        wr_u16(0xc18242,g->width); wr_u16(0xc18244,g->height);
        wr_u16(0xc18246,(uint16_t)g->viewport_x); wr_u16(0xc18248,(uint16_t)g->viewport_y);
        wr_u16(0xc1824a,g->modes); wr_u32(0xc1824e,0xc18256);
        wr_u32(0xc18252,g->color_map?MAP_BASE:0);
        wr_u32(0xc18256,0); wr_u32(0xc1825a,bitmap_value(s,g->raster_bitmap));
        wr_u16(0xc1825e,g->raster_x); wr_u16(0xc18260,g->raster_y);
        wr_u32(0xc18262,bitmap_value(s,g->bitmaps[0])); wr_u32(0xc18266,bitmap_value(s,g->bitmaps[1]));
        for(i=0;i<2;++i) {
            uint32_t a=i?0xc18292:0xc1826a;
            FA18NativeGraphicsBitmap *b=&g->bitmap_storage[i];
            memset(fa18_machine->slow+a-FA18_SLOW_BASE,0,40);
            wr_u16(a,b->row_bytes); wr_u16(a+2,b->rows); wr_u8(a+4,b->flags); wr_u8(a+5,b->depth);
            for(j=0;j<8;++j) wr_u32(a+8+4*j,plane_value(b->planes[j]));
            a=i?0xc18336:0xc182d2;
            memset(fa18_machine->slow+a-FA18_SLOW_BASE,0,100); wr_u32(a+4,bitmap_value(s,g->drawing_bitmap[i]));
        }
    }
    wr_u32(LONG_TABLE,s->display.stable_palette?DYNAMIC_BASE:0);
    if(s->storage.map_allocated) {
        memset(fa18_machine->slow+MAP_BASE-FA18_SLOW_BASE,0,72);
        wr_u16(MAP_BASE+2,32); wr_u32(MAP_BASE+4,MAP_BASE+8);
        memcpy(fa18_machine->slow+MAP_BASE+8-FA18_SLOW_BASE,s->storage.colors,64);
    }
    if(s->storage.dynamic_allocated) for(i=0;i<32;++i) wr_u16(DYNAMIC_BASE+2*i,s->storage.dynamic_palette[i]);
    memcpy(fa18_machine->slow+PLANE_BASE-FA18_SLOW_BASE,s->storage.renderer.chip_bytes,40000);
}
static void record_source(unsigned kind,uint32_t value) {
    if(source_count==16) abort();
    graphics_events[source_count]=(GraphicsEvent){kind,value};
    memcpy(graphics_ram[source_count],fa18_machine->chip,FA18_CHIP_SIZE);
    memcpy(graphics_ram[source_count]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE); ++source_count;
}
static int record_native(NativeGraphics *s,unsigned kind,uint32_t value) {
    store_graphics(s);
    if(native_count>=source_count || graphics_events[native_count].kind!=kind ||
       graphics_events[native_count].value!=value || !equal_graphics_ram(graphics_ram[native_count])) {
        fprintf(stderr,"graphics case %u boundary %u differs kind %u value %08X\n",graphics_case,native_count,kind,value); exit(1);
    }
    ++native_count; return 1;
}
static unsigned failing_allocation(void) { unsigned n=graphics_case%8; return n>=2 && n<=6?n-1:0; }
static FA18NativeDisplayService *native_open(void *context,unsigned version) {
    NativeGraphics *s=context; record_native(s,0,version);
    return graphics_case%8==1?NULL:s->backend.open_display(s->backend.context,version);
}
static FA18NativeGraphicsPlane *native_plane(void *context,size_t bytes,uint32_t flags) {
    NativeGraphics *s=context; s->initialized=1; record_native(s,1,(uint32_t)bytes);
    if(flags!=0x10002) abort();
    if(++s->allocations==failing_allocation()) return NULL;
    return s->backend.allocate_plane(s->backend.context,bytes,flags);
}
static AmigaRgb4Palette *native_map(void *context,size_t count) {
    NativeGraphics *s=context; record_native(s,2,(uint32_t)count);
    return s->backend.allocate_color_map(s->backend.context,count);
}
static uint16_t *native_dynamic(void *context,size_t bytes,uint32_t flags) {
    NativeGraphics *s=context; record_native(s,3,(uint32_t)bytes);
    if(flags!=0x10002) abort();
    return s->backend.allocate_dynamic_palette(s->backend.context,bytes,flags);
}
static int native_make(void *context,FA18NativeGraphicsSetup *g) {
    NativeGraphics *s=context; unsigned slot=slot_for(g);
    record_native(s,4,slot);
    if((graphics_case&64) && slot) g->width=0;
    s->backend.make_viewport(s->backend.context,g);
    if(g->display->saved_pair.display_list==&s->storage.lists[slot].display_list) s->made|=1u<<slot;
    return 1;
}
static int native_merge(void *context,FA18NativeGraphicsSetup *g) {
    NativeGraphics *s=context; unsigned slot=slot_for(g);
    record_native(s,5,slot); s->backend.merge_view(s->backend.context,g);
    if(g->display->saved_pair.view==&s->storage.lists[slot].view_list) s->merged|=1u<<slot;
    return 1;
}
static void native_terminate(void *context,int32_t reason) {
    NativeGraphics *s=context; record_native(s,6,(uint32_t)reason);
    s->backend.terminate(s->backend.context,reason);
}
static int original_graphics(void) {
    unsigned step,i;
    for(step=0;step<10000;++step) {
        uint32_t pc=REG_PC;
        uint16_t opcode;
        if(pc==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        if(pc==0xc53ca0) {
            if(rd_u32(REG_A[7]+4)!=0xc07ffc || rd_u32(REG_A[7]+8)!=29) return 0;
            record_source(0,29); REG_D[0]=graphics_case%8==1?0:SERVICE_BASE; REG_PC=m68ki_pull_32(); continue;
        }
        if(pc==0xc50de8) { record_source(6,rd_u32(REG_A[7]+4)); return 2; }
        if(pc==0xc53f54) {
            if(rd_u32(REG_A[7]+4)!=0xc18218 || !amiga_host_init_view(&graphics_host,0xc18218)) return 0;
            REG_PC=m68ki_pull_32(); continue;
        }
        if(pc==0xc53ef0) {
            if(rd_u32(REG_A[7]+4)!=0xc1822a) return 0;
            memset(fa18_machine->slow+0xc1822a-FA18_SLOW_BASE,0,40); REG_PC=m68ki_pull_32(); continue;
        }
        if(pc==0xc53f68) {
            uint32_t a=rd_u32(REG_A[7]+4),depth=rd_u32(REG_A[7]+8);
            if((a!=0xc1826a && a!=0xc18292) || depth!=(a==0xc1826a?5u:4u) ||
                rd_u32(REG_A[7]+12)!=320 || rd_u32(REG_A[7]+16)!=200) return 0;
            memset(fa18_machine->slow+a-FA18_SLOW_BASE,0,40); wr_u16(a,40); wr_u16(a+2,200); wr_u8(a+5,(uint8_t)depth);
            REG_PC=m68ki_pull_32(); continue;
        }
        if(pc==0xc53edc) {
            uint32_t a=rd_u32(REG_A[7]+4);
            if(a!=0xc182d2 && a!=0xc18336) return 0;
            memset(fa18_machine->slow+a-FA18_SLOW_BASE,0,100); REG_PC=m68ki_pull_32(); continue;
        }
        if(pc==0xc53b30) {
            uint32_t size=rd_u32(REG_A[7]+4);
            if(rd_u32(REG_A[7]+8)!=0x10002) return 0;
            if(size==8000) {
                record_source(1,size);
                if(++source_allocations==failing_allocation()) REG_D[0]=0;
                else { REG_D[0]=PLANE_BASE+8000*source_planes++; memset(fa18_machine->slow+REG_D[0]-FA18_SLOW_BASE,0,8000); }
            } else if(size==64) { record_source(3,size); REG_D[0]=DYNAMIC_BASE; memset(fa18_machine->slow+DYNAMIC_BASE-FA18_SLOW_BASE,0,64); }
            else return 0;
            REG_PC=m68ki_pull_32(); continue;
        }
        if(pc==0xc53fec) {
            if(rd_u32(REG_A[7]+4)!=32) return 0;
            record_source(2,32); memset(fa18_machine->slow+MAP_BASE-FA18_SLOW_BASE,0,72);
            wr_u16(MAP_BASE+2,32); wr_u32(MAP_BASE+4,MAP_BASE+8); REG_D[0]=MAP_BASE; REG_PC=m68ki_pull_32(); continue;
        }
        if(pc==0xc53f18) {
            unsigned slot=rd_u32(0xc1825a)==0xc18292;
            if(rd_u32(REG_A[7]+4)!=0xc18218 || rd_u32(REG_A[7]+8)!=0xc1822a) return 0;
            record_source(4,slot); if((graphics_case&64) && slot) wr_u16(0xc18242,0);
            REG_D[0]=amiga_host_make_viewport(&graphics_host,0xc18218,0xc1822a)?0:1;
            REG_PC=m68ki_pull_32(); continue;
        }
        if(pc==0xc53f04) {
            unsigned slot=rd_u32(0xc1825a)==0xc18292;
            if(rd_u32(REG_A[7]+4)!=0xc18218) return 0;
            record_source(5,slot); REG_D[0]=amiga_host_merge_view(&graphics_host,0xc18218)?0:1;
            REG_PC=m68ki_pull_32(); continue;
        }
        for(i=0;i<sizeof graphics_source_bytes/sizeof graphics_source_bytes[0];++i) if(graphics_source_bytes[i].pc==pc) break;
        if(i==sizeof graphics_source_bytes/sizeof graphics_source_bytes[0]) return 0;
        graphics_visited[i]=1; opcode=rd_u16(pc); REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
        m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    return 0;
}
static void start_graphics(uint32_t entry) {
    unsigned i;
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    REG_A[7]=0xc7ff00; wr_u32(REG_A[7],0xc70000); REG_PC=entry;
    m68k_set_reg(M68K_REG_SR,0x2700|(graphics_case&31)); SET_CYCLES(1000000000); fa18_next_event=INT64_MAX;
}
int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0;
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size),*rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
    NativeGraphics *native=malloc(sizeof *native);
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):4096,i,j;
    char error[256];
    if(!state || !rom || !m || !base || !before || !expected || !native || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) return 1;
    free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
    for(i=0;i<sizeof graphics_source_bytes/sizeof graphics_source_bytes[0];++i)
        for(j=0;j<graphics_source_bytes[i].length;++j) if(rd_u8(graphics_source_bytes[i].pc+j)!=graphics_source_bytes[i].bytes[j]) return 1;
    for(graphics_case=0;graphics_case<cases;++graphics_case) {
        int result;
        AmigaGuestBank banks[]={{0,FA18_CHIP_SIZE,0,m->chip},{FA18_SLOW_BASE,FA18_SLOW_SIZE,0,m->slow}};
        FA18NativeGraphicsSetupOps ops={native_open,native_plane,native_map,native_dynamic,native_make,native_merge,native_terminate,native};
        memcpy(m,base,sizeof *m); memset(native,0,sizeof *native);
        memset(m->slow+0xc18218-FA18_SLOW_BASE,0,0x182);
        memset(m->slow+0xc45660-FA18_SLOW_BASE,0,0x82);
        for(i=0;i<0x1000;++i) m->slow[0xc6a000+i-FA18_SLOW_BASE]=(uint8_t)random_value();
        wr_u16(DRAW_PAGE,(uint16_t)random_value()); wr_u32(SERVICE_BASE+34,PREVIOUS_VIEW);
        for(i=0;i<32;++i) { native->palette[i]=(uint16_t)random_value(); wr_u16(0xc1aa9c+2*i,native->palette[i]); }
        for(i=0;i<40000;++i) m->slow[PLANE_BASE+i-FA18_SLOW_BASE]=(uint8_t)random_value();
        memcpy(native->storage.renderer.chip_bytes,m->slow+PLANE_BASE-FA18_SLOW_BASE,40000);
        native->setup.display=&native->display; native->setup.initial_palette=native->palette;
        native->display.draw_page=rd_u16(DRAW_PAGE); native->storage.service.active_view=&native->previous_view;
        if(!fa18_bind_native_graphics_storage(&native->storage,&native->backend)) return 1;
        memset(&graphics_host,0,sizeof graphics_host); graphics_host.memory=(AmigaGuestMemory){banks,2};
        graphics_host.free_count=1; graphics_host.free[0].base=0xc6a000;
        graphics_host.free[0].size=0x1000; graphics_host.free[0].attributes=7;
        source_count=native_count=source_allocations=source_planes=0; start_graphics(0xc15db4); memcpy(before,m,sizeof *m);
        result=original_graphics();
        if(!result) { fprintf(stderr,"source graphics case %u failed at %06X\n",graphics_case,REG_PC); return 1; }
        if(result==1) { ++normal_calls; start_graphics(0xc160d6); if(original_graphics()!=1) return 1; ++second_calls; }
        else ++terminal_calls;
        memcpy(expected,m->chip,FA18_CHIP_SIZE); memcpy(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m);
        if(fa18_initialize_native_graphics(&native->setup,&ops)!=(result==1)) return 1;
        if(result==1 && !fa18_build_native_second_display(&native->setup,&ops)) return 1;
        store_graphics(native);
        if(source_count!=native_count || !equal_graphics_ram(expected)) return 1;
        total_boundaries+=native_count;
    }
    printf("native graphics setup: %u first-entry calls (%u normal, %u terminating), %u second-entry calls matched full RAM outside ABI stack and %u native allocation/construction boundaries\n",cases,normal_calls,terminal_calls,second_calls,total_boundaries);
    printf("visited:"); for(i=0;i<sizeof graphics_visited/sizeof graphics_visited[0];++i) if(graphics_visited[i]) printf(" %06X",graphics_source_bytes[i].pc); putchar('\n');
    free(native); free(expected); free(before); free(base); free(m); return 0;
}
