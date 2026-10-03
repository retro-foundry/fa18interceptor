/* Original layouts and valid RAM/Custom bases; all full CCR combinations. */
extern void fa18_hud_machine_restore(void);
static uint32_t expected_sp;
static void fixture(unsigned p) {
    static const uint16_t edges[]={0,1,0xffff,0x8000,0x7fff,4,314,315,319,320,32,90,179,180,48,80};
    unsigned i;
    fa18_hud_machine_restore();
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    for(i=0;i<96;++i) wr_u32(0xc7fd80u+4*i,random_value());
    REG_A[0]=(p&32)?0xdff000u:0xc64000u; REG_A[1]=0xc62000u; REG_A[2]=0xc61000u;
    REG_A[3]=0xc63000u; REG_A[4]=0x10000u; REG_A[5]=0xc64000u; REG_A[6]=0xc65040u;
    if(selected_entry<0xc31000u) {
        /* Original blit dimensions are bounded here; addresses remain valid. */
        REG_D[0]=0x18000u; REG_D[1]=0x10000u; REG_D[3]=0x1c000u;
        REG_D[4]=0x14000u; REG_D[7]=(p&64)?0x40:0;
        SET_W(REG_D[2],(p&32)?((p&128)?0x0100:0x09f0):edges[(p>>7)&15]);
        SET_W(REG_D[6],(p&32)?((p&256)?0x0042:0x0041):edges[(p>>3)&15]);
        wr_u32(0xc61000u,((p>>9)&15)*40u); wr_u32(0xc62000u,0xc63000u); wr_u32(0xc63000u,0x18000u);
        for(i=0;i<4;++i) wr_u16(0x18000u+2*i,edges[(p>>(i+3))&15]);
    } else if(selected_entry==0xc31b76u) {
        wr_u16(0xc45ae6u,edges[(p>>3)&15]); wr_u8(0xc457b3u,(uint8_t)(p&64?1:0));
        wr_u16(0xc45776u,edges[(p>>7)&15]); wr_u16(0xc45778u,edges[(p>>11)&15]);
    } else SET_W(REG_D[0],edges[(p>>3)&15]);
    wr_u16(0xc45988u,edges[(p>>7)&15]); wr_u16(0xc458d8u,edges[(p>>11)&15]);
#ifdef HS_ORIGINAL_CHILDREN
    if(selected_entry==0xc33ad6u || selected_entry==0xc33b06u) {
        static const int16_t screen_offsets[]={0,1,-1,-90,89,90,120,179};
        static const uint8_t planes[]={0,1,2,4,8};
        /* Original line child with valid screen offsets and zero/one enabled
         * plane. Multiple planes wait for DMA, which the whole-call clock
         * holds. Controlled contracts retain all signed word offset edges. */
        wr_u16(0xc458d8u,(uint16_t)screen_offsets[(p>>11)&7]);
        wr_u8(0xc456e7u,planes[(p>>5)%5]);
    }
#endif
    wr_u16(0xc45954u,(uint16_t)((p>>4)&15)); wr_u16(0xc45984u,179);
    wr_u32(0xc456b6u,0xc64000u); for(i=0;i<4;++i) wr_u32(0xc64000u+4*i,0x10000u+0x4000u*i);
    wr_u32(0xc45b22u,(p&128)?0:0x01234567u);
    REG_PPC=0xc10024u; REG_A[7]=0xc7ff00u; expected_sp=REG_A[7]+4; wr_u32(REG_A[7],0xc70000u);
    m68k_set_reg(M68K_REG_SR,0x2700u|(p&31u)); REG_PC=selected_entry;
    SET_CYCLES(1000000000); fa18_next_event=INT64_MAX; fa18_recomp_abort=0;
}
