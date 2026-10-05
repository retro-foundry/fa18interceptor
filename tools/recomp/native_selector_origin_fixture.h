/* Original-shaped fixtures; validation only. */
static void origin_fixture(unsigned scenario) {
    static const uint32_t limits[]={0,1,0x23f,0x240,0x241,0x27ff,0x2800,0x2801,
        0x7fff,0x8000,0x8001,0xcfff,0xd000,0xd001,0x2ffff,0x30000,0x30001,
        0x5ffff,0x60000,0x60001,0xfffff,0x100000,0x100001,0x1fffff,0x200000,
        0x200001,0x7fffff,0x800000,0x800001,0xcfffff,0xd00000,0xd00001,
        0x1bfffff,0x1c00000,0x1c00001,0x7fffffff,0x80000000u,0xffffffffu};
    static const uint8_t enables[]={0xff,1,2,3,16,17,32,63,64,65,127};
    static const uint8_t types[]={0x11,0x14,0x30,0x17};
    static const uint8_t details[]={0,1,5,0xff};
    static const uint16_t angle[]={0,1,0x1c1f,0x1c20,0x5460,0x5461,0x7fff,0x8000,0xffff};
    static const uint8_t counters[]={0,1,2,5,0x7f,0x80,0xff};
    static const uint8_t auxiliary_flags[]={0,1,0x7f,0x80,0xff};
    static const uint32_t auxiliary_delta[]={0,0xffefffffu,0xfff00000u,0xfff00001u,0x80000000u,0x7fffffffu};
    static const uint16_t floor_words[]={0,1,0x7ff8,0x7ff9,0x7fff,0x8000,0xfff8,0xfff9,0xffff};
    uint32_t magnitude=limits[(scenario/9u)%38u];
    unsigned i,lane=scenario%16u,mode=(scenario/16u)%9u;
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    REG_A[6]=0xc62080u;
    for(i=0;i<32;++i) wr_u32(0xc62040u+4*i,random_value());
    wr_u8(ORIGIN_ENABLE,1); wr_u8(ORIGIN_GATE_B,1); wr_u8(ORIGIN_GATE_A,0);
    wr_u16(ORIGIN_RECORD_OFFSET,(uint16_t)((scenario/64u)%4u)*512u);
    wr_u8(ORIGIN_GATE_MODE,0); wr_u8(ORIGIN_DETAIL_MODE,2);
    wr_u8(ORIGIN_ADJUSTMENT_MODE,(uint8_t)mode);
    wr_u8(ORIGIN_VARIANT_SELECTOR,(uint8_t)((scenario/144u)%4u));
    wr_u8(ORIGIN_DETAIL_COUNTER,counters[(scenario/144u)%7u]);
    wr_u8(ORIGIN_AUXILIARY_FLAG,auxiliary_flags[(scenario/288u)%5u]);
    for(i=0;i<3;++i) {
        uint32_t origin=random_value(),delta=(i==scenario%3u)?magnitude:0;
        if((scenario/512u)&1u) delta=0u-delta;
        wr_u32(SELECTOR_ORIGIN+4*i,origin);
        wr_u32(ORIGIN_CANDIDATE_TRIPLE+4*i,origin+delta);
        wr_u32(ORIGIN_SMOOTHED_DELTA+4*i,(scenario&0x100u)?random_value():0);
    }
    if(mode==7) wr_u32(ORIGIN_AUXILIARY_DELTA,auxiliary_delta[(scenario/432u)%6u]);
    if(lane<3) {
        if(lane==0) wr_u8(ORIGIN_ENABLE,0);
        else if(lane==1) wr_u8(ORIGIN_GATE_B,0);
        else wr_u8(ORIGIN_GATE_A,1);
    } else if(lane==3) {
        wr_u8(ORIGIN_GATE_MODE,1); wr_u8(ORIGIN_DETAIL_MODE,0);
    } else if(lane<8) {
        gaddr record=CONTROL_RECORDS+(gaddr)(int32_t)rd_s16(ORIGIN_RECORD_OFFSET);
        wr_u8(ORIGIN_DETAIL_MODE,details[(scenario/16u)%4u]);
        wr_u8(ORIGIN_ENABLE,enables[(scenario/64u)%11u]);
        wr_u8(ORIGIN_DETAIL_INDEX,(uint8_t)((scenario/704u)%10u));
        wr_u8(record+0x62u,types[(scenario/32u)%4u]);
        wr_u16(record+0x68u,angle[(scenario/128u)%9u]);
        wr_u16(ORIGIN_ANGLE_HISTORY,angle[(scenario/1152u)%9u]);
        wr_u8(record+4u,(uint8_t)((rd_u8(record+4u)&0x3fu)|(((scenario/256u)%4u)<<6)));
        wr_u16(record+0x4eu,floor_words[(scenario/256u)%9u]);
    } else if((scenario/256u)&1u) wr_u8(ORIGIN_DETAIL_MODE,(uint8_t)(6+scenario%3u));
    if(selected_entry==0xc29368u) {
        /* A real terminated list shape, using original control-record slots.
         * Record fields outside the source selection contract stay sealed. */
        wr_u32(ORIGIN_RECORD_LIST,0xc60000u);
        for(i=0;i<16;++i) {
            gaddr record=CONTROL_RECORDS+512*i;
            wr_u16(0xc60000u+10*i,0); wr_u16(0xc60004u+10*i,(uint16_t)i);
            wr_u8(record+0x62u,((scenario+i)%3u)?0x11:0x30);
            wr_u8(record+1u,(uint8_t)(((scenario+i)%4u)?0x40:0)|((scenario&16u)?8u:0));
        }
        wr_u16(0xc60000u+10*((scenario/32u)%17u),0xffffu);
    }
    REG_A[7]=0xc7ff00u; wr_u32(REG_A[7],0xc70000u);
    m68k_set_reg(M68K_REG_SR,0x2700u|(scenario&31u)); REG_PC=selected_entry;
    fa18_recomp_abort=0; fa18_next_event=INT64_MAX; SET_CYCLES(100000000);
}
