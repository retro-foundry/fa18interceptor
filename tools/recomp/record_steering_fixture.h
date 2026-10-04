/* Original BSR callers, arbitrary CPU high words and signed word boundaries. */
extern void fa18_hud_machine_restore(void);
static uint32_t expected_sp;
static void fixture(unsigned p){
 static const int16_t edges[]={0,1,-1,0x4f,0x50,0x51,0x18f,0x190,0x191,
  0x383f,0x3840,0x3841,0x6eef,0x6ef0,0x6ef1,0x702f,0x7030,0x7031,32767,-32768,-32767};
 unsigned i,n=p>>5;gaddr r=0xc46184u+((n/21)%4)*512;
 fa18_hud_machine_restore();for(i=0;i<16;++i)REG_DA[i]=random_value();
 for(i=0;i<128;++i)wr_u32(r+4*i,random_value());
 for(i=0;i<96;++i)wr_u32(0xc7fd80u+4*i,random_value());
 wr_u8(r+0x64,(uint8_t)(n/21));wr_u8(r+0x65,(uint8_t)random_value());
 wr_u16(r+0x56,(uint16_t)edges[(n/3)%21]);wr_u16(r+0x58,(uint16_t)edges[(n/7)%21]);
 wr_u16(r+0x6a,(uint16_t)edges[n%21]);
 REG_D[3]=(REG_D[3]&0xffff0000u)|(uint16_t)edges[(n/5)%21];
 if(n>=256){
  static const int16_t turns[]={-32768,-1,0,1,32767};
  static const int16_t limits[]={-32768,-1,0,1,32767};unsigned k=n-256;
  wr_u8(r+0x64,(uint8_t)((k%3)==0?0x60:(k%3)==1?0x80:0));
  wr_u16(r+0x6a,(uint16_t)edges[(k/3)%21]);
  REG_D[3]=(REG_D[3]&0xffff0000u)|(uint16_t)turns[(k/63)%5];
  wr_u16(r+0x58,(uint16_t)limits[(k/7)%5]);
 }
 REG_A[1]=r;REG_A[7]=0xc7fe00u;expected_sp=REG_A[7]+4;wr_u32(REG_A[7],0xc70000u);
 m68k_set_reg(M68K_REG_SR,0x2700u|(p&31u));REG_PC=selected_entry;
 switch(selected_entry){
 case 0xc2ca26u:REG_PPC=0xc2c9eau;break;
 case 0xc2ca92u:REG_PPC=0xc2c260u;break;
 case 0xc2caa0u:REG_PPC=0xc2c268u;break;
 case 0xc2cb86u:REG_PPC=0xc2cb5eu;break;
 case 0xc2cb82u:REG_PPC=0xc2bfc2u;break;
 case 0xc2cbbcu:REG_PPC=0xc2bd32u;break;
 }
 fa18_next_event=INT64_MAX;SET_CYCLES(1000000000);fa18_recomp_abort=0;
}
