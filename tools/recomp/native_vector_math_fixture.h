/* Original-shaped input families shared by differential proofs only. */
#ifndef FA18_NATIVE_VECTOR_MATH_FIXTURE_H
#define FA18_NATIVE_VECTOR_MATH_FIXTURE_H
static void vector_fixture(unsigned scenario,unsigned entry,uint32_t *scale_word,int32_t input[3]) {
    unsigned i;
    static const int16_t scales[]={0,1,-1,192,-192,512,-512,32767,-32768,64,-64,256,-256};
    static const int16_t components[]={0,1,-1,192,-192,512,-512,0x4800,0x1000,32767,-32768,0x7f00};
        *scale_word=(random_value()&0xffff0000u)|(uint16_t)scales[scenario%13];
        for(i=0;i<3;++i) input[i]=(int32_t)((random_value()&0xffff0000u)|
            (uint16_t)(scenario%7==0?(int16_t)random_value():components[(scenario/13+i*scenario)%12]));
        if(scenario%9==0) for(i=0;i<FA18_SCENE_MAGNITUDE_WORDS;++i)
            wr_u16(MAGNITUDE_TABLE+2*i,scenario%27==0?0xffff:scenario%27==9?0:0x4000);
        if(entry==0) {
            REG_D[0]=*scale_word; for(i=0;i<3;++i) REG_D[5+i]=(uint32_t)input[i];
        } else if(entry==1) {
            *scale_word=(uint32_t)(uint16_t)(scenario%3==0?0x8000:scenario%3==1?0:1)<<16|
                (uint16_t)scales[scenario%13];
            wr_u32(REG_A[7]+4,*scale_word);
            for(i=0;i<3;++i) wr_u32(REG_A[7]+8+4*i,(uint32_t)input[i]);
        } else for(i=0;i<3;++i) REG_D[2+i]=(uint32_t)input[i];
        if(entry==2) REG_D[4]=(uint32_t)input[2];
}
#endif
