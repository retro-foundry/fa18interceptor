/* Reference only: C30764 on captured original / reconstructed native inputs.
 * Native inputs come from a complete body compared with the playable runner;
 * original inputs come directly from the actual C30764 call boundary. */
#define FA18_HUD_ORACLE_LIBRARY
#include "native_hud_oracle.c"

int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0;char error[256];
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=file_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=argc==3?file_bytes(argv[1],&nd):NULL;
    FA18Machine *machine=calloc(1,sizeof *machine),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    if(!state || !rom || !data || nd!=0x100000 || !machine || !before || !expected ||
       !fa18_os_wait_blit_signature_matches(rom)) return 1;
    if(!fa18_machine_load_state(machine,state,ns,rom,nr,error,sizeof error)) return 1;
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;
    memcpy(machine->chip,data,0x80000);memcpy(machine->slow,data+0x80000,0x80000);
    memcpy(before,machine,sizeof *before);
    host_draw_panel_frame();
    memcpy(expected,machine->chip,0x80000);memcpy(expected+0x80000,machine->slow,0x80000);
    memcpy(machine,before,sizeof *before);
    if(!hud_original(0xc30764u)) return 1;
    for(unsigned i=0;i<0xffc00;++i) {
        uint8_t actual=i<0x80000?machine->chip[i]:machine->slow[i-0x80000];
        if(actual!=expected[i]) {
            fprintf(stderr,"Panel owner RAM %06X original=%02X native=%02X\n",
                i<0x80000?i:0xc00000+i-0x80000,actual,expected[i]);return 1;
        }
    }
    FILE *out=fopen(argv[2],"wb");
    if(!out || fwrite(expected,1,0x100000,out)!=0x100000 || fclose(out)) return 1;
    puts("{\"complete_non_stack_ram_matching\":true}");
    free(expected);free(before);free(machine);free(data);free(rom);free(state);
    return 0;
}
