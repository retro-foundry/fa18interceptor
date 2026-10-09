/* Validation only: actual independent qualification message states, no
 * fabricated variants. Native C11B44/C11BFC/C322EE owners vs original code. */
#define FA18_HUD_ORACLE_LIBRARY
#include "native_hud_oracle.c"

int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0;char error[256];
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=file_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=argc==2?file_bytes(argv[1],&nd):NULL;
    FA18Machine *machine=calloc(1,sizeof *machine);
    FA18Machine *before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    if(!state || !rom || !data || nd!=0x100000 || !machine || !before || !expected ||
       !fa18_os_wait_blit_signature_matches(rom)) return 1;
    if(!fa18_machine_load_state(machine,state,ns,rom,nr,error,sizeof error)) {
        fputs(error,stderr);return 1;
    }
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;
    static const struct {uint32_t entry;void (*host)(void);} owners[]={
        {0xc11b44,tick_notification_cadence},
        {0xc11bfc,update_message},
        {0xc322ee,check_message_return}
    };
    for(unsigned owner=0;owner<sizeof owners/sizeof owners[0];++owner) {
        /* Each owner gets the unmodified observed state. This is component
         * validation, not a fabricated full-frame execution order. */
        memcpy(machine->chip,data,0x80000);memcpy(machine->slow,data+0x80000,0x80000);
        memcpy(before,machine,sizeof *before);
        checked_return=(NativeInputReturn){0};
        owners[owner].host();
        memcpy(expected,machine->chip,0x80000);memcpy(expected+0x80000,machine->slow,0x80000);
        memcpy(machine,before,sizeof *before);
        if(!hud_original(owners[owner].entry)) return 1;
        if(checked_return.owner!=NATIVE_INPUT_RETURN_UNKNOWN && checked_return.value!=(uint8_t)REG_D[4]) {
            fprintf(stderr,"Message %06X returned original %02X/native %02X\n",
                owners[owner].entry,(uint8_t)REG_D[4],checked_return.value);return 1;
        }
        unsigned differences=0;
        for(unsigned i=0;i<0xffc00;++i) {
            const uint8_t actual=i<0x80000?machine->chip[i]:machine->slow[i-0x80000];
            if(actual!=expected[i]) {
                if(differences<8) fprintf(stderr,"Message %06X at %06X: original %02X/native %02X\n",
                    owners[owner].entry,i<0x80000?i:0xc00000+i-0x80000,actual,expected[i]);
                ++differences;
            }
        }
        if(differences) {fprintf(stderr,"%u non-stack byte differences\n",differences);return 1;}
    }
    puts("Three observed-state message owners match original non-stack RAM and defined text return");
    free(expected);free(before);free(machine);free(data);free(rom);free(state);
    return 0;
}
