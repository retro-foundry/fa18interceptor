/* Caller layouts, ASCII/hex fields, leading zeros and all CCR combinations. */
extern void fa18_hud_machine_restore(void);
static uint32_t expected_sp;
static void fixture(unsigned p) {
    static const uint16_t words[]={0,1,0xffff,0x8000,0x7fff,4,19,20,39,40,55,90,179,180,314,320};
    static const uint16_t positions[]={0,1,0xffff,0xffd8,0x7fff,0x8000,38,39,40,41,2,4,6,8,10,12};
    static const uint8_t counters[]={0,1,127,128,255,2,3,4,0,1,2,127,128,255,3,4};
    static const uint32_t digits[]={0,1,0x00000100,0x1abcf0,0xffffffffu,0x87654321u,0x10000000,0x99999999};
    unsigned i,count=((p>>3)&7)+1;
    fa18_hud_machine_restore(); for(i=0;i<15;++i) REG_DA[i]=random_value();
    for(i=0;i<96;++i) wr_u32(0xc7fd80u+4*i,random_value());
    REG_A[0]=0xc64000u+count; REG_A[1]=0xc62000u; REG_A[2]=0xc64000u;
    REG_A[3]=0xc63000u; REG_A[4]=0x10000u+((p>>6)&1u); REG_A[5]=positions[(p>>5)&15]; REG_A[6]=0xc66000u;
    SET_W(REG_D[0],selected_entry==0xc31c20u?words[(p>>3)&15]:count-1); REG_D[0]=(REG_D[0]&0xffffu)|(uint32_t)positions[(p>>5)&15]<<16;
    SET_W(REG_D[2],((p>>4)&7)); SET_B(REG_D[4],p&128?1:0); SET_W(REG_D[5],(p>>4)&12u); SET_W(REG_D[6],words[(p>>7)&15]); SET_W(REG_D[7],0);
    /* A0 is the independent digit end; D2 selects an independently bounded
     * digit count, including the one-digit leading-zero scan. */
    REG_A[0]=0xc64008u;
    for(i=0;i<64;++i) { wr_u16(0xc62000u+4*i,positions[(p/128+i)&15]); wr_u16(0xc62002u+4*i,(uint16_t)(0x1000*(i&15))); wr_u8(0xc64000u+i,(i&3)?(uint8_t)('0'+((p+i)&15)):' '); }
    wr_u16(0xc62000u,words[(p>>4)&15]);
    wr_u8(0xc45837u,counters[(p>>3)&15]); wr_u8(0xc458dbu,(p&256)?1:0); wr_u8(0xc457aeu,(p&512)?1:0);
    wr_u16(0xc45986u,positions[(p>>8)&15]); wr_u32(0xc45918u,(p>>6)&1u); wr_u32(0xc45b22u,digits[(p>>7)&7]);
    wr_u8(0xc45955u,(uint8_t)(p>>4)); wr_u32(0xc456b6u,0xc65000u); for(i=0;i<4;++i) wr_u32(0xc65000u+4*i,0x10000u+0x4000u*i);
    wr_u8(0xc456e7u,0);
#ifdef HP_ORIGINAL_CHILDREN
    /* Source text clips at its original column bounds with the clock held.
     * Controlled contracts cover active glyph and fault paths separately. */
    REG_A[5]=40; SET_W(REG_D[6],0); REG_D[0]&=0xffffu; REG_A[4]=0x10000u;
    wr_u16(0xc45986u,20); wr_u32(0xc45918u,0);
    if(selected_entry!=0xc31c20u) for(i=0;i<64;++i) wr_u16(0xc62000u+4*i,0);
#endif
    REG_PPC=0xc10024u;REG_A[7]=0xc7ff00u; expected_sp=REG_A[7]+4;wr_u32(REG_A[7],0xc70000u);
    m68k_set_reg(M68K_REG_SR,0x2700u|(p&31u));REG_PC=selected_entry;
    SET_CYCLES(1000000000);fa18_next_event=INT64_MAX;fa18_recomp_abort=0;
}
