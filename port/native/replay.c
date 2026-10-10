#include "replay.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include "../game/globals.h"

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
/* Anchors name source callback entry events, not fitted native frame offsets.
 * Input ordinals within each segment come from verified original JSR/LINK
 * identities. Loading and reporting occur outside the gameplay memory lock. */
int native_replay_load_anchors(NativeReplay *replay,const char *path,char *error,size_t capacity) {
    FILE *file=fopen(path,"r");char line[256];unsigned row=1;
    const char *reason="cannot open anchors";
    if(!file) goto failed;
    reason="invalid FA18_REPLAY_ANCHORS_V1 header";
    if(!fgets(line,sizeof line,file)) goto failed;
    line[strcspn(line,"\r\n")]=0;
    if(strcmp(line,"FA18_REPLAY_ANCHORS_V1")) goto failed;
    while(fgets(line,sizeof line,file)) {
        char tokens[4][32],extra;unsigned first,mode,tick,stage=0;
        ++row;
        if(line[0]=='#' || line[0]=='\n' || line[0]=='\r') continue;
        reason="invalid anchor row (SOURCE_FIRST MODE STAGE GAME_TICK)";
        if(sscanf(line,"%31s %31s %31s %31s %c",tokens[0],tokens[1],tokens[2],tokens[3],&extra)!=4 ||
           !read_unsigned(tokens[0],&first) || !read_unsigned(tokens[1],&mode) ||
           !read_unsigned(tokens[3],&tick) || strlen(tokens[2])!=6 || mode>255 || tick>65535) goto failed;
        for(unsigned i=0;i<6;++i) {
            unsigned char ch=(unsigned char)tokens[2][i];unsigned value;
            if(ch>='0' && ch<='9') value=ch-'0';
            else if(ch>='a' && ch<='f') value=ch-'a'+10;
            else if(ch>='A' && ch<='F') value=ch-'A'+10;
            else goto failed;
            stage=stage*16+value;
        }
        reason="anchor capacity, source range or ordering invalid";
        if(replay->anchor_count==NATIVE_REPLAY_ANCHOR_CAPACITY || !first || first>replay->end ||
           (replay->anchor_count && first<=replay->anchors[replay->anchor_count-1].source_first)) goto failed;
        reason="first anchor must be the ordinary cold main-menu origin";
        if(!replay->anchor_count && (first!=1 || mode || stage!=0xc0fcb4 || tick)) goto failed;
        replay->anchors[replay->anchor_count++]=(NativeReplayAnchor){first,mode,stage,tick,0,0};
    }
    reason="anchor read error or missing anchors";
    if(ferror(file) || !replay->anchor_count) goto failed;
    if(fclose(file)) {file=NULL;reason="anchor close error";goto failed;}
    return 1;
failed:
    if(file) fclose(file);
    snprintf(error,capacity,"Native anchors %s row %u: %s",path,row,reason);
    return 0;
}
int native_replay_write_anchors(const NativeReplay *replay,const char *path) {
    FILE *file=fopen(path,"w");int ok;
    if(!file) return 0;
    ok=fprintf(file,"{\"format\":\"FA18_REPLAY_ANCHORS_V1\",\"complete\":%s,\"failed\":%s,"
        "\"native_updates\":%u,\"source_position\":%u,\"keys_consumed\":%zu,\"anchors\":[",
        replay->anchor_complete?"true":"false",replay->anchor_failed?"true":"false",
        replay->iteration,replay->source_iteration,replay->next)>=0;
    for(unsigned i=0;i<replay->anchor_count;++i) {
        const NativeReplayAnchor *anchor=&replay->anchors[i];
        if(fprintf(file,"%s{\"source_first\":%u,\"mode\":%u,\"stage\":\"%06X\",\"game_tick\":%u,"
            "\"native_first\":%u,\"frame\":%u}",i?",":"",anchor->source_first,anchor->mode,
            anchor->stage,anchor->game_tick,anchor->native_first,anchor->frame)<0) ok=0;
    }
    if(fputs("]}\n",file)==EOF) ok=0;
    if(fclose(file)) ok=0;
    return ok;
}
static int anchor_matches(const NativeReplayAnchor *anchor) {
    return rd_u8(MODE_SELECT)==anchor->mode && rd_u32(STAGE_CALLBACK)==anchor->stage &&
           rd_u16(UPDATE_TICK)==anchor->game_tick;
}
void native_replay_update(NativeFrontend *game,void *context) {
    NativeReplay *replay=context;
    if(!replay->started) {
        if(game->screen!=NATIVE_MENU) return;
        replay->started=1;
    }
    ++replay->iteration;
    unsigned source=replay->iteration;
    if(replay->anchor_count) {
        NativeReplayAnchor *anchor=&replay->anchors[replay->anchor_active];
        if(!anchor->native_first) {
            if(!anchor_matches(anchor)) {replay->anchor_failed=1;return;}
            anchor->native_first=replay->iteration;anchor->frame=game->ticks;
        }
        if(replay->anchor_active+1<replay->anchor_count && anchor_matches(anchor+1)) {
            /* An event may never skip an undelivered previous-segment key. */
            if(replay->next<replay->count && replay->events[replay->next].iteration<(anchor+1)->source_first) {
                replay->anchor_failed=1;return;
            }
            anchor=&replay->anchors[++replay->anchor_active];
            anchor->native_first=replay->iteration;anchor->frame=game->ticks;
        }
        source=anchor->source_first+replay->iteration-anchor->native_first;
        replay->source_iteration=source;
        /* Every native wait remains observed/counted. Future-segment keys
         * wait for their actual declared event, even on a longer menu path. */
        if(replay->anchor_active+1<replay->anchor_count && source>=(anchor+1)->source_first) return;
        if(source>replay->end || (replay->next<replay->count && replay->events[replay->next].iteration<source)) {
            replay->anchor_failed=1;return;
        }
    }
    while(replay->next<replay->count && replay->events[replay->next].iteration==source) {
        NativeReplayEvent *event=&replay->events[replay->next++];
        native_frontend_raw_event(game,event->key,event->down);
    }
    if(replay->anchor_count && replay->anchor_active+1==replay->anchor_count && source==replay->end)
        replay->anchor_complete=replay->next==replay->count;
}
