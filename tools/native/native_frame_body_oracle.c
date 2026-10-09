/* Compare actual native frame assembly C0EFEA-C0F3C0 against original bytes.
 * Before/after exports come from the actual runner; no native body is copied
 * into this oracle. CPU/chipset/ROM belong only to the reference process. */
#define main records_fixture_main
#include "native_records_oracle.c"
#undef main
#include "hardware.h"

/* Supply the native PAL interval through the complete original input callback.
 * RGB4 publication is a host boundary and excluded from drawing
 * acceptance; its game-side control stores still execute original bytes. */
static int palette_tick(void) {
    uint32_t saved[16],pc=REG_PC;memcpy(saved,REG_DA,sizeof saved);
    uint16_t sr=(uint16_t)m68k_get_reg(NULL,M68K_REG_SR);
    REG_A[7]=0xc7fd80;wr_u32(REG_A[7],0xc70000);REG_PC=0xc1718e;
    for(unsigned step=0;step<10000;++step) {
        if(REG_PC==0xc70000 && REG_A[7]==0xc7fd84) {
            memcpy(REG_DA,saved,sizeof saved);REG_PC=pc;m68k_set_reg(M68K_REG_SR,sr);return 1;
        }
        if(REG_PC==0xc53ec0) {REG_PC=rd_u32(REG_A[7]);REG_A[7]+=4;continue;}
        uint16_t opcode=rd_u16(REG_PC);REG_PPC=REG_PC;REG_IR=opcode;REG_PC+=2;
        m68ki_instruction_jump_table[opcode]();USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    fputs("Original palette callback did not finish\n",stderr);return 0;
}

int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0,ne=0;char error[256];
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=file_bytes("local/system/kick13.rom",&nr);
    const int owner_exit=argc==8 && !strcmp(argv[7],"owner-exit");
    const int stage_only=getenv("FA18_FRAME_STAGE_ONLY")!=NULL;
    uint8_t *data=(argc==6 || argc==7 || owner_exit)?file_bytes(argv[1],&nd):NULL;
    uint8_t *expected=(argc==6 || argc==7 || owner_exit)?file_bytes(argv[2],&ne):NULL;
    FA18Machine *m=calloc(1,sizeof *m);
    if(!state||!rom||!data||!expected||nd!=0x100000||ne!=nd||!m) return 1;
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) return 1;
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;
    memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
    /* These keyboard-only paths supply no intervening mouse movement. */
    m->joy0dat=(uint16_t)(rd_u16(0xc1ac08)<<8|rd_u16(0xc1ac06));
    m->mouse_x=m->joy0dat&255;m->mouse_y=m->joy0dat>>8;
    unsigned first_tick=(unsigned)strtoul(argv[3],NULL,10),last_tick=(unsigned)strtoul(argv[4],NULL,10);
    native_clock_set(first_tick);unsigned in_timer=0,timer_samples=0;
    memset(REG_DA,0,sizeof REG_DA);REG_A[6]=0xc7ff80;REG_A[7]=0xc7ff00;
    /* Independent preceding original input result, never supplied to native
     * gameplay. Idle bodies preserve it instead of assigning a new byte. */
    const char *initial_carry=getenv("FA18_FRAME_INITIAL_INPUT_CARRY");
    if(initial_carry) REG_D[4]=(uint32_t)strtoul(initial_carry,NULL,0);
    wr_u32(REG_A[6],0);wr_u32(REG_A[6]+4,0xc70000);
    wr_u16(REG_A[6]-2,(uint16_t)strtoul(argv[5],NULL,10));REG_A[4]=rd_u16(LINE_LAST_ROW);
    REG_PC=0xc0efea;
    if(stage_only) {
        if(rd_u8(RECORDER_MODE) || rd_u16(RAW_KEY_LATCH) || rd_u16(RECORD_WORD_A) || rd_u16(RECORD_WORD_B)) {
            fputs("Original idle stage requires empty keyboard/recorder input\n",stderr);return 1;
        }
        REG_A[7]=0xc7ff88;wr_u32(REG_A[7],0xc70000);REG_PC=0xc0efd4;m->joy1dat=0;
    }
    m68k_set_reg(M68K_REG_SR,0x2700);
    fa18_next_event=INT64_MAX;SET_CYCLES(1000000000);
    unsigned step,guidance_fault_returns=0;
    const char *trace_pixel=getenv("FA18_FRAME_TRACE_PIXEL");
    const gaddr pixel=trace_pixel?(gaddr)strtoul(trace_pixel,NULL,16):0;
    const char *trace_matrix=getenv("FA18_FRAME_TRACE_MATRIX");
    const gaddr matrix_record=trace_matrix?(gaddr)strtoul(trace_matrix,NULL,16):0;
    int in_matrix=0;
    const int trace_input_carry=getenv("FA18_FRAME_TRACE_INPUT_CARRY")!=NULL;
    const int trace_model=getenv("FA18_FRAME_TRACE_MODEL")!=NULL;
    const int trace_line=getenv("FA18_FRAME_TRACE_LINE")!=NULL;
    const int trace_plot=getenv("FA18_FRAME_TRACE_PLOT")!=NULL;
    const char *radar_dump=getenv("FA18_FRAME_RADAR_DUMP");
    unsigned radar_exports=0;
    const char *message_dump=getenv("FA18_FRAME_MESSAGE_DUMP");
    unsigned message_exports=0;
    gaddr message_return=0,message_stack=0;
    const char *panel_dump=getenv("FA18_FRAME_PANEL_DUMP");
    unsigned panel_exports=0;
    gaddr panel_stack=0;
    const char *descriptor_dump=getenv("FA18_FRAME_DESCRIPTOR_DUMP");
    const char *descriptor_stream=getenv("FA18_FRAME_DESCRIPTOR_STREAM");
    unsigned descriptor_exports=0;
    const int trace_normalise=getenv("FA18_FRAME_TRACE_NORMALISE")!=NULL;
    const int trace_depth=getenv("FA18_FRAME_TRACE_DEPTH")!=NULL;
    const char *trace_word=getenv("FA18_FRAME_TRACE_WORD");
    const gaddr watched_word=trace_word?(gaddr)strtoul(trace_word,NULL,16):0;
    gaddr carry_writer=0;
    gaddr depth_writer=0;
    unsigned context_sorts=0,cached_entries=0,far_entries=0;
    unsigned restore_first=0,restore_second=0;
    uint16_t context_factor=0;
    for(step=0;step<10000000;++step) {
        if(panel_dump && REG_PC==0xc30764u && !panel_exports) {
            if(rd_u32(REG_A[7])!=0xc0f182u) return 1;
            panel_stack=REG_A[7]+4;
        }
        if(panel_dump && ((REG_PC==0xc30764u && !panel_exports) ||
            (panel_exports==1 && REG_PC==0xc0f182u && REG_A[7]==panel_stack))) {
            char path[4096];
            if(snprintf(path,sizeof path,"%s.%s.dat",panel_dump,
                panel_exports?"after":"before")>=(int)sizeof path) return 1;
            FILE *out=fopen(path,"wb");
            if(!out || fwrite(m->chip,1,0x80000,out)!=0x80000 ||
                fwrite(m->slow,1,0x80000,out)!=0x80000 || fclose(out)) return 1;
            ++panel_exports;
        }
        if(message_dump && REG_PC==0xc322eeu && !message_exports) {
            message_return=rd_u32(REG_A[7]);message_stack=REG_A[7]+4;
            if(message_return!=0xc0f286u && message_return!=0xc0f2dcu) return 1;
        }
        if(message_dump && ((REG_PC==0xc322eeu && !message_exports) ||
           (message_exports==1 && REG_PC==message_return && REG_A[7]==message_stack))) {
            char path[4096];
            if(snprintf(path,sizeof path,"%s.%s.dat",message_dump,
                        message_exports?"after":"before")>=(int)sizeof path) return 1;
            FILE *out=fopen(path,"wb");
            if(!out || fwrite(m->chip,1,0x80000,out)!=0x80000 ||
               fwrite(m->slow,1,0x80000,out)!=0x80000 || fclose(out)) return 1;
            ++message_exports;
        }
        if(radar_dump && ((REG_PC==0xc31226u && !(radar_exports&1u)) ||
                          (REG_PC==0xc0f18eu && !(radar_exports&2u)))) {
            const int after=REG_PC==0xc0f18eu;
            char path[4096];
            if(snprintf(path,sizeof path,"%s.%s.dat",radar_dump,after?"after":"before")>=(int)sizeof path) return 1;
            FILE *out=fopen(path,"wb");
            if(!out || fwrite(m->chip,1,0x80000,out)!=0x80000 ||
               fwrite(m->slow,1,0x80000,out)!=0x80000 || fclose(out)) return 1;
            radar_exports|=after?2u:1u;
        }
        if(descriptor_dump && descriptor_stream && REG_PC==0xc096cau &&
           rd_u32(0xc45a36u)==(gaddr)strtoul(descriptor_stream,NULL,16)) {
            char path[4096];
            if(snprintf(path,sizeof path,"%s.%u.dat",descriptor_dump,descriptor_exports++)>=(int)sizeof path) return 1;
            FILE *out=fopen(path,"wb");
            if(!out || fwrite(m->chip,1,0x80000,out)!=0x80000 ||
               fwrite(m->slow,1,0x80000,out)!=0x80000 || fclose(out)) return 1;
            fprintf(stderr,"descriptor input %s frame=%06X SP=%06X\n",path,REG_A[6],REG_A[7]);
        }
        if(trace_line && REG_PC==0xc2fa7eu)
            fprintf(stderr,"line %d,%d -> %d,%d colour=%u return=%06X\n",
                (int16_t)REG_D[0],(int16_t)REG_D[1],(int16_t)REG_D[2],(int16_t)REG_D[3],
                rd_u16(CURRENT_COLOUR),rd_u32(REG_A[7]));
        if(trace_plot && (REG_PC==0xc2f5f4u || REG_PC==0xc2f60au || REG_PC==0xc2f66eu))
            fprintf(stderr,"plot pc=%06X x=%d y=%d colour=%u return=%06X record=%06X\n",
                REG_PC,(int16_t)REG_D[0],(int16_t)REG_D[1],rd_u16(CURRENT_COLOUR),
                rd_u32(REG_A[7]),rd_u32(0xc18214u));
        if(REG_PC==0xc0a12e && REG_A[1]==CONTROL_RECORDS+0x800) ++restore_first;
        if(REG_PC==0xc0a12e && REG_A[1]==CONTROL_RECORDS+0xc00) ++restore_second;
        if(REG_PC==0xc1e328 && rd_u8(CONTEXT_SELECT)) context_factor=(uint16_t)(REG_D[3]>>16);
        if(REG_PC==0xc1e440 && rd_u8(CONTEXT_SELECT)) ++context_sorts;
        if(REG_PC==0xc1e3a0 && rd_u8(CONTEXT_SELECT)) ++cached_entries;
        if(REG_PC==0xc1e38e && rd_u8(CONTEXT_SELECT)) ++far_entries;
        if(trace_depth && (REG_PC==0xc1e328 || REG_PC==0xc1e37a || REG_PC==0xc1e42e ||
                          REG_PC==0xc1e440 || REG_PC==0xc1e484))
            fprintf(stderr,"depth pc=%06X value=%08X count=%04X list=%06X flags=%04X next=%02X upper-writer=%06X\n",
                REG_PC,REG_D[3],(uint16_t)REG_D[7],REG_A[1],rd_u16(REG_A[1]),rd_u8(SORT_LIST_NEXT),depth_writer);
        if(trace_normalise && (REG_PC==0xc2574a || REG_PC==0xc25754 || REG_PC==0xc25764 ||
            REG_PC==0xc25784 || REG_PC==0xc257ae || REG_PC==0xc257c2))
            fprintf(stderr,"normalise pc=%06X return=%06X frame=%06X record=%06X viewer=%06X d0=%08X d1=%08X d2=%08X d3=%08X d4=%08X vector=%08X/%08X/%08X\n",
                REG_PC,rd_u32(REG_A[7]),REG_A[6],REG_A[1],REG_A[3],REG_D[0],REG_D[1],REG_D[2],REG_D[3],REG_D[4],
                REG_D[5],REG_D[6],REG_D[7]);
        if(stage_only && REG_PC==0xc0efea && REG_A[7]==0xc7ff82) break;
        if(trace_model && (REG_PC==0xc1ee14 || REG_PC==0xc1f074 || REG_PC==0xc1f8de)) {
            const gaddr accumulator=(REG_PC==0xc1ee14?REG_A[7]-4:REG_A[6])-0x7c;
            fprintf(stderr,"model pc=%06X record=%04X sp=%06X frame=%06X accumulator=%06X value=%04X\n",
                REG_PC,rd_u16(SCRIPT_RECORD),REG_A[7],REG_A[6],accumulator,rd_u16(accumulator));
        }
        if(trace_input_carry && ((REG_PC>=0xc0f250 && REG_PC<=0xc0f28c && (REG_PC-0xc0f250)%6==0) ||
            REG_PC==0xc0f2f6 || REG_PC==0xc0f2fc ||
            REG_PC==0xc0f380 || REG_PC==0xc0f386 || REG_PC==0xc0f3ac ||
            REG_PC==0xc0f3b2 || REG_PC==0xc0f3ba || REG_PC==0xc0f3c0))
            fprintf(stderr,"input-carry boundary=%06X value=%08X last-change=%06X opcode=%04X\n",
                REG_PC,REG_D[4],carry_writer,carry_writer?rd_u16(carry_writer):0);
        if(trace_matrix && REG_PC==0xc2dee0 && REG_A[1]==matrix_record) in_matrix=1;
        if(in_matrix && (REG_PC==0xc2e024 || REG_PC==0xc2e048 || REG_PC==0xc2e0dc || REG_PC==0xc2e118 || REG_PC==0xc2e202 ||
                        REG_PC==0xc2e208 || REG_PC==0xc2e242 || REG_PC==0xc2e300 || REG_PC==0xc2e334))
            fprintf(stderr,"extraction %06X d0=%08X d1=%08X d3=%08X d4/d5/d6=%08X/%08X/%08X main=%08X companion=%08X\n",
                REG_PC,REG_D[0],REG_D[1],REG_D[3],REG_D[4],REG_D[5],REG_D[6],
                rd_u32(MATRIX_TRANSFORM_PRODUCT+4),rd_u32(MATRIX_TRANSFORM_PRODUCT+16));
        if(REG_PC==0xc2d704) in_matrix=0;
        if(trace_matrix && REG_A[1]==matrix_record &&
           (REG_PC==0xc2dee0 || REG_PC==0xc2d704 || REG_PC==0xc2d76e || REG_PC==0xc2d94e))
            fprintf(stderr,"matrix %06X record=%06X flags=%02X old=%04X/%04X/%04X d4/d5/d6=%08X/%08X/%08X\n",
                REG_PC,matrix_record,rd_u8(matrix_record+3),rd_u16(matrix_record+0x66),
                rd_u16(matrix_record+0x68),rd_u16(matrix_record+0x6a),REG_D[4],REG_D[5],REG_D[6]);
        if((!owner_exit && REG_PC==0xc0f3c0 && REG_A[7]==0xc7ff00) ||
           (owner_exit && REG_PC==0xc70000 && REG_A[7]==0xc7ff88)) {wait_blitter();break;}
        if(REG_PC==0xc25312) in_timer=1;
        if(REG_PC==0xc2c34e) ++guidance_fault_returns; /* C06C02 has returned to the countdown owner. */
        if(REG_PC==0xc53c78) {
            if(in_timer && timer_samples++==1) {
                for(unsigned tick=first_tick;tick<last_tick;++tick) if(!palette_tick()) return 1;
                native_clock_set(last_tick);
            }
            native_clock_request();REG_PC=rd_u32(REG_A[7]);REG_A[7]+=4;continue;
        }
        if(REG_PC==0xc53f9c) {REG_PC=rd_u32(REG_A[7]);REG_A[7]+=4;continue;}
        if(stage_only && (REG_PC==0xc53c08 || REG_PC==0xc53c8c || REG_PC==0xc1715c)) {
            /* Empty GetMsg/consumed descriptor release and zero physical
             * buttons are identical host contracts to the native fixture.
             * C0F3C4, C0F5F8 and the selected setup callback execute original
             * game instructions, including their reset/time-accounting children. */
            if(REG_PC==0xc53c08) {
                const gaddr handle=rd_u32(REG_A[7]+4);
                if(handle!=rd_u32(EXTERNAL_INPUT_HANDLE) && handle!=rd_u32(KEYBOARD_INPUT_HANDLE)) return 1;
            }
            if(REG_PC!=0xc53c8c) REG_D[0]=0;
            REG_PC=rd_u32(REG_A[7]);REG_A[7]+=4;continue;
        }
        if(REG_PC==0xc53f4c) {
            REG_A[7]-=4;wr_u32(REG_A[7],0xc53f50);REG_PC=0xfc5a58;continue;
        }
        int cycles=GET_CYCLES();uint16_t opcode=rd_u16(REG_PC);
        const uint8_t previous_pixel=trace_pixel?rd_u8(pixel):0;
        const uint16_t previous_word=trace_word?rd_u16(watched_word):0;
        const uint32_t previous_carry=REG_D[4];
        const uint32_t previous_depth=REG_D[3];
        const gaddr previous_viewer=REG_A[3];
        REG_PPC=REG_PC;REG_IR=opcode;REG_PC+=2;
        m68ki_instruction_jump_table[opcode]();USE_CYCLES(CYC_INSTRUCTION[opcode]);
        m->cycle+=cycles-GET_CYCLES();
        if(trace_depth && (REG_D[3]>>16)!=(previous_depth>>16)) depth_writer=REG_PPC;
        if(trace_normalise && REG_A[3]!=previous_viewer)
            fprintf(stderr,"viewer %06X -> %06X at %06X record=%06X\n",
                previous_viewer,REG_A[3],REG_PPC,REG_A[1]);
        if(trace_input_carry && REG_D[4]!=previous_carry) carry_writer=REG_PPC;
        if(trace_word && rd_u16(watched_word)!=previous_word)
            fprintf(stderr,"word %06X %04X -> %04X at %06X frame=%06X sp=%06X d0=%08X d1=%08X d2=%08X d3=%08X\n",
                watched_word,previous_word,rd_u16(watched_word),REG_PPC,REG_A[6],REG_A[7],
                REG_D[0],REG_D[1],REG_D[2],REG_D[3]);
        if(trace_pixel && rd_u8(pixel)!=previous_pixel)
            fprintf(stderr,"pixel %06X %02X -> %02X at %06X d0=%08X d1=%08X destination=%06X colour=%u record=%06X\n",
                pixel,previous_pixel,rd_u8(pixel),REG_PPC,REG_D[0],REG_D[1],REG_A[3],rd_u16(CURRENT_COLOUR),rd_u32(0xc18214));
    }
    if(step==10000000) {fprintf(stderr,"Frame body did not return at %06X\n",REG_PC);return 1;}
    if(message_dump && message_exports!=2) {
        fputs("Frame message owner did not produce both actual boundaries\n",stderr);return 1;
    }
    if(panel_dump && panel_exports!=2) {
        fputs("Frame panel owner did not produce both actual boundaries\n",stderr);return 1;
    }
    printf("Frame input carry: %u\n",REG_D[4]);
    if(restore_first || restore_second)
        printf("Mode-five restoration: %u first-record calls, %u second-record calls\n",
            restore_first,restore_second);
    if(rd_u8(CONTEXT_SELECT))
        printf("Context depth: %u sorted lists, %u cached entries, %u fixed-far entries, incoming word=%04X\n",
            context_sorts,cached_entries,far_entries,context_factor);
    const char *expected_carry=getenv("FA18_FRAME_EXPECT_INPUT_CARRY");
    if(expected_carry && (uint8_t)REG_D[4]!=(uint8_t)strtoul(expected_carry,NULL,0)) {
        fprintf(stderr,"Frame input carry differs: source %02X native %02X\n",
            (uint8_t)REG_D[4],(uint8_t)strtoul(expected_carry,NULL,0));return 1;
    }
    unsigned differences=0,plane_differences=0,scratch_differences=0,voice_differences=0,busy_differences=0;
    uint8_t *voice_mask=calloc(0x100000,1);
    if(!voice_mask) return 1;
    /* Current native resource owners populate slots 0..36. */
    for(unsigned slot=0;slot<37;++slot) {
        gaddr record=rd_u32(SOUND_VOICES+4*slot);
        if(!record) continue;
        unsigned offset=record<0x80000?record:record-0xc00000+0x80000;
        if(offset>0x100000-64) return 1;
        memset(voice_mask+offset,1,64);
    }
    memset(voice_mask+VOICE_SLOTS-0xc00000+0x80000,1,16);
    for(unsigned i=0;i<0xffc00;++i) {
        uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
        if(actual==expected[i]) continue;
        /* Ordinary native locals have no original ABI-stack equivalent.
         * Samples are asynchronous outside this frame-body fixture; their
         * callbacks and PCM streams have separate complete source checks.
         * Synchronous native raster submission has no blitter busy polls. */
        if((i>=0x3f30 && i<0x4b10) || (i>=0x306a && i<0x3080)) {++scratch_differences;continue;}
        if(voice_mask[i]) {++voice_differences;continue;}
        if(i>=ACTIVE_PLANE_BUSY_1-0xc00000+0x80000 && i<ACTIVE_PLANE_BUSY_3-0xc00000+0x80000+4) {
            ++busy_differences;continue;
        }
        if(differences<70) fprintf(stderr,"frame %06X source %02X native %02X\n",
            i<0x80000?i:i-0x80000+0xc00000,actual,expected[i]);
        ++differences;if(i>=0x34000 && i<0x4a000) ++plane_differences;
    }
    printf("Frame body: %u gameplay differences, %u display bytes; %u instructions, %u timer samples; excluded scratch=%u voice=%u busy=%u\n",
        differences,plane_differences,step,timer_samples,scratch_differences,voice_differences,busy_differences);
    if(guidance_fault_returns) printf("Source guidance C06C02 returns: %u\n",guidance_fault_returns);
    if(argc>=7) {
        FILE *out=fopen(argv[6],"wb");
        if(!out || fwrite(m->chip,1,0x80000,out)!=0x80000 || fwrite(m->slow,1,0x80000,out)!=0x80000 || fclose(out)) return 1;
    }
    free(voice_mask);free(m);free(expected);free(data);free(state);free(rom);
    return differences?1:0;
}
