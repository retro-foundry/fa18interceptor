/* CPU, ROM and captured RAM are used only for this differential proof. */
#define FA18_SCENE_SOURCE_HEADER "../../build/recomp/native_record_action_placement_source.h"
#define FA18_SCENE_PLAYER_HELPERS_ONLY
#include "native_scene_player_oracle.c"
#include "../../port/native_record_action_placement.c"
#include "../../port/native_scene_placement.c"
#include "../../port/native_record_orientation.c"
#include "../../port/two_angle_matrix.c"
#include "../../port/flight.c"
#include "../../port/disk.h"

static int verify_action_assets(void) {
    FA18Disk disk; FA18Hunks hunks; FA18NativeRecordActionPlacementAssets assets;
    size_t size=0; uint8_t *image; unsigned row,i;
    if(!fa18_disk_open(&disk,"FA-18 Interceptor (1988)(Electronic Arts)[cr A-Ha].adf")) return 0;
    image=fa18_disk_read(&disk,"F-18 Interceptor",&size);
    if(!image || !fa18_hunks_load(&hunks,image,size) ||
       !fa18_load_native_record_action_placement_assets(&hunks,&assets)) return 0;
    for(i=0;i<0x4e;++i) if(assets.offsets_10.bytes[i]!=rd_u8(0xc23a32+i)) { fprintf(stderr,"asset byte %u disk %02X RAM %02X\n",i,assets.offsets_10.bytes[i],rd_u8(0xc23a32+i)); return 0; }
    for(row=0;row<2;++row) {
        FA18NativeScenePointerGroup *g=row?&assets.alternate:&assets.primary;
        uint32_t offsets[3]={row?0xba2:0xa6c,row?0xba2:0xa6c,row?0xba0:0xa6a};
        if(g->procedure!=FA18_SCENE_PROCEDURE_RECORD_STREAM || g->data[3].data.bytes) return 0;
        for(i=0;i<3;++i) {
            const FA18NativeAssetReference *r=g->data+i;
            if(r->segment!=51 || r->offset!=offsets[i] || r->data.bytes!=hunks.segments[51].data ||
               r->data.origin!=offsets[i] || rd_u32(SCENE_POINTER_TABLE+20*row+4+4*i)!=0xc383f0+offsets[i]) { fprintf(stderr,"asset ref row %u field %u segment %u offset %X source %X\n",row,i,r->segment,r->offset,rd_u32(SCENE_POINTER_TABLE+20*row+4+4*i)); return 0; }
            if(r->data.bytes[r->data.origin]!=rd_u8(0xc383f0+offsets[i])) return 0;
        }
    }
    { int16_t value;
      if(port_field_window_s16(&assets.offsets_other,42,&value)) return 0; }
    puts("action assets: 78 original Hunk-17 table/adjacent bytes and six Hunk-51 references match; adjacent relocated operands require explicit owners");
    fa18_hunks_free(&hunks); free(image); fa18_disk_close(&disk); return 1;
}

static int original_action(void) {
    unsigned step,i;
    for(step=0;step<1000;++step) {
        uint32_t pc=REG_PC; uint16_t opcode;
        if(pc==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
            if(scene_source_bytes[i].pc==pc) break;
        if(i==sizeof scene_source_bytes/sizeof scene_source_bytes[0]) {
            fprintf(stderr,"unexpected action PC %06X\n",pc); return 0;
        }
        scene_seen[i]=1; opcode=rd_u16(pc); REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
        m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    return 0;
}
/* Fixture handles map to sealed source descriptors only in the oracle. Native
 * assets contain ordinary references; no guest pointer is a runtime API. */
static void fixture_group(FA18NativeScenePointerGroup *g,unsigned row) {
    unsigned i; memset(g,0,sizeof *g); g->procedure=FA18_SCENE_PROCEDURE_RECORD_STREAM;
    for(i=0;i<4;++i) if(rd_u32(SCENE_POINTER_TABLE+20*row+4+4*i)) {
        g->data[i].segment=(uint16_t)(row+1); g->data[i].offset=i+1;
    }
}
static int export_group(unsigned slot,const FA18NativeScenePointerGroup *g) {
    unsigned row,i;
    if(g->procedure==FA18_SCENE_PROCEDURE_NONE) return 1;
    if(g->procedure!=FA18_SCENE_PROCEDURE_RECORD_STREAM) return 0;
    row=g->data[0].segment-1;
    if(row>1) return 0;
    wr_u32(SCENE_POINTERS+20*slot,rd_u32(SCENE_POINTER_TABLE+20*row));
    for(i=0;i<4;++i) {
        uint32_t value=rd_u32(SCENE_POINTER_TABLE+20*row+4+4*i);
        if(value && (g->data[i].segment!=row+1 || g->data[i].offset!=i+1)) return 0;
        if(!value && (g->data[i].segment || g->data[i].offset)) return 0;
        wr_u32(SCENE_POINTERS+20*slot+4+4*i,value);
    }
    return 1;
}
int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0; char error[256];
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE),tables[4096];
    SceneState *native=malloc(sizeof *native);
    FA18NativeRecordActionPlacementAssets assets; FA18NativeScenePointerGroup groups[16];
    FA18NativeRecordViewWork work; FA18NativeRecordActionPlacement placement;
    uint16_t slot_word,viewed,selector,primary,alternate;
    PortFieldByte viewed_fields[2];
    uint8_t space,pending,enable,limit,divisor,alert,fire,changed,redraw;
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):8192,entry,scenario,i,j;
    unsigned aliased=0,launched=0,inactive=0; uint32_t carried;
    static const uint8_t limits[]={0,1,2,3,0x7f,0x80,0xff};
    if(!state || !rom || !m || !base || !before || !expected || !native || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) return 1;
    free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
    if(!verify_action_assets()) { fprintf(stderr,"action asset binding differs\n"); return 1; }
    for(entry=0;entry<2;++entry) for(scenario=0;scenario<cases;++scenario) {
        unsigned slot=(scenario/32)%16,companion=scenario%5==0?slot:scenario%3==0?0:(scenario/7)%16;
        uint32_t r=CONTROL_RECORDS+slot*512,c=CONTROL_RECORDS+companion*512;
        memcpy(m,base,sizeof *m); selected_entry=entry?0xc2377e:0xc2374c;
        scene_fixture(scenario); REG_A[1]=r; REG_A[2]=c;
        wr_u32(MODE_TABLE,0xc60000); wr_u16(0xc6003e,(uint16_t)random_value());
        wr_u16(0xc60042,(uint16_t)random_value());
        wr_u8(c+0x63,(uint8_t)((scenario%3==0?0x30:0x20)|(scenario&15)));
        wr_u8(c+0x62,(uint8_t)(scenario%4==0?0x10:scenario%4==1?0x30:0x12));
        wr_u8(c+0x5f,(uint8_t)scenario);
        wr_u16(STREAM_MODE,(uint16_t)slot);
        wr_u16(VIEW_RECORD,(uint16_t)((scenario&1)?companion*512:(companion^1)*512));
        wr_u8(0xc45789,(uint8_t)(scenario%11!=0));
        wr_u8(0xc458a7,limits[scenario%7]); wr_u8(0xc458b5,(uint8_t)(scenario%3==0));
        wr_u16(0xc458da,scenario%4==0?(uint16_t)random_value():0);
        if(scenario%13==0) for(i=0;i<9;++i) wr_u16(c+0x92+2*i,(uint16_t)((i&1)?0x8000:0x7fff));
        if(scenario%17==0) for(i=0;i<3;++i) wr_u32(c+0x14+4*i,0x7fffffff);
        /* Reject every accidental modification of an audited instruction. */
        for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
            for(j=0;j<scene_source_bytes[i].length;++j)
                if(rd_u8(scene_source_bytes[i].pc+j)!=scene_source_bytes[i].bytes[j]) return 1;
        if(!load_scene(native) || !verify_record_owners(native)) return 1;
        memset(&assets,0,sizeof assets); memset(groups,0,sizeof groups);
        fixture_group(&assets.primary,0); fixture_group(&assets.alternate,1);
        for(i=0;i<sizeof tables;++i) tables[i]=rd_u8(0xc23a32-2048+i);
        assets.offsets_10=(PortFieldWindow){.bytes=tables,.byte_count=sizeof tables,.origin=2048};
        assets.offsets_other=(PortFieldWindow){.bytes=tables,.byte_count=sizeof tables,.origin=2048+0x24};
        slot_word=rd_u16(STREAM_MODE); viewed=rd_u16(VIEW_RECORD); selector=rd_u16(0xc458da);
        alternate=rd_u16(0xc6003e); primary=rd_u16(0xc60042);
        space=rd_u8(SPACE_COMMAND_LATCH); pending=rd_u8(FIRE_RECORD_PENDING);
        enable=rd_u8(0xc45789); limit=rd_u8(0xc458a7); divisor=rd_u8(0xc458b5);
        alert=rd_u8(FIRE_ALERT_COUNTDOWN); fire=rd_u8(FIRE_STATE);
        changed=rd_u8(MODE_TABLE_CHANGED); redraw=rd_u8(UPDATE_MASK);
        work=(FA18NativeRecordViewWork){.carried_axis=REG_D[4]};
        viewed_fields[0]=(PortFieldByte){.unsigned_word=&viewed,.shift=8};
        viewed_fields[1]=(PortFieldByte){.unsigned_word=&viewed};
        placement=(FA18NativeRecordActionPlacement){.records=&native->bank,.assets=&assets,
            .pointer_groups=groups,.pointer_group_count=16,.view_work=&work,
            .current_slot=&slot_word,.viewed_record=viewed_fields,.selector_word=&selector,
            .primary_count=&primary,.alternate_count=&alternate,
            .warning_causes=&native->warnings,.events=&native->events,.space_latch=&space,
            .fire_pending=&pending,.secondary_enable=&enable,.limit=&limit,.divisor=&divisor,
            .alert_countdown=&alert,.fire_state=&fire,.mode_changed=&changed,
            .stores_redraw_a=&native->flight.weapon_mode_redraws,.stores_redraw_b=&native->flight.weapon_redraws,
            .scene_redraw=&redraw};
        memcpy(before,m,sizeof *m);
        if(!original_action()) { fprintf(stderr,"source placement failed %06X case %u\n",selected_entry,scenario); return 1; }
        carried=REG_D[4];
        memcpy(expected,m->chip,FA18_CHIP_SIZE); memcpy(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m);
        if(!(entry?fa18_place_native_secondary_record(&placement,slot,companion):
                   fa18_place_native_primary_record(&placement,slot,companion))) return 1;
        if(work.carried_axis!=carried) {
            fprintf(stderr,"carried axis %06X case %u expected %08X got %08X\n",selected_entry,scenario,carried,work.carried_axis); return 1;
        }
        store_scene(native); wr_u16(0xc6003e,alternate); wr_u16(0xc60042,primary);
        wr_u8(SPACE_COMMAND_LATCH,space); wr_u8(FIRE_RECORD_PENDING,pending);
        wr_u8(FIRE_ALERT_COUNTDOWN,alert); wr_u8(FIRE_STATE,fire); wr_u8(MODE_TABLE_CHANGED,changed);
        wr_u8(UPDATE_MASK,redraw);
        for(i=0;i<16;++i) if(!export_group(i,groups+i)) return 1;
        if(groups[slot].procedure) { ++launched; if(slot==companion) ++aliased; }
        else ++inactive;
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            uint32_t a=i<FA18_CHIP_SIZE?i:FA18_SLOW_BASE+i-FA18_CHIP_SIZE;
            if(a>=0xc7fd00 && a<0xc7ff00) continue;
            if(rd_u8(a)!=expected[i]) {
                fprintf(stderr,"placement %06X case %u slot %u companion %u byte %06X source %02X native %02X\n",
                    selected_entry,scenario,slot,companion,a,expected[i],rd_u8(a)); return 1;
            }
        }
        if(!verify_record_owners(native)) return 1;
    }
    printf("native primary/secondary placement: %u complete calls match all game RAM, typed records/descriptors and carried axis; %u launches (%u aliased), %u inactive; no child contracts\n",
        cases*2,launched,aliased,inactive);
    printf("visited:"); for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
        if(scene_seen[i]) printf(" %06X",scene_source_bytes[i].pc);
    puts(""); free(native); free(expected); free(before); free(base); free(m); return 0;
}
