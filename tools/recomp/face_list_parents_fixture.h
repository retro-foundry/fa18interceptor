/* Original demo RAM remains the base. Source descriptors, colours, flags,
 * signed arithmetic and loop counts vary without changing original bytes. */
extern void fa18_hud_machine_restore(void);
static uint32_t expected_sp;
static void fixture(unsigned p) {
 static const uint16_t words[]={0,1,0xffff,0x8000,0x7fff,8,15,16,31,32,63,127,128,255,256,0x600};
 static const uint16_t counts[]={0,1,2,3,0xffff,0x8000};
 static const uint16_t modes[]={0,0x4000,0x8000,0xffff,0x1000,0x2000,0x3000,0x6000};
 static const uint32_t distances[]={0,0xffffffffu,0xfffffebfu,0xfffffec0u,0xffffff7fu,0xffffff80u,0x80000000u,0x7fffffffu};
 unsigned i,j,n=p>>5;gaddr frame=0xc650c0u,stream=0xc64000u,face=0xc62000u;
 fa18_hud_machine_restore();for(i=0;i<15;++i)REG_DA[i]=random_value();
 for(i=0;i<96;++i)wr_u32(0xc7fd80u+4*i,random_value());
 for(i=0;i<128;++i){wr_u32(0xc63000u+4*i,random_value());wr_u32(0xc65000u+4*i,random_value());}
 REG_A[0]=0xc60000u;REG_A[1]=face;REG_A[2]=stream;REG_A[3]=0xc48390u;REG_A[4]=0xc61000u;REG_A[5]=0xc66000u;REG_A[6]=frame;
 for(i=0;i<256;++i)wr_u16(0xc48390u+2*i,words[(n+i/3)&15]);
 for(i=0;i<64;++i)wr_u16(0xc4bf90u+2*i,words[(n+i)&15]);
 for(i=0;i<128;++i)wr_u16(0xc60000u+2*i,words[(n+i)&15]);
 for(i=0;i<128;++i)wr_u16(stream+2*i,1);
 wr_u16(stream,0);wr_u16(stream+2,24);wr_u16(stream+4,48);wr_u16(stream+6,modes[n&7]);wr_u16(stream+32,0xffff);wr_u16(stream+64,12);wr_u16(stream+128,(uint16_t)(n&63));
 wr_u32(0xc45a32u,0xc60000u);wr_u16(0xc45b2au,words[(n>>2)&15]);wr_u16(0xc45b2eu,words[(n>>3)&15]);wr_u32(0xc45a78u,distances[n&7]);wr_u8(0xc4586cu,(n&8)?1:0);wr_u8(frame-0x7f,(p&128)?1:0);
 wr_u16(frame-0x26,words[n&15]);wr_u16(frame-0x22,words[(n+1)&15]);wr_u16(frame-0x28,words[(n>>2)&15]);wr_u16(frame-0x32,words[(n+2)&15]);wr_u16(frame-0x34,words[(n+3)&15]);
 if(selected_entry==0xc2005cu) {for(i=0;i<n%3+2;++i)wr_u16(stream+2*i,(uint16_t)(12*i));wr_u16(stream+2*i,(uint16_t)(12*i)|0x8000u);wr_u16(stream+2*i+2,modes[n&7]);wr_u16(stream+2*i+4,(uint16_t)(n&63));}
 if(selected_entry==0xc20100u) {
  wr_u32(stream,face);wr_u16(stream+4,0);wr_u16(stream+6,(n&16)?32:0xffff);wr_u16(stream+8,0xffff);
  for(j=0;j<2;++j){for(i=0;i<n%3+2;++i)wr_u16(face+32*j+2*i,(uint16_t)(12*i));wr_u16(face+32*j+2*i,(uint16_t)(12*i)|0x8000u);wr_u16(face+32*j+2*i+2,modes[n&7]);wr_u16(face+32*j+2*i+4,(uint16_t)(n&63));}
 }
 if(selected_entry==0xc21060u) {for(j=0;j<n%3+1;++j)for(i=0;i<4;++i)wr_u16(stream+8*j+2*i,(uint16_t)(12*i));wr_u16(stream+8*j,0xffff);}
 if(selected_entry==0xc20c38u||selected_entry==0xc20c22u||selected_entry==0xc20a52u||selected_entry==0xc20a40u) {
  j=(selected_entry==0xc20c22u||selected_entry==0xc20a40u)?2:0;wr_u16(stream,(uint16_t)(n&63));wr_u16(stream+j,(uint16_t)((n%4)*6));wr_u16(stream+j+2,counts[n%6]);for(i=0;i<4;++i)wr_u16(stream+j+4+2*i,counts[(n+i+1)%6]);
 }
 if(selected_entry==0xc2084au)REG_A[3]=0xc6000au;
 if(selected_entry==0xc2082au)wr_u16(stream,(uint16_t)((n%4)*8));
 wr_u16(0xc459b6u,(uint16_t)((n%3)*0x90));for(i=0;i<64;++i)wr_u16(0xc46184u+(n%3)*0x90+0xa4+2*i,words[(n+i/3)&15]);wr_u8(0xc46184u+(n%3)*0x90+0x7cu,(uint8_t)(n*13));
 if(selected_entry==0xc219aeu){wr_u16(stream,0);wr_u16(stream+2,48);wr_u16(stream+4,(uint16_t)((n%4)*6));}
 if(selected_entry==0xc21c4cu||selected_entry==0xc21c2eu)wr_u16(stream,(uint16_t)((n%4)*6));
 if(n&16)for(i=0;i<256;++i)wr_u16(0xc48390u+2*i,(i%3==2)?0xffffu:0);
#ifdef HP_ORIGINAL_CHILDREN
 /* Original polygon children are bounded by degenerate or behind-view
  * source geometry. Normalization, derived points and splits stay varied. */
 if(selected_entry!=0xc219aeu&&selected_entry!=0xc21c4cu&&selected_entry!=0xc21c2eu&&selected_entry!=0xc2084au&&selected_entry!=0xc2082au) {
  for(i=0;i<256;++i)wr_u16(0xc48390u+2*i,((n&16)&&i%3==2)?0xffffu:0);
  for(i=0;i<64;++i)wr_u16(0xc4bf90u+2*i,0);
 }
#endif
 REG_PPC=0xc1f942u;REG_A[0]=selected_entry;
 if(selected_entry==0xc2084au)REG_PPC=0xc1efdcu;
 if(selected_entry==0xc21c4cu)REG_PPC=0xc21c40u;
 REG_A[7]=0xc7ff00u;expected_sp=REG_A[7]+4;wr_u32(REG_A[7],0xc70000u);
 m68k_set_reg(M68K_REG_SR,0x2700u|(p&31u));REG_PC=selected_entry;SET_CYCLES(1000000000);fa18_next_event=INT64_MAX;fa18_recomp_abort=0;
}
