/* The CPU, captured machine and pointer encodings belong only to this proof. */
#define FA18_SCENE_SOURCE_HEADER "../../build/recomp/native_scene_regions_source.h"
#define FA18_SCENE_PLAYER_HELPERS_ONLY
#include "native_scene_player_oracle.c"
#include "../../port/native_scene_regions.c"
#include "../../port/native_scene_placement.c"
#include "../../port/native_record_orientation.c"
#include "../../port/two_angle_matrix.c"
#include "../../port/flight.c"
#include "../../port/disk.h"

enum { REGION_DIRECTORY=0xc29720, REGION_DATA=0xc60000, REGION_STRIDE=64,
    PARAMETER_BASE=0xc295e0, PARAMETER_BYTES=4096, PARAMETER_ORIGIN=1024,
    MODEL_BASE=0xc63000, MODEL_STRIDE=0x2000, MODEL_BYTES=4102,
    TRIG_BASE=0xc3e5e8, TRIG_BYTES=65536, TRIG_ORIGIN=32768,
    DESCRIPTOR_BASE=0xc22048, DESCRIPTOR_BANK=0xc22188, REGION_COUNT=12 };
typedef struct {
    SceneState scene;
    FA18NativeSceneRegions regions;
    FA18NativeSceneRegionAssets assets;
    FA18NativeRecordViewWork work;
    FA18NativeScenePointerGroup sources[2],groups[16];
    FA18NativeRegionDescriptor descriptors[18];
    PortFieldWindow parameter_window;
    PortFieldWindow directory[REGION_COUNT+1];
    uint8_t rows[REGION_COUNT][REGION_STRIDE],models[2][MODEL_BYTES],parameters[PARAMETER_BYTES],trig[TRIG_BYTES];
    uint8_t recorder,occupied,mode,admitted,active,reuse;
    uint16_t random,jitter_x,jitter_z;
} RegionFixture;

static int region_original_assets(void) {
    FA18Disk disk; FA18Hunks hunks; FA18NativeSceneRegionAssets assets;
    PortFieldWindow parameters,directory[16]; FA18NativeScenePointerGroup groups[3];
    FA18NativeRegionDescriptor descriptors[3];
    static const uint32_t selectors[]={0x3c,0x78,0xc8};
    static const uint32_t origins[]={0x152,0x186,0x1a6,0x1bc};
    size_t size=0; uint8_t *file; unsigned i,j;
    if(!fa18_disk_open(&disk,"FA-18 Interceptor (1988)(Electronic Arts)[cr A-Ha].adf")) return 0;
    file=fa18_disk_read(&disk,"F-18 Interceptor",&size);
    if(!file || !fa18_hunks_load(&hunks,file,size)) return 0;
    for(i=0;i<3;++i) {
        if(!fa18_load_native_scene_pointer_group(&hunks,selectors[i],groups+i) ||
           groups[i].procedure!=FA18_SCENE_PROCEDURE_COMPONENT_ACCUMULATION ||
           groups[i].data[1].carried_value_bound) {
            uint32_t segment=0,offset=0;
            fa18_hunk_pointer(&hunks,16,selectors[i],&segment,&offset);
            fprintf(stderr,"region descriptor %X procedure %u+%X\n",selectors[i],segment,offset); return 0;
        }
        descriptors[i]=(FA18NativeRegionDescriptor){.selector=(int16_t)selectors[i],.group=groups+i};
        for(j=0;j<4;++j) {
            uint32_t segment,offset;
            if(fa18_hunk_pointer(&hunks,16,selectors[i]+4+4*j,&segment,&offset)) {
                if(groups[i].data[j].segment!=segment || groups[i].data[j].offset!=offset ||
                   groups[i].data[j].data.bytes!=hunks.segments[segment].data) {
                    fprintf(stderr,"region reference %X field %u expected %u+%X got %u+%X\n",selectors[i],j,segment,offset,groups[i].data[j].segment,groups[i].data[j].offset); return 0;
                }
            } else if(groups[i].data[j].data.bytes) { fprintf(stderr,"unexpected region reference\n"); return 0; }
        }
    }
    if(!fa18_load_native_scene_region_assets(&hunks,&assets,&parameters,directory,16,descriptors,3) ||
       assets.directory_count!=5 || parameters.bytes!=hunks.segments[27].data ||
       parameters.byte_count!=2328 || assets.descriptors!=descriptors) {
        fprintf(stderr,"region asset directory count %zu parameter bytes %zu\n",assets.directory_count,parameters.byte_count); return 0;
    }
    for(i=0;i<2328;++i) if(parameters.bytes[i]!=rd_u8(PARAMETER_BASE+i) &&
        !(i>=0x140 && i<0x150)) {
            fprintf(stderr,"region asset byte %X disk %02X source %02X\n",i,parameters.bytes[i],rd_u8(PARAMETER_BASE+i)); return 0;
        }
    for(i=0;i<4;++i) {
        uint16_t before,after;
        if(directory[i].bytes!=parameters.bytes || directory[i].origin!=origins[i] ||
           !port_field_window_u16(directory+i,0,&before)) {
            fprintf(stderr,"region window %u expected %X got %zX\n",i,origins[i],directory[i].origin); return 0;
        }
        hunks.segments[27].data[origins[i]+1]^=1;
        if(!port_field_window_u16(directory+i,0,&after) || after!=(uint16_t)(before^1)) return 0;
        hunks.segments[27].data[origins[i]+1]^=1;
    }
    if(directory[4].bytes || fa18_load_native_scene_region_assets(&hunks,&assets,&parameters,directory,4,descriptors,3)) {
        fprintf(stderr,"region asset capacity/terminator mismatch\n"); return 0;
    }
    puts("region assets: all 2328 Hunk-27 bytes/relocations, four live region windows and three original descriptor groups verified; numeric operands remain explicit");
    fa18_hunks_free(&hunks); free(file); fa18_disk_close(&disk); return 1;
}

static int region_source(void) {
    unsigned step,i;
    for(step=0;step<50000;++step) {
        uint32_t pc=REG_PC; uint16_t opcode;
        if(pc==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
            if(scene_source_bytes[i].pc==pc) break;
        if(i==sizeof scene_source_bytes/sizeof scene_source_bytes[0]) {
            fprintf(stderr,"unexpected region PC %06X\n",pc); return 0;
        }
        scene_seen[i]=1; opcode=rd_u16(pc); REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
        m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    fprintf(stderr,"region source did not return at %06X\n",REG_PC); return 0;
}
static void region_machine_fixture(unsigned scenario) {
    unsigned i,j;
    static const uint8_t modes[]={0,1,2,3,4,125,127,128,255};
    static const uint16_t links[]={0,0x100,0x200,0x8000,0x8100,0x8200};
    static const uint16_t counts[]={0,1,3,0xffff,0x8000};
    wr_u8(0xc45790,(uint8_t)(scenario%17==0)); wr_u8(0xc4579d,(uint8_t)random_value());
    wr_u8(0xc458a6,modes[scenario%9]);
    wr_u8(0xc458aa,(uint8_t)(scenario%7==0?0x80:scenario%7==1?0xff:100));
    wr_u8(0xc458a9,(uint8_t)(scenario%8==0?100:scenario%8==1?0x80:0));
    wr_u8(0xc4582b,(uint8_t)(scenario&1));
    wr_u16(0xc45af8,(uint16_t)random_value());
    wr_u16(0xc45b18,(uint16_t)random_value()); wr_u16(0xc45b1a,(uint16_t)random_value());
    for(i=0;i<16;++i) {
        uint32_t r=CONTROL_RECORDS+512*i;
        uint16_t flags=(uint16_t)random_value();
        flags=(uint16_t)((flags&~64u)|((scenario+i)%7==0?64u:0u)); wr_u16(r,flags);
        wr_u8(r+0x62,(uint8_t)((scenario+i)%11==0?21:32));
        if((scenario+i)%11==1) wr_u16(r+6,0);
        wr_u8(r+0x38,(uint8_t)((scenario+i)%5==0?0xff:(scenario+i)%5==1?0x81:0x80));
        wr_u8(r+0x7a,(uint8_t)((scenario+i)%6));
    }
    wr_u16(CONTROL_RECORDS+6,(uint16_t)(scenario%13==0?0x8000:scenario%13==1?0x7fff:0));
    wr_u16(CONTROL_RECORDS+8,(uint16_t)(scenario%13==2?0x8000:scenario%13==3?0x7fff:0));
    for(i=0;i<2;++i) {
        uint32_t model=MODEL_BASE+i*MODEL_STRIDE;
        for(j=0;j<MODEL_BYTES;++j) wr_u8(model+j,(uint8_t)random_value());
        wr_u16(model,(uint16_t)(scenario%3==0?0x8000|(scenario&0xfff):scenario%3==1?0x4000:0));
        wr_u16(model+2,(uint16_t)(scenario&0xfff)); wr_u16(model+4,(uint16_t)((scenario+1)&0xfff));
        wr_u32(DESCRIPTOR_BASE+20*i,0xc1ed4c);
        wr_u32(DESCRIPTOR_BASE+20*i+4,model);
        wr_u32(DESCRIPTOR_BASE+20*i+8,model+0x100);
        wr_u32(DESCRIPTOR_BASE+20*i+12,model+0x200);
        wr_u32(DESCRIPTOR_BASE+20*i+16,0);
    }
    for(i=0;i<16;++i) for(j=0;j<5;++j)
        wr_u32(DESCRIPTOR_BANK+20*i+4*j,rd_u32(DESCRIPTOR_BASE+20*(i&1)+4*j));
    for(i=0;i<32;++i) {
        wr_u16(PARAMETER_BASE+2*i,(uint16_t)(0x600+10*i));
        for(j=0;j<5;++j) wr_u16(PARAMETER_BASE+0x600+10*i+2*j,
            scenario%19==0?0x8000:scenario%19==1?0x7fff:scenario%19==2?0:(uint16_t)random_value());
    }
    for(i=0;i<REGION_COUNT;++i) {
        uint32_t r=REGION_DATA+REGION_STRIDE*i;
        int inside=(scenario+i)%3!=0;
        wr_u32(REGION_DIRECTORY+4*i,r);
        wr_u16(r,inside?0x8000:1); wr_u16(r+2,0x7fff);
        wr_u16(r+4,0x8000); wr_u16(r+6,0x7fff);
        wr_u16(r+8,counts[(scenario+i)%5]);
        for(j=0;j<3;++j) {
            uint16_t slot=(uint16_t)((scenario/4+i+j)%16);
            uint16_t link=(uint16_t)(links[(scenario+i+j)%6]|slot);
            uint16_t selector=(uint16_t)((scenario+j)%5==0?0x140+20*((slot+1)%16):20*(j&1));
            uint16_t options=(uint16_t)(random_value()&0xf080u);
            if((scenario+i+j)%4==0) options|=(uint16_t)(((scenario+i)%15+1)<<8);
            wr_u16(r+10+10*j,selector);
            wr_u16(r+12+10*j,(uint16_t)((scenario+i+j)%4==0?0x10:(scenario+i+j)%4==1?0x20:0x30));
            wr_u16(r+14+10*j,link); wr_u16(r+16+10*j,(uint16_t)(2*((scenario+i+j)%32)));
            wr_u16(r+18+10*j,options);
        }
    }
    wr_u32(REGION_DIRECTORY+4*REGION_COUNT,UINT32_MAX);
}
static void region_fixture_group(RegionFixture *f,FA18NativeScenePointerGroup *g,unsigned source) {
    unsigned i; memset(g,0,sizeof *g); g->procedure=FA18_SCENE_PROCEDURE_RECORD_STREAM;
    for(i=0;i<3;++i) {
        g->data[i].segment=(uint16_t)(source+1); g->data[i].offset=i*0x100;
        g->data[i].data=(PortFieldWindow){.bytes=f->models[source],.byte_count=MODEL_BYTES,.origin=i*0x100};
        g->data[i].carried_value=rd_u32(DESCRIPTOR_BASE+20*source+4+4*i);
        g->data[i].carried_value_bound=1;
    }
}
static int region_fixture_load(RegionFixture *f) {
    unsigned i,j;
    memset(f,0,sizeof *f);
    if(!load_scene(&f->scene)) return 0;
    for(i=0;i<2;++i) {
        for(j=0;j<MODEL_BYTES;++j) f->models[i][j]=rd_u8(MODEL_BASE+MODEL_STRIDE*i+j);
        region_fixture_group(f,f->sources+i,i);
        f->descriptors[i]=(FA18NativeRegionDescriptor){.selector=(int16_t)(20*i),.group=f->sources+i};
    }
    for(i=0;i<16;++i) {
        f->groups[i]=f->sources[i&1];
        f->descriptors[2+i]=(FA18NativeRegionDescriptor){.selector=(int16_t)(0x140+20*i),.group=f->groups+i};
    }
    for(i=0;i<PARAMETER_BYTES;++i) f->parameters[i]=rd_u8(PARAMETER_BASE-PARAMETER_ORIGIN+i);
    for(i=0;i<TRIG_BYTES;++i) f->trig[i]=rd_u8(TRIG_BASE-TRIG_ORIGIN+i);
    for(i=0;i<REGION_COUNT;++i) {
        for(j=0;j<REGION_STRIDE;++j) f->rows[i][j]=rd_u8(REGION_DATA+REGION_STRIDE*i+j);
        f->directory[i]=(PortFieldWindow){.bytes=f->rows[i],.byte_count=REGION_STRIDE};
    }
    f->recorder=rd_u8(0xc45790); f->occupied=rd_u8(0xc4579d); f->mode=rd_u8(0xc458a6);
    f->admitted=rd_u8(0xc458aa); f->active=rd_u8(0xc458a9); f->reuse=rd_u8(0xc4582b);
    f->random=rd_u16(0xc45af8); f->jitter_x=rd_u16(0xc45b18); f->jitter_z=rd_u16(0xc45b1a);
    f->work.carried_axis=REG_D[4];
    f->parameter_window=(PortFieldWindow){.bytes=f->parameters,.byte_count=PARAMETER_BYTES,.origin=PARAMETER_ORIGIN};
    f->assets=(FA18NativeSceneRegionAssets){
        .parameters=&f->parameter_window,
        .directory=f->directory,.directory_count=REGION_COUNT+1,
        .descriptors=f->descriptors,.descriptor_count=18,
        .trig={.bytes=f->trig,.byte_count=TRIG_BYTES,.quarter_offset=TRIG_ORIGIN}};
    f->regions=(FA18NativeSceneRegions){.records=&f->scene.bank,.view_work=&f->work,.assets=&f->assets,
        .pointer_groups=f->groups,.pointer_group_count=16,
        .recorder_on=&f->recorder,.occupied=&f->occupied,.mode=&f->mode,.admitted=&f->admitted,
        .active=&f->active,.reuse_jitter=&f->reuse,.random_word=&f->random,
        .jitter_x=&f->jitter_x,.jitter_z=&f->jitter_z};
    return 1;
}
static int region_fixture_store(const RegionFixture *f) {
    unsigned i,j;
    store_scene(&f->scene);
    wr_u8(0xc45790,f->recorder); wr_u8(0xc4579d,f->occupied); wr_u8(0xc458a6,f->mode);
    wr_u8(0xc458aa,f->admitted); wr_u8(0xc458a9,f->active); wr_u8(0xc4582b,f->reuse);
    wr_u16(0xc45af8,f->random); wr_u16(0xc45b18,f->jitter_x); wr_u16(0xc45b1a,f->jitter_z);
    for(i=0;i<16;++i) {
        const FA18NativeScenePointerGroup *g=f->groups+i;
        unsigned source=g->data[0].segment-1;
        if(source>1 || g->procedure!=FA18_SCENE_PROCEDURE_RECORD_STREAM ||
           g->data[1].carried_value!=f->sources[source].data[1].carried_value) return 0;
        wr_u32(DESCRIPTOR_BANK+20*i,0xc1ed4c);
        for(j=0;j<4;++j) {
            if(j<3 && (g->data[j].segment!=source+1 || g->data[j].offset!=j*0x100)) return 0;
            if(j==3 && g->data[j].data.bytes) return 0;
            wr_u32(DESCRIPTOR_BANK+20*i+4+4*j,j<3?MODEL_BASE+MODEL_STRIDE*source+j*0x100:0);
        }
    }
    return 1;
}
static int region_verify(const RegionFixture *f) {
    unsigned i;
    if(!verify_record_owners(&f->scene) || f->occupied!=rd_u8(0xc4579d) ||
       f->active!=rd_u8(0xc458a9) || f->jitter_x!=rd_u16(0xc45b18) || f->jitter_z!=rd_u16(0xc45b1a)) return 0;
    for(i=0;i<16;++i) {
        const FA18NativeScenePointerGroup *g=f->groups+i;
        unsigned source=g->data[0].segment-1;
        if(source>1 || rd_u32(DESCRIPTOR_BANK+20*i+4)!=MODEL_BASE+source*MODEL_STRIDE ||
           g->data[1].carried_value!=rd_u32(DESCRIPTOR_BANK+20*i+8)) return 0;
    }
    return 1;
}
int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0; char error[256];
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
    RegionFixture *f=malloc(sizeof *f);
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):8192,entry,scenario,i,j;
    static const uint32_t entries[]={0xc28996,0xc28b16,0xc28b34,0xc28f16,0xc2d954};
    if(!state || !rom || !m || !base || !before || !expected || !f || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) return 1;
    free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
    if(!region_original_assets()) { fprintf(stderr,"original region asset binding differs\n"); return 1; }
    for(entry=0;entry<5;++entry) for(scenario=0;scenario<cases;++scenario) {
        uint32_t index,axis,source_index;
        uint16_t counter=2,angles[3],coordinates[4];
        uint32_t altitude; unsigned slot=scenario%16; int ok;
        memcpy(m,base,sizeof *m); selected_entry=entries[entry];
        scene_fixture(scenario); region_machine_fixture(scenario);
        REG_A[0]=REGION_DIRECTORY; REG_A[3]=REGION_DIRECTORY; REG_A[2]=REGION_DATA+10;
        REG_A[1]=CONTROL_RECORDS+512*slot;
        if(entry==3) REG_A[0]=REG_A[1];
        REG_D[0]=(REG_D[0]&0xffff0000u)|counter;
        REG_D[7]=scenario%3==0?UINT32_MAX:scenario&255u;
        source_index=REG_D[7]; index=source_index;
        for(i=0;i<4;++i) coordinates[i]=(uint16_t)REG_D[2+i];
        altitude=REG_D[6];
        for(i=0;i<3;++i) {
            static const uint16_t boundary[]={0,1,0x7080,0x7081,0x8000,0xffff,0x1234,0xfffe};
            angles[i]=scenario<32?boundary[(scenario/4+i)%8]:(uint16_t)random_value();
            if(scenario%13==i) angles[i]=0;
            if(entry==4) REG_D[4+i]=(REG_D[4+i]&0xffff0000u)|angles[i];
        }
        if(entry==4 && scenario%4==1) for(i=0;i<TRIG_BYTES;++i) wr_u8(TRIG_BASE-TRIG_ORIGIN+i,(uint8_t)random_value());
        if(entry==4 && scenario%4==2) for(i=0;i<TRIG_BYTES;i+=2) wr_u16(TRIG_BASE-TRIG_ORIGIN+i,0x8000);
        for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
            for(j=0;j<scene_source_bytes[i].length;++j)
                if(rd_u8(scene_source_bytes[i].pc+j)!=scene_source_bytes[i].bytes[j]) {
                    fprintf(stderr,"fixture changes source %06X\n",scene_source_bytes[i].pc); return 1;
                }
        if(!region_fixture_load(f)) return 1;
        memcpy(before,m,sizeof *m);
        if(!region_source()) { fprintf(stderr,"original region failed %06X case %u\n",selected_entry,scenario); return 1; }
        axis=REG_D[4]; source_index=REG_D[7];
        memcpy(expected,m->chip,FA18_CHIP_SIZE); memcpy(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m);
        if(entry==0) ok=fa18_update_native_scene_regions(&f->regions);
        else if(entry==1) ok=fa18_spawn_native_region_records(&f->regions,f->directory,&index);
        else if(entry==2) {
            PortFieldWindow rows=f->directory[0]; rows.origin=10;
            ok=fa18_dispatch_native_region_records(&f->regions,&rows,counter,&index);
        } else if(entry==3) ok=fa18_set_native_region_view(f->scene.bank.records+slot,coordinates,altitude);
        else ok=fa18_publish_native_record_orientation_with_axis(f->scene.bank.records+slot,angles,&f->assets.trig,&f->work.carried_axis);
        if(!ok) { fprintf(stderr,"native region failed %06X case %u\n",selected_entry,scenario); return 1; }
        if(f->work.carried_axis!=axis || ((entry==1 || entry==2) && index!=source_index)) {
            fprintf(stderr,"region work %06X case %u axis expected %08X got %08X index expected %08X got %08X\n",
                selected_entry,scenario,axis,f->work.carried_axis,source_index,index); return 1;
        }
        if(!region_fixture_store(f)) return 1;
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            uint32_t address=i<FA18_CHIP_SIZE?i:FA18_SLOW_BASE+i-FA18_CHIP_SIZE;
            if(address>=0xc7fd00 && address<0xc7ff00) continue;
            if(rd_u8(address)!=expected[i]) {
                fprintf(stderr,"region %06X case %u byte %06X source %02X native %02X\n",selected_entry,scenario,address,expected[i],rd_u8(address)); return 1;
            }
        }
        memcpy(m->chip,expected,FA18_CHIP_SIZE); memcpy(m->slow,expected+FA18_CHIP_SIZE,FA18_SLOW_SIZE);
        if(!region_verify(f)) { fprintf(stderr,"independent region owner differs\n"); return 1; }
    }
    printf("native regions: %u complete calls match full game RAM, typed descriptors/records, carried axis and mutable region order; no child contracts\n",cases*5);
    printf("visited:"); for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
        if(scene_seen[i]) printf(" %06X",scene_source_bytes[i].pc);
    puts(""); free(f); free(expected); free(before); free(base); free(m); return 0;
}
