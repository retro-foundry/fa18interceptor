/* Full active-origin parent and its actual matrices/math; CPU and ROM are validation only. */
#define FA18_SCENE_SOURCE_HEADER "../../build/recomp/native_selector_origin_source.h"
#define FA18_ORIGIN_MATH_MAIN origin_math_validation_main
#include "native_selector_origin_adjustment_oracle.c"
#include "../../port/flight.c"
#include "../../port/current_record_matrix.c"
#define add_long transform_add_long
#include "../../port/matrix_transform_components.c"
#undef add_long
#define add_long selector_add_long
#define sub_long selector_sub_long
#define asr_long selector_asr_long
#include "../../port/native_selector_origin.c"
#undef add_long
#undef sub_long
#undef asr_long
#include "native_selector_origin_fixture.h"

#define ORIGIN_BYTES(X) \
 X(enable,ORIGIN_ENABLE) X(gate_b,ORIGIN_GATE_B) X(gate_a,ORIGIN_GATE_A) \
 X(gate_mode,ORIGIN_GATE_MODE) X(detail_mode,ORIGIN_DETAIL_MODE) X(detail_index,ORIGIN_DETAIL_INDEX) \
 X(adjustment_mode,ORIGIN_ADJUSTMENT_MODE) X(threshold_flag,ORIGIN_THRESHOLD_FLAG) \
 X(auxiliary_flag,ORIGIN_AUXILIARY_FLAG) X(variant_selector,ORIGIN_VARIANT_SELECTOR) \
 X(detail_counter,ORIGIN_DETAIL_COUNTER)
#define ORIGIN_WORDS(X) X(angle_history,ORIGIN_ANGLE_HISTORY) X(status_word,ORIGIN_STATUS_WORD)
typedef struct {
 SceneState scene; FA18NativeSelectorOrigin owner; FA18NativeSceneRecord *active;
 FA18NativeSelectorOriginTables tables;
 FA18FlightTrigData trig; FA18NativeVectorMath math; PortFieldWindow magnitude_table;
 uint8_t trig_bytes[65536],magnitude_bytes[65536],table_bytes[4][2048];
 int16_t matrix[3][3],normalized[3]; uint16_t magnitude;
 int32_t origin[3],candidate[3],smoothed[3],companion[3];
#define B(name,address) uint8_t name;
 ORIGIN_BYTES(B)
#undef B
#define W(name,address) uint16_t name;
 ORIGIN_WORDS(W)
#undef W
} OriginState;
static unsigned origin_case,source_regenerations;
static int original_origin(void) {
 unsigned step,i;
 for(step=0;step<5000;++step) {
  uint32_t pc=REG_PC; uint16_t opcode;
  if(pc==0xc70000 && REG_A[7]==0xc7ff04) return 1;
  if(pc==0xc2578a && !REG_D[0] && (int32_t)REG_D[1]>=0) return 2;
  if(pc==0xc0910c) ++source_regenerations;
  for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
   if(scene_source_bytes[i].pc==pc) break;
  if(i==sizeof scene_source_bytes/sizeof scene_source_bytes[0]) {
   fprintf(stderr,"unexpected origin PC %06X\n",pc); return 0;
  }
  scene_seen[i]=1; opcode=rd_u16(pc); REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
  m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
 }
 return 0;
}
static int load_origin(OriginState *n) {
 static const uint32_t table_addresses[]={ORIGIN_MATRIX_TABLE,ORIGIN_MATRIX_CLASS_11,
  ORIGIN_MATRIX_CLASS_14,ORIGIN_MATRIX_CLASS_30};
 PortFieldWindow *windows[4]; unsigned i,j,offset=rd_u16(ORIGIN_RECORD_OFFSET);
 if(!load_scene(&n->scene)) return 0;
 if(offset%512 || offset/512>=FA18_NATIVE_SCENE_RECORDS) return 0;
 n->active=n->scene.bank.records+offset/512;
 n->trig=(FA18FlightTrigData){.bytes=n->trig_bytes,.byte_count=sizeof n->trig_bytes,.quarter_offset=32768};
 n->magnitude_table=(PortFieldWindow){.bytes=n->magnitude_bytes,.byte_count=sizeof n->magnitude_bytes,.origin=32768};
 for(i=0;i<65536;++i) {
  n->trig_bytes[i]=rd_u8(0xc3e5e8-32768+i);
  n->magnitude_bytes[i]=rd_u8(MAGNITUDE_TABLE-32768+i);
 }
 windows[0]=&n->tables.general; windows[1]=&n->tables.class_11;
 windows[2]=&n->tables.class_14; windows[3]=&n->tables.class_30;
 for(i=0;i<4;++i) {
  for(j=0;j<2048;++j) n->table_bytes[i][j]=rd_u8(table_addresses[i]-768+j);
  *windows[i]=(PortFieldWindow){.bytes=n->table_bytes[i],.byte_count=2048,.origin=768};
 }
 for(i=0;i<3;++i) {
  n->origin[i]=rd_s32(SELECTOR_ORIGIN+4*i); n->candidate[i]=rd_s32(ORIGIN_CANDIDATE_TRIPLE+4*i);
  n->smoothed[i]=rd_s32(ORIGIN_SMOOTHED_DELTA+4*i); n->companion[i]=rd_s32(ORIGIN_NEGATED_COMPANION+4*i);
  n->normalized[i]=rd_s16(NORMALIZED+2*i);
  for(j=0;j<3;++j) n->matrix[i][j]=rd_s16(0xc45c0eu+6*i+2*j);
 }
 n->magnitude=rd_u16(MAGNITUDE); n->math=(FA18NativeVectorMath){&n->magnitude_table,&n->magnitude,n->normalized};
 n->owner=(FA18NativeSelectorOrigin){.records=&n->scene.bank,.active_record=&n->active,
  .vector_math=&n->math,.trig=&n->trig,.matrix=n->matrix,.tables=&n->tables,
  .origin=n->origin,.candidate=n->candidate,
  .smoothed_delta=n->smoothed,.negated_companion=n->companion,.auxiliary_delta=n->smoothed+1};
#define B(name,address) n->name=rd_u8(address); n->owner.name=&n->name;
 ORIGIN_BYTES(B)
#undef B
#define W(name,address) n->name=rd_u16(address); n->owner.name=&n->name;
 ORIGIN_WORDS(W)
#undef W
 return verify_record_owners(&n->scene);
}
static void store_origin(OriginState *n) {
 unsigned i,j;
 store_scene(&n->scene);
 for(i=0;i<3;++i) {
  wr_u32(SELECTOR_ORIGIN+4*i,(uint32_t)n->origin[i]); wr_u32(ORIGIN_CANDIDATE_TRIPLE+4*i,(uint32_t)n->candidate[i]);
  wr_u32(ORIGIN_SMOOTHED_DELTA+4*i,(uint32_t)n->smoothed[i]); wr_u32(ORIGIN_NEGATED_COMPANION+4*i,(uint32_t)n->companion[i]);
  wr_u16(NORMALIZED+2*i,(uint16_t)n->normalized[i]);
  for(j=0;j<3;++j) wr_u16(0xc45c0eu+6*i+2*j,(uint16_t)n->matrix[i][j]);
 }
 wr_u16(MAGNITUDE,n->magnitude);
#define B(name,address) wr_u8(address,n->name);
 ORIGIN_BYTES(B)
#undef B
#define W(name,address) wr_u16(address,n->name);
 ORIGIN_WORDS(W)
#undef W
}
int main(int argc,char **argv) {
 size_t state_size=0,rom_size=0; char error[256]; unsigned entry,i,j,completed=0,loops=0;
 unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):8192;
 uint8_t *state,*rom,*expected;
 FA18Machine *m,*base,*before; OriginState *n;
 static const uint32_t entries[]={0xc29042,0xc2daf2,0xc091a8,0xc091ce};
 if(origin_math_validation_main(argc,argv)) return 1;
 state=read_file("captures/native/demo01/state.bin",&state_size); rom=read_file("local/system/kick13.rom",&rom_size);
 m=calloc(1,sizeof *m); base=malloc(sizeof *base); before=malloc(sizeof *before);
 n=calloc(1,sizeof *n); expected=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
 if(!state || !rom || !m || !base || !before || !n || !expected || !cases) return 1;
 if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) return 1;
 free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
 for(entry=0;entry<4;++entry) for(origin_case=0;origin_case<cases;++origin_case) {
  int source_ok,ok; int32_t input[3],output[3]; uint32_t result[3];
  selected_entry=entries[entry]; memcpy(m,base,sizeof *m); scene_fixture(origin_case); origin_fixture(origin_case);
  for(i=0;i<3;++i) input[i]=(int32_t)REG_D[3+i];
  for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
   for(j=0;j<scene_source_bytes[i].length;++j)
    if(rd_u8(scene_source_bytes[i].pc+j)!=scene_source_bytes[i].bytes[j]) return 1;
  if(!load_origin(n)) return 1;
  memcpy(before,m,sizeof *m); source_ok=original_origin();
  if(!source_ok) { fprintf(stderr,"origin source stopped %06X case %u\n",selected_entry,origin_case); return 1; }
  for(i=0;i<3;++i) result[i]=REG_D[i];
  memcpy(expected,m->chip,FA18_CHIP_SIZE); memcpy(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
  memcpy(m,before,sizeof *m);
  if(entry==0) ok=fa18_update_native_selector_origin(&n->owner);
  else if(entry==1) ok=fa18_prepare_native_selector_matrix(&n->owner);
  else ok=fa18_transform_native_selector_components(&n->owner,entry==2,input,output);
  if(ok!=(source_ok==1)) return 1;
  if(entry>=2) for(i=0;i<3;++i) if((uint32_t)output[i]!=result[i]) {
   fprintf(stderr,"origin transform %06X case %u axis %u %08X/%08X\n",selected_entry,origin_case,i,result[i],(uint32_t)output[i]); return 1;
  }
  store_origin(n);
  if(memcmp(m->chip,expected,FA18_CHIP_SIZE) || memcmp(m->slow,expected+FA18_CHIP_SIZE,0x7fd00) ||
     memcmp(m->slow+0x7ff00,expected+FA18_CHIP_SIZE+0x7ff00,FA18_SLOW_SIZE-0x7ff00))
  for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
   uint32_t a=i<FA18_CHIP_SIZE?i:FA18_SLOW_BASE+i-FA18_CHIP_SIZE;
   if(a>=0xc7fd00 && a<0xc7ff00) continue;
   if(rd_u8(a)!=expected[i]) { fprintf(stderr,"origin %06X case %u RAM %06X %02X/%02X\n",selected_entry,origin_case,a,expected[i],rd_u8(a)); return 1; }
  }
  if(!verify_record_owners(&n->scene)) return 1;
  if(source_ok==1) ++completed; else ++loops;
 }
 printf("native active selector-origin and matrices: %u calls match all game RAM, typed records and transform outputs; %u complete/%u proven source loops; %u actual C0910C regeneration calls; no child contracts\n",cases*4,completed,loops,source_regenerations);
 /* The full-RAM cases above cover stores; exhaust every word angle separately. */
 selected_entry=0xc2daf2;
 for(i=0;i<65536;++i) {
  unsigned axis,column,slot=(unsigned)(n->active-n->scene.bank.records);
  wr_u16(CONTROL_RECORDS+512*slot+0x68,(uint16_t)i); n->active->geometry->angle=(uint16_t)i;
  REG_A[7]=0xc7ff00; wr_u32(REG_A[7],0xc70000); REG_PC=selected_entry;
  if(original_origin()!=1 || !fa18_prepare_native_selector_matrix(&n->owner)) return 1;
  for(axis=0;axis<3;++axis) for(column=0;column<3;++column)
   if(n->matrix[axis][column]!=rd_s16(0xc45c0eu+6*axis+2*column)) {
    fprintf(stderr,"current matrix angle %04X component %u/%u differs\n",i,axis,column); return 1;
   }
 }
 puts("native current-record matrix: all 65536 word angles match original C2DAF2/C2E370/C2E6DA at full 4000 scale");
 printf("visited:"); for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i) if(scene_seen[i]) printf(" %06X",scene_source_bytes[i].pc);
 puts(""); free(expected); free(n); free(before); free(base); free(m); return 0;
}
