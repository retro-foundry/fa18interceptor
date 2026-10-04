/* Independent source inputs: matrix/record edge words, stack shifts, gun cue
 * transitions, closure-rate cache flags, and the conditional glyph tail. */
extern void fa18_hud_machine_restore(void);
static uint32_t expected_sp;
static void fixture(unsigned p) {
    static const uint16_t words[]={0,1,0xffff,0x8000,0x7fff,8,9,127,128,159,255,256,0x600,0x900,0x7f00,0x7f01};
    static const uint16_t shifts[]={0,1,8,15,16,31,32,63,64,65,0xffff,0x8000,2,4,7,14};
    static const uint16_t columns[]={0,1,0xffff,0xffd8,0x7fff,0x8000,38,39,40,41,2,4,6,8,10,12};
    static const uint32_t longs[]={0,1,0xffffffffu,0x80000000u,0x7fffffffu,0x10000000u,0xff000000u,0x87654321u};
    unsigned i,j; uint16_t mark=(p&1024)?0:159,delta=words[(p>>7)&15];
    fa18_hud_machine_restore();for(i=0;i<15;++i) REG_DA[i]=random_value();
    for(i=0;i<96;++i) wr_u32(0xc7fd80u+4*i,random_value());
    REG_A[0]=0xc64008u;REG_A[1]=0xc62000u;REG_A[2]=0xc64000u;REG_A[3]=0xc63000u;
    REG_A[4]=0x10000u+((p>>6)&1);REG_A[5]=columns[(p>>5)&15];REG_A[6]=0xc65040u;
    SET_W(REG_D[0],(p>>3)&3u); SET_W(REG_D[5],(p>>4)&12u);SET_W(REG_D[6],words[(p>>5)&15]);SET_W(REG_D[7],0);
    wr_u16(REG_A[6]-8,shifts[(p>>5)&15]);
    for(i=0;i<3;++i) wr_u16(REG_A[3]+2*i,words[(p>>(i+4))&15]);
    wr_u16(0xc458deu,0);
    for(j=0;j<2;++j) {
        gaddr record=0xc46184u+j*512;
        for(i=0;i<3;++i) wr_u32(record+0x14+4*i,longs[(p>>(i+4))&7]);
        for(i=0;i<9;++i) wr_u16(record+0x92+2*i,words[(p/64+i)&15]);
        wr_u8(record+0x63,(p&32)?0x10:0x20);wr_u8(record+4,(p>>4)&1u);
        wr_u16(record+0x4a,words[(p>>4)&15]);
    }
    for(i=0;i<9;++i) wr_u16(0xc45bd8u+2*i,words[(p/32+i)&15]);
    for(i=0;i<3;++i) { wr_u32(0xc45716u+4*i,random_value());wr_u32(0xc45722u+4*i,longs[(p>>(i+5))&7]); }
    wr_u16(0xc45a42u,(p&128)?127:128);wr_u16(0xc459c0u,(p&64)?0xffff:0);
    wr_u16(0xc4593eu,mark);wr_u16(0xc45940u,91);
    wr_u16(0xc4593au,(uint16_t)(mark+delta));wr_u16(0xc4593cu,(uint16_t)(91+((p&2048)?-9:0)));
    wr_u16(0xc458d8u,words[(p>>8)&15]);wr_u8(0xc458b4u,(p&256)?1:0);
    wr_u8(0xc457aeu,(p&512)?1:0);wr_u8(0xc45785u,(p&32)?1:0);wr_u8(0xc45793u,(p&64)?1:0);wr_u8(0xc457a1u,(p&128)?1:0);
    wr_u32(0xc45b54u,random_value());wr_u16(0xc45b44u,words[(p>>6)&15]);wr_u16(0xc45b46u,words[(p>>7)&15]);
    wr_u32(0xc456b6u,0xc65000u);for(i=0;i<4;++i) wr_u32(0xc65000u+4*i,0x10000u+0x4000u*i);
    for(i=0;i<64;++i) { wr_u16(0xc62000u+4*i,columns[(p/128+i)&15]);wr_u16(0xc62002u+4*i,(uint16_t)(0x1000*(i&15)));wr_u8(0xc64000u+i,(uint8_t)('0'+i%10)); }
#ifdef HP_ORIGINAL_CHILDREN
    /* Actual projection is clipped behind the original view plane, text at
     * its source column bound, and the outer HUD at its source context gate.
     * Active paths have separate controlled and live recording evidence. */
    for(i=0;i<9;++i) wr_u16(0xc45bd8u+2*i,(i==0||i==4||i==8)?256:0);
    for(i=0;i<3;++i) { wr_u32(0xc46198u+4*i,0);wr_u32(0xc45722u+4*i,i==2?0xff000000u:0); }
    wr_u16(REG_A[6]-8,0);wr_u16(REG_A[3],0);wr_u16(REG_A[3]+2,0);wr_u16(REG_A[3]+4,0xffff);
    if(selected_entry==0xc33b38u) { wr_u16(0xc459c0u,0xffff);wr_u16(0xc4593au,0);wr_u16(0xc4593eu,0);wr_u8(0xc458b4u,(p&256)?1:0); }
    if(selected_entry==0xc332bcu) wr_u8(0xc45785u,1);
    REG_A[5]=40;SET_W(REG_D[6],0);REG_A[4]=0x10000u;
    for(i=0;i<64;++i) wr_u16(0xc62000u+4*i,0);
#endif
    REG_PPC=0xc10024u;REG_A[7]=0xc7ff00u;expected_sp=REG_A[7]+4;wr_u32(REG_A[7],0xc70000u);
    m68k_set_reg(M68K_REG_SR,0x2700u|(p&31u));REG_PC=selected_entry;
    SET_CYCLES(1000000000);fa18_next_event=INT64_MAX;fa18_recomp_abort=0;
}
