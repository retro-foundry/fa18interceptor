/* Source layouts and cache inputs; full CCR and caller register combinations. */
extern void fa18_hud_machine_restore(void);
static uint32_t expected_sp;
static void fixture(unsigned p) {
    static const uint16_t words[]={0,1,0xffff,0x8000,0x7fff,4,19,20,39,40,55,90,179,180,314,320};
    static const uint32_t longs[]={0,1,0xffffffffu,0x80000000u,0x7fffffffu,99998,99999,100000,0x10000000u,0x0f000000u,0x00100000u,0xfff00000u,128,32767,32768,0x87654321u};
    static const uint8_t counters[]={0,1,127,128,255,2,3,4,0,1,2,127,128,255,3,4};
    unsigned i; gaddr record=0xc46184u+512*((p>>8)&15u); uint32_t speed;
    fa18_hud_machine_restore();
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    for(i=0;i<96;++i) wr_u32(0xc7fd80u+4*i,random_value());
    REG_A[0]=0xc61004u; REG_A[1]=0xc31998u; REG_A[2]=0xc64000u;
    REG_A[3]=0xc63000u; REG_A[4]=0x10000u; REG_A[5]=0xc64000u; REG_A[6]=0xc65040u;
    SET_W(REG_D[0],words[(p>>3)&15]);
    wr_u16(0xc458deu,(uint16_t)(record-0xc46184u)); wr_u8(record,(p&1024)?128:0);
    wr_u8(record+0x63,(uint8_t)(p>>4)); wr_u8(record+0x5f,(uint8_t)(p>>3)); wr_u16(record+0x60,words[(p>>4)&15]);
    wr_u8(record+0x62,(p&512)?0x11:(uint8_t)(p>>3)); wr_u16(record+0x56,words[(p>>6)&15]);
    wr_u16(record+0x68,words[(p>>5)&15]); wr_u16(record+0x6e,words[(p>>7)&15]);
    wr_u8(record+0x2b,(uint8_t)(p>>3)); wr_u32(record+0x72,longs[(p>>5)&15]); wr_u32(record+0x18,longs[(p>>6)&15]);
    wr_u32(record+0x14,longs[(p>>3)&15]); wr_u32(record+0x1c,longs[(p>>4)&15]);
    wr_u8(0xc45785u,(p&32)?1:0); wr_u8(0xc457d9u,(p&64)?1:0); wr_u8(0xc458cdu,(p&128)?64:0);
    wr_u8(0xc457a4u,(p&256)?1:0); wr_u32(0xc4565cu,longs[(p>>7)&15]);
    wr_u8(0xc45837u,counters[(p>>3)&15]); wr_u8(0xc45839u,counters[(p>>4)&15]); wr_u8(0xc4583au,counters[(p>>5)&15]); wr_u8(0xc45844u,counters[(p>>6)&15]);
    wr_u8(0xc458dbu,(p&512)?1:0); wr_u16(0xc458cau,words[(p>>5)&15]); wr_u16(0xc45946u,words[(p>>4)&15]);
    speed=(rd_u8(record)&128)?0:(uint16_t)(rd_s16(record+0x6e)<0?-rd_s16(record+0x6e):rd_s16(record+0x6e));
    wr_u16(0xc458f8u,(p&2048)?(uint16_t)speed:words[(p>>4)&15]);
    wr_u16(0xc458f6u,(p&2048)?(uint16_t)(rd_s32(record+0x72)>>8):words[(p>>4)&15]);
    { int16_t v=(int16_t)((int8_t)rd_u8(record+0x2b)*256); uint32_t v2=(uint32_t)(int32_t)(v<0?(int16_t)-v:v); wr_u16(0xc458fcu,(p&2048)?(uint16_t)(v2/0x133):words[(p>>4)&15]); }
    wr_u32(0xc45900u,(p&2048)?0x80000001u:longs[(p>>4)&15]);
    for(i=0;i<2;++i) { uint32_t v=rd_u32(record+(i?0x14:0x1c))-(i?0x0f000000u:0x10000000u); int32_t z=(int32_t)v>>8, d=i?0x5999:0x7000, q=z/d; uint32_t out=(q>=-32768&&q<=32767)?(uint32_t)(uint16_t)(z%d)<<16|(uint16_t)q:(uint32_t)z; wr_u16(i?0xc4595eu:0xc4595cu,(p&2048)?(uint16_t)(out+(i?0x4c4:0x177)):words[(p>>4)&15]); }
    wr_u16(0xc45988u,words[(p>>7)&15]); wr_u16(0xc458d8u,0); wr_u16(0xc45986u,(p>>8)&1u); wr_u32(0xc45918u,(p>>7)&1u);
    wr_u32(0xc456b6u,0xc64000u); for(i=0;i<4;++i) wr_u32(0xc64000u+4*i,0x10000u+0x4000u*i);
    wr_u32(0xc45b22u,(p&128)?0x00000001u:0x001abcf0u); wr_u8(0xc456e7u,0);
#ifdef HP_ORIGINAL_CHILDREN
    /* Original small text is clipped by its own column bound; 8-pixel text
     * and points use original no-draw bounds while the clock is held. */
    wr_u16(0xc45988u,320); wr_u16(0xc45986u,20); wr_u32(0xc45918u,0);
    wr_u8(0xc45785u,0); /* Context font paths use their fixed zero column. */
#endif
    REG_PPC=0xc10024u; REG_A[7]=0xc7ff00u; expected_sp=REG_A[7]+4; wr_u32(REG_A[7],0xc70000u);
    m68k_set_reg(M68K_REG_SR,0x2700u|(p&31u)); REG_PC=selected_entry;
    SET_CYCLES(1000000000); fa18_next_event=INT64_MAX; fa18_recomp_abort=0;
}
