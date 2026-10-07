/* Real keyboard-driven Free Flight, sharing every runtime object with the
 * playable runner. Capture launch and expiry bodies for original comparison. */
#include "native/frontend.h"
#include "native/control_effects.h"
#include "native/input.h"
#include "../../port/native/frame_capture.h"
#include "globals.h"
#include "menu_setup.h"
#include "stages.h"
#include "view.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    NativeFrameCapture capture;
    NativeReplay clock;
    const char *prefix;
    char path[4096];
    unsigned captures,launches,expiry;
    int message_sampling;
    unsigned message_case;
    int clear_sampling;
    unsigned clear_case;
    int hud_sampling;
    unsigned hud_case;
    int debug_sampling;
    unsigned debug_case;
    int label_sampling;
    unsigned label_case;
    int marker_sampling;
    unsigned marker_case;
    int grid_sampling;
    unsigned grid_case;
    int cleanup_sampling;
    unsigned cleanup_case;
    gaddr setup_stage;
    NativeInputReturn setup_output;
    NativeFrameCapture idle_stage;
} CountermeasureFixture;
static int pending_input(NativeFrontend *game,CountermeasureFixture *fixture,unsigned variant) {
    const unsigned settings=variant%12,mode=1+settings/4;
    const int chain=variant>=12 && variant<24;
    const int message=variant>=24;
    wr_u8(RECORDER_MODE,(uint8_t)mode);wr_u8(MODE_SELECT,1);
    wr_u16(RECORD_WORD_A,chain?6:(settings&2)?4:2);wr_u16(RECORD_WORD_B,0);
    wr_u16(PENDING_COMMAND_WORD_A,0);wr_u16(PENDING_COMMAND_WORD_B,0);
    wr_u8(MISSION_LEVEL_A,chain?(settings&1):(settings&1)?0x80:1);
    wr_u8(MISSION_LEVEL_B,chain?2:(settings&1)?0x80:1);
    wr_u8(KEY_TAKEN,chain || message?0:1);wr_u8(KEY_COUNT,0);
    wr_u8(KEY_WRITE,(settings&2)?9:0);wr_u8(KEY_TRANSLATED_WRITE,(settings&2)?7:0);
    wr_u8(KEY_STATE,0);wr_u8(KEY_STATE+1,0);wr_u8(KEY_STATE+2,0);
    wr_u32(EXTERNAL_INPUT_HANDLE,0x6400);wr_u32(KEYBOARD_INPUT_HANDLE,0x6500);
    game->joystick_directions=0;game->mouse_buttons=0;
    snprintf(fixture->path,sizeof fixture->path,"%s.pending.%u",fixture->prefix,variant);
    NativeFrameCapture capture={.replay=&fixture->clock,.prefix=fixture->path,
        .iteration=fixture->clock.iteration,.count=1};
    /* Controlled C0F3C4 parents after ordinary disk/input startup. Chain
     * cases establish KEY_TAKEN through a real successful flare publication. */
    native_frame_capture(game,NATIVE_FRAME_BODY_BEGIN,0,&capture);
    native_input_process(game);
    native_frame_capture(game,NATIVE_FRAME_BODY_END,0,&capture);
    printf("{\"pending_input\":%u,\"recorder_mode\":%u,\"chain\":%s}\n",variant,mode,chain?"true":"false");
    return capture.complete && !rd_u16(RECORD_WORD_A) && !rd_u16(RECORD_WORD_B) &&
        !game->input_count && (!chain || rd_u8(KEY_TAKEN)==1);
}
static int fd_input(NativeFrontend *game,CountermeasureFixture *fixture,unsigned variant) {
    const uint8_t raw=(uint8_t)(0x50+variant*9);
    wr_u8(RECORDER_MODE,0xfd);wr_u8(MODE_SELECT,1);
    wr_u8(ORIGIN_DETAIL_MODE,0);wr_u8(COMMAND_EVENT_COUNTER,1);
    wr_u8(KEY_STATE,0);wr_u8(KEY_STATE+1,0);wr_u8(KEY_TAKEN,0);
    wr_u32(EXTERNAL_INPUT_HANDLE,0x6400);wr_u32(KEYBOARD_INPUT_HANDLE,0x6500);
    game->joystick_directions=0;game->mouse_buttons=0;
    snprintf(fixture->path,sizeof fixture->path,"%s.fd.%u",fixture->prefix,variant);
    NativeFrameCapture capture={.replay=&fixture->clock,.prefix=fixture->path,
        .iteration=fixture->clock.iteration,.count=1};
    /* These files bracket C0F3C4 alone, with a controlled recorder mode. */
    native_frame_capture(game,NATIVE_FRAME_BODY_BEGIN,0,&capture);
    native_input_enqueue_raw(game,raw);native_input_process(game);
    native_frame_capture(game,NATIVE_FRAME_BODY_END,0,&capture);
    printf("{\"fd_input\":%u,\"raw\":%u}\n",variant,raw);
    return capture.complete && game->input_count==0;
}
static int interposed_input(NativeFrontend *game,CountermeasureFixture *fixture,unsigned variant) {
    const uint8_t keys[]={0x60,0xe0,0x66,0xe6,0x67,0xe7,0x70,0xf0,0x4c,0x4e,0x4c,0x4e,
        0x4c,0x4d,0x4e,0x4f,0xcc,0xce,0x38,0xb8,0x0c,0x8c,0x25,0x24,
        0x3e,0x1e,0x2d,0x2f,0x3d,0x1d,0x3f,0x1f,0x3c,0x1b,0x1a,0x9a,
        0x13,0x13,0x13,0x44,0x44,0x44,0x44,0x14,0x0d,0x20,0x21,0x26,
        0x40,0xc0,0x40,0x40,0x23,0x23,0x33,0x33,0x12,0x12,0x12,0x23,
        0x01,0x02,0x09,0x03,0x50,0x59,0x50,0x59,0x50,0x59,0x50,0x59,
        0x43,0x43,0x43,0x43,0x43,0x37,0x37,0x37,0x19,0x19,0x43,0x43};
    const uint8_t raw=keys[variant];
    wr_u8(RECORDER_MODE,0);wr_u16(RECORD_WORD_A,0);wr_u16(RECORD_WORD_B,0);
    wr_u16(PENDING_COMMAND_WORD_A,0);wr_u16(PENDING_COMMAND_WORD_B,0);
    const int controls=variant>=12;
    wr_u8(ORIGIN_DETAIL_MODE,controls?(variant==16 || variant==17 || variant==19 || variant==21?3:0):(variant&1?3:0));
    wr_u8(KEY_TAKEN,0);wr_u8(KEY_COUNT,controls?10:0);wr_u8(KEY_WRITE,0);
    wr_u8(KEY_TRANSLATED_WRITE,variant==11?255:7);
    wr_u8(KEY_STATE,0);wr_u8(KEY_STATE+1,0);wr_u8(KEY_STATE+2,0);
    wr_u8(COMMAND_EVENT_COUNTER,variant==8?254:variant==9?128:1);
    if(variant==22) wr_u8(POST_INPUT_EXPIRED,127); /* Actual signed wrap result. */
    if(variant==23) {
        wr_u8(CONTROL_RECORDS+3,rd_u8(CONTROL_RECORDS+3)&0x7f);
        wr_u32(COMMAND_GEAR_GATE,0x40);
    }
    if(variant>=24) {
        wr_u8(ORIGIN_DETAIL_MODE,0);wr_u8(ORIGIN_ENABLE,variant>=31 && variant<=34?3:0);
        wr_u8(VIEW_MODE,variant==27?11:0);
        wr_u8(ORIGIN_GATE_B,variant>=33 && variant<=34);
        wr_u8(ORIGIN_GATE_MODE,variant>=33 && variant<=34);
        wr_u32(SELECTOR_ORIGIN_MIDDLE,0x04000000);
    }
    if(variant>=36 && variant<48) {
        wr_u8(MODE_SELECT,1);wr_u16(VIEW_RECORD,0);wr_u8(COMMAND_BLOCK_FLAGS,variant==42?1:0);
        wr_u8(CONTROL_RECORDS+0x62,0x11);
        wr_u8(CONTROL_RECORDS+0x63,(uint8_t[]){0x09,0x0b,0x0d,0x80,0,0x30,0x10,0x19,0x10,0x10,0x10,0x10}[variant-36]);
    }
    if(variant>=48 && variant<60) {
        wr_u8(MODE_SELECT,variant==59?6:1);wr_u8(COMMAND_BLOCK_FLAGS,0);
        wr_u8(CONTROL_RECORDS+0x63,variant==50?0x10:0x30);
        wr_u8(COMMAND_WEAPON_PAUSE,0);
        if(variant==51) wr_u8(KEY_STATE,1); /* Modified space preserves output. */
        wr_u8(MISSION_LEVEL_B,variant==52?2:1);wr_u8(MISSION_LEVEL_A,variant==54?2:0x80);
        wr_u8(ORIGIN_GATE_A,variant==57?1:0);
        if(variant>=57) wr_u8(KEY_STATE,1);
        if(variant==58) {wr_u8(KEY_COUNT,0);wr_u8(KEY_TRANSLATED_WRITE,255);}
        if(variant==59) {
            wr_u16(COMMAND_SPAWN_GATE,0);
            wr_u8(CONTROL_RECORDS+0x201,rd_u8(CONTROL_RECORDS+0x201)&~0x40);
        }
    }
    if(variant>=60 && variant<72) {
        wr_u8(MODE_SELECT,1);wr_u8(COMMAND_BLOCK_FLAGS,0);
        wr_u8(COMMAND_ENABLE_GATE,variant==60 || variant==61);
        wr_u8(COMMAND_MODE_GATE,variant==62?0:1);
        wr_u8(COCKPIT_FLAGS,(rd_u8(COCKPIT_FLAGS)&~8)|(variant==63?8:0));
        wr_u8(PLAYER_READY,variant==64 || variant==67?0:1);
        wr_u8(FUNCTION_KEY_LEVEL,variant==66?12:0);wr_u8(CONTROL_RECORDS+0x2b,0);
        wr_u8(ORIGIN_GATE_A,variant==68 || variant==69);
        if(variant>=70) wr_u8(RECORDER_MODE,0xfd);
    }
    if(variant>=72) {
        wr_u8(MODE_SELECT,1);wr_u8(COMMAND_BLOCK_FLAGS,0);
        wr_u8(COMMAND_ENABLE_GATE,0);wr_u8(COMMAND_MODE_GATE,1);
        wr_u8(COCKPIT_FLAGS,rd_u8(COCKPIT_FLAGS)&~8);
        wr_u8(ORIGIN_ENABLE,variant==73 || variant==74 || variant==76 || variant==78 || variant==80 || variant>=82?3:0);
        wr_u8(ORIGIN_GATE_MODE,variant==79);wr_u8(ORIGIN_GATE_A,variant==81);
        wr_u16(VIEW_RECORD,512);
        wr_u8(KEY_STATE,variant==73 || variant==74 || variant==75 || variant==76 || variant==83);
        /* These are existing disk pose/preset rows. Record 14 is the real
         * aircraft from the collision fixture; record 15 is inactive. */
        wr_u8(SCENE_POSE_ENTRY,variant==73 || variant==75?3:variant==74?4:variant==83?1:0);
    }
    snprintf(fixture->path,sizeof fixture->path,"%s.interposed.%u",fixture->prefix,variant);
    NativeFrameCapture capture={.replay=&fixture->clock,.prefix=fixture->path,
        .iteration=fixture->clock.iteration,.count=1};
    native_frame_capture(game,NATIVE_FRAME_BODY_BEGIN,0,&capture);
    native_input_enqueue_raw(game,raw);native_input_process(game);
    native_frame_capture(game,NATIVE_FRAME_BODY_END,0,&capture);
    printf("{\"interposed_input\":%u,\"raw\":%u,\"return_owner\":%u,\"input_byte\":%u}\n",
        variant,raw,game->completed_input_return.owner,game->completed_input_return.value);
    if(variant>=36 && variant<60) {
        const unsigned owners[]={13,13,13,13,13,13,13,11,11,11,11,11,
            13,11,13,11,13,11,13,11,11,11,12,11};
        const uint8_t values[]={13,9,11,0x30,0x30,0x20,1,0,0,0,0,0,
            0x30,0,0x10,0,0x23,0,0x33,0,0,0,255,0};
        if(game->completed_input_return.owner!=owners[variant-36] ||
           game->completed_input_return.value!=values[variant-36]) {
            fprintf(stderr,"Interposed action %u returned owner %u value %u\n",variant,
                game->completed_input_return.owner,game->completed_input_return.value);return 0;
        }
    }
    if(variant>=60 && variant<72) {
        const unsigned owners[]={15,15,15,15,15,15,15,15,11,11,15,15};
        const uint8_t values[]={17,16,9,3,12,192,1,121,0,0,187,187};
        if(game->completed_input_return.owner!=owners[variant-60] ||
           game->completed_input_return.value!=values[variant-60]) {
            fprintf(stderr,"Indexed action %u returned owner %u value %u\n",variant,
                game->completed_input_return.owner,game->completed_input_return.value);return 0;
        }
        /* Next controlled recorder parent resumes ordinary Free Flight
         * readiness; it retains the actual command output just captured. */
        wr_u8(PLAYER_READY,1);
    }
    if(variant>=72) {
        const unsigned owners[]={16,16,16,16,16,16,16,16,11,11,16,16};
        const uint8_t values[]={0,47,20,0,0,0,0,0,0,0,0,0};
        if(game->completed_input_return.owner!=owners[variant-72] ||
           game->completed_input_return.value!=values[variant-72]) {
            fprintf(stderr,"Context action %u returned owner %u value %u\n",variant,
                game->completed_input_return.owner,game->completed_input_return.value);return 0;
        }
    }
    return capture.complete && game->completed_input_return.owner!=NATIVE_INPUT_RETURN_UNKNOWN;
}
static int collision_parent(NativeFrontend *game,CountermeasureFixture *fixture,unsigned variant) {
    const gaddr record=0xc45c72u,target=CONTROL_RECORDS+14*512;
    for(unsigned i=0;i<0x500;++i) wr_u8(record+i,0);
    wr_u8(MISSION_FLAGS_A,0);wr_u8(0xc46201u,0);
    wr_u8(0xc457bdu,0);wr_u8(0xc457aeu,0);
    wr_u8(0xc4585eu,1);wr_u32(0xc459c6u,0x6500);
    wr_u16(0x6500,0x0e10);wr_u32(0x6502,0x6600);wr_u32(0x6604,0x6700);
    wr_u16(0x6700,0);wr_u8(0x6707,variant&1?16:0);
    wr_u16(target,rd_u16(target)|0x40);wr_u8(target+123,0);
    wr_u16(record+48,rd_u16(target+6));wr_u16(record+50,rd_u16(target+8));
    wr_u32(record,(uint32_t)(int32_t)rd_s16(target+12)<<8);
    wr_u32(record+4,(rd_u32(target+16)<<8)+(variant>=2?0x10000:0));
    wr_u32(record+8,(uint32_t)(int32_t)rd_s16(target+14)<<8);
    wr_u16(record+38,0x401);wr_u16(record+40,10);
    snprintf(fixture->path,sizeof fixture->path,"%s.collision.%u",fixture->prefix,variant);
    NativeFrameCapture capture={.replay=&fixture->clock,.prefix=fixture->path,
        .iteration=fixture->clock.iteration,.count=1};
    /* These files bracket C1518C alone; they are not complete frame bodies. */
    native_frame_capture(game,NATIVE_FRAME_BODY_BEGIN,0,&capture);
    native_control_effects();
    native_frame_capture(game,NATIVE_FRAME_BODY_END,0,&capture);
    printf("{\"control_parent\":%u,\"collision_hit\":%s}\n",variant,
           rd_u16(record+38)&0x10?"true":"false");
    return capture.complete;
}
static void observe(NativeFrontend *game,enum NativeFrameBoundary boundary,uint16_t saved_tick,void *context) {
    CountermeasureFixture *fixture=context;
    fixture->clock.iteration=game->update_iterations;
    if(fixture->cleanup_sampling) {
        if(fixture->cleanup_case>=96) {
            if(boundary==NATIVE_FRAME_INPUT_BEGIN) {
                fixture->setup_stage=rd_u32(STAGE_CALLBACK);
                if(game->input_count || rd_u8(RECORDER_MODE) || rd_u16(RAW_KEY_LATCH) ||
                   rd_u16(RECORD_WORD_A) || rd_u16(RECORD_WORD_B)) {
                    fputs("Idle stage fixture has unexpected pending input\n",stderr);abort();
                }
                snprintf(fixture->path,sizeof fixture->path,"%s.stage.%u",fixture->prefix,fixture->cleanup_case);
                fixture->idle_stage=(NativeFrameCapture){.replay=&fixture->clock,.prefix=fixture->path,
                    .iteration=game->update_iterations,.count=1};
                native_frame_capture(game,NATIVE_FRAME_BODY_BEGIN,saved_tick,&fixture->idle_stage);
            } else if(boundary==NATIVE_FRAME_BODY_BEGIN) {
                /* Before the controlled body gates below: actual input/stage
                 * stores, bracketed separately from notification/drawing. */
                native_frame_capture(game,NATIVE_FRAME_BODY_END,saved_tick,&fixture->idle_stage);
                fixture->setup_output=game->completed_input_return;
            }
        }
        if(boundary==NATIVE_FRAME_BODY_BEGIN && (saved_tick&31)!=8 && (saved_tick&31)!=16) {
            /* New setup parents can enable record work. Bracket their real
             * input/stage stores first, then select a separate source idle
             * body so its result feeds recorder input without drawing. */
            if(fixture->cleanup_case>=108) wr_u8(POST_INPUT_AUX,0);
            /* Select a lost target and queue gates; the actual source cleanup
             * resets the view and computes publication. No output is seeded. */
            wr_u16(TARGET_RECORD,15);wr_u16(CONTROL_RECORDS+15*512,0);
            wr_u8(CONTEXT_SELECT,0);wr_u8(ORIGIN_ENABLE,0);wr_u8(ORIGIN_GATE_MODE,0);
            wr_u8(ORIGIN_DETAIL_MODE,0);wr_u8(UPDATE_TAIL_CONDITION,0);
            wr_u8(KEY_TAKEN,fixture->cleanup_case%3==1);
            wr_u8(KEY_COUNT,fixture->cleanup_case%3==2?10:0);wr_u8(KEY_WRITE,0);
            wr_u8(KEY_TRANSLATED_WRITE,(uint8_t[]){0,7,9,255}[(fixture->cleanup_case/3)%4]);
            snprintf(fixture->path,sizeof fixture->path,"%s.cleanup.%u",fixture->prefix,fixture->cleanup_case);
            fixture->capture=(NativeFrameCapture){.replay=&fixture->clock,.prefix=fixture->path,
                .iteration=game->update_iterations,.count=1};
        }
        if(fixture->capture.prefix) native_frame_capture(game,boundary,saved_tick,&fixture->capture);
        if(boundary==NATIVE_FRAME_BODY_END && fixture->capture.complete)
            printf("{\"cleanup_body\":%u,\"before_tick\":%u,\"after_tick\":%u,\"saved_tick\":%u,\"return_owner\":%u,\"input_byte\":%u,\"active\":%s,\"stage\":\"%06X\",\"stage_return_owner\":%u,\"stage_input_byte\":%u}\n",
                fixture->cleanup_case,fixture->capture.before_tick,fixture->capture.after_tick,
                fixture->capture.saved_tick,game->completed_input_return.owner,game->completed_input_return.value,
                rd_u8(POST_INPUT_AUX)?"true":"false",fixture->setup_stage,
                fixture->setup_output.owner,fixture->setup_output.value);
        return;
    }
    if(fixture->grid_sampling) {
        if(boundary==NATIVE_FRAME_BODY_BEGIN && (saved_tick&31)!=8 && (saved_tick&31)!=16) {
            /* Validation-only grid selection on ordinary Free Flight state;
             * source owners still compute observer, matrices, depth and lines. */
            wr_u8(ORIGIN_ENABLE,1);wr_u8(ORIGIN_GATE_MODE,1);
            wr_u8(ORIGIN_DETAIL_MODE,0);wr_u8(ORIGIN_GATE_A,0);
            snprintf(fixture->path,sizeof fixture->path,"%s.grid.%u",fixture->prefix,fixture->grid_case);
            fixture->capture=(NativeFrameCapture){.replay=&fixture->clock,.prefix=fixture->path,
                .iteration=game->update_iterations,.count=1};
        }
        if(fixture->capture.prefix)
            native_frame_capture(game,boundary,saved_tick,&fixture->capture);
        if(boundary==NATIVE_FRAME_BODY_END && fixture->capture.complete)
            printf("{\"grid_body\":%u,\"before_tick\":%u,\"after_tick\":%u,\"saved_tick\":%u,\"return_owner\":%u,\"input_byte\":%u}\n",
                fixture->grid_case,fixture->capture.before_tick,fixture->capture.after_tick,
                fixture->capture.saved_tick,game->completed_input_return.owner,game->completed_input_return.value);
        return;
    }
    if(fixture->marker_sampling) {
        if(boundary==NATIVE_FRAME_BODY_BEGIN && (saved_tick&31)!=8 && (saved_tick&31)!=16) {
            /* Controlled indicator-line redraw after normal Free Flight startup;
             * later bar gates skip, leaving the actual horizontal line result. */
            wr_u8(BAR_REDRAWS_A,3);wr_u8(BAR_REDRAWS_B,0);
            wr_u8(BAR_REDRAWS_C,0);wr_u8(BAR_REDRAWS_E,0);
            snprintf(fixture->path,sizeof fixture->path,"%s.marker.%u",fixture->prefix,fixture->marker_case);
            fixture->capture=(NativeFrameCapture){.replay=&fixture->clock,.prefix=fixture->path,
                .iteration=game->update_iterations,.count=1};
        }
        if(fixture->capture.prefix)
            native_frame_capture(game,boundary,saved_tick,&fixture->capture);
        if(boundary==NATIVE_FRAME_BODY_END && fixture->capture.complete)
            printf("{\"marker_body\":%u,\"before_tick\":%u,\"after_tick\":%u,\"saved_tick\":%u,\"return_owner\":%u,\"input_byte\":%u}\n",
                fixture->marker_case,fixture->capture.before_tick,fixture->capture.after_tick,
                fixture->capture.saved_tick,game->completed_input_return.owner,game->completed_input_return.value);
        return;
    }
    if(fixture->label_sampling) {
        if(boundary==NATIVE_FRAME_BODY_BEGIN) {
            /* Validation-only map/view selection after ordinary startup.
             * Observer height comes from the actual source start-position owner. */
            int32_t position[3];start_position(position);
            set_observer_position(position[0],position[1],position[2]);
            wr_s32(CONTROL_RECORDS+0x18,position[1]);
            wr_s32(CONTROL_RECORDS+0x10,position[1]>>8);
            wr_u8(ORIGIN_ENABLE,1);wr_u8(ORIGIN_GATE_MODE,0);
            wr_u8(ORIGIN_DETAIL_MODE,(uint8_t)(5+fixture->label_case%3));
            wr_u8(ORIGIN_DETAIL_COUNTER,5); /* Source detail hold while adjustment runs. */
            wr_u8(ORIGIN_GATE_A,(uint8_t)((fixture->label_case>>2)&1));
            wr_u8(0xc45848u,(uint8_t)(fixture->label_case%5));wr_u8(0xc45857u,0);
            snprintf(fixture->path,sizeof fixture->path,"%s.label.%u",fixture->prefix,fixture->label_case);
            fixture->capture=(NativeFrameCapture){.replay=&fixture->clock,.prefix=fixture->path,
                .iteration=game->update_iterations,.count=1};
        }
        if(fixture->capture.prefix)
            native_frame_capture(game,boundary,saved_tick,&fixture->capture);
        if(boundary==NATIVE_FRAME_BODY_END && fixture->capture.complete)
            printf("{\"label_body\":%u,\"before_tick\":%u,\"after_tick\":%u,\"saved_tick\":%u,\"return_owner\":%u,\"input_byte\":%u,\"detail_mode\":%u,\"position_bias\":%d,\"label_row\":%u}\n",
                fixture->label_case,fixture->capture.before_tick,fixture->capture.after_tick,
                fixture->capture.saved_tick,game->completed_input_return.owner,game->completed_input_return.value,
                rd_u8(ORIGIN_DETAIL_MODE),rd_s32(POSITION_BIAS),rd_u16(0xc459aau));
        return;
    }
    if(fixture->debug_sampling) {
        if(boundary==NATIVE_FRAME_BODY_BEGIN) {
            /* Controlled debug selection after ordinary startup; retain
             * normal page, update counter and flight drawing order. */
            wr_u8(UPDATE_TAIL_CONDITION,1);wr_u8(0xc457b3u,(uint8_t)(fixture->debug_case&1));
            snprintf(fixture->path,sizeof fixture->path,"%s.debug.%u",fixture->prefix,fixture->debug_case);
            fixture->capture=(NativeFrameCapture){.replay=&fixture->clock,.prefix=fixture->path,
                .iteration=game->update_iterations,.count=1};
        }
        if(fixture->capture.prefix)
            native_frame_capture(game,boundary,saved_tick,&fixture->capture);
        if(boundary==NATIVE_FRAME_BODY_END && fixture->capture.complete)
            printf("{\"debug_body\":%u,\"before_tick\":%u,\"after_tick\":%u,\"saved_tick\":%u,\"return_owner\":%u,\"input_byte\":%u}\n",
                fixture->debug_case,fixture->capture.before_tick,fixture->capture.after_tick,
                fixture->capture.saved_tick,game->completed_input_return.owner,game->completed_input_return.value);
        return;
    }
    if(fixture->hud_sampling) {
        if(boundary==NATIVE_FRAME_BODY_BEGIN && (saved_tick&31)!=8) {
            snprintf(fixture->path,sizeof fixture->path,"%s.hud.%u",fixture->prefix,fixture->hud_case);
            fixture->capture=(NativeFrameCapture){.replay=&fixture->clock,.prefix=fixture->path,
                .iteration=game->update_iterations,.count=1};
        }
        if(fixture->capture.prefix)
            native_frame_capture(game,boundary,saved_tick,&fixture->capture);
        if(boundary==NATIVE_FRAME_BODY_END && fixture->capture.complete)
            printf("{\"hud_body\":%u,\"before_tick\":%u,\"after_tick\":%u,\"saved_tick\":%u,\"return_owner\":%u,\"input_byte\":%u}\n",
                fixture->hud_case,fixture->capture.before_tick,fixture->capture.after_tick,
                fixture->capture.saved_tick,game->completed_input_return.owner,game->completed_input_return.value);
        return;
    }
    if(fixture->clear_sampling) {
        if(boundary==NATIVE_FRAME_BODY_BEGIN && (saved_tick&31)==8) {
            snprintf(fixture->path,sizeof fixture->path,"%s.clear.%u",fixture->prefix,fixture->clear_case);
            fixture->capture=(NativeFrameCapture){.replay=&fixture->clock,.prefix=fixture->path,
                .iteration=game->update_iterations,.count=1};
        }
        if(fixture->capture.prefix)
            native_frame_capture(game,boundary,saved_tick,&fixture->capture);
        if(boundary==NATIVE_FRAME_BODY_END && fixture->capture.complete)
            printf("{\"clear_body\":%u,\"before_tick\":%u,\"after_tick\":%u,\"saved_tick\":%u,\"return_owner\":%u,\"input_byte\":%u}\n",
                fixture->clear_case,fixture->capture.before_tick,fixture->capture.after_tick,
                fixture->capture.saved_tick,game->completed_input_return.owner,game->completed_input_return.value);
        return;
    }
    if(fixture->message_sampling) {
        native_frame_capture(game,boundary,saved_tick,&fixture->capture);
        if(boundary==NATIVE_FRAME_BODY_END && fixture->capture.complete)
            printf("{\"message_body\":%u,\"before_tick\":%u,\"after_tick\":%u,\"saved_tick\":%u,\"assigned\":%s,\"input_byte\":%u}\n",
                fixture->message_case,fixture->capture.before_tick,fixture->capture.after_tick,
                fixture->capture.saved_tick,(game->completed_input_return.owner==NATIVE_INPUT_RETURN_MESSAGE)?"true":"false",game->completed_input_return.value);
        return;
    }
    if(boundary==NATIVE_FRAME_BODY_BEGIN && (!fixture->capture.begun)) {
        int16_t timer=-1;
        for(unsigned i=0;i<10;++i) {
            const gaddr record=0xc45c72u+128*i;
            if(rd_u16(record+38)&1) timer=rd_s16(record+40);
        }
        const int launch=rd_u8(MISSION_FLAGS_A)!=0;
        const int expiry=timer==1;
        if((launch || expiry) && fixture->captures<4) {
            snprintf(fixture->path,sizeof fixture->path,"%s.%u",fixture->prefix,fixture->captures);
            fixture->capture=(NativeFrameCapture){.replay=&fixture->clock,.prefix=fixture->path,
                .iteration=game->update_iterations,.count=1};
            fixture->launches+=launch;fixture->expiry+=expiry;
        }
    }
    if(!fixture->capture.prefix || fixture->capture.complete) return;
    native_frame_capture(game,boundary,saved_tick,&fixture->capture);
    if(fixture->capture.complete) {
        printf("{\"capture\":%u,\"before_tick\":%u,\"after_tick\":%u,\"saved_tick\":%u}\n",
            fixture->captures,fixture->capture.before_tick,fixture->capture.after_tick,fixture->capture.saved_tick);
        ++fixture->captures;
    }
}
int main(int argc,char **argv) {
    if(argc!=4) return 1;
    setvbuf(stdout,NULL,_IONBF,0);
    NativeFrontend *game=calloc(1,sizeof *game);char error[256];int result=1;
    CountermeasureFixture fixture={.prefix=argv[3]};
    if(!game) return 1;
    if(!native_frontend_open(game,argv[1],argv[2],error,sizeof error)) {fprintf(stderr,"%s\n",error);goto done;}
    game->observe_frame=observe;game->frame_context=&fixture;
    const unsigned times[]={1800,3000,4100,5000,5400,6200,6900};
    const int keys[]={32,50,13,50,49,'F','C'};
    while(game->ticks<7800) {
        for(unsigned i=0;i<sizeof times/sizeof times[0];++i) {
            if(game->ticks==times[i]) native_frontend_event(game,keys[i],1);
            if(game->ticks==times[i]+2) native_frontend_event(game,keys[i],0);
        }
        native_frontend_tick(game);
    }
    if(fixture.captures!=4 || fixture.launches!=2 || fixture.expiry!=2 ||
       rd_u8(MISSION_LEVEL_A)!=15 || rd_u8(MISSION_LEVEL_B)!=15 || rd_u32(STAGE_CALLBACK)!=0xc10dae) {
        fprintf(stderr,"Countermeasure integration failed: captures=%u launch=%u expiry=%u stocks=%u/%u\n",
            fixture.captures,fixture.launches,fixture.expiry,rd_u8(MISSION_LEVEL_A),rd_u8(MISSION_LEVEL_B));goto done;
    }
    /* Existing source message 6 is a controlled input after ordinary startup.
     * Capture its real final renderer and consume the returned byte in the
     * next first depleted pending command; no captured register is seeded. */
    reset_message_sequence();queue_top_level_menu_messages(NULL);wr_u8(0xc457e0u,0);
    fixture.message_sampling=1;
    for(unsigned i=0;i<15;++i) {
        if(i==13) wr_u32(MESSAGE_QUEUE,0); /* No selector preserves body carry. */
        if(i==14) wr_u8(0xc45871u,1); /* Disabled final renderer also preserves. */
        fixture.message_case=i;
        snprintf(fixture.path,sizeof fixture.path,"%s.message.%u",fixture.prefix,i);
        fixture.capture=(NativeFrameCapture){.replay=&fixture.clock,.prefix=fixture.path,
            .iteration=game->update_iterations+1,.count=1};
        unsigned limit=game->ticks+100;
        while(!fixture.capture.complete && game->ticks<limit) native_frontend_tick(game);
        if(!fixture.capture.complete || (game->completed_input_return.owner==NATIVE_INPUT_RETURN_MESSAGE)!=(i<13) ||
           (i<12 && !pending_input(game,&fixture,24+i))) {
            fprintf(stderr,"Message carry integration failed at case %u\n",i);goto done;
        }
    }
    wr_u8(0xc45871u,0);
    fixture.message_sampling=0;
    fixture.clear_sampling=1;
    for(unsigned i=0;i<12;++i) {
        fixture.clear_case=i;fixture.capture=(NativeFrameCapture){0};
        /* Let the original counter reach its periodic clear; do not seed
         * UPDATE_TICK or import any original capture into native gameplay. */
        wr_u8(RECORDER_MODE,0);
        unsigned limit=game->ticks+200;
        while(!fixture.capture.complete && game->ticks<limit) native_frontend_tick(game);
        if(!fixture.capture.complete || game->completed_input_return.owner!=NATIVE_INPUT_RETURN_PAGE_CLEAR ||
           game->completed_input_return.value || !pending_input(game,&fixture,36+i)) {
            fprintf(stderr,"Page clear carry integration failed at case %u\n",i);goto done;
        }
    }
    fixture.clear_sampling=0;
    fixture.hud_sampling=1;
    for(unsigned i=0;i<12;++i) {
        fixture.hud_case=i;fixture.capture=(NativeFrameCapture){0};
        wr_u8(RECORDER_MODE,0);
        unsigned limit=game->ticks+100;
        while(!fixture.capture.complete && game->ticks<limit) native_frontend_tick(game);
        if(!fixture.capture.complete ||
           (game->completed_input_return.owner!=NATIVE_INPUT_RETURN_HUD_BAR &&
            game->completed_input_return.owner!=NATIVE_INPUT_RETURN_HUD_TEXT &&
            game->completed_input_return.owner!=NATIVE_INPUT_RETURN_REDRAW) ||
           !pending_input(game,&fixture,48+i)) {
            fprintf(stderr,"HUD carry integration failed at case %u\n",i);goto done;
        }
    }
    fixture.hud_sampling=0;
    const uint8_t previous_debug=rd_u8(UPDATE_TAIL_CONDITION),previous_fields=rd_u8(0xc457b3u);
    fixture.debug_sampling=1;
    for(unsigned i=0;i<12;++i) {
        fixture.debug_case=i;fixture.capture=(NativeFrameCapture){0};
        wr_u8(RECORDER_MODE,0);
        unsigned limit=game->ticks+100;
        while(!fixture.capture.complete && game->ticks<limit) native_frontend_tick(game);
        if(!fixture.capture.complete || game->completed_input_return.owner!=NATIVE_INPUT_RETURN_DEBUG_TEXT ||
           !pending_input(game,&fixture,60+i)) {
            fprintf(stderr,"Debug text carry integration failed at case %u\n",i);goto done;
        }
    }
    fixture.debug_sampling=0;
    wr_u8(UPDATE_TAIL_CONDITION,previous_debug);wr_u8(0xc457b3u,previous_fields);
    const uint8_t previous_map=rd_u8(ORIGIN_ENABLE),previous_grid=rd_u8(ORIGIN_GATE_MODE),
        previous_detail=rd_u8(ORIGIN_DETAIL_MODE),previous_detail_count=rd_u8(ORIGIN_DETAIL_COUNTER),previous_gate=rd_u8(ORIGIN_GATE_A),
        previous_row=rd_u8(0xc45848u),previous_blink=rd_u8(0xc45857u);
    uint32_t previous_observer[6];
    const uint32_t previous_player_y=rd_u32(CONTROL_RECORDS+0x18),previous_player_height=rd_u32(CONTROL_RECORDS+0x10);
    for(unsigned i=0;i<6;++i) previous_observer[i]=rd_u32(OBSERVER+4*i);
    fixture.label_sampling=1;
    for(unsigned i=0;i<12;++i) {
        fixture.label_case=i;fixture.capture=(NativeFrameCapture){0};
        wr_u8(RECORDER_MODE,0);
        unsigned limit=game->ticks+100;
        while(!fixture.capture.complete && game->ticks<limit) native_frontend_tick(game);
        if(!fixture.capture.complete ||
           (game->completed_input_return.owner!=NATIVE_INPUT_RETURN_SCENE_LABEL &&
            game->completed_input_return.owner!=NATIVE_INPUT_RETURN_HUD_TEXT) ||
           !pending_input(game,&fixture,72+i)) {
            fprintf(stderr,"Scene-label carry integration failed at case %u\n",i);goto done;
        }
    }
    fixture.label_sampling=0;
    wr_u8(ORIGIN_ENABLE,previous_map);wr_u8(ORIGIN_GATE_MODE,previous_grid);
    wr_u8(ORIGIN_DETAIL_MODE,previous_detail);wr_u8(ORIGIN_GATE_A,previous_gate);
    wr_u8(ORIGIN_DETAIL_COUNTER,previous_detail_count);
    wr_u8(0xc45848u,previous_row);wr_u8(0xc45857u,previous_blink);
    for(unsigned i=0;i<6;++i) wr_u32(OBSERVER+4*i,previous_observer[i]);
    wr_u32(CONTROL_RECORDS+0x18,previous_player_y);wr_u32(CONTROL_RECORDS+0x10,previous_player_height);
    for(unsigned i=0;i<24;++i) if(!pending_input(game,&fixture,i)) goto done;
    for(unsigned i=0;i<2;++i) if(!fd_input(game,&fixture,i)) goto done;
    for(unsigned i=0;i<4;++i) if(!collision_parent(game,&fixture,i)) goto done;
    /* Marker frames get their own ordinary disk/key startup, independent of
     * the preceding controlled map views and collision parent state. */
    native_frontend_close(game);memset(game,0,sizeof *game);
    if(!native_frontend_open(game,argv[1],argv[2],error,sizeof error)) {fprintf(stderr,"%s\n",error);goto done;}
    while(game->ticks<7800) {
        for(unsigned i=0;i<5;++i) {
            if(game->ticks==times[i]) native_frontend_event(game,keys[i],1);
            if(game->ticks==times[i]+2) native_frontend_event(game,keys[i],0);
        }
        native_frontend_tick(game);
    }
    fixture.capture=(NativeFrameCapture){0};fixture.marker_sampling=1;
    game->observe_frame=observe;game->frame_context=&fixture;
    for(unsigned i=0;i<12;++i) {
        fixture.marker_case=i;fixture.capture=(NativeFrameCapture){0};
        wr_u8(RECORDER_MODE,0);
        unsigned limit=game->ticks+100;
        while(!fixture.capture.complete && game->ticks<limit) native_frontend_tick(game);
        if(!fixture.capture.complete || game->completed_input_return.owner!=NATIVE_INPUT_RETURN_HUD_LINE ||
           !pending_input(game,&fixture,84+i)) {
            fprintf(stderr,"Marker-line carry integration failed at case %u: span=%d row=%d bars=%u/%u/%u/%u map=%u hud=%u\n",i,
                rd_s16(SPAN_ORIGIN_Y),rd_s16(REDRAW_STATE_WORD),rd_u8(BAR_REDRAWS_A),rd_u8(BAR_REDRAWS_B),
                rd_u8(BAR_REDRAWS_C),rd_u8(BAR_REDRAWS_E),rd_u8(ORIGIN_ENABLE),rd_u8(UPDATE_HUD_MODE));goto done;
        }
    }
    fixture.marker_sampling=0;
    fixture.grid_sampling=1;
    for(unsigned i=0;i<12;++i) {
        fixture.grid_case=i;fixture.capture=(NativeFrameCapture){0};
        wr_u8(RECORDER_MODE,0);
        unsigned limit=game->ticks+100;
        while(!fixture.capture.complete && game->ticks<limit) native_frontend_tick(game);
        if(!fixture.capture.complete || game->completed_input_return.owner!=NATIVE_INPUT_RETURN_GRID_MARKER ||
           !pending_input(game,&fixture,96+i)) {
            fprintf(stderr,"Grid-marker carry integration failed at case %u: owner=%u grid=%u map=%u detail=%u\n",i,
                game->completed_input_return.owner,rd_u8(ORIGIN_GATE_MODE),rd_u8(ORIGIN_ENABLE),rd_u8(ORIGIN_DETAIL_MODE));goto done;
        }
    }
    fixture.grid_sampling=0;
    fixture.cleanup_sampling=1;
    for(unsigned i=0;i<96;++i) {
        fixture.cleanup_case=i;fixture.capture=(NativeFrameCapture){0};
        wr_u8(RECORDER_MODE,0);
        unsigned limit=game->ticks+100;
        while(!fixture.capture.complete && game->ticks<limit) native_frontend_tick(game);
        /* A context request can stop flight updates. The viewport stage and
         * idle body preserve the real preceding input output. */
        if(!fixture.capture.complete || (i<84?game->completed_input_return.owner!=NATIVE_INPUT_RETURN_VIEW_KEY:
                game->completed_input_return.owner==NATIVE_INPUT_RETURN_UNKNOWN) ||
           (rd_u8(POST_INPUT_AUX) && rd_u16(TARGET_RECORD)) || (i>=12 && !interposed_input(game,&fixture,i-12)) ||
           !pending_input(game,&fixture,108+i)) {
            fprintf(stderr,"Selection-cleanup carry integration failed at case %u: owner=%u target=%u\n",
                i,game->completed_input_return.owner,rd_u16(TARGET_RECORD));goto done;
        }
    }
    /* Consume idle output directly, without an intervening command assigning
     * a new result. The preceding recorder parent supplies the actual output;
     * only source viewport-stage gates are controlled in this fixture. */
    for(unsigned i=96;i<108;++i) {
        fixture.cleanup_case=i;fixture.capture=(NativeFrameCapture){0};
        wr_u8(RECORDER_MODE,0);wr_u32(STAGE_CALLBACK,ROUTINE_VIEWPORT_CHANGE);
        wr_u8(VIEWPORT_MODE,0);wr_u8(VIEWPORT_TARGET,15);
        unsigned limit=game->ticks+100;
        while(!fixture.capture.complete && game->ticks<limit) native_frontend_tick(game);
        if(!fixture.capture.complete || rd_u8(POST_INPUT_AUX) ||
           game->completed_input_return.owner==NATIVE_INPUT_RETURN_UNKNOWN ||
           !pending_input(game,&fixture,108+i)) {
            fprintf(stderr,"Idle input preservation failed at case %u: owner=%u active=%u\n",
                i,game->completed_input_return.owner,rd_u8(POST_INPUT_AUX));goto done;
        }
    }
    /* Each callback inherits a real preceding recorder output. These source
     * conditions exercise wait, publication, reset and clock branches only
     * in the validation entry; no output value/owner is supplied. */
    for(unsigned i=108;i<156;++i) {
        const unsigned family=(i-108)/12,variant=(i-108)%12;
        fixture.cleanup_case=i;fixture.capture=(NativeFrameCapture){0};
        wr_u8(RECORDER_MODE,0);wr_u8(POST_INPUT_AUX,0);
        wr_u8(KEY_TAKEN,0);wr_u8(SEQUENCE_PHASE,0);
        wr_u32(STAGE_CALLBACK,(gaddr[]){0xc10ab2,0xc10ae6,0xc10c08,0xc11a50}[family]);
        if(family==0) wr_u16(POST_INPUT_COUNTDOWN,(variant&1)?1:0);
        else if(family==1) wr_u8(COMMAND_ENABLE_GATE,(variant&1)?1:0);
        else if(family==2) {
            wr_u8(CONTEXT_SELECT,variant%3!=0);
            wr_u8(POST_INPUT_EVENT,variant%3==2?0xff:0);
        } else {
            wr_u8(POST_INPUT_EVENT,variant%3==0?0xff:0);
            wr_u8(CONTEXT_STATE,variant%3==1?6:0);
            wr_u32(MENU_TIME_PENDING,variant&1?0xffffffffu:0);
            wr_u32(MENU_TIME_OPTIONAL,variant&1?1:0);
        }
        unsigned limit=game->ticks+100;
        while(!fixture.capture.complete && game->ticks<limit) native_frontend_tick(game);
        if(!fixture.capture.complete || !fixture.idle_stage.complete || rd_u8(POST_INPUT_AUX) ||
           game->completed_input_return.owner==NATIVE_INPUT_RETURN_UNKNOWN ||
           !pending_input(game,&fixture,108+i)) {
            fprintf(stderr,"Setup stage input preservation failed at case %u: owner=%u stage=%06X\n",
                i,game->completed_input_return.owner,fixture.setup_stage);goto done;
        }
    }
    /* Childless postflight callbacks retain the preceding input result.
     * Negative player phase lets C11958's own sequence tests run without
     * C0F5F8's earlier positive-player phase selector replacing the callback. */
    static const gaddr postflight_stages[]={0xc11872,0xc118e6,0xc118fc,0xc11934,
        0xc11958,0xc119d4,0xc1104c,0xc0f946,0xc0f974};
    for(unsigned i=156;i<264;++i) {
        const unsigned family=(i-156)/12,variant=(i-156)%12;
        fixture.cleanup_case=i;fixture.capture=(NativeFrameCapture){0};
        wr_u8(RECORDER_MODE,0);wr_u8(POST_INPUT_AUX,0);wr_u8(KEY_TAKEN,0);
        wr_u8(PLAYER_PHASE,0xf0);wr_u8(SEQUENCE_PHASE,0);
        wr_u32(STAGE_CALLBACK,postflight_stages[family]);
        wr_u16(POST_INPUT_COUNTDOWN,(variant&1)?1:0);
        if(family==1 || family==2) wr_u8(MESSAGE_STATE_C,(variant&1)?0:0xff);
        else if(family==4) {
            wr_u8(MESSAGE_STATE_C,variant%4==0?0xff:0);
            wr_u8(SEQUENCE_PHASE,(uint8_t[]){0,0xff,1,0}[variant%4]);
            wr_u8(SEQUENCE_FLAG,1);
        } else if(family==5) wr_u8(CONTEXT_REQUEST,(variant/2)&1);
        else if(family==7) {
            wr_u16(POST_INPUT_COUNTDOWN,variant%3==2?1:0);
            wr_u8(VIEWPORT_MODE,0);wr_u8(VIEWPORT_TARGET,variant%3==0?0:15);
        }
        unsigned limit=game->ticks+100;
        while(!fixture.capture.complete && game->ticks<limit) native_frontend_tick(game);
        if(!fixture.capture.complete || !fixture.idle_stage.complete || rd_u8(POST_INPUT_AUX) ||
           fixture.setup_output.owner==NATIVE_INPUT_RETURN_UNKNOWN ||
           game->completed_input_return.owner==NATIVE_INPUT_RETURN_UNKNOWN ||
           !pending_input(game,&fixture,108+i)) {
            fprintf(stderr,"Postflight stage input preservation failed at case %u: owner=%u stage=%06X\n",
                i,fixture.setup_output.owner,fixture.setup_stage);goto done;
        }
    }
    fixture.cleanup_sampling=0;
    result=0;
done:
    if(game) {native_frontend_close(game);free(game);}return result;
}
