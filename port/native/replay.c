#include "replay.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

static int read_unsigned(const char *text,unsigned *value) {
    char *end;
    if(*text<'0' || *text>'9') return 0;
    errno=0;
    unsigned long number=strtoul(text,&end,10);
    if(errno || *end || number>UINT_MAX) return 0;
    *value=(unsigned)number;return 1;
}

void native_replay_close(NativeReplay *replay) { free(replay->events);memset(replay,0,sizeof *replay); }
int native_replay_load(NativeReplay *replay,const char *path,char *error,size_t capacity) {
    FILE *file=fopen(path,"r");char line[256];unsigned row=1;size_t allocated=0;
    const char *reason="cannot open input";
    memset(replay,0,sizeof *replay);
    if(!file) goto failed;
    reason="invalid FA18_LOOP_INPUT_V1 or FA18_GAME_INPUT_V1 header";
    if(!fgets(line,sizeof line,file)) goto failed;
    line[strcspn(line,"\r\n")]=0;
    if(strcmp(line,"FA18_LOOP_INPUT_V1") && strcmp(line,"FA18_GAME_INPUT_V1")) goto failed;
    while(fgets(line,sizeof line,file)) {
        unsigned iteration,frame,key,down;char tokens[5][32],extra;int fields;
        ++row;
        if(line[0]=='#' || line[0]=='\n') continue;
        reason="events after end row";
        if(replay->end) goto failed;
        fields=sscanf(line,"%31s %31s %31s %31s %31s %c",
                      tokens[0],tokens[1],tokens[2],tokens[3],tokens[4],&extra);
        if(fields>0 && !strcmp(tokens[0],"end")) {
            reason="invalid end iteration";
            if(fields!=3 || !read_unsigned(tokens[1],&iteration) || !read_unsigned(tokens[2],&frame) ||
               !iteration || (replay->count && iteration<replay->events[replay->count-1].iteration)) goto failed;
            replay->end=iteration;continue;
        }
        reason="unsupported replay event (native keyboard delivery supports K)";
        if(fields>=3 && strcmp(tokens[2],"K")) goto failed;
        reason="invalid or unordered raw keyboard event";
        if(fields!=5 || !read_unsigned(tokens[0],&iteration) || !read_unsigned(tokens[1],&frame) ||
           !read_unsigned(tokens[3],&key) || !read_unsigned(tokens[4],&down) || !iteration || key>127 || down>1 ||
           (replay->count && iteration<replay->events[replay->count-1].iteration)) goto failed;
        if(replay->count==allocated) {
            allocated=allocated?allocated*2:256;
            NativeReplayEvent *events=realloc(replay->events,allocated*sizeof *events);
            reason="cannot allocate replay events";
            if(!events) goto failed;
            replay->events=events;
        }
        replay->events[replay->count++]=(NativeReplayEvent){iteration,(uint8_t)key,(uint8_t)down};
    }
    reason="input read error or missing end row";
    if(ferror(file) || !replay->end) goto failed;
    if(fclose(file)) { file=NULL;reason="input close error";goto failed; }
    return 1;
failed:
    if(file) fclose(file);
    snprintf(error,capacity,"Native input %s row %u: %s",path,row,reason);
    native_replay_close(replay);return 0;
}
void native_replay_update(NativeFrontend *game,void *context) {
    NativeReplay *replay=context;
    if(!replay->started) {
        if(game->screen!=NATIVE_MENU) return;
        replay->started=1;
    }
    ++replay->iteration;
    while(replay->next<replay->count && replay->events[replay->next].iteration==replay->iteration) {
        NativeReplayEvent *event=&replay->events[replay->next++];
        native_frontend_raw_event(game,event->key,event->down);
    }
}
