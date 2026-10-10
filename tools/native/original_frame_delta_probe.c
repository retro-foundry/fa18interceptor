/* Reference-only compact exports. Forward the actual bus and loop unchanged;
 * read host storage directly so diagnostics add no emulated bus cycles. */
#define fa18_bus_begin original_bus_begin
#include "../../port/machine/bus.c"
#undef fa18_bus_begin
#include "../../port/recomp/loop_input.h"
#include "../../port/native/frame_delta.c"

static void delta_error(unsigned line) {
    fprintf(stderr,"Original frame delta rejected at line %u, iteration %ld, PC %06X\n",
        line,fa18_loop_iterations(),REG_PC);
    exit(2);
}
#define abort() delta_error(__LINE__)

static FA18FrameDelta delta;
static FILE *registers_out;
static char registers_buffer[4096];
/* The checker explicitly reserves the complete trace's share of its budget. */
static size_t metadata_bytes,shared_budget=256u*1024u*1024u;
static long first,count,last_iteration;
static unsigned exported;
static uint32_t message_return,message_stack,panel_stack;

static const uint8_t *delta_reader(void *context,uint32_t address,size_t size) {
    const FA18Machine *machine=context;
    if(address<FA18_CHIP_SIZE && size<=FA18_CHIP_SIZE-address) return machine->chip+address;
    if(address>=0xc00000u && address<0xc00000u+FA18_SLOW_SIZE &&
       size<=0xc00000u+FA18_SLOW_SIZE-address) return machine->slow+address-0xc00000u;
    return NULL;
}
static uint32_t storage_word(uint32_t address) {
    const uint8_t *bytes=delta_reader(fa18_machine,address,4);
    if(!bytes) abort();
    return (uint32_t)bytes[0]<<24|(uint32_t)bytes[1]<<16|(uint32_t)bytes[2]<<8|bytes[3];
}
static void close_delta(void) {
    if(!fa18_frame_delta_close(&delta) || (registers_out && fclose(registers_out))) abort();
    registers_out=NULL;
}
static void initialize_delta(void) {
    static int initialized;
    if(initialized) return;
    initialized=1;
    const char *range=getenv("FA18_ORIGINAL_DELTA_RANGE");
    const char *path=getenv("FA18_ORIGINAL_DELTA_PATH");
    const char *registers=getenv("FA18_ORIGINAL_DELTA_REGISTERS");
    char *end;
    const char *budget=getenv("FA18_ORIGINAL_DELTA_BUDGET_MIB");
    if(budget) {
        unsigned long mib=strtoul(budget,&end,10);
        shared_budget=(size_t)mib*1024*1024;
        if(*end || !mib || mib>10000000 || shared_budget/1024/1024!=mib) abort();
    }
    if(!range || !path || !registers) abort();
    first=strtol(range,&end,10);
    if(*end!='+') abort();
    count=strtol(end+1,&end,10);
    if(*end || first<1 || first>10000000 || count<1 || count>10000000-first+1) abort();
    if(!fa18_frame_delta_open(&delta,path,shared_budget)) abort();
    registers_out=fopen(registers,"w");
    if(!registers_out || setvbuf(registers_out,registers_buffer,_IOFBF,sizeof registers_buffer)) abort();
    if(atexit(close_delta)) abort();
}
static void capture(long iteration,uint32_t pc,unsigned bit) {
    initialize_delta();
    if(iteration<first || iteration-first>=count) return;
    if(iteration!=last_iteration) {
        exported=0;message_return=message_stack=panel_stack=0;last_iteration=iteration;
    }
    if(exported&bit) return;
    char row[2048];
    int length=snprintf(row,sizeof row,
        "{\"iteration\":%ld,\"frame\":%llu,\"pc\":%u,\"ppc\":%u,\"registers\":[",
        iteration,(unsigned long long)fa18_machine->frame,pc,REG_PPC);
    if(length<0 || length>=(int)sizeof row) abort();
    for(unsigned i=0;i<16;++i) {
        const int written=snprintf(row+length,sizeof row-(size_t)length,"%s%u",i?",":"",REG_DA[i]);
        if(written<0 || written>=(int)(sizeof row-(size_t)length)) abort();
        length+=written;
    }
    const int written=snprintf(row+length,sizeof row-(size_t)length,"],\"sr\":%u}\n",m68k_get_reg(NULL,M68K_REG_SR));
    if(written<0 || written>=(int)(sizeof row-(size_t)length)) abort();
    length+=written;
    if((size_t)length>shared_budget-delta.bytes-metadata_bytes) abort();
    if(fwrite(row,1,(size_t)length,registers_out)!=(size_t)length) abort();
    metadata_bytes+=(size_t)length;delta.budget=shared_budget-metadata_bytes;
    const uint8_t *tick=delta_reader(fa18_machine,0xc458da,2);
    if(!tick || !fa18_frame_delta_write(&delta,(unsigned)iteration,(unsigned)fa18_machine->frame,
        pc,(uint16_t)((unsigned)tick[0]<<8|tick[1]),delta_reader,fa18_machine)) abort();
    exported|=bit;
}
/* Called before the existing loop increments, traces and delivers input. */
void original_frame_delta_entry(void) {
    if(REG_PC!=FA18_LOOP_UPDATE_ENTRY) abort();
    capture(fa18_loop_iterations()+1,REG_PC,1);
}
void fa18_bus_begin(uint32_t pc) {
    initialize_delta();
    const long iteration=fa18_loop_iterations();
    if(iteration>=first && iteration-first<count) {
        if(pc==0xc0efea) capture(iteration,pc,2);
        if(pc==0xc0f3c0) capture(iteration,pc,4);
        if(pc==0xc30764 && !(exported&128)) {
            panel_stack=REG_A[7]+4;
            if(storage_word(REG_A[7])!=0xc0f182) abort();
            capture(iteration,pc,128);
        }
        if(pc==0xc0f182 && (exported&128) && REG_A[7]==panel_stack)
            capture(iteration,pc,256);
        if(pc==0xc31226 && !(exported&8)) {
            if(storage_word(REG_A[7])!=0xc0f18e) abort();
            capture(iteration,pc,8);
        }
        if(pc==0xc0f18e && (exported&8)) capture(iteration,pc,16);
        if(pc==0xc322ee && !(exported&32)) {
            message_return=storage_word(REG_A[7]);message_stack=REG_A[7]+4;
            if(message_return!=0xc0f286 && message_return!=0xc0f2dc) abort();
            capture(iteration,pc,32);
        }
        if((exported&32) && pc==message_return && REG_A[7]==message_stack)
            capture(iteration,pc,64);
    }
    original_bus_begin(pc);
}
