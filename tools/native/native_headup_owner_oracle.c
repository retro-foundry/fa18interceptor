/* External comparison of C332BC on actual radar-return RAM. This executes
 * the native head-up owner and original instructions separately; no state
 * or replacement pixels are supplied to the playable game. */
#define FA18_HUD_ORACLE_LIBRARY
#include "native_hud_oracle.c"

static gaddr headup_plane(unsigned plane) {
    unsigned draw=rd_u16(0xc4566cu);
    if(draw>1 || rd_u32(PAGE_PLANE_TABLE)!=0xc4566eu+16*draw) abort();
    gaddr address=rd_u32(0xc4566eu+16*(draw^(plane/4))+4*(plane%4));
    if(address>0x80000u-8000) abort();
    return address;
}
static int compare_headup(FA18Machine *machine,FA18Machine *before,uint8_t *expected) {
    memcpy(before,machine,sizeof *before);
    host_draw_postflight_hud();
    memcpy(expected,machine->chip,0x80000);memcpy(expected+0x80000,machine->slow,0x80000);
    memcpy(machine,before,sizeof *before);
    if(!hud_original(0xc332bcu)) return 0;
    for(unsigned i=0;i<0xffc00;++i) {
        const uint8_t actual=i<0x80000?machine->chip[i]:machine->slow[i-0x80000];
        if(actual!=expected[i]) {
            fprintf(stderr,"Head-up owner RAM %06X original=%02X native=%02X\n",
                i<0x80000?i:0xc00000+i-0x80000,actual,expected[i]);return 0;
        }
    }
    return 1;
}
static uint8_t probe_byte(unsigned plane,unsigned byte) {
    return (uint8_t)((plane*37u+byte*29u)^(byte>>3));
}

int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0;char error[256];
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=file_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=(argc==3 || argc==4)?file_bytes(argv[1],&nd):NULL;
    FA18Machine *machine=calloc(1,sizeof *machine),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    if(!state || !rom || !data || nd!=0x100000 || !machine || !before || !expected ||
       !fa18_os_wait_blit_signature_matches(rom)) return 1;
    if(!fa18_machine_load_state(machine,state,ns,rom,nr,error,sizeof error)) {
        fputs(error,stderr);return 1;
    }
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;
    memcpy(machine->chip,data,0x80000);memcpy(machine->slow,data+0x80000,0x80000);
    const uint32_t marker=rd_u32(SELECTION_MARKER),projected=rd_u32(POSTFLIGHT_MARK);
    const int16_t range=rd_s16(RANGE_RATE),previous_range=rd_s16(POSTFLIGHT_RANGE_LAST);
    if(!compare_headup(machine,before,expected)) return 1;
    const uint32_t marker_after=rd_u32(SELECTION_MARKER),projected_after=rd_u32(POSTFLIGHT_MARK);
    const int16_t range_after=rd_s16(RANGE_RATE),previous_range_after=rd_s16(POSTFLIGHT_RANGE_LAST);
    FILE *out=fopen(argv[2],"wb");
    if(!out || fwrite(expected,1,0x100000,out)!=0x100000 || fclose(out)) return 1;
    if(argc==4) {
        /* Infer the exact per-bit set/clear/preserve/invert operation from
         * zero and one pages. Both variants execute original instructions.
         * Actual pages and a spatially mixed third probe must satisfy that
         * complete transfer, so background-dependent writes cannot silently
         * disappear from an independent drawing-history prediction. */
        uint8_t *transfer=malloc(128000),*actual_pages=malloc(64000);
        if(!transfer || !actual_pages) return 1;
        for(unsigned plane=0;plane<8;++plane)
            memcpy(actual_pages+8000*plane,expected+headup_plane(plane),8000);
        for(unsigned variant=0;variant<3;++variant) {
            memcpy(machine->chip,data,0x80000);memcpy(machine->slow,data+0x80000,0x80000);
            for(unsigned plane=0;plane<8;++plane) {
                const gaddr address=headup_plane(plane);
                for(unsigned byte=0;byte<8000;++byte)
                    machine->chip[address+byte]=variant<2?(uint8_t)(variant?255:0):probe_byte(plane,byte);
            }
            if(!compare_headup(machine,before,expected)) return 1;
            for(unsigned plane=0;plane<8;++plane) {
                const gaddr address=headup_plane(plane);
                if(variant<2) memcpy(transfer+64000*variant+8000*plane,expected+address,8000);
                else for(unsigned byte=0;byte<8000;++byte) {
                    unsigned offset=8000*plane+byte;
                    const uint8_t zero=transfer[offset],dependent=zero^transfer[64000+offset];
                    if(expected[address+byte]!=(uint8_t)(zero^(dependent&probe_byte(plane,byte))) ||
                       actual_pages[offset]!=(uint8_t)(zero^(dependent&data[address+byte]))) {
                        fprintf(stderr,"Head-up bit transfer differs at plane %u byte %u\n",plane,byte);return 1;
                    }
                }
            }
        }
        out=fopen(argv[3],"wb");
        if(!out || fwrite(transfer,1,128000,out)!=128000 || fclose(out)) return 1;
        free(actual_pages);free(transfer);
    }
    printf("{\"complete_non_stack_ram_matching\":true,\"selection_marker_before\":%u,"
        "\"selection_marker_after\":%u,\"projected_mark_before\":%u,\"projected_mark_after\":%u,"
        "\"range_rate_before\":%d,\"range_rate_after\":%d,"
        "\"previous_range_before\":%d,\"previous_range_after\":%d,\"complete_bit_transfer_matching\":%s}\n",
        marker,marker_after,projected,projected_after,range,range_after,previous_range,previous_range_after,argc==4?"true":"false");
    free(expected);free(before);free(machine);free(data);free(rom);free(state);
    return 0;
}
