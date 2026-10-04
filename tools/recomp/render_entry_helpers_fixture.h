/* Reuse sealed original hardware/input initialization; supply only these
 * entries' original argument domains and byte-backed incoming provenance. */
#define fixture render_entry_base_fixture
#include "render_leaf_helpers_fixture.h"
#undef fixture
static void fixture(unsigned p) {
 unsigned i,n=p>>5;uint32_t entry=selected_entry;
 if(entry==0xc301f6u)selected_entry=0xc301f0u;
 if(entry==0xc330feu)selected_entry=0xc32806u;
 render_entry_base_fixture(p);selected_entry=entry;REG_PC=entry;
 if(entry==0xc2f688u){
  unsigned rows=(n>>4)&1u;REG_A[3]=rows?0xc2f7c6u:0xc2f766u;REG_A[4]=rows?0xc2f7e6u:0xc2f786u;
  if(n&256u){REG_D[1]=(REG_D[1]&0xffff0000u)|1;wr_u16(0xc45954u,n&15u);wr_u16(0xc456e8u,0xffff);}
 }
 if(entry!=0xc2f688u && entry!=0xc2f63au && entry!=0xc2f64eu && entry!=0xc301f6u && entry!=0xc330feu){
  for(i=0;i<8;++i)REG_D[i]=random_value();
  for(i=0;i<4;++i)REG_A[i]=0x18000u+0x10000u*i+2*(n&7u);
  if(n&16u)for(i=1;i<4;++i)REG_A[i]=REG_A[0];
  if(n&32u){REG_A[1]=REG_A[0]+40;REG_A[3]=REG_A[2]+40;}
  for(i=0;i<4;++i){wr_u32(REG_A[i],random_value());wr_u32(REG_A[i]+40,random_value());}
 }
 switch(entry){
 case 0xc2f688u:REG_PPC=0xc2f5eau;break;
 case 0xc2f63au:REG_PPC=0xc31156u;break;
 case 0xc2f64eu:REG_PPC=0xc31160u;break;
 case 0xc301f6u:REG_PPC=0xc2fef4u;break;
 case 0xc330feu:REG_PPC=0xc32b6eu;break;
 case 0xc2f826u:REG_PPC=0xc2f764u;break;
 case 0xc2f83au:REG_PPC=0xc2f764u;break;
 case 0xc2f844u:REG_PPC=0xc2f764u;break;
 case 0xc2f84eu:REG_PPC=0xc2f764u;break;
 case 0xc2f858u:REG_PPC=0xc2f764u;break;
 case 0xc2f862u:REG_PPC=0xc2f764u;break;
 case 0xc2f86cu:REG_PPC=0xc2f764u;break;
 case 0xc2f876u:REG_PPC=0xc2f764u;break;
 case 0xc2f880u:REG_PPC=0xc2f764u;break;
 case 0xc2f88au:REG_PPC=0xc2f764u;break;
 case 0xc2f894u:REG_PPC=0xc2f764u;break;
 case 0xc2f89eu:REG_PPC=0xc2f764u;break;
 case 0xc2f8a8u:REG_PPC=0xc2f764u;break;
 case 0xc2f8b2u:REG_PPC=0xc2f764u;break;
 case 0xc2f8bcu:REG_PPC=0xc2f764u;break;
 case 0xc2f8c6u:REG_PPC=0xc2f764u;break;
 case 0xc2f8d0u:REG_PPC=0xc2f764u;break;
 case 0xc2f8eau:REG_PPC=0xc2f764u;break;
 case 0xc2f904u:REG_PPC=0xc2f764u;break;
 case 0xc2f91eu:REG_PPC=0xc2f764u;break;
 case 0xc2f938u:REG_PPC=0xc2f764u;break;
 case 0xc2f952u:REG_PPC=0xc2f764u;break;
 case 0xc2f96cu:REG_PPC=0xc2f764u;break;
 case 0xc2f986u:REG_PPC=0xc2f764u;break;
 case 0xc2f9a0u:REG_PPC=0xc2f764u;break;
 case 0xc2f9bau:REG_PPC=0xc2f764u;break;
 case 0xc2f9d4u:REG_PPC=0xc2f764u;break;
 case 0xc2f9eeu:REG_PPC=0xc2f764u;break;
 case 0xc2fa08u:REG_PPC=0xc2f764u;break;
 case 0xc2fa22u:REG_PPC=0xc2f764u;break;
 case 0xc2fa3cu:REG_PPC=0xc2f764u;break;
 case 0xc2fa56u:REG_PPC=0xc2f764u;break;
 default:abort();
 }
}
