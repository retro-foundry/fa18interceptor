/* Recorded ordinary keys drive the playable runtime. Snapshot observation
 * never changes flight state; original instructions run in a separate tool. */
#include "native/frontend.h"
#include "../../port/native/replay.h"
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    NativeReplay replay;
    const char *prefix;
    uint8_t *before, *entry_before;
    unsigned body, captures, first_tick, iteration, window;
    uint16_t saved_tick, contact, qualification;
    uint8_t phase;
    gaddr stage, previous_stage;
    gaddr entry_stage, previous_entry_stage;
    unsigned entries, entry_tick, entry_keys[256], key_count;
    uint8_t entry_phase, previous_entry_phase;
    int keep_entry;
    int begun, airborne, landed, success, restarted, reloaded;
} Sequence;

static void snapshot(const Sequence *run,const char *tag,unsigned index,const char *suffix,const uint8_t *data) {
    char path[4096];
    int size=snprintf(path,sizeof path,"%s.%s.%u.%s.dat",run->prefix,tag,index,suffix);
    if(size<0 || size>=(int)sizeof path) abort();
    FILE *file=fopen(path,"wb");
    if(!file) {perror(path);abort();}
    const int written=fwrite(data,1,0x100000,file)==0x100000;
    if(fclose(file) || !written) {fprintf(stderr,"Cannot write %s\n",path);abort();}
}

static void observe(NativeFrontend *game,enum NativeFrameBoundary boundary,
                    uint16_t saved_tick,void *context) {
    Sequence *run=context;
    if(boundary==NATIVE_FRAME_INPUT_BEGIN) {
        run->entry_stage=rd_u32(STAGE_CALLBACK);run->entry_phase=rd_u8(PLAYER_PHASE);
        run->entry_tick=game->ticks;run->key_count=game->input_count;
        run->keep_entry=run->entry_stage!=run->previous_entry_stage ||
            run->entry_phase!=run->previous_entry_phase || (run->window && run->key_count) ||
            run->entry_stage==0xc110a4;
        if(run->keep_entry) {
            memcpy(run->entry_before,game->storage.buffers,0x80000);
            memcpy(run->entry_before+0x80000,game->storage.source,0x80000);
            for(unsigned i=0;i<run->key_count;++i)
                run->entry_keys[i]=game->input_keys[(game->input_read+i)&255u];
        }
        run->previous_entry_stage=run->entry_stage;run->previous_entry_phase=run->entry_phase;
        return;
    }
    if(boundary==NATIVE_FRAME_BODY_BEGIN) {
        if(run->begun) abort();
        run->begun=1;
        ++run->body;
        run->first_tick=game->ticks;run->saved_tick=saved_tick;
        run->iteration=run->replay.iteration;run->stage=rd_u32(STAGE_CALLBACK);
        run->phase=rd_u8(PLAYER_PHASE);
        run->contact=rd_u16(CONTROL_RECORDS+2);
        run->qualification=rd_u16(rd_u32(MODE_TABLE));
        memcpy(run->before,game->storage.buffers,0x80000);
        memcpy(run->before+0x80000,game->storage.source,0x80000);
        if(run->keep_entry) {
            if(run->entries+run->captures>=240) abort();
            snapshot(run,"entry",run->entries,"before",run->entry_before);
            snapshot(run,"entry",run->entries,"after",run->before);
            printf("{\"entry\":%u,\"iteration\":%u,\"stage\":\"%06X\",\"tick\":%u,\"keys\":[",
                run->entries++,run->iteration,run->entry_stage,run->entry_tick);
            for(unsigned i=0;i<run->key_count;++i) printf("%s%u",i?",":"",run->entry_keys[i]);
            printf("],\"phase_before\":%u,\"phase_after\":%u,\"qualification\":%u}\n",
                run->entry_phase,run->phase,run->qualification);run->keep_entry=0;
        }
        /* A repeat qualification still executes the success/write path when
         * the original ADF already has qualification word one. */
        if(run->landed && run->entry_stage==0xc110a4 && run->entry_phase==0xff &&
           run->phase==0xef && run->qualification==1) run->success=1;
        if(run->success && run->entry_stage==0xc0f992) run->restarted=1;
        return;
    }
    if(!run->begun) return;
    run->begun=0;
    const uint16_t contact=rd_u16(CONTROL_RECORDS+2);
    const uint8_t phase=rd_u8(PLAYER_PHASE);
    const uint16_t qualification=rd_u16(rd_u32(MODE_TABLE));
    if(rd_u8(MODE_SELECT)==9 && run->stage==0xc10dae && !(contact&0x80)) run->airborne=1;
    const int touchdown=run->airborne && !run->landed && (contact&0xc080)==0xc080;
    if(touchdown) {run->landed=1;run->window=96;}
    const int keep=run->stage!=run->previous_stage || run->body%128==0 ||
        run->window || phase!=run->phase || qualification!=run->qualification ||
        ((contact^run->contact)&0x80);
    if(keep) {
        if(run->captures+run->entries>=240) {fputs("Qualification capture budget exhausted\n",stderr);abort();}
        snapshot(run,"body",run->captures,"before",run->before);
        /* NativeStorage's two banks are not assumed contiguous. */
        uint8_t *after=malloc(0x100000);
        if(!after) abort();
        memcpy(after,game->storage.buffers,0x80000);
        memcpy(after+0x80000,game->storage.source,0x80000);
        snapshot(run,"body",run->captures,"after",after);free(after);
        printf("{\"capture\":%u,\"body\":%u,\"iteration\":%u,\"stage\":\"%06X\","
               "\"before_tick\":%u,\"after_tick\":%u,\"saved_tick\":%u,\"owner_exit\":%s,"
               "\"phase_before\":%u,\"phase_after\":%u,\"contact_before\":%u,\"contact_after\":%u,"
               "\"qualification_before\":%u,\"qualification_after\":%u,\"touchdown\":%s,\"landing_window\":%s}\n",
               run->captures++,run->body,run->iteration,run->stage,run->first_tick,game->ticks,
               run->saved_tick,boundary==NATIVE_FRAME_OWNER_EXIT?"true":"false",
               run->phase,phase,run->contact,contact,run->qualification,qualification,
               touchdown?"true":"false",run->window?"true":"false");
    }
    if(run->window) --run->window;
    run->previous_stage=run->stage;
}

int main(int argc,char **argv) {
    if(argc!=5 && (argc!=6 || strcmp(argv[5],"new-pilot"))) return 1;
    /* ADF, fresh save directory, consumed input, prefix, optional saved-pilot fixture. */
    NativeFrontend *game=calloc(1,sizeof *game);
    Sequence run={0};char error[256];int result=1;
    run.prefix=argv[4];run.before=malloc(0x100000);run.entry_before=malloc(0x100000);
    if(!game || !run.before || !run.entry_before) goto done;
    if(!native_replay_load(&run.replay,argv[3],error,sizeof error) ||
       !native_frontend_open(game,argv[1],argv[2],error,sizeof error)) {
        fprintf(stderr,"%s\n",error);goto done;
    }
    if(argc==6) {
        /* Validation-only saved qualification word, never flight state.
         * Reopen the game through its normal loader before replay starts. */
        native_frontend_enlist(game);
        if(!rd_u16(MENU_FILE_READY) || rd_u16(MENU_TABLE_STATUS)) goto done;
        wr_u16(rd_u32(MODE_TABLE),0);
        native_frontend_save_log(game);native_frontend_close(game);
        if(!native_frontend_open(game,argv[1],argv[2],error,sizeof error)) {
            fprintf(stderr,"%s\n",error);goto done;
        }
        if(rd_u16(rd_u32(MODE_TABLE))!=0) goto done;
    }
    const unsigned initial_qualification=rd_u16(rd_u32(MODE_TABLE));
    game->begin_update=native_replay_update;game->update_context=&run.replay;
    game->observe_frame=observe;game->frame_context=&run;
    while(game->ticks<30000 && run.replay.iteration<run.replay.end) {
        if(game->ticks==1800) native_frontend_event(game,32,1);
        if(game->ticks==1802) native_frontend_event(game,32,0);
        native_frontend_tick(game);
    }
    const unsigned mode=rd_u8(MODE_SELECT),queued=game->input_count,resets=game->postflight_resets;
    const gaddr terminal_stage=rd_u32(STAGE_CALLBACK);
    if(run.replay.iteration!=run.replay.end || !run.airborne || !run.landed ||
       !run.success || !run.restarted || resets || queued || mode!=9 || terminal_stage!=0xc10dae) goto done;
    const gaddr log=rd_u32(MODE_TABLE);
    uint8_t saved[78];char path[4096];
    if(snprintf(path,sizeof path,"%s/config",argv[2])>=(int)sizeof path) goto done;
    FILE *file=fopen(path,"rb");
    if(!file) goto done;
    const int read=fread(saved,1,sizeof saved,file)==sizeof saved && fgetc(file)==EOF;
    if(fclose(file) || !read) goto done;
    for(unsigned i=0;i<sizeof saved;++i) if(saved[i]!=rd_u8(log+i)) goto done;
    native_frontend_close(game);
    /* A second cold startup must consume precisely the persisted log. */
    if(!native_frontend_open(game,argv[1],argv[2],error,sizeof error)) {
        fprintf(stderr,"Reload failed: %s\n",error);goto done;
    }
    const gaddr loaded=rd_u32(MODE_TABLE);
    for(unsigned i=0;i<sizeof saved;++i) if(saved[i]!=rd_u8(loaded+i)) goto done;
    run.reloaded=1;
    printf("{\"sequence\":true,\"iterations\":%u,\"events\":%zu,\"captures\":%u,\"bodies\":%u,"
           "\"airborne\":%s,\"landed\":%s,\"success\":%s,\"restarted\":%s,\"mode\":%u,"
           "\"stage\":\"%06X\",\"queued\":%u,\"crash_resets\":%u,\"initial_qualification\":%u,\"reloaded\":%s}\n",
           run.replay.iteration,run.replay.next,run.captures,run.body,
           run.airborne?"true":"false",run.landed?"true":"false",run.success?"true":"false",
           run.restarted?"true":"false",mode,terminal_stage,queued,resets,initial_qualification,run.reloaded?"true":"false");
    result=0;
done:
    if(game) native_frontend_close(game);
    native_replay_close(&run.replay);free(run.entry_before);free(run.before);free(game);return result;
}
