/* Actual static incoming sites are sealed in corner_view_source_scope.json.
 * The four rejection peers enter at the original shared return labels. */
extern void fa18_hud_machine_restore(void);
static uint32_t expected_sp;
static void fixture(unsigned p) {
 static const int16_t edge[]={0,1,-1,2,-2,7,-7,100,-100,320,-320,32767,-32768,32766,-32767,16};
 unsigned i,n=p>>5;uint32_t entry=selected_entry;gaddr r;
 fa18_hud_machine_restore();for(i=0;i<15;++i)REG_DA[i]=random_value();
 for(i=0;i<96;++i)wr_u32(0xc7fd80u+4*i,random_value());
 for(i=0;i<256;++i){wr_u32(0xc64000u+4*i,random_value());wr_u32(0xc65000u+4*i,random_value());}
 for(i=0;i<4;++i)wr_u32(0xc62000u+4*i,0x18000u+0x10000u*i);
 for(i=0;i<1024;++i)wr_u32(0x18000u+4*i,random_value());
 wr_u32(0xc456b6u,0xc62000u);wr_u32(0xc456e2u,0x20000u);wr_u16(0xc45954u,n&15);wr_u16(0xc456e8u,(n&16)?0xffffu:0);
 wr_u8(0xc456ebu,(uint8_t)((n/11)&15));wr_u8(0xc456e9u,(uint8_t)((n/13)&15));
 wr_u16(0xc45988u,200);wr_u16(0xc458d8u,0);wr_u16(0xc45984u,199);wr_u16(0xc45986u,320);
 wr_u16(0xc4596eu,0x42);wr_u32(0xc45960u,0x18000u);
 REG_A[0]=0xc64000u;REG_A[1]=0xc63000u;REG_A[2]=0xc64000u;REG_A[3]=0xc65000u;REG_A[4]=0xc65080u;REG_A[5]=0xc66000u;REG_A[6]=0xc650c0u;
 for(i=0;i<9;++i)wr_u16(0xc45bd8u+2*i,(uint16_t)((n&1)?edge[(n+i*3)&15]:((i%4)==0?256:0)));
 for(i=0;i<3;++i)wr_u16(0xc45a72u+2*i,(uint16_t)edge[(n/3+i*5)&15]);
 wr_u16(0xc4594cu,(uint16_t)edge[(n/7)&15]);wr_u16(0xc4594eu,(uint16_t)edge[(n/11)&15]);
 wr_u32(0xc45a66u,(n&2)?random_value():0);wr_u32(0xc45a78u,(n&4)?random_value():0);
 for(i=0;i<8;++i){
  gaddr at=0xc4b390u+16*i;unsigned k;
  for(k=0;k<8;++k)wr_u16(at+2*k,(uint16_t)edge[(n+i*3+k*5)&15]);
  if(n&1){wr_u16(at,(uint16_t)((int)(random_value()%801)-400));wr_u16(at+2,(uint16_t)((int)(random_value()%801)-400));wr_u16(at+4,(uint16_t)(random_value()%200+1));}
#ifdef HP_ORIGINAL_CHILDREN
  /* Returning positive-depth cone inputs are independent of the separately
   * observed original C2EA02 non-returning domain. */
  wr_u16(at,(uint16_t)((int)(random_value()%401)-200));wr_u16(at+2,(uint16_t)((int)(random_value()%401)-200));wr_u16(at+4,100);
#endif
  wr_u32(0xc4b990u+8*i,random_value());wr_u16(0xc4e854u+2*i,(uint16_t)random_value());
 }
 wr_u32(0xc45ac6u,random_value());wr_u16(0xc45acau,(uint16_t)edge[n&15]);
 REG_D[3]=(REG_D[3]&0xffff0000u)|(uint16_t)edge[n&15];REG_D[4]=(REG_D[4]&0xffff0000u)|(uint16_t)edge[(n/3)&15];REG_D[5]=(REG_D[5]&0xffff0000u)|(uint16_t)edge[(n/7)&15];
 REG_A[7]=0xc7fe00u;expected_sp=REG_A[7]+4;wr_u32(REG_A[7],0xc70000u);wr_u32(REG_A[7]+4,n&3);
 r=0xc45c72u+(n&3)*64;
 for(i=0;i<32;++i)wr_u16(r+2*i,(uint16_t)edge[(n+i*7)&15]);
 wr_u32(r,((uint32_t)(uint16_t)edge[n&15])<<8);wr_u32(r+4,((uint32_t)(uint16_t)edge[(n/3)&15])<<8);wr_u32(r+8,((uint32_t)(uint16_t)edge[(n/7)&15])<<8);
 wr_u16(r+0x28,(uint16_t)((n&1)?0x40:(n%64)));
 if(entry==0xc2d082u||entry==0xc2d3a4u){
  static const uint32_t longs[]={0,1,0xffffffffu,0x4000,0x4001,0x40000,0x40001,0x80000000u,0x7fffffffu,0x8000,0x8001,0x10000};
  REG_D[0]=longs[n%12];REG_D[1]=longs[(n/3)%12];REG_D[2]=longs[(n/7)%12];REG_D[3]=(REG_D[3]&0xffff0000u)|(uint16_t)edge[(n/13)&15];
  REG_D[4]=(REG_D[4]&0xffff0000u)|(uint16_t)(n%8);REG_D[7]=(n&15)|((n%128)<<16);wr_u32(0xc45a78u,0);
 }
 if(entry==0xc200f6u){wr_u32(REG_A[7],random_value());wr_u32(REG_A[7]+4,random_value());wr_u32(REG_A[7]+8,0xc70000u);expected_sp=REG_A[7]+12;wr_u16(REG_A[6]-0x7e,(uint16_t)edge[n&15]);}
 m68k_set_reg(M68K_REG_SR,0x2700u|(p&31u));REG_PC=entry;
 switch(entry){
 case 0xc2e758u:REG_PPC=0xc0d7dau;break;
 case 0xc2ce82u:REG_PPC=0xc0d1f4u;break;
 case 0xc2cca0u:REG_PPC=0xc1562au;break;
 case 0xc2cd28u:REG_PPC=0xc15618u;break;
 case 0xc2cd94u:REG_PPC=0xc155fau;break;
 case 0xc2d082u:REG_PPC=0xc1f8d2u;break;
 case 0xc2d3a4u:REG_PPC=0xc2d0b6u;break;
 case 0xc200f6u:REG_PPC=0xc20112u;break;
 case 0xc203ccu:REG_PPC=0xc203e8u;break;
 case 0xc2058eu:REG_PPC=0xc205a2u;break;
 case 0xc20826u:REG_PPC=0xc20840u;break;
 case 0xc22c70u:REG_PPC=0xc22af2u;break;
 }
 fa18_next_event=INT64_MAX;SET_CYCLES(1000000000);fa18_recomp_abort=0;
}
