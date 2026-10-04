/* Source tables remain intact. Valid original stream/frame/record cursors
 * exercise full parents; real children use original degenerate/view bounds. */
extern void fa18_hud_machine_restore(void);
static uint32_t expected_sp;
static void fixture(unsigned p) {
 static const uint16_t words[]={0,1,0xffff,0x8000,0x7fff,8,15,16,31,32,63,127,128,255,256,0x600};
 static const uint16_t shifts[]={0,1,7,8,15,16,31,32,63,0xffff,2,4,9,10,12,14};
 static const uint32_t heights[]={0,0xffffffffu,0xfff00000u,0xffefffffu,0xffff0000u,0xfffeffffu,0x80000000u,0x7fffffffu};
 unsigned i,j,case_index=p>>5;gaddr frame=0xc650c0u,stream=0xc64000u,record=0xc46184u;
 fa18_hud_machine_restore();for(i=0;i<15;++i)REG_DA[i]=random_value();
 for(i=0;i<96;++i)wr_u32(0xc7fd80u+4*i,random_value());
 for(i=0;i<128;++i){wr_u32(0xc63000u+4*i,random_value());wr_u32(0xc65000u+4*i,random_value());}
 REG_A[0]=0xc60000u;REG_A[1]=0xc62000u;REG_A[2]=stream;REG_A[3]=0x180u*((case_index>>3)&7u);REG_A[4]=0xc61000u;REG_A[5]=0xc66000u;REG_A[6]=frame;
 wr_u32(0xc45a32u,0xc60000u);for(i=0;i<96;++i)wr_u16(0xc60000u+2*i,words[(case_index+i)&15]);
 wr_u8(0xc60006u,(uint8_t)(case_index&15));wr_u8(0xc60007u,(uint8_t)(case_index%4));
 for(i=0;i<16;++i)wr_u16(stream+2*i,0);wr_u16(stream,((case_index%4)<<8)|(case_index&63));wr_u16(stream+2,0);wr_u16(stream+4,24);wr_u16(stream+6,0xffff);
 for(i=0;i<48;++i)wr_u16(0xc48390u+2*i,words[(case_index+i/3)&15]);
 for(i=0;i<36;++i)wr_u16(0xc4bf90u+2*i,words[(case_index+i)&15]);
 for(i=0;i<18;++i)wr_u16(0xc45bc6u+2*i,(i%4==0)?256:0);
 for(i=0;i<9;++i)wr_u16(0xc45bd8u+2*i,(i%4==0)?256:0);
 wr_u16(frame-8,shifts[(case_index>>1)&15]);wr_u16(frame-6,shifts[(case_index>>2)&15]);wr_u16(frame-2,0);wr_u32(frame-0x2c,0xc60000u);
 for(i=0;i<3;++i){wr_u32(frame-32+4*i,heights[(case_index+i)&7]);wr_u32(0xc45b30u+4*i,heights[(case_index+i+2)&7]);wr_u16(frame-0x26+2*i,words[(case_index+i)&15]);}
 wr_u16(frame-0x78,words[case_index&15]);wr_u16(frame-0x76,words[(case_index+1)&15]);wr_u16(frame-0x74,words[(case_index+2)&15]);wr_u8(frame-0x7f,(p&128)?1:0);
 wr_u16(0xc45ab8u,shifts[(case_index>>2)&15]);wr_u16(0xc45b2au,words[(case_index>>3)&15]);wr_u16(0xc45b2eu,words[(case_index>>4)&15]);
 wr_u16(0xc45a72u,words[case_index&15]);wr_u16(0xc45a76u,words[(case_index>>1)&15]);wr_u32(0xc45a78u,heights[(case_index>>2)&7]);wr_u32(0xc45a66u,heights[case_index&7]);
 wr_u16(0xc459b6u,0);wr_u16(0xc459b4u,(p&256)?512:0);wr_u16(0xc458dcu,0);wr_u8(0xc45785u,(p&512)?1:0);wr_u8(0xc45786u,(p&128)?1:0);wr_u8(0xc4586bu,(uint8_t)(case_index%5));
 wr_u8(record+4,(uint8_t)((case_index%4)<<6));wr_u8(record+0x62,(case_index%3==0)?0x30:(case_index%3==1)?0x14:0x20);wr_u16(record+0x4e,words[case_index&15]);wr_u32(record+24,0x80000000u);
 for(i=0;i<32;++i)wr_u16(record+0xa4+2*i,words[(case_index+i)&15]);
 wr_u16(0xc4b432u,(uint16_t)(case_index%4));for(i=0;i<8;++i)wr_u16(0xc4b434u+2*i,words[(case_index+i)&15]);wr_u16(0xc45988u,words[(case_index>>2)&15]);wr_u16(0xc458d8u,words[(case_index>>3)&15]);
 SET_W(REG_D[0],case_index%4);SET_W(REG_D[3],case_index%4);SET_B(REG_D[4],case_index%7);SET_W(REG_D[6],words[(case_index>>1)&15]);SET_W(REG_D[7],shifts[(case_index>>2)&15]);
 if(selected_entry==0xc1fb82u) {static const uint16_t modes[]={0,0x1000,0x2000,0x3000,0x0400,0x0800,0x0c00,0x1400,0x1800,0x1c00};SET_W(REG_D[7],modes[case_index%10]);wr_u16(stream-4,(uint16_t)((case_index%8)*6));wr_u16(stream,(uint16_t)((case_index%8)*12));}
 if(selected_entry==0xc1f99au)SET_W(REG_D[7],0);
 if(selected_entry==0xc2168au) {wr_u16(stream,case_index&63);wr_u16(stream+2,0);wr_u16(stream+4,0);wr_u16(stream+6,(p&256)?0x8006:6);wr_u16(stream+8,24);wr_u16(stream+10,0);}
 if(selected_entry==0xc201a6u) {
  wr_u16(stream,words[case_index&15]);wr_u16(stream+2,(uint16_t)(case_index%3+1));wr_u16(stream+4,(p&256)?1:0);
  for(i=0;i<3;++i)wr_u16(stream+6+2*i,6*i);
  j=6+2*(case_index%3+1);wr_u16(stream+j,0xffff);
 }
#ifndef HP_ORIGINAL_CHILDREN
 if((p&1024) && selected_entry==0xc2d16cu) {
  unsigned clamp=(case_index>>1)&3;
  SET_W(REG_D[0],clamp==1?200:clamp==3?0:0x8000);
  SET_W(REG_D[1],clamp==1?200:clamp==2?0:0x8000);SET_W(REG_D[2],200);
  SET_W(REG_D[3],0);SET_B(REG_D[4],3);SET_W(REG_D[7],0);
 }
 if((p&2048) && (selected_entry==0xc21500u||selected_entry==0xc2168au)) {
  for(i=0;i<48;++i)wr_u16(0xc48390u+2*i,(i%3==2)?0xffff:0);
  wr_u32(0xc45a78u,0);wr_u16(0xc45a72u,0);wr_u16(0xc45a76u,0);
 }
#endif
 if((p&1024) && selected_entry==0xc201a6u) {
  unsigned variant=case_index&15;
  wr_u8(record+4,0);wr_u8(record+0x62,0x20);wr_u8(0xc45785u,0);wr_u8(0xc4586bu,2);wr_u16(0xc459b4u,512);
  wr_u32(0xc45a66u,variant==0?0xffefffffu:variant==4?0xfff50000u:0xfffe0000u);
  if(variant==1||variant==2){wr_u16(0xc459b4u,0);wr_u8(0xc4586bu,1);wr_u32(0xc45a66u,variant==1?0xfffffc18u:0xffffdcd8u);}
  if(variant==5)wr_u8(0xc4586bu,1);
  if(variant>=6)wr_u8(0xc45785u,1);
  wr_u16(stream,variant==6?0x8000:variant==7?0x9000:0);wr_u16(0xc45ab8u,0);wr_u16(frame-6,0);wr_u16(frame-8,0);
  wr_u16(stream+2,3);wr_u16(stream+4,variant==8?0:1);wr_u16(stream+6,0);wr_u16(stream+8,6);wr_u16(stream+10,12);
  wr_u16(record+0xa4,0);wr_u16(record+0xa8,0);wr_u16(record+0xaa,256);wr_u16(record+0xae,0);wr_u16(record+0xb0,0);wr_u16(record+0xb4,256);
  if(variant==8)wr_u16(record+0xb4,0xff00);
  wr_u16(stream+12,variant==9?0:variant==10?0x7fff:0xffff);
  wr_u16(stream+14,3);wr_u16(stream+16,1);wr_u16(stream+18,0);wr_u16(stream+20,6);wr_u16(stream+22,12);wr_u16(stream+24,0xffff);
 }
#ifdef HP_ORIGINAL_CHILDREN
 /* Degenerate source geometry and behind-view projections bound original
  * drawing while keeping parent stream/count paths explicit. */
 for(i=0;i<48;++i)wr_u16(0xc48390u+2*i,0);
 for(i=0;i<18;++i)wr_u16(0xc45bc6u+2*i,0);
 for(i=0;i<8;++i)wr_u16(0xc4b434u+2*i,0xffff);
 for(i=0;i<12;++i)wr_u16(0xc4b392u+2*i,0x4000);
 wr_u16(0xc45988u,0x4000);wr_u16(0xc458d8u,0x4000);
 if(selected_entry!=0xc1fb82u)for(i=0;i<128;++i)wr_u16(0xc60000u+2*i,0);
 if(selected_entry==0xc1f99au)wr_u8(0xc60007u,(uint8_t)(case_index%4));
#endif
 REG_PPC=0xc10024u;REG_A[7]=0xc7ff00u;expected_sp=REG_A[7]+4;wr_u32(REG_A[7],0xc70000u);
 if(selected_entry==0xc21500u||selected_entry==0xc2122au||selected_entry==0xc20592u||selected_entry==0xc2168au||selected_entry==0xc203d0u||selected_entry==0xc201a6u){REG_PPC=0xc1f942u;REG_A[0]=selected_entry;}
 m68k_set_reg(M68K_REG_SR,0x2700u|(p&31u));REG_PC=selected_entry;SET_CYCLES(1000000000);fa18_next_event=INT64_MAX;fa18_recomp_abort=0;
}
