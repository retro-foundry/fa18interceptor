/* Parent fixtures retain original candidate bytes and caller contracts. */
extern void fa18_hud_machine_restore(void);
static uint32_t expected_sp;
static void fixture(unsigned p){
 static const int16_t edges[]={0,1,-1,2,-2,7,-7,0x383f,0x3840,0x3841,32767,-32768,32766,-32767,256,-256};
 unsigned i,n=p>>5;
 fa18_hud_machine_restore();for(i=0;i<16;++i)REG_DA[i]=random_value();
 for(i=0;i<96;++i)wr_u32(0xc7fd80u+4*i,random_value());
 for(i=0;i<256;++i){wr_u32(0xc64000u+4*i,random_value());wr_u32(0xc65000u+4*i,random_value());}
 for(i=0;i<8;++i){gaddr at=0xc4b390u+16*i;unsigned k;
  for(k=0;k<8;++k)wr_u16(at+2*k,(uint16_t)edges[(n+i*3+k*5)&15]);
  wr_u16(at,40);wr_u16(at+2,20);wr_u16(at+4,128);wr_u32(0xc4b990u+8*i,random_value());
  wr_u16(0xc4e854u+2*i,(uint16_t)random_value());
 }
 for(i=0;i<18;++i)wr_u16(0xc45bd8u+2*i,(uint16_t)edges[(n+i*7)&15]);
 wr_u32(0xc45a66u,random_value());wr_u8(0xc45785u,(uint8_t)((n/32)&1));wr_u16(0xc458cau,(uint16_t)(((n/16)&1)?2:0));
 wr_u16(0xc45a92u,(uint16_t)edges[(n/3)&15]);wr_u16(0xc45a94u,(uint16_t)edges[(n/5)&15]);wr_u16(0xc45a8au,(uint16_t)edges[(n/7)&15]);
 wr_u32(0xc45ac6u,0x00280014u);wr_u16(0xc45acau,128);
#ifdef HP_ORIGINAL_CHILDREN
 /* All original corner endpoints have positive depth. Original C2EA02
  * non-returning behavior is separately proven by the corner/view suite. */
 for(i=1;i<8;i+=2){gaddr at=0xc4b390u+16*i;
  wr_u16(at,(uint16_t)((int)(random_value()%401)-200));
  wr_u16(at+2,(uint16_t)((int)(random_value()%401)-200));
 }
 for(i=0;i<2;++i){gaddr matrix=0xc45bd8u+18*i;
  static const int16_t scale[]={0,1,4,8};
  wr_u16(matrix,(uint16_t)((n&4)?scale[n&3]:-scale[n&3]));
  wr_u16(matrix+2,(uint16_t)(((int)((n/16)%5)-2)*128));wr_u16(matrix+4,0);
  wr_u16(matrix+6,0);wr_u16(matrix+8,(uint16_t)(((int)((n/7)%5)-2)*128));
  wr_u16(matrix+10,(uint16_t)((n&1)?scale[(n/3)&3]:-scale[(n/3)&3]));
  wr_u16(matrix+12,0);wr_u16(matrix+14,(uint16_t)((n&8)?128:256));wr_u16(matrix+16,0);
 }
 wr_u32(0xc45a66u,0x02000000u);
#endif
 REG_D[0]=(REG_D[0]&0xffff0000u)|(uint16_t)edges[n&15];REG_D[1]=(REG_D[1]&0xffff0000u)|(uint16_t)edges[(n/3)&15];
 REG_A[0]=0xc64000u;REG_A[1]=0xc65000u;REG_A[2]=0xc64080u;REG_A[3]=0xc65080u;REG_A[4]=0xc640a0u;REG_A[5]=0xc650a0u;REG_A[6]=0xc650c0u;
 REG_A[7]=0xc7fe00u;expected_sp=REG_A[7]+4;wr_u32(REG_A[7],0xc70000u);
 m68k_set_reg(M68K_REG_SR,0x2700u|(p&31u));REG_PC=selected_entry;
 switch(selected_entry){
 case 0xc0d74au:REG_PPC=0xc2ff1au;break;
 case 0xc0d752u:REG_PPC=0xc2feecu;break;
 case 0xc0daa0u:REG_PPC=0xc0d890u;break;
 case 0xc0dad0u:REG_PPC=0xc0d8a6u;break;
 case 0xc0dad4u:REG_PPC=0xc0d8a2u;break;
 case 0xc0dadcu:REG_PPC=0xc0d894u;break;
 case 0xc0dae6u:REG_PPC=0xc0d898u;break;
 }
 fa18_next_event=INT64_MAX;SET_CYCLES(1000000000);fa18_recomp_abort=0;
}
