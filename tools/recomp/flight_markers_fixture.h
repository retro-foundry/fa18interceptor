/* Valid original scene/marker streams; source bytes and tables are untouched. */
static uint32_t expected_sp=0xc7ff04u;
static void fixture(unsigned p) {
    static const uint16_t words[]={0,1,0xffff,0x8000,0x7fff,8,175,320};
    static const uint32_t longs[]={0,1,0xffffffffu,0x80000000u,0x7fffffffu,0x01000000u,0xff000000u,0x12345678u};
    static const uint16_t buckets[]={0,1,0xffff,0x8000,0x7fff,8,175,320,899,900,2699,2700,28899,28900,32766,32767};
    unsigned i,j,index=(p>>6)&1; gaddr record=0xc46184u+512*index;
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    for(i=0;i<96;++i) wr_u32(0xc7fd80u+4*i,random_value());
    REG_A[0]=record; REG_A[1]=0xc60800u; REG_A[2]=0xc60c00u; REG_A[3]=0xc62000u;
    REG_A[4]=0xc45c3eu; REG_A[5]=0xc63000u; REG_A[6]=0xc62080u;
    for(i=0;i<16;++i) for(j=0;j<128;++j) wr_u32(0xc46184u+512*i+4*j,0);
    wr_u8(0xc457adu,(p&1)?0:(p&2)?0xff:1); wr_u8(0xc45785u,(p&4)?0:1);
    wr_u8(0xc458a6u,(p&8)?3:1); wr_u8(0xc458aeu,(uint8_t)(4+(p>>4)%4));
    wr_u32(0xc45a66u,(p&128)?0xfe800000u:0xfe7fffffu); wr_u8(0xc45883u,(uint8_t)(p>>8));
    wr_u8(0xc457aeu,(p&256)?1:0); wr_u8(0xc45848u,(uint8_t)((p>>9)&3)); wr_u8(0xc45857u,(uint8_t)words[(p>>3)&7]);
    wr_u16(0xc458deu,(uint16_t)(512*index)); wr_u16(0xc459bau,(uint16_t)(512*index));
    wr_u8(0xc458afu,(uint8_t)words[(p>>7)&7]); wr_u8(0xc457b5u,(p&512)?1:0); wr_u16(0xc458dau,(uint16_t)(p>>4));
    for(i=0;i<3;++i) wr_u32(0xc45c3eu+4*i,longs[(p>>(i+8))&7]);
    for(i=0;i<9;++i) wr_u16(0xc45bd8u+2*i,0);
    for(i=0;i<3;++i) wr_u16(0xc4c592u+2*i,(uint16_t)(i==2?(p&1024)?64:0:0));
    wr_u16(REG_A[6]-2,words[(p>>5)&7]);
    for(i=0;i<16;++i) {
        gaddr r=0xc46184u+512*i;
        wr_u16(r,(uint16_t)((i<3 && !(p&2048))?((p>>(i+5))&1)?0x48:0x40:0));
        wr_u8(r+3,(p&4096)?128:0); wr_u8(r+98,(uint8_t)((i%3)*16)); wr_u8(r+32,(p&8192)?64:0);
        /* Keep angle buckets and cached nibbles independent of the draw gates
         * and returned projection coordinates. */
        wr_u16(r+104,buckets[(p>>(i%4+10))&15]); wr_u8(r+112,(uint8_t)((p>>3)^(p>>7)^0x9b)); wr_u8(r+113,(uint8_t)((p>>5)&15));
        for(j=0;j<3;++j) wr_u32(r+20+4*j,longs[(p>>(j+7))&7]);
    }
    /* Four 16-byte scene rows: ordinary coordinates, active record reference,
     * inactive reference, signed stream terminator. */
    for(i=0;i<4;++i) for(j=0;j<4;++j) wr_u32(0xc42a02u+16*i+4*j,0);
    wr_u16(0xc42a02u,1); wr_u16(0xc42a04u,2); wr_u16(0xc42a06u,(uint16_t)((p>>8)&3));
    wr_u16(0xc42a12u,0x8001); wr_u16(0xc42a22u,0x8002); wr_u16(0xc42a32u,0xffff);
    for(i=0;i<8;++i) wr_u16(0xc1d7e2u+2*i,words[(p>>(i%4+5))&7]);
    wr_u32(0xc4b390u,0x00100060u); wr_u32(0xc4b394u,0x01400060u);
    if(selected_entry==0xc2affau) {
        for(i=0;i<9;++i) wr_u16(0xc45bd8u+2*i,words[(p>>(i%5+1))&7]);
        for(i=0;i<5;++i) REG_D[i]=longs[(p>>(i+1))&7];
    }
    REG_PPC=0xc10024u; REG_A[7]=0xc7ff00u; expected_sp=REG_A[7]+4; wr_u32(REG_A[7],0xc70000u);
    m68k_set_reg(M68K_REG_SR,0x2700u|(p&31u)); REG_PC=selected_entry;
    SET_CYCLES(1000000000); fa18_next_event=INT64_MAX; fa18_recomp_abort=0;
}
