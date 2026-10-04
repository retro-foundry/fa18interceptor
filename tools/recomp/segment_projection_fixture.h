/* Byte-backed entry contracts. Returning fixtures exercise all SR bits and
 * source-width edge values. The unrounded zero-denominator loops have a
 * separate non-returning proof; they are never replaced by a fixture guard. */
extern void fa18_hud_machine_restore(void);
static uint32_t expected_sp;
static void fixture(unsigned p) {
 static const int16_t edge[]={0,1,-1,2,-2,7,-7,100,-100,320,-320,32767,-32768,32766,-32767,16};
 unsigned i,n=p>>5;int16_t point[6];int axis,side,rounded;uint32_t entry=selected_entry;gaddr at;
 fa18_hud_machine_restore();for(i=0;i<15;++i)REG_DA[i]=random_value();
 for(i=0;i<96;++i)wr_u32(0xc7fd80u+4*i,random_value());
 for(i=0;i<256;++i){wr_u32(0xc64000u+4*i,random_value());wr_u32(0xc65000u+4*i,random_value());}
 for(i=0;i<4;++i)wr_u32(0xc62000u+4*i,0x18000u+0x10000u*i);
 for(i=0;i<1024;++i)wr_u32(0x18000u+4*i,random_value());
 wr_u32(0xc456b6u,0xc62000u);wr_u32(0xc456e2u,0x20000u);
 wr_u16(0xc45954u,(uint16_t)(n&15));wr_u16(0xc456e8u,(n&16)?0xffffu:0);
 wr_u8(0xc456ebu,(uint8_t)((n/11)&15));wr_u8(0xc456e9u,(uint8_t)((n/13)&15));
 wr_u16(0xc45988u,200);wr_u16(0xc458d8u,0);wr_u16(0xc45984u,199);wr_u16(0xc45986u,320);
 wr_u16(0xc4596eu,0x42);wr_u32(0xc45960u,0x18000u);
 REG_A[0]=0xc64000u;REG_A[1]=0xc63000u;REG_A[2]=0xc64000u;REG_A[3]=0xc48390u;REG_A[4]=0xc65000u;REG_A[5]=0xc66000u;REG_A[6]=0xc650c0u;
 for(i=0;i<6;++i)point[i]=edge[(n*(2*i+1)+n/16+i*3)&15];
 /* Half the parent rows are ordinary inside/outside segments; retain the
  * full-word edge cases in the other half, including wrapped NEG $8000. */
 if(n&1u){for(i=0;i<6;++i)point[i]=(int16_t)((int)(random_value()%801)-400);point[2]=(int16_t)(n%200+1);point[5]=(int16_t)((n/3)%200+1);}
 if((n&7u)==0){
  static const int16_t pairs[][6]={
   {0,0,100,0,0,100},{100,100,100,-100,-100,100},
   {-32768,0,320,0,0,100},{0,-32768,320,0,0,100},
   {0,0,100,-32768,0,320},{0,0,100,0,-32768,320},
   {0,0,100,100,100,100},{1,2,-1,3,4,-2},
   {400,400,100,-10,20,200},{-400,-400,100,10,-20,200},
   {400,-400,100,-10,20,200},{-400,400,100,10,-20,200},
   {0,400,100,0,0,200},{0,-400,100,0,0,200},
   {100,100,100,0,0,0},{0,0,0,100,100,100}
  };
  for(i=0;i<6;++i)point[i]=pairs[(n/8)&15][i];
 }
#ifdef HP_ORIGINAL_CHILDREN
 /* The original clipped decision chain can enter its infinite branch with
  * wrapped extreme coordinates. Keep those non-returning inputs separate
  * from the completed-call domain; the native function retains the loop. */
 if(entry==0xc1ffa4u||entry==0xc2ee4au)for(i=0;i<6;++i)
  if(point[i]>400||point[i]<-400)point[i]=(int16_t)(point[i]%401);
#endif
 for(i=0;i<6;++i)wr_u16(0xc4c592u+2*i,(uint16_t)point[i]);
 wr_u32(0xc45ac6u,random_value());wr_u16(0xc45acau,(uint16_t)edge[(n/7)&15]);
 wr_u32(0xc45a78u,(n&2)?0xffffff3fu:0xffffff40u);
 wr_u16(REG_A[2],0);wr_u16(REG_A[2]+2,6);wr_u16(REG_A[2]+4,(uint16_t)(n&15));
 for(i=0;i<6;++i)wr_u16(0xc48390u+2*i,(uint16_t)point[i]);
 if(entry==0xc1ff9cu||entry==0xc1ffa4u||entry==0xc2ed70u||entry==0xc2ee4au)goto parent;
 rounded=entry<0xc2ed70u;axis=entry==0xc2f128u||entry==0xc2f156u||entry==0xc2eb4cu||entry==0xc2ebc2u;
 side=(entry==0xc2f0f4u||entry==0xc2f156u||entry==0xc2ead0u||entry==0xc2ebc2u)?-1:1;
 if(rounded && (n&15u)==0)point[5]=(int16_t)(point[2]+side*(point[3+axis]-(int16_t)(point[axis]-1)));
 REG_D[1]=(REG_D[1]&0xffff0000u)|(uint16_t)((int)(n%9)-4)*2u;
 for(i=0;i<3;++i)REG_D[3+i]=(REG_D[3+i]&0xffff0000u)|(uint16_t)point[i];
 /* Choose returning input pairs. Zero-denominator original loops are
  * proven independently and retained unchanged in production. */
 if(!rounded && (uint16_t)(side*(point[axis]-point[3+axis])+point[5]-point[2])==0)point[5]=(int16_t)(point[5]+1);
 at=REG_A[1]+(rounded?(gaddr)(int32_t)(int16_t)REG_D[1]:6u);
 for(i=0;i<3;++i)wr_u16(at+2*i,(uint16_t)point[3+i]);
parent:
 /* Original actual incoming calls are sealed in the source inventory. */
 switch(entry){
 case 0xc1ff9cu:REG_PPC=0xc1f942u;break;case 0xc1ffa4u:REG_PPC=0xc0984au;break;
 case 0xc2ed70u:case 0xc2ee4au:REG_PPC=0xc1fff6u;break;
 case 0xc2f0c6u:REG_PPC=0xc2ee74u;break;case 0xc2f0f4u:REG_PPC=0xc2ee8au;break;
 case 0xc2f128u:REG_PPC=0xc2ef26u;break;case 0xc2f156u:REG_PPC=0xc2eee8u;break;
 case 0xc2ea5au:REG_PPC=0xc2e7d0u;break;case 0xc2ead0u:REG_PPC=0xc2e7e6u;break;
 case 0xc2eb4cu:REG_PPC=0xc2e870u;break;case 0xc2ebc2u:REG_PPC=0xc2e852u;break;
 default:abort();
 }
 REG_A[7]=0xc7ff00u;expected_sp=REG_A[7]+4;wr_u32(REG_A[7],0xc70000u);
 m68k_set_reg(M68K_REG_SR,0x2700u|(p&31u));REG_PC=entry;SET_CYCLES(1000000000);fa18_next_event=INT64_MAX;fa18_recomp_abort=0;
}
