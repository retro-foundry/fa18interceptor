/* Resolve negative terrain visibility indices from the original loaded image. */
#define main records_fixture_main
#include "native_records_oracle.c"
#undef main
#include "map_detail_fields.h"
#include "map_packet_static_data.h"

static int original_fields(const FA18MapDetailFieldsInput *input) {
    memset(REG_DA,0,sizeof REG_DA);REG_A[6]=0x4800;
    REG_D[0]=(uint32_t)input->coordinate_x;REG_D[1]=(uint32_t)input->coordinate_y;
    REG_D[3]=(uint32_t)input->offset_x;REG_D[4]=(uint32_t)input->offset_y;
    if(!input->alternate_layout) {REG_D[3]<<=4;REG_D[4]<<=4;}
    wr_u16(REG_A[6]-0x3e,input->alternate_layout);
    wr_u16(REG_A[6]-0x22,input->visibility_gate);
    wr_u8(REG_A[6]-0x24,input->force_visible);
    wr_u32(REG_A[6]-0x28,(uint32_t)input->detail_metric);
    wr_u8(ZOOM_FLAGS,input->zoom_endpoint);wr_u16(ZOOM_SCALE,(uint16_t)input->zoom_scale);
    m68k_set_reg(M68K_REG_SR,0x2700);REG_PC=0xc2ae66;
    fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
    for(unsigned steps=0;steps<200;++steps) {
        if(REG_PC==0xc2aef8u) return 1;
        uint16_t opcode=rd_u16(REG_PC);REG_PPC=REG_PC;REG_IR=opcode;REG_PC+=2;
        m68ki_instruction_jump_table[opcode]();USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    fprintf(stderr,"map fields did not finish at %06X\n",REG_PC);return 0;
}
int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0;char error[256];
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=file_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=argc==2?file_bytes(argv[1],&nd):NULL;
    FA18Machine *m=calloc(1,sizeof *m);
    if(!state || !rom || !data || nd!=0x100000 || !m) return 1;
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) {fputs(error,stderr);return 1;}
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;
    FA18MapPacketStaticData image={m->slow+FA18_MAP_PACKET_CONTROL_RUNTIME_BASE-0xc00000u,
        0xc80000u-FA18_MAP_PACKET_CONTROL_RUNTIME_BASE,NULL,0};
    static const int32_t metrics[]={-6,-1,-256,-257,-0x1234,-0x20000,-0x800000,
                                  0,1,0x1100,0x1200,0x7fffffff};
    static const int16_t zooms[]={-1,0,1,128};
    for(unsigned variant=0;variant<384;++variant) {
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        FA18MapDetailFieldsInput input={0};
        input.alternate_layout=(uint8_t)(variant&1);
        input.force_visible=(uint8_t)((variant&16)?1:0);
        input.visibility_gate=(variant&8)?0:1;
        input.detail_metric=metrics[(variant/32)%12];input.zoom_endpoint=(uint8_t)((variant>>2)&1);
        input.zoom_scale=zooms[(variant>>1)%4];
        input.coordinate_x=(int32_t)(0xfedcba98u+variant*12345u);
        input.coordinate_y=(int32_t)(0x80000001u-variant*98765u);
        if(variant%19==0) input.coordinate_x=0x01800000; /* Far X with a zero selector word. */
        input.offset_x=(int32_t)(variant&1?0xff000000u:0x01000000u);
        input.offset_y=(int32_t)(variant&2?0xfe000000u:0x02000000u);
        input.resolve_visibility_limit=fa18_resolve_map_packet_visibility_limit;
        input.visibility_context=&image;
        if(!original_fields(&input)) return 1;
        FA18MapDetailFieldsResult actual;
        if(fa18_apply_map_detail_fields(&input,&actual) || (uint32_t)actual.coordinate_x!=REG_D[0] ||
           (uint32_t)actual.coordinate_y!=REG_D[1] || actual.visible!=(uint16_t)REG_D[7] ||
           actual.visibility_limit_written!=(!input.force_visible && !!input.visibility_gate) ||
           (actual.visibility_limit_written && actual.visibility_limit_register!=REG_D[6])) {
            fprintf(stderr,"map detail case %u metric=%d limit=%08X/%08X differs\n",
                variant,input.detail_metric,actual.visibility_limit_register,REG_D[6]);
            fprintf(stderr,"x=%08X/%08X y=%08X/%08X visible=%04X/%04X\n",(unsigned)actual.coordinate_x,REG_D[0],(unsigned)actual.coordinate_y,REG_D[1],actual.visible,(unsigned)(uint16_t)REG_D[7]);return 1;
        }
    }
    puts("384 terrain visibility cases match original C2AE66-C2AEF8, including negative image indices");
    free(m);free(data);free(state);free(rom);return 0;
}
