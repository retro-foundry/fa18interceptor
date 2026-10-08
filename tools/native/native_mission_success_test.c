/* Validation pilot for normally reached mission objectives and landing.
 * Only keyboard events are delivered; all flight and outcome RAM is read-only. */
#include "native/frontend.h"
#include "globals.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    double home[3], forward[3], previous_x, previous_y;
    unsigned scene, phase, target, started, objective, target_press, completions, grade;
    int rudder, pitch, roll, throttle;
    FILE *keys;
} Pilot;

typedef struct {
    const char *prefix;
    uint8_t *before, *entry_before;
    unsigned body, captures, entries, first_tick, entry_tick, iteration, window;
    unsigned entry_keys[256], key_count;
    uint16_t saved_tick, contact, completions, speed;
    uint8_t phase, region, confirmation, entry_phase, previous_entry_phase;
    gaddr stage, previous_stage, entry_stage, previous_entry_stage;
    int begun, keep_entry, airborne, landed;
} Observation;

static void copy_ram(uint8_t *destination,const NativeFrontend *game) {
    memcpy(destination,game->storage.buffers,0x80000);
    memcpy(destination+0x80000,game->storage.source,0x80000);
}
static void snapshot(const Observation *run,const char *tag,unsigned index,
                     const char *suffix,const uint8_t *data) {
    char path[4096];
    int size=snprintf(path,sizeof path,"%s.%s.%u.%s.dat",run->prefix,tag,index,suffix);
    if(size<0 || size>=(int)sizeof path) abort();
    FILE *file=fopen(path,"wb");
    if(!file) {perror(path);abort();}
    int written=fwrite(data,1,0x100000,file)==0x100000;
    if(fclose(file) || !written) {fprintf(stderr,"Cannot write %s\n",path);abort();}
}
static void capture_budget(const Observation *run) {
    /* At most 480 MiB before comparison; passing cases are then removed. */
    if(run->captures+run->entries>=240) {
        fputs("Mission capture budget exhausted\n",stderr);abort();
    }
}
static void observe(NativeFrontend *game,enum NativeFrameBoundary boundary,
                    uint16_t saved_tick,void *context) {
    Observation *run=context;
    if(boundary==NATIVE_FRAME_INPUT_BEGIN) {
        run->entry_stage=rd_u32(STAGE_CALLBACK);run->entry_phase=rd_u8(PLAYER_PHASE);
        run->entry_tick=game->ticks;run->key_count=game->input_count;
        run->keep_entry=run->entry_stage!=run->previous_entry_stage ||
            run->entry_phase!=run->previous_entry_phase || run->entry_stage==0xc110a4;
        if(run->keep_entry) {
            copy_ram(run->entry_before,game);
            for(unsigned i=0;i<run->key_count;++i)
                run->entry_keys[i]=game->input_keys[(game->input_read+i)&255u];
        }
        run->previous_entry_stage=run->entry_stage;
        run->previous_entry_phase=run->entry_phase;
        return;
    }
    if(boundary==NATIVE_FRAME_BODY_BEGIN) {
        if(run->begun) abort();
        run->begun=1;++run->body;
        run->first_tick=game->ticks;run->saved_tick=saved_tick;
        run->iteration=game->update_iterations;run->stage=rd_u32(STAGE_CALLBACK);
        run->phase=rd_u8(PLAYER_PHASE);run->contact=rd_u16(CONTROL_RECORDS+2);
        run->region=rd_u8(CONTROL_RECORDS+4);run->speed=rd_u16(CONTROL_RECORDS+110);
        run->confirmation=rd_u8(PLAYER_FLAGS_F);
        run->completions=rd_u16(rd_u32(MODE_TABLE)+56);
        copy_ram(run->before,game);
        if(run->keep_entry) {
            capture_budget(run);
            snapshot(run,"entry",run->entries,"before",run->entry_before);
            snapshot(run,"entry",run->entries,"after",run->before);
            printf("{\"entry\":%u,\"iteration\":%u,\"stage\":\"%06X\",\"tick\":%u,\"keys\":[",
                run->entries++,run->iteration,run->entry_stage,run->entry_tick);
            for(unsigned i=0;i<run->key_count;++i) printf("%s%u",i?",":"",run->entry_keys[i]);
            printf("],\"phase_before\":%u,\"phase_after\":%u,\"completions\":%u}\n",
                run->entry_phase,run->phase,run->completions);
            run->keep_entry=0;
        }
        return;
    }
    /* Menu ticks can publish BODY_END without entering a flight body. */
    if(!run->begun) return;
    run->begun=0;
    const uint16_t contact=rd_u16(CONTROL_RECORDS+2),speed=rd_u16(CONTROL_RECORDS+110);
    const uint8_t phase=rd_u8(PLAYER_PHASE),region=rd_u8(CONTROL_RECORDS+4);
    const uint8_t confirmation=rd_u8(PLAYER_FLAGS_F);
    const uint16_t completions=rd_u16(rd_u32(MODE_TABLE)+56);
    if(rd_u8(MODE_SELECT)==3 && run->stage==0xc10dae && !(contact&0x80)) run->airborne=1;
    const int touchdown=run->airborne && !run->landed && (contact&0x80);
    if(touchdown) {run->landed=1;run->window=96;}
    const int ready=run->landed && (region&4) && !speed;
    const int ready_before=run->landed && (run->region&4) && !run->speed;
    const int keep=run->stage!=run->previous_stage || run->body%512==0 ||
        run->window || phase!=run->phase || completions!=run->completions ||
        ((contact^run->contact)&0x80) || confirmation!=run->confirmation ||
        (run->landed && ((region^run->region)&4)) || (ready && !ready_before);
    if(keep) {
        capture_budget(run);
        snapshot(run,"body",run->captures,"before",run->before);
        /* The entry buffer is free until the next INPUT_BEGIN. */
        copy_ram(run->entry_before,game);
        snapshot(run,"body",run->captures,"after",run->entry_before);
        printf("{\"capture\":%u,\"body\":%u,\"iteration\":%u,\"stage\":\"%06X\","
               "\"before_tick\":%u,\"after_tick\":%u,\"saved_tick\":%u,\"owner_exit\":%s,"
               "\"phase_before\":%u,\"phase_after\":%u,\"contact_before\":%u,\"contact_after\":%u,"
               "\"confirmation_before\":%u,\"confirmation_after\":%u,\"selected\":%d,"
               "\"view_record\":%u,\"region_before\":%u,\"region_after\":%u,\"speed\":%u,"
               "\"completions_before\":%u,\"completions_after\":%u,"
               "\"touchdown\":%s,\"landing_window\":%s,\"ready\":%s}\n",
               run->captures++,run->body,run->iteration,run->stage,run->first_tick,game->ticks,
               run->saved_tick,boundary==NATIVE_FRAME_OWNER_EXIT?"true":"false",
               run->phase,phase,run->contact,contact,run->confirmation,confirmation,
               rd_s16(SELECTED_RECORD),rd_u16(VIEW_RECORD),run->region,region,speed,
               run->completions,completions,touchdown?"true":"false",
               run->window?"true":"false",ready?"true":"false");
    }
    if(run->window) --run->window;
    run->previous_stage=run->stage;
}

static void key(Pilot *pilot,NativeFrontend *game,int code,int down) {
    native_frontend_event(game,code,down);
    fprintf(pilot->keys,"F %u K %d 0 0 %d\n",game->ticks,code,down);
}
static void held(Pilot *pilot,NativeFrontend *game,int *current,int code) {
    if(*current==code) return;
    if(*current) key(pilot,game,*current,0);
    if(code) key(pilot,game,code,1);
    *current=code;
}
static double angle_delta(double a,double b) {
    double result=a-b;
    while(result>3.141592653589793) result-=6.283185307179586;
    while(result< -3.141592653589793) result+=6.283185307179586;
    return result;
}
static void fly(Pilot *pilot,NativeFrontend *game) {
    if(game->flight_timer_pending || game->scene_frames==pilot->scene) return;
    pilot->scene=game->scene_frames;
    if(!pilot->started && rd_u32(STAGE_CALLBACK)==0xc10dae) {
        pilot->started=game->ticks;
        pilot->completions=rd_u16(rd_u32(MODE_TABLE)+56);
        pilot->grade=rd_u8(rd_u32(MODE_TABLE)+21);
        for(unsigned i=0;i<3;++i) {
            pilot->home[i]=(double)rd_s32(CONTROL_RECORDS+20+4*i)/256;
            pilot->forward[i]=(double)rd_s16(CONTROL_RECORDS+150+6*i)/16384;
        }
    }
    if(!pilot->started) return;
    if(pilot->phase==2 && (rd_u8(CONTROL_RECORDS+3)&0x80)) pilot->phase=3;
    if(pilot->phase==3) {
        held(pilot,game,&pilot->pitch,0);held(pilot,game,&pilot->roll,0);
        if(rd_u8(CONTROL_RECORDS+4)&4) {
            held(pilot,game,&pilot->rudder,0);
            held(pilot,game,&pilot->throttle,rd_u16(CONTROL_RECORDS+110)?45:0);
            return;
        }
    }
    if(!pilot->objective && rd_s16(SELECTED_RECORD)<0 && game->ticks>pilot->target_press+200) {
        key(pilot,game,116,1);pilot->target_press=game->ticks;
    }
    if(game->ticks<pilot->started+650) {
        held(pilot,game,&pilot->throttle,61);
        held(pilot,game,&pilot->pitch,game->ticks>=pilot->started+550?274:0);
        held(pilot,game,&pilot->rudder,0);
        return;
    }
    double point[3],position[3],local[3]={0};
    for(unsigned i=0;i<3;++i) position[i]=(double)rd_s32(CONTROL_RECORDS+20+4*i)/256;
    if(!pilot->target) {
        double nearest=1e30;
        for(unsigned slot=1;slot<16;++slot) {
            const gaddr record=CONTROL_RECORDS+512*slot;
            if(!(rd_u8(record+1)&0x40) || (rd_u8(record+98)&0xf0)!=0x10) continue;
            double distance=0;
            for(unsigned i=0;i<3;++i) {
                double delta=(double)rd_s32(record+20+4*i)/256-position[i];distance+=delta*delta;
            }
            if(distance<nearest) {nearest=distance;pilot->target=slot;}
        }
    }
    if(rd_u8(PLAYER_PHASE)==0xff || rd_u8(PLAYER_PHASE)==1) {
        if(!pilot->objective) printf("{\"objective\":true,\"tick\":%u,\"phase\":%u}\n",game->ticks,rd_u8(PLAYER_PHASE));
        pilot->objective=1;if(!pilot->phase) pilot->phase=1;
    }
    if(!pilot->objective && pilot->target) {
        const gaddr target=CONTROL_RECORDS+512*pilot->target;
        for(unsigned i=0;i<3;++i) point[i]=(double)rd_s32(target+20+4*i)/256;
        /* Takeoff safety is pilot input, not a modification to game physics. */
        if(position[1]<pilot->home[1]+200) point[1]=position[1]+1000;
    } else {
        for(unsigned i=0;i<3;++i) point[i]=pilot->home[i]-pilot->forward[i]*12000;
        point[1]=pilot->home[1]+700;
        if(pilot->phase==1 && hypot(point[0]-position[0],point[2]-position[2])<1800) pilot->phase=2;
        if(pilot->phase>=2) {
            for(unsigned i=0;i<3;++i) point[i]=pilot->home[i]+pilot->forward[i]*2500;
            point[1]=pilot->home[1];
            held(pilot,game,&pilot->throttle,285); /* Source F4 level for approach. */
        }
    }
    for(unsigned i=0;i<3;++i) for(unsigned j=0;j<3;++j)
        local[i]+=(point[j]-position[j])*rd_s16(CONTROL_RECORDS+146+6*j+2*i)/16384;
    const double x=atan2(local[0],local[2]);
    const double y=atan2(local[1],hypot(local[0],local[2]));
    const double px=x+4*angle_delta(x,pilot->previous_x),py=y+4*angle_delta(y,pilot->previous_y);
    pilot->previous_x=x;pilot->previous_y=y;
    double bank=rd_u16(CONTROL_RECORDS+106)*6.283185307179586/28800;
    if(bank>3.141592653589793) bank-=6.283185307179586;
    held(pilot,game,&pilot->roll,pilot->phase==2 && fabs(x)<0.25?
        (bank>0.025?275:bank< -0.025?276:0):0);
    held(pilot,game,&pilot->rudder,px>0.006?44:px< -0.006?46:0);
    held(pilot,game,&pilot->pitch,pilot->phase==3?0:py>0.006?274:py< -0.006?273:0);
}

int main(int argc,char **argv) {
    /* ADF, fresh save, output keys, capture prefix. Mode three/aircraft two. */
    if(argc!=5) return 1;
    NativeFrontend *game=calloc(1,sizeof *game);Pilot pilot={0};Observation run={0};
    char error[256];int result=1;
    run.prefix=argv[4];run.before=malloc(0x100000);run.entry_before=malloc(0x100000);
    if(!game || !run.before || !run.entry_before) goto done;
    pilot.keys=fopen(argv[3],"w");
    if(!pilot.keys) goto done;
    fputs("E9K_INPUT_V1\n",pilot.keys);
    if(!native_frontend_open(game,argv[1],argv[2],error,sizeof error)) {fprintf(stderr,"%s\n",error);goto done;}
    game->observe_frame=observe;game->frame_context=&run;
    const unsigned times[]={1800,3000,4500,6500,8000,14500};
    const int keys[]={32,54,282,13,13,50};
    while(game->ticks<32000) {
        for(unsigned i=0;i<6;++i) {
            if(game->ticks==times[i]) key(&pilot,game,keys[i],1);
            if(game->ticks==times[i]+2) key(&pilot,game,keys[i],0);
        }
        if(pilot.target_press && game->ticks==pilot.target_press+2) key(&pilot,game,116,0);
        fly(&pilot,game);native_frontend_tick(game);
        if(game->postflight_resets) break;
        if(pilot.started && rd_u16(rd_u32(MODE_TABLE)+56)!=pilot.completions) break;
    }
    const unsigned mode=rd_u8(MODE_SELECT),phase=rd_u8(PLAYER_PHASE),ticks=game->ticks;
    const unsigned resets=game->postflight_resets,region=rd_u8(CONTROL_RECORDS+4);
    const unsigned speed=rd_u16(CONTROL_RECORDS+110),contact=rd_u16(CONTROL_RECORDS+2);
    const gaddr log=rd_u32(MODE_TABLE),stage=rd_u32(STAGE_CALLBACK);
    const unsigned completions=rd_u16(log+56),grade=rd_u8(log+21);
    if(!run.airborne || !run.landed || !pilot.objective || !pilot.started || resets ||
       mode!=3 || phase!=0xfc || !(contact&0x80) || !(region&4) || speed ||
       completions!=pilot.completions+1 || rd_u8(log+6)!=3 || rd_u8(log+7)!=pilot.grade ||
       grade!=(pilot.grade<3?pilot.grade+1:3)) {
        fprintf(stderr,"Mission incomplete: tick %u, mode %u, phase %u, completions %u -> %u, resets %u\n",
            ticks,mode,phase,pilot.completions,completions,resets);goto done;
    }
    uint8_t saved[78];char path[4096];
    if(snprintf(path,sizeof path,"%s/config",argv[2])>=(int)sizeof path) goto done;
    FILE *file=fopen(path,"rb");
    if(!file) {perror(path);goto done;}
    int read=fread(saved,1,sizeof saved,file)==sizeof saved && fgetc(file)==EOF;
    if(fclose(file) || !read) goto done;
    for(unsigned i=0;i<sizeof saved;++i) if(saved[i]!=rd_u8(log+i)) goto done;
    native_frontend_close(game);
    if(!native_frontend_open(game,argv[1],argv[2],error,sizeof error)) {
        fprintf(stderr,"Reload failed: %s\n",error);goto done;
    }
    const gaddr loaded=rd_u32(MODE_TABLE);
    for(unsigned i=0;i<sizeof saved;++i) if(saved[i]!=rd_u8(loaded+i)) goto done;
    printf("{\"finished\":true,\"airborne\":true,\"landed\":true,\"objective\":true,"
           "\"mode\":%u,\"stage\":\"%06X\",\"phase\":%u,\"ticks\":%u,\"target\":%u,"
           "\"completions_before\":%u,\"completions_after\":%u,\"grade_before\":%u,\"grade_after\":%u,"
           "\"region\":%u,\"speed\":%u,\"crash_resets\":%u,\"reloaded\":true}\n",
           mode,stage,phase,ticks,pilot.target,pilot.completions,completions,pilot.grade,grade,
           region,speed,resets);
    result=0;
done:
    if(pilot.keys) fclose(pilot.keys);
    if(game) native_frontend_close(game);
    free(run.entry_before);free(run.before);free(game);return result;
}
