/* Original records/history/stream data and both numeric and analog tapes.
 * Bounded analog values still exercise every source tape/BCD boundary. */
extern void fa18_hud_machine_restore(void);
static uint32_t expected_sp;
static void fixture(unsigned p) {
    static const uint16_t words[]={0,1,0xffff,0x8000,0x7fff,8,9,127,128,159,255,256,0x600,0x900,0x7f00,0x7f01};
    static const uint32_t magnitudes[]={0,1,0x4000,0x4001,0x40000,0x40001,0x80000000u,0xffffffffu};
    static const uint32_t altitudes[]={0,1,99998,99999,100000,0x80000000u,0xffffffffu,3000};
    unsigned i,j;int analog=(p&64)!=0;gaddr record=0xc46184u;
    fa18_hud_machine_restore();for(i=0;i<15;++i) REG_DA[i]=random_value();
    for(i=0;i<96;++i) wr_u32(0xc7fd80u+4*i,random_value());
    REG_A[0]=0xc61000u;REG_A[1]=0xc62000u;REG_A[2]=0xc64000u;REG_A[3]=0xc63000u;
    REG_A[4]=0x10000u;REG_A[5]=0xc65000u;REG_A[6]=0xc65040u;
    wr_u16(REG_A[6]-8,words[(p>>5)&15]);wr_u16(REG_A[6]-40,words[(p>>6)&15]);
    wr_u16(REG_A[2],(p&128)?6:0);wr_u16(REG_A[2]+2,words[(p>>5)&15]);wr_u16(REG_A[2]+4,words[(p>>6)&15]);
    for(i=0;i<6;++i) wr_u16(0xc48390u+2*i,words[(p>>(i%3+5))&15]);
    wr_u16(0xc459b6u,0);wr_u16(0xc4fdd2u,(p&32)?512:0);
    wr_u8(0xc4fdd0u,(p&128)?6:5);wr_u8(0xc4fdd1u,(uint8_t)((p>>5)%5));
    for(j=0;j<2;++j) {
        record=0xc46184u+512*j;wr_u8(record+0x3d,(uint8_t)((p>>5)%7));wr_u8(record+0x20,(p&256)?2:0);
        wr_u8(record,(p&4096)?128:0);wr_u8(record+0x62,analog?0x10:0x20);
        wr_u16(record+0x6e,analog?(uint16_t)((p>>5)%1200):words[(p>>7)&15]);wr_u16(record+0x68,words[(p>>6)&15]);
        for(i=0;i<3;++i) { wr_u32(record+0x14+4*i,(p&1024)?0xffff0000u:0x10000u);wr_u32(record+0x3eu+4*i,(p&2048)?0xffff0100u:0x100u); }
        wr_u32(record+0x18,analog?((p>>5)%200)*1024u:(p&1024)?0xffff0000u:0x10000u);
    }
    for(i=0;i<3;++i) wr_u32(0xc45a7cu+4*i,0);
    for(j=0;j<12;++j) for(i=0;i<3;++i) wr_u32(0xc4fdd4u+12*j+4*i,magnitudes[(p/32+i+j)&7]);
    wr_u16(0xc458deu,0);wr_u8(0xc457a4u,(p&128)?1:0);wr_u32(0xc45658u,analog?(p>>5)%1000:altitudes[(p>>5)&7]);
    wr_u16(0xc45988u,words[(p>>7)&15]);wr_u16(0xc458d8u,words[(p>>8)&15]);wr_u16(0xc45986u,20);wr_u32(0xc45918u,0);wr_u8(0xc45785u,0);
    wr_u32(0xc456b6u,0xc65000u);for(i=0;i<4;++i) wr_u32(0xc65000u+4*i,0x10000u+0x4000u*i);
    wr_u32(0xc45b22u,(p&256)?0x360:0x00000071u);wr_u8(0xc456e7u,0);wr_u16(0xc45ab8u,0xfffcu);
#ifdef HP_ORIGINAL_CHILDREN
    /* Source clipping bounds are separate from controlled active paths. */
    for(i=0;i<6;++i) wr_u16(0xc48390u+2*i,(i%3==2)?0xffff:0);
    for(i=0;i<9;++i) wr_u16(0xc45bd8u+2*i,(i==0||i==4||i==8)?256:0);
    wr_u16(REG_A[6]-8,0);wr_u16(REG_A[6]-40,0);
    for(j=0;j<12;++j) { wr_u32(0xc4fdd4u+12*j,0);wr_u32(0xc4fdd8u+12*j,0);wr_u32(0xc4fddcu+12*j,0xffff0000u); }
    wr_u8(0xc4fdd0u,6);wr_u8(0xc4fdd1u,(uint8_t)((p>>5)%6));
    wr_u8(0xc46184u+0x62,0x20);wr_u32(0xc45658u,(p>>5)%1000);wr_u32(0xc4619cu,0);wr_u16(0xc45988u,320);wr_u16(0xc458d8u,0);
#endif
    REG_PPC=0xc10024u;REG_A[7]=0xc7ff00u;expected_sp=REG_A[7]+4;wr_u32(REG_A[7],0xc70000u);
    if(selected_entry==0xc1fe24u || selected_entry==0xc1fe46u || selected_entry==0xc0cf98u) { REG_PPC=0xc1f942u;REG_A[0]=selected_entry; }
    m68k_set_reg(M68K_REG_SR,0x2700u|(p&31u));REG_PC=selected_entry;
    SET_CYCLES(1000000000);fa18_next_event=INT64_MAX;fa18_recomp_abort=0;
}
