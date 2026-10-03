/* Original HUD layouts, bounded stream storage and full CCR combinations. */
extern void fa18_hud_machine_restore(void);
static uint32_t expected_sp;
static void fixture(unsigned p) {
    static const uint16_t words[]={0,1,0xffff,0x8000,0x7fff,4,19,20,39,40,55,90,179,180,314,320};
    static const uint8_t counters[]={0,1,127,128,255,2,3,4,0,1,2,127,128,255,3,4};
    static const gaddr gates[]={0xc45836,0xc45837,0xc4583b,0xc4583d,0xc4583e,0xc4583f,0xc45840,0xc45842,0xc45845,0xc45846};
    unsigned i; gaddr record=0xc46184u+512*((p>>8)&15u);
    fa18_hud_machine_restore();
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    for(i=0;i<96;++i) wr_u32(0xc7fd80u+4*i,random_value());
    REG_A[0]=0xc61004u; REG_A[1]=0xc62000u; REG_A[2]=0xc64000u;
    REG_A[3]=0xc63000u; REG_A[4]=0x10000u; REG_A[5]=0xc64000u; REG_A[6]=0xc65040u;
    for(i=0;i<sizeof gates/sizeof gates[0];++i) wr_u8(gates[i],counters[((p>>(4+i%2))+3*i)&15]);
    wr_u16(0xc458deu,(uint16_t)(record-0xc46184u)); wr_u16(record,(p&512)?0x0800:0);
    wr_u8(record+2,(uint8_t)(p>>4)); wr_u8(record+3,(uint8_t)(p>>3));
    wr_u8(record+0x20,(uint8_t)(p>>5)); wr_u8(record+0x63,(uint8_t)(p>>4)); wr_u8(record+0x7c,(uint8_t)(p>>4));
    wr_u8(0xc458dbu,(uint8_t)(p>>5)); wr_u8(0xc4586eu,(uint8_t)(p>>3)); wr_u8(0xc45884u,(uint8_t)(p>>6));
    wr_u8(0xc458b5u,(uint8_t)(p&128?1:0)); wr_u8(0xc457abu,(uint8_t)(p&64?1:0));
#ifndef HP_ORIGINAL_CHILDREN
    /* Independent status-E flag: low scenario bits also select the counter
     * gates, so using their bit 6 excluded the active bit-test branch. */
    if(selected_entry==0xc30b5cu) wr_u8(0xc457abu,(uint8_t)(p&1024?1:0));
#endif
    wr_u16(0xc45988u,words[(p>>7)&15]); wr_u16(0xc458d8u,words[(p>>9)&15]);
    wr_u16(0xc45984u,words[(p>>3)&15]); wr_u16(0xc45986u,words[(p>>7)&15]); wr_u16(0xc459a2u,words[(p>>4)&15]);
    wr_u16(0xc458c4u,words[(p>>3)&15]); wr_u16(0xc45a42u,(p&64)?((p&128)?128:64):words[(p>>3)&15]);
    wr_u32(0xc45918u,(p>>8)&1u); wr_u32(0xc456b6u,0xc64000u);
    for(i=0;i<4;++i) wr_u32(0xc64000u+4*i,0x10000u+0x4000u*i);
    wr_u32(0xc62000u,0xc63000u); wr_u32(0xc63000u,0x18000u);
    wr_u32(0xc45b22u,(p&128)?0x00000001u:0x001abcf0u);
    wr_u16(0xc45984u,179); wr_u8(0xc456e7u,0);
#ifdef HP_ORIGINAL_CHILDREN
    /* Source children with clock-held DMA use the original no-draw bounds.
     * Controlled contracts independently exercise complete parent paths. */
    wr_u16(0xc45988u,320); wr_u16(0xc458d8u,0); wr_u16(0xc45986u,20); wr_u32(0xc45918u,0);
    for(i=0;i<sizeof gates/sizeof gates[0];++i) wr_u8(gates[i],0);
#endif
    REG_PPC=0xc10024u; REG_A[7]=0xc7ff00u; expected_sp=REG_A[7]+4; wr_u32(REG_A[7],0xc70000u);
    m68k_set_reg(M68K_REG_SR,0x2700u|(p&31u)); REG_PC=selected_entry;
    SET_CYCLES(1000000000); fa18_next_event=INT64_MAX; fa18_recomp_abort=0;
}
