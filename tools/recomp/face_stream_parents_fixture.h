/* Original demo RAM is the base. Bounded original stream records vary CCR,
 * word arithmetic, shared tails, view rejection and signed shift domains. */
extern void fa18_hud_machine_restore(void);
static uint32_t expected_sp;
static void fixture(unsigned p) {
 static const uint16_t words[]={0,1,0xffff,0x8000,0x7fff,8,15,16,31,32,63,127,128,255,256,0x600};
 static const uint16_t shifts[]={0,1,7,8,15,16,31,32,63,0xffff,0xfff0,0x8000,0xffc1,2,4,9};
 static const uint16_t counts[]={0,1,2,3,0xffff,0x8000};
 static const uint32_t distances[]={0,0xffffffffu,0xffffff3fu,0xffffff40u,0xffffff7fu,0xffffff80u,0x80000000u,0x7fffffffu};
 unsigned i,j,n=p>>5;gaddr frame=0xc650c0u,stream=0xc64000u;
 fa18_hud_machine_restore();for(i=0;i<15;++i)REG_DA[i]=random_value();
 for(i=0;i<96;++i)wr_u32(0xc7fd80u+4*i,random_value());
 for(i=0;i<128;++i){wr_u32(0xc63000u+4*i,random_value());wr_u32(0xc65000u+4*i,random_value());}
 REG_A[0]=0xc60000u;REG_A[1]=0xc62000u;REG_A[2]=stream;REG_A[3]=0xc48390u;REG_A[4]=0xc61000u;REG_A[5]=0xc66000u;REG_A[6]=frame;
 for(i=0;i<256;++i)wr_u16(0xc48390u+2*i,words[(n+i/3)&15]);
 for(i=0;i<64;++i)wr_u16(0xc4bf90u+2*i,words[(n+i)&15]);
 for(i=0;i<96;++i)wr_u16(0xc60000u+2*i,words[(n+i)&15]);
 wr_u32(0xc45a32u,0xc60000u);wr_u8(0xc60006u,(uint8_t)(n&15));
 wr_u16(0xc45ab8u,shifts[(n>>1)&15]);wr_u16(0xc45b2au,words[(n>>2)&15]);wr_u16(0xc45b2eu,words[(n>>3)&15]);
 wr_u16(0xc45a72u,words[n&15]);wr_u16(0xc45a76u,words[(n>>1)&15]);wr_u32(0xc45a78u,distances[n&7]);
 for(i=0;i<128;++i)wr_u16(stream+2*i,1);
 wr_u16(stream,(uint16_t)(n&63));wr_u16(stream+2,0);wr_u16(stream+4,24);wr_u16(stream+6,48);wr_u16(stream+8,72);
 if(selected_entry==0xc2129cu||selected_entry==0xc212b0u||selected_entry==0xc2131cu) {
  j=selected_entry==0xc2131cu?4:2;
  for(i=0;i<n%3+1;++i){wr_u16(stream+j,(uint16_t)(i*12));wr_u16(stream+j+2,(uint16_t)(24+i*12)|((i==n%3)?0x8000u:0));j+=4;}
 }
 if(selected_entry==0xc211dcu){wr_u16(stream,(uint16_t)(((n%4)<<8)|(n&63)));wr_u16(stream+2,(uint16_t)((n%4)*12));}
 if(selected_entry==0xc20f10u||selected_entry==0xc20ec4u||selected_entry==0xc217eau){wr_u16(stream,(uint16_t)((n%4)*6));wr_u16(stream+2,shifts[n&15]);}
 if(selected_entry==0xc21a20u){wr_u16(stream,(uint16_t)((n%4)*6));wr_u16(stream+2,48);wr_u16(stream+4,(n&1)?0xfffcu:4);}
 if(selected_entry==0xc20d68u||selected_entry==0xc20904u){wr_u16(stream+2,(uint16_t)((n%4)*6));wr_u16(stream+4,counts[n%6]);for(i=0;i<4;++i)wr_u16(stream+6+2*i,counts[(n+i+1)%6]);}
 if(selected_entry==0xc210e6u){wr_u16(stream,(uint16_t)((n%4)*6));wr_u16(stream+2,counts[n%6]);}
 if(selected_entry==0xc2159eu){wr_u16(stream+2,0);wr_u16(stream+4,(n&1)?0x8006u:6);wr_u16(stream+6,24);wr_u16(stream+8,48);}
 if((n&16) && (selected_entry==0xc2129cu||selected_entry==0xc212b0u||selected_entry==0xc20e4eu||selected_entry==0xc20e40u||selected_entry==0xc2139eu||selected_entry==0xc21412u||selected_entry==0xc21490u||selected_entry==0xc2159eu)) {
  for(i=0;i<256;++i)wr_u16(0xc48390u+2*i,(i%3==2)?0xffffu:0);
  for(i=0;i<64;++i)wr_u16(0xc4bf90u+2*i,0xffffu);
 }
#ifdef HP_ORIGINAL_CHILDREN
 /* Original drawing children receive degenerate geometry. Active DMA under
  * a frozen clock remains an explicit later timing scope. No service stub. */
 if(selected_entry!=0xc20f10u&&selected_entry!=0xc20ec4u&&selected_entry!=0xc217eau&&selected_entry!=0xc21a20u) {
  int behind=(n&16) && (selected_entry==0xc2129cu||selected_entry==0xc212b0u||selected_entry==0xc20e4eu||selected_entry==0xc20e40u||selected_entry==0xc2139eu||selected_entry==0xc21412u||selected_entry==0xc21490u||selected_entry==0xc2159eu);
  for(i=0;i<256;++i)wr_u16(0xc48390u+2*i,(behind && i%3==2)?0xffffu:0);
  for(i=0;i<64;++i)wr_u16(0xc4bf90u+2*i,behind?0xffffu:0);
 }
#endif
 REG_PPC=0xc1f942u;REG_A[0]=selected_entry;REG_A[7]=0xc7ff00u;expected_sp=REG_A[7]+4;wr_u32(REG_A[7],0xc70000u);
 m68k_set_reg(M68K_REG_SR,0x2700u|(p&31u));REG_PC=selected_entry;SET_CYCLES(1000000000);fa18_next_event=INT64_MAX;fa18_recomp_abort=0;
}
