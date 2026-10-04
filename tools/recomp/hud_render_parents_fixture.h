/* Original source bytes and tables remain authoritative. Counts and private
 * geometry are bounded for whole-call proofs; no guard is added to the game. */
extern void fa18_hud_machine_restore(void);
static uint32_t expected_sp;
static void fixture(unsigned p) {
 static const uint16_t words[]={0,1,0xffff,0x8000,0x7fff,8,15,16,31,32,63,127,128,255,256,0x600};
 static const int16_t origins[]={-320,-160,-1,0,1,160,320,32767,-32768};
 static const int16_t xs[]={-1,0,1,2,5,14,15,88,96,99,141,157,159,173,180,219,222,230,306,317,318,319,320,32767,-32768};
 static const int16_t ys[]={-1,0,1,39,45,46,57,71,90,91,129,132,134,141,144,172,32767,-32768};
 static const int8_t ring[]={-128,0,5,0,4,2,2,4,0,5,0,-128};
 unsigned i,n=p>>5;gaddr frame=0xc650c0u,stream=0xc64000u;
 fa18_hud_machine_restore();for(i=0;i<15;++i)REG_DA[i]=random_value();
 for(i=0;i<96;++i)wr_u32(0xc7fd80u+4*i,random_value());
 for(i=0;i<128;++i){wr_u32(0xc63000u+4*i,random_value());wr_u32(0xc65000u+4*i,random_value());}
 REG_A[0]=0xc60002u;REG_A[1]=0xc62000u;REG_A[2]=stream;REG_A[3]=0xc48390u;REG_A[4]=0xc61000u;REG_A[5]=0xc66000u;REG_A[6]=frame;
 for(i=0;i<128;++i)wr_u16(stream+2*i,0);
 for(i=0;i<128;++i)wr_u16(0xc60000u+2*i,words[(n+i)&15]);
 for(i=0;i<sizeof ring;++i)wr_u8(0xc60000u+i,(uint8_t)ring[i]);
 for(i=0;i<256;++i)wr_u16(0xc48390u+2*i,(i%3==2 && (n&8))?0xffffu:0);
#ifndef HP_ORIGINAL_CHILDREN
 if(!(n&8))for(i=0;i<256;++i)wr_u16(0xc48390u+2*i,words[(n+i/3)&15]);
#endif
 wr_u16(0xc45988u,(uint16_t)origins[n%9]);wr_u16(0xc458d8u,(uint16_t)origins[(n/9)%9]);wr_u16(0xc458deu,0);wr_u16(0xc459b6u,0);
 REG_D[0]=(REG_D[0]&0xffff0000u)|(uint16_t)xs[(n/9)%25];REG_D[1]=(REG_D[1]&0xffff0000u)|(uint16_t)ys[(n/7)%18];REG_D[2]=(REG_D[2]&0xffff0000u)|(uint16_t)(n%20);REG_D[4]=(REG_D[4]&0xffff0000u)|(n&1u);
 wr_u16(stream,(uint16_t)(4+n%3));
 if(selected_entry==0xc09952u){for(i=0;i<6;++i)wr_u16(stream+2+2*i,(uint16_t)(6*i));wr_u16(stream+2+2*(4+n%3),(uint16_t)(n%16));}
 if(selected_entry==0xc099aau)wr_u16(stream+2,(uint16_t)(n%16));
 if(selected_entry==0xc099f6u)wr_u16(stream+2,(uint16_t)(6*(n%3)));
 if(selected_entry==0xc098c6u){REG_D[0]=(REG_D[0]&0xffff0000u)|((n%6==4)?0xffffu:(n%6==5)?0x8000u:n%6);REG_D[7]=(REG_D[7]&0xffff0000u)|(uint16_t)(4*(n%4));wr_u32(0xc45a32u,0xc62000u);for(i=0;i<32;++i)wr_u16(0xc62000u+2*i,words[(n+i)&15]);for(i=0;i<9;++i)wr_u16(0xc45bd8u+2*i,words[(n+3*i)&15]);for(i=0;i<6;++i)wr_u16(frame-0x86+2*i,words[(n+i)&15]);wr_u16(frame-8,words[(n/3)&15]);wr_u16(frame-0x78,words[(n+1)&15]);wr_u16(frame-0x76,words[(n+2)&15]);wr_u16(frame-0x74,words[(n+3)&15]);wr_u16(0xc45b2au,words[(n+4)&15]);wr_u16(0xc45b2eu,words[(n+5)&15]);}
 wr_u16(0xc4598cu,(uint16_t)xs[(n/3)%25]);wr_u16(0xc459c0u,(n&2)?0xffffu:0);wr_u16(0xc45942u,(uint16_t)xs[(n/9)%25]);wr_u16(0xc45944u,(uint16_t)ys[(n/7)%18]);wr_u16(0xc4593au,(uint16_t)(xs[(n/9)%25]+(int)(n%15)-7));wr_u16(0xc4593cu,(uint16_t)(ys[(n/7)%18]+(int)((n/15)%15)-7));wr_u16(0xc45936u,(uint16_t)xs[(n/11)%25]);wr_u16(0xc45938u,(uint16_t)ys[(n/13)%18]);wr_u8(0xc457aeu,(n&4)?1:0);wr_u32(0xc45b50u,(n&16)?0x4200u:((n&32)?0x200u:0));wr_u32(0xc45b54u,words[n&15]);wr_u8(0xc461e7u,(uint8_t)((n%4)*16));wr_u8(0xc461e6u,(uint8_t)(16+n%8));wr_u16(0xc461ceu,words[(n/5)&15]);wr_u16(0xc461dau,(n&64)?1:0);wr_u16(0xc4619au,(uint16_t)((n%8)*4));wr_u16(0xc458dau,(uint16_t)(n%4));wr_u16(0xc45b46u,(uint16_t)((n&64)?-676:-675));wr_u8(0xc458b4u,(uint8_t)(n%3));wr_u8(0xc458dbu,(n&128)?9:8);
 wr_u16(0xc45986u,(uint16_t)(n%18-4));wr_u32(0xc45918u,0);
 if(selected_entry==0xc304fau){REG_D[0]=(REG_D[0]&0xffff0000u)|(uint16_t)(4*(n%4));wr_u16(0xc4596eu,(uint16_t)(0x41+n%4));wr_u32(0xc45968u,0);wr_u32(0xc45964u,0x12000u);wr_u16(0xc45982u,(uint16_t)(183+origins[(n/9)%9]+(n%8)));wr_u16(0xc4597cu,(uint16_t)((n%18+8)*16));}
 if(selected_entry==0xc322eeu){wr_u16(0xc458ccu,(n&1)?0x81:0);wr_u8(0xc45887u,(n&4)?0:1);wr_u8(0xc45886u,(uint8_t)((n&8)?0xff:n%2));wr_u16(0xc459c4u,(uint16_t)((n%9<4)?n%9:(n%9<8)?0x8000u+n%4:0xffffu));wr_u8(0xc4583cu,(uint8_t)(n%4-1));wr_u8(0xc45862u,(uint8_t)((n%4)*64));wr_u8(0xc461a4u,(n&32)?0x40:0);wr_u32(0xc4619cu,words[n&15]);wr_u16(0xc461ecu,(uint16_t)(n*8));wr_u16(0xc461f2u,(uint16_t)(n*12));wr_u16(0xc45adeu,(uint16_t)(n%16));wr_u16(0xc45ae4u,(uint16_t)((n&64)?n%16:n%16+1));wr_u8(0xc45785u,(n&128)?1:0);wr_u8(0xc45793u,(n&256)?1:0);wr_u8(0xc45861u,(uint8_t)(n%3-1));}
#ifdef HP_ORIGINAL_CHILDREN
 /* Original line clipping rejects at its source Y ceiling. Message cells
  * reject at their source column bound; marker polygons reject at count 0.
  * These frozen-clock fixtures do not claim active multi-plane DMA. */
 wr_u16(0xc45984u,0x8000u);
 if(selected_entry==0xc322eeu)wr_u16(0xc45986u,80);
 if(selected_entry==0xc3003au){wr_u16(0xc4b432u,0);wr_u16(0xc458d8u,40);}
#endif
 if(selected_entry==0xc33dc8u){wr_u8(0xc461e7u,(uint8_t)(((n/4)%4)*16));wr_u16(0xc45942u,(n&32)?90:159);wr_u16(0xc45944u,(n&64)?40:91);wr_u16(0xc4593au,(uint16_t)(159+(int)(n%15)-7));wr_u16(0xc4593cu,(uint16_t)(91+(int)((n/15)%15)-7));}
 if(selected_entry==0xc322eeu){wr_u8(0xc461e6u,(uint8_t)(16+(n/32)%8));wr_u8(0xc45886u,(uint8_t)((n/8)%3-1));wr_u8(0xc4583cu,(uint8_t)((n/16)%4-1));wr_u16(0xc461f2u,words[(n/16)&15]);}
 if(selected_entry==0xc342d0u && (n&128) && !(n&256)){wr_u16(0xc45936u,159);wr_u16(0xc45938u,91);wr_u16(0xc45942u,(uint16_t)(159+(int)(n%63)-31));wr_u16(0xc45944u,(uint16_t)(91+(int)((n/31)%63)-31));wr_u8(0xc461e7u,0x20);wr_u8(0xc457aeu,0);}
 if(selected_entry==0xc348b2u && (n&128)){REG_D[0]=(REG_D[0]&0xffff0000u)|159;REG_D[1]=(REG_D[1]&0xffff0000u)|(uint16_t)ys[n%18];REG_D[4]=(REG_D[4]&0xffff0000u)|1;wr_u16(0xc458dau,1);wr_u16(0xc45988u,0);wr_u16(0xc458d8u,0);}
 if(selected_entry==0xc342d0u && (n&256) && !(n&128)){wr_u16(0xc45936u,159);wr_u16(0xc45938u,91);wr_u16(0xc45942u,0);wr_u16(0xc45944u,0);wr_u8(0xc461e7u,0x20);wr_u8(0xc457aeu,0);}
 if(selected_entry==0xc33dc8u)wr_u16(0xc45b46u,(uint16_t)((n&128)?-676:-675));
 if(selected_entry==0xc322eeu && (n&128)){wr_u16(0xc459c0u,0);wr_u16(0xc458ccu,0);wr_u8(0xc45887u,0);wr_u8(0xc45886u,0xff);wr_u8(0xc4583cu,0);wr_u16(0xc459c4u,3);}
 if(selected_entry==0xc322eeu && (n&256)){wr_u16(0xc459c0u,0);wr_u16(0xc458ccu,0);wr_u8(0xc45887u,0);wr_u8(0xc4583cu,1);wr_u16(0xc459c4u,(uint16_t)(0x8001u+(n/8)%3));wr_u8(0xc461e6u,(uint8_t)(16+n%8));wr_u8(0xc461a4u,0x40);wr_u8(0xc46184u,(n&64)?0x80:0);}
 if(selected_entry==0xc098c6u)REG_PPC=0xc09898u;
 if(selected_entry==0xc09952u||selected_entry==0xc099f6u||selected_entry==0xc099aau){REG_PPC=0xc1f942u;REG_A[0]=selected_entry;}
 if(selected_entry==0xc332feu)REG_PPC=0xc332ceu;
 if(selected_entry==0xc34146u)REG_PPC=0xc332d2u;
 if(selected_entry==0xc34066u)REG_PPC=0xc33958u;
 if(selected_entry==0xc342d0u)REG_PPC=0xc332d6u;
 if(selected_entry==0xc347f2u)REG_PPC=0xc33c82u;
 if(selected_entry==0xc33dc8u)REG_PPC=0xc332dau;
 if(selected_entry==0xc322eeu)REG_PPC=0xc0f280u;
 if(selected_entry==0xc3003au)REG_PPC=0xc0f250u;
 if(selected_entry==0xc304fau)REG_PPC=0xc301e6u;
 if(selected_entry==0xc345a0u)REG_PPC=0xc33c3eu;
 if(selected_entry==0xc348b2u)REG_PPC=0xc33f40u;
 REG_A[7]=0xc7ff00u;expected_sp=REG_A[7]+4;wr_u32(REG_A[7],0xc70000u);
 m68k_set_reg(M68K_REG_SR,0x2700u|(p&31u));REG_PC=selected_entry;SET_CYCLES(1000000000);fa18_next_event=INT64_MAX;fa18_recomp_abort=0;
}
