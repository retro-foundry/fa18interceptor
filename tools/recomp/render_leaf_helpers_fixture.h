/* Source tables are immutable. Whole glyph fixtures use original font row
 * counts; the domain retains the full DBRA zero-count behavior. */
extern void fa18_hud_machine_restore(void);
static uint32_t expected_sp;
static void fixture(unsigned p) {
 static const uint16_t edge[]={0,1,2,15,16,31,127,199,200,319,320,0xffff,0x7fff,0x8000,0xfffe,8};
 unsigned i,n=p>>5;gaddr planes=0xc62000u;
 fa18_hud_machine_restore();for(i=0;i<15;++i)REG_DA[i]=random_value();
 for(i=0;i<96;++i)wr_u32(0xc7fd80u+4*i,random_value());
 for(i=0;i<256;++i){wr_u32(0xc64000u+4*i,random_value());wr_u32(0xc65000u+4*i,random_value());}
 for(i=0;i<4;++i){wr_u32(planes+4*i,0x18000u+0x10000u*i);}
 for(i=0;i<1024;++i)wr_u32(0x18000u+4*i,random_value());
 wr_u32(0xc456b6u,planes);wr_u32(0xc456e2u,0x20000u);
 wr_u16(0xc45954u,(uint16_t)((n/3)&15));wr_u8(0xc456e7u,(uint8_t)((n/7)&15));
 wr_u16(0xc456e8u,(n&16)?0xffffu:0);wr_u8(0xc456ebu,(uint8_t)((n/11)&15));wr_u8(0xc456e9u,(uint8_t)((n/13)&15));
 wr_u16(0xc45988u,edge[(n/5)&15]);wr_u16(0xc458d8u,edge[(n/9)&15]);wr_u16(0xc45984u,(uint16_t)(n%6+1));
 wr_u16(0xc45986u,edge[(n/7)&15]);wr_u16(0xc4596eu,0x42);wr_u32(0xc45960u,0x18000u);
 REG_A[0]=0xc64000u;REG_A[1]=planes;REG_A[2]=0xc65000u;REG_A[3]=0xc2f766u;REG_A[4]=0xc2f786u;REG_A[5]=0xc66000u;REG_A[6]=0xc650c0u;
 REG_D[0]=(REG_D[0]&0xffff0000u)|edge[n&15];REG_D[1]=(REG_D[1]&0xffff0000u)|edge[(n/16)&15];
 REG_D[2]=(REG_D[2]&0xffff0000u)|edge[(n/3)&15];REG_D[3]=(REG_D[3]&0xffff0000u)|edge[(n/5)&15];
 if(selected_entry==0xc310e2u){REG_A[4]=edge[(n/11)&15];REG_D[7]=(REG_D[7]&0xffff0000u)|edge[n&15];}
 if(selected_entry==0xc32806u){
  REG_D[1]=0x18000u;REG_D[4]=0xc64000u;REG_D[2]=(REG_D[2]&0xffff0000u)|((n&15)<<12)|(((n/16)&15)<<4);REG_D[7]=(REG_D[7]&0xffff0000u)|((n%8+1)<<6);
 }
 if(selected_entry==0xc2fa78u||selected_entry==0xc2fa7eu){
  static const int16_t x[]={-320,-1,0,1,2,15,16,31,159,319,320,321,8,127,255,300};
  static const int16_t y[]={-1,0,1,2,3,7,15,31,99,198,199,200,201,8,127,159};
  REG_D[0]=(REG_D[0]&0xffff0000u)|(uint16_t)x[n&15];REG_D[1]=(REG_D[1]&0xffff0000u)|(uint16_t)y[(n/16)&15];
  REG_D[2]=(REG_D[2]&0xffff0000u)|(uint16_t)x[(n/3)&15];REG_D[3]=(REG_D[3]&0xffff0000u)|(uint16_t)y[(n/5)&15];
 }
 if(selected_entry==0xc301f0u){
  wr_u16(0xc4b390u,(uint16_t)(3+n%4));
  for(i=0;i<7;++i){wr_u16(0xc4b392u+4*i,(uint16_t)((n%20)+(i%3)*((n/20)%5)));wr_u16(0xc4b394u+4*i,(uint16_t)((n%12)+(i%3)*((n/30)%5)));}
  wr_u8(0xc457a2u,(n&16)?1:0);
  if(n&128u){
   static const int16_t point[][2]={{0,0},{8,0},{0,8},{8,8},{-8,-8},{-1,0},{2,1},{1,2},{320,200},{-32768,32767},{32767,-32768},{2,2}};
   for(i=0;i<7;++i){unsigned k=(n+5*i)%12;wr_u16(0xc4b392u+4*i,(uint16_t)point[k][0]);wr_u16(0xc4b394u+4*i,(uint16_t)point[k][1]);}
#ifdef HP_ORIGINAL_CHILDREN
   for(i=0;i<7;++i){unsigned k=(n+5*i)%12;if(k==9||k==10){wr_u16(0xc4b392u+4*i,k==9?16:7);wr_u16(0xc4b394u+4*i,k==9?7:16);}}
#endif
   if((n&64u)&&!(n&256u))for(i=0;i<7;++i)wr_u16(0xc4b394u+4*i,(uint16_t)(200+i%3));
  }
  if(n&256u){
   int16_t x=(int16_t)(n%7-3),y=(int16_t)(n%5-2);unsigned dx=(n/7)%4,dy=(n/28)%4;
   wr_u16(0xc4b390u,3);
   for(i=0;i<3;++i){unsigned k=(n&64)?2-i:i;wr_u16(0xc4b392u+4*i,(uint16_t)(x+(k%2)*dx));wr_u16(0xc4b394u+4*i,(uint16_t)(y+(k%2)*dy));}
   wr_u8(0xc457a2u,(n&32)?1:0);
  }
 }
 if(selected_entry==0xc2f66eu && (n&256u)){REG_D[1]=(REG_D[1]&0xffff0000u)|1u;wr_u16(0xc45984u,199);wr_u16(0xc45954u,n&15);wr_u16(0xc456e8u,0xffff);}
 if(selected_entry==0xc2fd8cu){
  wr_u8(0xc4589bu,(n&1)?1:0);wr_u8(0xc45785u,(n&2)?1:0);
  for(i=0;i<3;++i)wr_u32(0xc4591cu+4*i,((uint32_t)edge[(n+i)&15]<<16)|edge[(n/3+i)&15]);
 }
 /* Byte-backed original call sites, sealed in the scope inventory. */
 switch(selected_entry){
 case 0xc2f5c0u:REG_PPC=0xc30142u;break;case 0xc2f5d4u:REG_PPC=0xc30166u;break;
 case 0xc2f5f4u:REG_PPC=0xc27cfcu;break;case 0xc2f60au:REG_PPC=0xc27d04u;break;
 case 0xc2f66eu:REG_PPC=0xc1ca20u;break;case 0xc310e2u:REG_PPC=0xc30078u;break;
 case 0xc301f0u:REG_PPC=0xc301dcu;break;case 0xc304b2u:REG_PPC=0xc3002au;break;
 case 0xc32806u:REG_PPC=0xc327eau;break;case 0xc2fa78u:REG_PPC=0xc30136u;break;
 case 0xc2fa7eu:REG_PPC=0xc2bae0u;break;case 0xc2fd8cu:REG_PPC=0xc0d73cu;break;
 default:REG_PPC=selected_entry;break;
 }
 REG_A[7]=0xc7ff00u;expected_sp=REG_A[7]+4;wr_u32(REG_A[7],0xc70000u);
 m68k_set_reg(M68K_REG_SR,0x2700u|(p&31u));REG_PC=selected_entry;SET_CYCLES(1000000000);fa18_next_event=INT64_MAX;fa18_recomp_abort=0;
}
