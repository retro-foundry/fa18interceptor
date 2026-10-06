/* Source-derived autopilot inputs shared by component and real-frame probes. */
static void fixture(unsigned scenario) {
    static const int16_t angles[]={0,0x50,0x190,0x320,0x640,0x960,0xe10,0x1c20,0x35c0,0x3840,0x5460,0x6270,0x6720,0x68b0,0x6e00,0x7080};
    static const int16_t timers[]={-1,0,1,2,15,30,60};
    static const uint8_t classes[]={0,0x10,0x14,0x15,0x20,0x30};
    unsigned i,profile=scenario/40,action=scenario%40+1;
    gaddr record=0xc65000u;
    for(i=0;i<8;++i) { REG_D[i]=random_value(); REG_A[i]=0xc68000u+32*i; }
    for(i=0;i<0x200;++i) wr_u8(record+i,0);
    wr_u16(record,0x80); wr_u16(record+2,(uint16_t)((profile&1)?0x80:0));
    wr_u8(record+32,(uint8_t)(((profile>>1)&3)*8));
    if(profile&256) wr_u8(record+32,2);
    if(profile&512) wr_u8(record+2,rd_u8(record+2)|1);
    wr_u8(record+5,(uint8_t)action);
    wr_u16(record+6,20); wr_u16(record+8,30);
    wr_u16(record+12,(uint16_t)((int)(random_value()%512)-256));
    wr_u16(record+14,(uint16_t)((int)(random_value()%512)-256));
    wr_u32(record+16,0x100000); wr_u32(record+24,(profile&16)?0x280000:0x80000);
    wr_u16(record+38,11); wr_u8(record+43,(uint8_t)((profile&32)?0x70:0x50));
    wr_u16(record+44,(uint16_t)((profile&64)?0xffff:(profile&2)?19:21)); wr_u16(record+46,(uint16_t)((profile&4)?29:31));
    wr_u16(record+48,(uint16_t)((profile&128)?0xff80:0x80)); wr_u16(record+50,0x100);
    wr_u32(record+52,(profile&16)?0x100000:(profile&4)?0x380000:0x180000); wr_u8(record+56,(uint8_t)((profile&8)?0xff:7));
    wr_u16(record+76,(uint16_t)timers[profile%7]);
    wr_u16(record+88,(uint16_t)((profile&16)?0xffd0:0x30)); wr_u16(record+86,rd_u16(record+88));
    wr_u8(record+98,classes[(profile>>2)%6]); wr_u8(record+100,(uint8_t)(profile>>3)); wr_u8(record+101,(uint8_t)profile);
    wr_u16(record+102,(uint16_t)angles[(profile>>1)%16]); wr_u16(record+106,(uint16_t)angles[(profile>>3)%16]);
    wr_u16(record+108,(uint16_t)((profile&8)?0x1800:0x1000)); wr_u16(record+110,(uint16_t)((profile&16)?0x840:0x500));
    wr_u8(record+122,(uint8_t)((profile&8)?3:1)); wr_u32(record+66,(profile&4)?0xffff0000u:0x10000);
    for(i=0;i<9;++i) {
        int diagonal=(profile&128)?(i==2 || i==4 || i==6):(i%4==0);
        wr_u16(record+146+2*i,(uint16_t)(diagonal?((profile&32)?-16384:16384):0));
    }
    wr_u8(0xc457aeu,0); wr_u8(0xc457afu,0); wr_u8(0xc4579au,(profile&2)?0xff:0);
    wr_u8(0xc45793u,(uint8_t)(profile&1)); wr_u8(0xc458a6u,0); wr_u8(0xc458a7u,(uint8_t)(profile%5));
    wr_u32(CURRENT_RECORD,record); REG_A[1]=record; REG_A[6]=0xc62080u;
    REG_PPC=0xc25c6au; REG_PC=selected_entry; REG_A[7]=0xc7ff00u; wr_u32(REG_A[7],0xc70000u);
    m68k_set_reg(M68K_REG_SR,0x2700u|(scenario&31u)); SET_CYCLES(1000000000); fa18_next_event=INT64_MAX; fa18_recomp_abort=0;
    /* Exercise every signed-byte index without replacing source fault behavior:
     * patch its exact original slot to a real source return arm. */
    if(profile>=1024 && action==1) {
        uint8_t byte=(uint8_t)(41+(profile-1024)%215);
        int index=(int8_t)(uint8_t)(byte-1);
        wr_u8(record+98,0x10); wr_u8(record+32,2);
        wr_u8(record+5,byte); wr_u32(0xc2baf8u+(gaddr)(4*index),0xc2c38au);
    }
}
