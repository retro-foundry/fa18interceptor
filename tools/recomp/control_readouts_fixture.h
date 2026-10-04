/* Whole-call inputs use the original caller/argument contract. Shared branch
 * stubs are reached through their original bounded C131BE selectors. */
extern void fa18_hud_machine_restore(void);
static uint32_t expected_sp;
static void fixture(unsigned p){
 static const int16_t edges[]={0,1,-1,2,-2,7,8,9,63,120,127,-128,32767,-32768,32766,-32767};
 static const uint32_t longs[]={0,1,0xffffffffu,2,0xfffffffeu,0x80000000u,0x7fffffffu,0x10000u,0xffff0000u,0x8000u,0xffff8000u,0x40000000u};
 static const uint8_t selectors[]={0,1,2,3,4,5,6,7,8,9,127,128,255};
 static const uint32_t pending[]={0,0x40,0x80,1,4,0x100,16,2,32,0x800,8,0xabcdef00u,0x200};
 unsigned i,n=p>>5;gaddr r=0xc46184u+(n%4)*512;
 fa18_hud_machine_restore();for(i=0;i<16;++i)REG_DA[i]=random_value();
 for(i=0;i<96;++i)wr_u32(0xc7fd80u+4*i,random_value());
 for(i=0;i<4;++i){unsigned k;gaddr rec=0xc46184u+i*512;
  for(k=0;k<256;++k)wr_u16(rec+k*2,(uint16_t)edges[(n+k*3+i*5)%16]);
  wr_u8(rec+0x2b,(uint8_t)edges[(n/3+i)%16]);wr_u8(rec+0x62,(uint8_t)((n%7==0)?0x30:0x20));
  wr_u8(rec+0x7c,(uint8_t)((n%9==0)?1:0));wr_u16(rec+2,(uint16_t)((((n/5)&1)?8:0)|(((n/5)&2)?128:0)));
 }
 wr_u32(0xc18210u,r);wr_u16(0xc458dcu,n%4);
 wr_u8(0xc458aeu,(uint8_t)(n%31==0));wr_u8(0xc457adu,(uint8_t)(n%37==0));wr_u8(0xc457aeu,(uint8_t)(n%41==0));wr_u8(0xc45795u,(uint8_t)(n%43!=0));
 wr_u8(0xc458b0u,(uint8_t)((n/13)%8==0?0:0xffu-((n/13)%8-1)));
 wr_u8(0xc45885u,selectors[(n/7)%13]);wr_u8(0xc4588au,(uint8_t)((n%19==0)?3:((n%23==0)?0xff:0)));
 wr_u32(0xc45b54u,pending[n%13]);wr_u8(0xc457b8u,selectors[(n/11)%13]);
 wr_u8(0xc4586au,(uint8_t)edges[n%16]);wr_u8(0xc45869u,(uint8_t)edges[(n/3)%16]);
 wr_u8(0xc45785u,selectors[(n/3)%13]);wr_u8(0xc457b5u,(uint8_t)(n&1));wr_u8(0xc458b2u,selectors[(n/5)%13]);
 wr_u8(0xc45797u,selectors[(n/17)%13]);wr_u8(0xc457c1u,(uint8_t)((n/19)%4));
 wr_u16(0xc45b42u,(uint16_t)edges[(n/7)%16]);wr_u8(0xc45889u,selectors[(n/23)%13]);
 /* These flags belong to the real sound setup. Original child fixtures use
  * the sealed voice state, not an invented sound completion. */
 wr_u8(0xc45b5au,(uint8_t)((n&1)?2:0));wr_u8(0xc45b5bu,(uint8_t)((n/2)&3));
 if(selected_entry==0xc12950u&&n>=384){
  /* Independent valid caller inputs for combinations that the broad edge
   * matrix's periods do not reach: every action in/out of view, record bit
   * three, countdown, and the original caller-visible stack write. */
  wr_u8(0xc458aeu,0);wr_u8(0xc457adu,0);wr_u8(0xc457aeu,0);wr_u8(0xc45795u,1);
  wr_u8(0xc45885u,0);wr_u32(0xc45b54u,0);wr_u8(0xc457b8u,0);wr_u8(0xc457c1u,0);
  wr_u8(0xc458b0u,(uint8_t)(0xffu-((n/2)%7)));wr_u8(0xc45785u,(uint8_t)((n/16)&1));wr_u8(0xc45797u,(uint8_t)((n/4)%3));
  wr_u8(r+0x62,0x20);wr_u16(r+2,(uint16_t)((n&1)?8:0));wr_u8(r+0x2b,(uint8_t)(n%2?120:8));
  if(n>=480){wr_u8(0xc458b0u,0xff);wr_u8(0xc45785u,0);wr_u8(0xc45797u,0);wr_u8(0xc45b5bu,1);wr_u16(r+2,8);wr_u8(r+0x2b,120);}
 }
 REG_A[7]=0xc7fe00u;expected_sp=REG_A[7]+4;wr_u32(REG_A[7],0xc70000u);
 wr_u32(REG_A[7]+4,(uint32_t)(int32_t)edges[n%16]);wr_u32(REG_A[7]+8,(uint32_t)(int32_t)edges[(n/3)%16]);
 REG_D[0]=longs[n%12];REG_D[1]=longs[(n/3)%12];
 m68k_set_reg(M68K_REG_SR,0x2700u|(p&31u));REG_PC=selected_entry;
 switch(selected_entry){
 case 0xc12950u:REG_PPC=0xc0f132u;break;
 case 0xc131beu:REG_PPC=0xc12d3cu;break;
 case 0xc13176u:REG_PPC=0xc12eccu;break;
 case 0xc133b2u:REG_PPC=0xc13112u;break;
 case 0xc13396u:REG_PPC=0xc13266u;break;
 case 0xc52ec8u:REG_PPC=0xc1331au;break;
 }
 fa18_next_event=INT64_MAX;SET_CYCLES(1000000000);fa18_recomp_abort=0;
}
