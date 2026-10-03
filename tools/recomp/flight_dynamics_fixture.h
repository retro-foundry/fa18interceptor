/* Source-derived, bounded input records for complete-call comparisons. */
static uint32_t expected_sp=0xc7ff04u;
static void fixture(unsigned p) {
    static const uint32_t edge[]={0,1,0xffffffffu,0x80000000u,0x7fffffffu,0x08000000u,0x7ffu,0xfffff800u};
    unsigned i,j; gaddr record=0xc60800u,stream=0xc63000u,geometry=0xc64000u;
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    for(i=0;i<128;++i) wr_u32(record+4*i,random_value());
    for(i=0;i<96;++i) wr_u32(0xc7fd80u+4*i,random_value());
    REG_A[0]=0xc60400u; REG_A[1]=record; REG_A[2]=stream; REG_A[3]=0xc61000u;
    REG_A[4]=0xc61200u; REG_A[5]=0xc61600u; REG_A[6]=0xc62080u;
    /* Keep unrelated state deterministic while varying the source's gates. */
    for(i=0;i<0x180;++i) wr_u8(0xc45780u+i,0);
    wr_u32(0xc1ab74u,0xc65000u);
    if(selected_entry==0xc25b66u) {
        static const uint16_t flags[]={0,0x40,0x100,0x400,0x1000,0x2000,0x1040,0x1440};
        static const uint8_t classes[]={0,0x10,21,0x20,0x30,0x40,0x11,0x31};
        wr_u16(record,flags[p&7]|((p&512)?4:0)|((p&1024)?2:0));
        wr_u16(record+2,(uint16_t)((p>>5)&0xff)); wr_u16(record+4,0);
        wr_u8(record+5,(p&32)?1:0); wr_u8(record+98,classes[(p>>3)&7]);
        wr_u16(record+76,(p&128)?0:20); wr_u16(record+102,(uint16_t)edge[(p>>7)&7]);
        wr_u16(record+104,(uint16_t)random_value()); wr_u16(record+106,(uint16_t)edge[(p>>10)&7]);
        wr_u16(record+110,(p&256)?1200:0); wr_u8(record+124,(uint8_t)(p>>4));
        wr_u8(record+125,0); wr_u32(record+20,edge[(p>>6)&7]);
        wr_u32(record+24,edge[(p>>9)&7]); wr_u32(record+28,edge[(p>>12)&7]);
        wr_u32(record+66,(p&64)?0xffffffffu:0); wr_u8(record+32,(uint8_t)(p>>8));
        wr_u16(0xc459b4u,(p&16)?1:0); wr_u16(0xc459b6u,(p&2048)?1:0);
        wr_u16(0xc458dau,(p&4096)?1:0); wr_u16(0xc458dcu,(p&16)?1:0);
        wr_u16(0xc458ccu,(p&8)?64:0); wr_u8(0xc45785u,(p&64)?1:0);
        wr_u8(0xc45788u,(p%64==0)?1:0); wr_u8(0xc457aeu,(p%64==1)?1:0);
        wr_u8(0xc4578cu,1); wr_u8(0xc4589au,(p&1024)?1:0);
        wr_u8(0xc4584eu,(p&4096)?1:0); wr_u8(0xc458dbu,(uint8_t)(p>>7));
        wr_u8(0xc4589fu,(p&512)?1:0); wr_u8(0xc45847u,(uint8_t)(p>>6));
        wr_u16(0xc4594cu,(uint16_t)(p&3)); wr_u16(0xc4594eu,(uint16_t)((p>>2)&3));
        if(p%64==0) { wr_u16(0xc459b4u,1); wr_u16(record,(p&128)?64:0); }
        /* Source warning branches need values within their narrow angle band. */
        if(p%32==10 || p%32==11) {
            wr_u16(record,0x100);
            wr_u16(0xc459b4u,0); wr_u8(0xc45785u,1); wr_u16(0xc458ccu,64);
            wr_u32(record+66,0xffffff00u); wr_u32(record+24,(p&512)?0x2000:0x100);
            wr_u16(record+102,0x1000); wr_u16(record+106,(p&128)?0x5000:0x1000);
            wr_u8(record+124,0); wr_u8(record+32,(p%32==11)?2:0); wr_u8(record+3,0);
        }
        if(p%32==12) { wr_u16(record,0x100); wr_u16(record+76,20); }
        if(p%32==13) { wr_u16(record,0x100); wr_u8(record+2,1); wr_u8(record+5,0); }
    } else if(selected_entry==0xc266aeu) {
        unsigned index=(p&16)?20:0; gaddr scene=0xc45c72u+64*index;
        for(i=0;i<16;++i) wr_u32(scene+4*i,0);
        for(i=0;i<3;++i) {
            wr_u32(scene+4*i,(p&128)?edge[(p>>(i+7))&7]:0);
            wr_u32(scene+12+4*i,edge[(p>>(i+5))&7]);
            wr_u32(scene+24+4*i,(p&256)?edge[(p>>(i+8))&7]:0);
        }
        wr_u16(scene+36,(p&8)?1:0); wr_u8(scene+38,(uint8_t)((p&1)?16:(p&2)?32:0));
        wr_u16(scene+46,2); wr_u16(scene+48,0); wr_u16(scene+50,0);
        wr_u16(0xc46190u,0); wr_u16(0xc46192u,0); wr_u32(0xc46194u,0);
        wr_u16(0xc459a6u,(p&512)?0x8000:2048); wr_u16(0xc459a8u,(p&1024)?0:2048);
        wr_u8(0xc4585eu,(p&64)?6:1); wr_u32(0xc459c6u,stream+120);
        for(i=0;i<6;++i) { wr_u16(stream+120-24*i,(p&4)?0x4000:0); wr_u32(stream+122-24*i,geometry); }
        for(i=0;i<11;++i) wr_u32(0xc4d790u+4*i,0xffffffffu);
        wr_u8(0xc457bdu,(p%64==0)?1:0); wr_u8(0xc457aeu,(p%64==1)?1:0);
        /* Component descriptor and its model metadata. */
        if(p&32) {
            wr_u16(stream+120,0x0110); wr_u16(0xc46384u,64);
            wr_u16(0xc4638au,0); wr_u16(0xc4638cu,0); wr_u16(0xc46390u,0); wr_u16(0xc46392u,0); wr_u32(0xc46394u,0);
            wr_u32(geometry+4,geometry+64); wr_u16(geometry+64,(p&256)?0:0x8000); wr_u16(geometry+68,0); wr_u8(geometry+71,(p&2048)?16:0);
            wr_u8(0xc463e6u,(p%3==0)?21:(p%3==1)?0:16); wr_u8(0xc463c0u,(p%3==2)?2:0);
            if(p&512) wr_u16(0xc46390u,0xffff);
        }
        if((p&2048) && !(p&32)) {
            gaddr params=0xc4d7c0u,chain=params+10;
            wr_u32(0xc4d790u,geometry); wr_u8(params-2,(p&512)?1:0); wr_u8(params-1,(p&512)?1:0);
            for(i=0;i<2;++i) {
                gaddr q=i?chain:params;
                wr_u16(q,100); wr_u16(q+2,0xff9c); wr_u16(q+4,100); wr_u16(q+6,0xff9c); wr_u16(q+8,100);
            }
            wr_u32(chain+10,0xffffffffu); wr_u16(geometry+24,1);
            for(i=0;i<6;++i) wr_u16(chain+14+2*i,0);
            wr_u16(geometry+32,0); wr_u16(geometry+34,(p&4096)?0xffff:0); wr_u16(geometry+36,0);
            if(p&8192) {
                /* Follow one real map chain, then terminate at its negative link. */
                gaddr next=0xc64200u; wr_u32(chain+10,next); wr_u32(geometry+20,geometry+256);
                for(i=0;i<5;++i) wr_u16(next+2*i,rd_u16(chain+2*i));
                wr_u32(next+10,0xffffffffu); wr_u16(geometry+270,1);
                for(i=0;i<3;++i) { wr_u16(next+14+2*i,0); wr_u16(geometry+278+2*i,0); }
                if(p&4096) wr_u16(chain+2,1);
            }
        }
        wr_u32(0xc7ff04u,index);
    } else {
        /* Region header, ten-byte stream row, source model and placement table. */
        wr_u8(0xc458a6u,(p&1)?3:(p&2)?125:2);
        if(p&4) wr_u8(0xc458a6u,4);
        wr_u8(0xc458aau,(p&8)?0:10); wr_u8(0xc458a9u,(p&16)?20:0);
        wr_u8(0xc4582bu,(p&32)?1:0); wr_u16(0xc45b18u,0x7fff); wr_u16(0xc45b1au,0x8000); wr_u16(0xc45af8u,(uint16_t)(p>>4));
        for(i=0;i<8;++i) {
            gaddr header=stream+256*i; wr_u32(0xc29720u+4*i,header);
            wr_u16(header,0xffff); wr_u16(header+2,1); wr_u16(header+4,0xffff); wr_u16(header+6,1);
            wr_u16(header+8,(p&64)?0:1);
            wr_u16(header+10,0); wr_u16(header+12,(uint16_t)((p&128)?21:(p&256)?32:16));
            wr_u16(header+14,(uint16_t)((p&512)?0x8101:(p&1024)?0x0101:1));
            wr_u16(header+16,0); wr_u16(header+18,(uint16_t)(((p*0x9e37u)&0xf000)|((p&128)?0x80:0)|((p%7==0)?0x300:0)));
        }
        for(i=0;i<2;++i) {
            gaddr root=0xc46184u+512*i;
            wr_u16(root,0); wr_u16(root+6,0); wr_u16(root+8,0); wr_u8(root+56,(p&2048)?0x81:0);
            wr_u8(root+98,(p&4096)?21:0); wr_u8(root+122,(p&8192)?4:3); wr_u8(root+125,0);
        }
        wr_u16(0xc46384u,(p%16==5 || (p&2048))?64:0); wr_u16(0xc4638au,(p&4096)?1:0);
        for(i=0;i<5;++i) wr_u32(0xc22048u+4*i,i==1?geometry:random_value());
        wr_u16(geometry,(p%3==0)?0x8000:(p%3==1)?0x4000:0); wr_u16(geometry+2,0); wr_u16(geometry+4,0); wr_u8(geometry+6,(uint8_t)(p&15));
        wr_u16(0xc295e0u,64); wr_u16(0xc295e2u,80); wr_u16(0xc295e4u,96);
        for(i=0;i<3;++i) for(j=0;j<5;++j) wr_u16(0xc29620u+16*i+2*j,(uint16_t)edge[(p>>(j+3))&7]);
        REG_A[3]=0xc29720u; REG_A[2]=stream+10; REG_D[0]=(p&64)?0:0x12340000u;
        wr_u32(0xc29740u,0xffffffffu);
        if(selected_entry==0xc28996u) {
            wr_u8(0xc45790u,(p%64==0)?1:0); wr_u8(0xc4579du,(uint8_t)(p>>4));
            wr_u16(0xc4618au,(p&4)?3:0); wr_u16(0xc4618cu,(p&8)?3:0);
            if(p&2) wr_u32(0xc29720u,0xffffffffu);
        }
    }
    REG_PPC=0xc10024u; REG_A[7]=0xc7ff00u; expected_sp=REG_A[7]+4; wr_u32(REG_A[7],0xc70000u);
    if(selected_entry!=0xc266aeu) wr_u32(REG_A[7]+4,0);
    m68k_set_reg(M68K_REG_SR,0x2700u|(p&31u)); REG_PC=selected_entry;
    SET_CYCLES(1000000000); fa18_next_event=INT64_MAX; fa18_recomp_abort=0;
}
