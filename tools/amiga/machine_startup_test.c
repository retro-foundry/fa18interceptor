/* Machine-constructor proof only. These synthetic instructions are fixtures,
 * not game startup, OS initialization or whole-game ROM-free acceptance. */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "startup.h"
#include "m68kcpu.h"
#include "recomp_runtime.h"
#include "recomp_ports.h"
static int cleared(const uint8_t *bytes,size_t size) {
    for (size_t i=0;i<size;++i) if (bytes[i]) return 0;
    return 1;
}
static void registers_match(const FA18MachineStartup *s) {
    assert(m68k_get_reg(NULL,M68K_REG_PC)==s->pc);
    assert(m68k_get_reg(NULL,M68K_REG_SR)==s->sr);
    assert(m68k_get_reg(NULL,M68K_REG_USP)==s->usp);
    assert(m68k_get_reg(NULL,M68K_REG_ISP)==s->isp);
    assert(m68k_get_reg(NULL,M68K_REG_SP)==(s->sr&0x2000?s->isp:s->usp));
    for (unsigned i=0;i<8;++i) assert(m68k_get_reg(NULL,(m68k_register_t)(M68K_REG_D0+i))==s->d[i]);
    for (unsigned i=0;i<7;++i) assert(m68k_get_reg(NULL,(m68k_register_t)(M68K_REG_A0+i))==s->a[i]);
}
int main(void) {
    FA18Machine *m=malloc(sizeof *m),*before=malloc(sizeof *before);
    FA18MachineStartup s={0}; char error[256];
    assert(m && before);
    s.pc=0xC7F000; s.usp=0xC7FC00; s.isp=0xC7FE00; s.sr=0x2700;
    s.vpos=FA18_FRAME_END_LINE;
    for (unsigned i=0;i<8;++i) s.d[i]=0x11220000+i;
    for (unsigned i=0;i<7;++i) s.a[i]=0xC60000+0x100*i;
    for (unsigned mode=0;mode<2;++mode) {
        memset(m,0xA5,sizeof *m); s.sr=mode?0x001F:0x2700;
        assert(fa18_machine_init(m,&s,error,sizeof error)); registers_match(&s);
        assert(cleared(m->chip,sizeof m->chip) && cleared(m->slow,sizeof m->slow));
        assert(cleared(m->rom,sizeof m->rom) && cleared(m->rtarea,sizeof m->rtarea));
        assert(m->runtime_guard.enabled && !m->cycle && !m->frame);
        assert(!m->runtime_guard.rom_reads && !m->runtime_guard.rom_instruction_fetches && !m->runtime_guard.unsupported_services);
        fa18_recomp_init(0); fa18_ports_init(FA18_PORTS_OFF,NULL);
        assert(fa18_machine_prepare_run(m,error,sizeof error));
        /* BRA to itself stays outside all game translation spans. */
        m->slow[0x7F000]=0x60; m->slow[0x7F001]=0xFE;
        fa18_machine_run_frame(m); fa18_machine_run_frame(m);
        assert(m->frame==2 && m->cycle>0 && m68k_get_reg(NULL,M68K_REG_PC)==s.pc);
        assert(!m->runtime_guard.rom_reads && !m->runtime_guard.rom_instruction_fetches && !m->runtime_guard.unsupported_services);
        assert(!fa18_machine_prepare_run(m,error,sizeof error));
    }
    memcpy(before,m,sizeof *m);
    uint32_t pc=m68k_get_reg(NULL,M68K_REG_PC);
    FA18MachineStartup bad=s; bad.pc=0xFC0000;
    assert(!fa18_machine_init(m,&bad,error,sizeof error));
    assert(!memcmp(before,m,sizeof *m) && m68k_get_reg(NULL,M68K_REG_PC)==pc);
    bad=s; bad.vpos=FA18_PAL_LINES;
    assert(!fa18_machine_init(m,&bad,error,sizeof error)); assert(!memcmp(before,m,sizeof *m));
    bad=s; bad.isp=0xC80002;
    assert(!fa18_machine_init(m,&bad,error,sizeof error)); assert(!memcmp(before,m,sizeof *m));
    assert(!fa18_machine_init(m,NULL,error,sizeof error)); assert(!memcmp(before,m,sizeof *m));
    memcpy(m->chip,&s,sizeof s); memcpy(before,m,sizeof *m);
    assert(!fa18_machine_init(m,(const FA18MachineStartup *)m->chip,error,sizeof error));
    assert(!memcmp(before,m,sizeof *m));
    /* Prime the persistent B holding register, then prove that a newly
     * constructed machine starts clear when a blit uses B without fetching it. */
    assert(fa18_machine_init(m,&s,error,sizeof error));
    fa18_recomp_init(0); fa18_ports_init(FA18_PORTS_OFF,NULL);
    m->custom[0x040/2]=0x05CC; m->custom[0x044/2]=m->custom[0x046/2]=0xFFFF;
    m->custom[0x04E/2]=0x0200; m->custom[0x056/2]=0x0100;
    m->chip[0x200]=m->chip[0x201]=0xFF;
    fa18_custom_write(m,0x096,0x8240); fa18_custom_write(m,0x058,0x0041);
    assert(fa18_chip16(m,0x100)==0xFFFF);
    assert(fa18_machine_init(m,&s,error,sizeof error));
    m->custom[0x040/2]=0x01CC; m->custom[0x044/2]=m->custom[0x046/2]=0xFFFF;
    m->custom[0x056/2]=0x0100;
    m->chip[0x100]=m->chip[0x101]=0xFF;
    fa18_custom_write(m,0x096,0x8240); fa18_custom_write(m,0x058,0x0041);
    assert(fa18_chip16(m,0x100)==0);
    puts("clean machine init: cleared banks, exact CPU handoff, repeated user/supervisor initialization and live frames; zero ROM reads/fetches/unsupported services");
    free(before); free(m); return 0;
}
