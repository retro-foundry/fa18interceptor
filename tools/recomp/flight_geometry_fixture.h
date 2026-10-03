/* Bounded original data structures for complete history, zone and geometry calls. */
static uint32_t expected_sp=0xc7ff04u;
static void fixture(unsigned p) {
    static const uint32_t edge[]={0,1,0xffffffffu,0x80000000u,0x7fffffffu,0x08000000u,0x7ffu,0xfffff800u};
    unsigned i,j,index=(p>>6)&1; gaddr root=0xc46184u+512*index,candidate=root+512,stream=0xc63000u,face=0xc64000u;
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    for(i=0;i<96;++i) wr_u32(0xc7fd80u+4*i,random_value());
    REG_A[0]=0xc60400u; REG_A[1]=0xc60800u; REG_A[2]=0xc60c00u; REG_A[3]=root;
    REG_A[4]=stream; REG_A[5]=face; REG_A[6]=0xc62080u;
    for(i=0;i<16;++i) for(j=0;j<128;++j) wr_u32(0xc46184u+512*i+4*j,0);
    wr_u32(0xc1ab74u,0xc65000u); wr_u16(0xc459b6u,(uint16_t)(512*index)); wr_u16(0xc458deu,(uint16_t)(512*index));
    wr_u8(0xc457aeu,0); wr_u8(0xc4585eu,1); wr_u32(0xc459c6u,stream+120);
    for(i=0;i<11;++i) wr_u32(0xc4d790u+4*i,0xffffffffu);
    wr_u16(stream+120,0); wr_u32(stream+122,face+128);
    if(selected_entry==0xc2651eu) {
        wr_u16(0xc4fdd2u,(uint16_t)((p&1)?0x400:512*index)); wr_u8(0xc457aeu,(p&2)?1:0);
        wr_u8(0xc4fdd1u,(uint8_t)(p>>3)); wr_u8(0xc4fdd0u,(uint8_t)(p>>7));
        wr_u16(root+110,(p&4)?1:0); wr_u8(root+2,(p&8)?16:0); wr_u8(root+61,(uint8_t)(p>>8));
        for(i=0;i<3;++i) wr_u32(root+20+4*i,edge[(p>>(i+5))&7]);
    } else if(selected_entry==0xc28e28u) {
        root=0xc46384u; wr_u16(0xc459b4u,(p&1)?0:1);
        wr_u8(root+98,(p&2)?0x30:16); wr_u8(root+5,(p&4)?8:0);
        wr_u8(root+93,(p&8)?0xff:(p&16)?0:1); wr_u8(root+122,(p&32)?5:(p&64)?4:3);
        wr_u16(root+6,(p&128)?2:0); wr_u16(root+8,(p&256)?2:0);
        wr_u32(0xc29720u,stream); wr_u16(stream,0xffff); wr_u16(stream+2,1); wr_u16(stream+4,0xffff); wr_u16(stream+6,1);
        wr_u16(stream+8,(p&512)?0:2);
        for(i=0;i<2;++i) { wr_u16(stream+14+10*i,(uint16_t)((p&1024)?2:i?1:2)); wr_u16(stream+16+10*i,0); }
        wr_u16(0xc295e0u,64); for(i=0;i<5;++i) wr_u16(0xc29620u+2*i,(uint16_t)edge[(p>>(i+5))&7]);
    } else {
        int32_t sx=(p&128)?(int32_t)edge[(p>>8)&7]:0,sy=(p&256)?(int32_t)edge[(p>>9)&7]:(p&512)?-256:0;
        int32_t sz=(p&1024)?(int32_t)edge[(p>>11)&7]:0;
        static const uint8_t classes[]={0,1,16,21,32,48,0x11,0x31};
        wr_u16(root,8); wr_u8(root+94,1); wr_u8(root+98,classes[(p>>3)&7]); wr_u8(root+123,(uint8_t)(p>>7)); wr_u8(root+125,(uint8_t)((p>>10)&15));
        wr_u32(root+16,(p&2048)?0x8000:(p&4096)?0xffffffffu:0);
        wr_u16(root+2,(p&8192)?0xc082:2); wr_u8(root+4,(uint8_t)(p>>4));
        for(i=0;i<3;++i) wr_u32(root+62+4*i,edge[(p>>(i+6))&7]);
        wr_u16(candidate,(p&1)?0:(p&2)?0x648:0x48); wr_u8(candidate+94,(p&4)?1:2);
        wr_u8(candidate+98,classes[(p>>6)&7]); wr_u8(candidate+125,(uint8_t)((p>>9)&15));
        if(p&8192) wr_u16(candidate,rd_u16(candidate)|0x1000);
        for(i=0;i<3;++i) { wr_u32(candidate+20+4*i,(p&16384)?edge[(p>>(i+8))&7]:0); wr_u32(candidate+62+4*i,edge[(p>>(i+3))&7]); }
        if(p%16==7) wr_u8(candidate+98,32);
        if(p%16==8) { wr_u8(root+98,0); wr_u16(candidate,0); }
        if(p%16==9) { wr_u8(root+98,16); wr_u16(candidate,0); }
        if(p%16==10) { wr_u8(root+98,48); wr_u16(candidate,0); }
        if(p&32) wr_u16(0xc458deu,(uint16_t)(512*(1-index)));
        /* Original polygon ring, face list and signed terminators. */
        wr_u16(face,0); wr_u16(face+2,6); wr_u16(face+4,0x800c); wr_u16(face+6,0xffff);
        wr_u16(face+32,0); wr_u16(face+34,0); wr_u16(face+36,6); wr_u16(face+38,12);
        for(i=0;i<3;++i) {
            wr_u16(candidate+164+6*i,(uint16_t)(i==1?100:0));
            wr_u16(candidate+166+6*i,(uint16_t)((p&64)?100:0));
            wr_u16(candidate+168+6*i,(uint16_t)(i==2?100:0));
        }
        for(i=0;i<6;++i) wr_u16(candidate+428+2*i,(uint16_t)edge[(p>>(i+2))&7]);
        for(i=0;i<6;++i) wr_u16(candidate+488+2*i,(uint16_t)edge[(p>>(i+2))&7]);
        { gaddr tables[]={0xc39168u,0xc39e48u,0xc391e4u,0xc39e68u};
          for(i=0;i<4;++i) {
            wr_u32(tables[i],i<2?face:face+32); wr_u32(tables[i]+4,face+32); wr_u32(tables[i]+8,0xffffffffu);
            wr_u32(tables[i]+22,face+32); wr_u32(tables[i]+26,face+32); wr_u32(tables[i]+30,0xffffffffu);
          }
        }
        REG_D[2]=(uint32_t)sx; REG_D[3]=(uint32_t)sy; REG_D[4]=(uint32_t)sz;
        if(selected_entry==0xc26ebeu && (p&31)>=16) {
            unsigned mode=p&31,q=p>>5;
            /* Independent profiles keep candidate activation, class, geometry and
             * child flags from sharing the same selector bits. */
            wr_u8(root+98,16); wr_u8(root+123,0); wr_u8(root+125,0);
            wr_u32(root+16,0); wr_u16(root+2,(uint16_t)(2|((q&1)?128:0)|((q&2)?0xc000:0)));
            wr_u16(0xc458deu,(uint16_t)(512*index));
            REG_D[2]=REG_D[4]=20*256; REG_D[3]=mode==16?0xffffff00u:0;
            if(mode<24) {
                wr_u16(candidate,0x48); wr_u8(candidate+94,2); wr_u8(candidate+98,32);
                wr_u16(face+2,12); wr_u16(face+4,0x8006);
                wr_u8(candidate+125,0);
                for(i=0;i<3;++i) wr_u32(candidate+20+4*i,0);
                if(q&128) { wr_u8(candidate+98,33); wr_u32(root+16,0xffffffffu); }
                for(i=0;i<3;++i) wr_u16(candidate+166+6*i,(uint16_t)(mode==16?7:mode==18?-7:0));
                if(mode==19) REG_D[2]=REG_D[4]=0;
                if(mode==20) REG_D[2]=REG_D[4]=200*256;
                if(mode==21) wr_u16(0xc458deu,(uint16_t)(512*(1-index)));
                for(i=0;i<9;++i) {
                    wr_u16(candidate+428+2*i,(uint16_t)random_value());
                    wr_u16(candidate+488+2*i,(uint16_t)random_value());
                }
                wr_u16(root+150,(uint16_t)(q&4?32767:0)); wr_u16(root+162,(uint16_t)(q&4?32767:0));
                wr_u16(candidate+150,(uint16_t)(q&4?32767:0)); wr_u16(candidate+162,(uint16_t)(q&4?32767:0));
                if(q&64) {
                    uint32_t distance=mode==16?0x20000u:mode==17?0x4800u:mode<20?0x1100u:mode==20?0x5100u:0;
                    wr_u8(root+98,(q&2)?16:(q&4)?1:0); wr_u8(root+94,(q&32)?0:1);
                    wr_u8(candidate+98,mode==17?21:mode<20?16:0);
                    wr_u16(root,8); wr_u16(candidate,(uint16_t)((q&16?0x40:0x48)|(q&8?0x1000:0)));
                    if(q&256) wr_u16(root,0);
                    wr_u16(root+2,(q&1)?0x100:0); wr_u16(candidate+2,0);
                    REG_D[2]=distance; REG_D[3]=mode==19?0u-distance:distance; REG_D[4]=distance;
                    for(i=0;i<3;++i) wr_u32(candidate+62+4*i,(uint32_t)(0u-2*REG_D[2+i]+((q&32)?512:0)));
                }
            } else {
                gaddr volume=0xc64200u,bounds=0xc4d7c0u,material=0xc64400u;
                wr_u16(candidate,0); wr_u8(root+98,mode==24 || mode==25 || mode==26?0:mode==27 || mode==28?16:48);
                wr_u32(root+16,(q&1)?10:0); wr_u8(root+125,0);
                for(i=0;i<3;++i) { wr_u32(root+62+4*i,(q&2)?256:0); wr_u16(root+164+2*i,0); }
                for(i=0;i<6;++i) wr_u16(root+170+2*i,(uint16_t)edge[(q>>(i+1))&7]);
                wr_u32(0xc4d790u,volume); wr_u8(bounds-2,0); wr_u8(bounds-1,0);
                for(i=0;i<2;++i) {
                    wr_u16(bounds+10*i,32767); wr_u16(bounds+2+10*i,0xff9c); wr_u16(bounds+4+10*i,100);
                    wr_u16(bounds+6+10*i,0xff9c); wr_u16(bounds+8+10*i,100);
                }
                wr_u32(bounds+20,0xffffffffu); wr_u16(volume+24,2);
                for(i=0;i<2;++i) {
                    for(j=0;j<3;++j) wr_u16(bounds+24+6*i+2*j,0);
                    wr_u16(volume+32+12*i,0); wr_u16(volume+34+12*i,(uint16_t)((q&4)?1:0xffff)); wr_u16(volume+36+12*i,0);
                }
                wr_u32(face+132,material); wr_u16(material,(uint16_t)(q&8?0x4000:0));
                wr_u16(material+2,0); wr_u16(material+4,(uint16_t)(q&16?0xffff:0)); wr_u8(material+7,(q&32)?16:0);
                if(mode==30) wr_u16(root+2,0);
                if(mode==31) wr_u8(bounds-2,2);
                if(q&64) {
                    if(mode==24) wr_u16(bounds,0xffff);
                    if(mode==25) wr_u16(bounds+2,1);
                    if(mode==26) wr_u16(bounds+4,0);
                    if(mode==27) wr_u16(bounds+6,100);
                    if(mode==28) wr_u16(bounds+10,0xffff);
                    if(mode==29) { wr_u16(bounds+2,1); wr_u8(root+7,0xff); }
                    if(mode==30) { wr_u8(root+9,0xff); wr_u8(bounds-1,0xff); wr_u16(root+2,2); }
                    if(mode==31) { wr_u16(stream+120,16); wr_u16(stream+96,64); }
                }
                if(q&128) {
                    wr_u8(0xc4585eu,12);
                    if(mode==24) {
                        wr_u16(bounds+10,0xffff); wr_u32(bounds+20,0xc64600u); wr_u32(volume+20,0xc64800u);
                        wr_u16(0xc64600u,0xffff); wr_u32(0xc6460au,0xffffffffu);
                    }
                    if(mode==25) { wr_u8(root+7,0xff); wr_u8(root+9,0xff); }
                }
            }
        }
        if(selected_entry==0xc27456u) {
            REG_A[3]=candidate; REG_A[2]=edge[(p>>3)&7]; REG_A[1]=edge[(p>>6)&7];
            wr_u32(stream,(p&1)?0xffffffffu:face+32); wr_u32(stream+4,(p&2)?face+32:0xffffffffu); wr_u32(stream+8,0xffffffffu);
            wr_u16(REG_A[6]-90,(uint16_t)edge[(p>>9)&7]); wr_u16(REG_A[6]-66,(uint16_t)edge[(p>>12)&7]);
            for(i=0;i<9;++i) if(p&4) wr_u16(candidate+164+2*i,(uint16_t)random_value());
        }
    }
    REG_PPC=0xc10024u; REG_A[7]=0xc7ff00u; expected_sp=REG_A[7]+4; wr_u32(REG_A[7],0xc70000u);
    m68k_set_reg(M68K_REG_SR,0x2700u|(p&31u)); REG_PC=selected_entry;
    SET_CYCLES(1000000000); fa18_next_event=INT64_MAX; fa18_recomp_abort=0;
}
