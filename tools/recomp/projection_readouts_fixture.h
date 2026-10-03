/* Valid readout layouts/planes and original packed glyph/source tables. */
static uint32_t expected_sp;
static void fixture(unsigned p) {
    static const uint16_t edges[]={0,1,0xffff,0x8000,0x7fff,8,71,112,160,180,319,320,32,90,48,80};
    static const uint16_t depths[]={1,2,16,160,512,0x7fff,0x8000,0xffff};
    static const int16_t modes[]={-5,-4,-3,-2,-1,0,1,2,3,8,16,31,32,63,-32768,32767};
    unsigned i; uint16_t x=edges[(p>>3)&15],y=edges[(p>>7)&15];
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    for(i=0;i<96;++i) wr_u32(0xc7fd80u+4*i,random_value());
    REG_A[0]=0xc61004u; REG_A[1]=0xc62000u; REG_A[2]=0xc61000u; REG_A[3]=0xc63000u;
    REG_A[4]=0x10000u; REG_A[5]=0xc64000u; REG_A[6]=0xc65040u;
    wr_u32(0xc456b6u,0xc64000u);
    for(i=0;i<4;++i) wr_u32(0xc64000u+4*i,0x10000u+0x4000u*i+(p&2048?1:0));
    wr_u32(0xc45b22u,(p&128)?0:(p&256)?0x00000123u:(p&512)?0x01234567u:0x99999999u);
    wr_u16(0xc45954u,(uint16_t)(p>>4)); wr_u16(0xc45984u,edges[(p>>5)&15]);
    wr_u16(0xc45ab8u,(uint16_t)modes[(p>>4)&15]); wr_u16(REG_A[6]-40,edges[(p>>8)&15]);
    for(i=0;i<4;++i) { wr_u16(0xc62000u+4*i,edges[(p>>(i%3+5))&15]); wr_u16(0xc62002u+4*i,(uint16_t)(p<<8)); }
    for(i=0;i<16;++i) wr_u8(0xc61000u+i,(uint8_t)((p&1024)?32:48+(i%10)));
    if(selected_entry>=0xc2ec90u && selected_entry<=0xc2eca8u) {
        SET_W(REG_D[0],x); SET_W(REG_D[1],y); SET_W(REG_D[2],depths[(p>>11)&7]); SET_W(REG_D[6],0);
#ifndef PR_ORIGINAL_CHILDREN
        /* The source fault child is a separate controlled return contract. */
        if((p&511)==17) { SET_W(REG_D[0],0x8000); SET_W(REG_D[1],0x8000); SET_W(REG_D[2],0); }
#endif
    } else if(selected_entry==0xc2ecd2u || selected_entry==0xc2ece4u) {
        /* Internal clamp segments are proven separately from complete calls. */
        SET_W(REG_D[0],x); SET_W(REG_D[1],y); SET_W(REG_D[2],512); SET_W(REG_D[7],modes[(p>>4)&15]); SET_W(REG_D[6],0);
    } else if(selected_entry==0xc32a44u) {
        SET_W(REG_D[0],x); SET_W(REG_D[1],y); SET_W(REG_D[2],(p>>4)&0x7fff); SET_W(REG_D[3],(p>>2)%3);
    } else if(selected_entry==0xc32ac8u) { SET_W(REG_D[0],(p>>2)%3); }
    else if(selected_entry==0xc33fb4u) {
        SET_W(REG_D[0],x); SET_W(REG_D[1],edges[(p>>5)&15]);
    }
    wr_u16(0xc45988u,edges[(p>>9)&15]); wr_u16(0xc4598cu,(uint16_t)(68+(p>>4)%48));
    REG_PPC=0xc10024u; REG_A[7]=0xc7ff00u; expected_sp=REG_A[7]+4; wr_u32(REG_A[7],0xc70000u);
    m68k_set_reg(M68K_REG_SR,0x2700u|(p&31u)); REG_PC=selected_entry;
    SET_CYCLES(1000000000); fa18_next_event=INT64_MAX; fa18_recomp_abort=0;
}
