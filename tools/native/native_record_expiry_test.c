/* Exercise the playable native scene owner after disk-backed demo startup.
 * Controlled destruction/selection inputs belong only to this validation main.
 * The linked runtime objects are exactly those used by fa18_native. */
#include "native/frontend.h"
#include "native/scene.h"
#include "../../port/native/replay.h"
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>

static int require(int condition,const char *message) {
    if(!condition) fprintf(stderr,"Native record expiry: %s\n",message);
    return condition;
}
int main(int argc,char **argv) {
    if(argc!=4) return 1;
    NativeFrontend *game=calloc(1,sizeof *game);
    NativeReplay replay={0}; char error[256];int result=1;
    if(!game) return 1;
    if(!native_replay_load(&replay,argv[2],error,sizeof error) ||
       !native_frontend_open(game,argv[1],argv[3],error,sizeof error)) {
        fprintf(stderr,"%s\n",error);goto done;
    }
    game->begin_update=native_replay_update;game->update_context=&replay;
    while(game->ticks<12000 && replay.iteration<2364) {
        if(game->ticks==1800) native_frontend_event(game,32,1);
        if(game->ticks==1802) native_frontend_event(game,32,0);
        native_frontend_tick(game);
    }
    if(!require(replay.iteration==2364 && rd_u16(UPDATE_TICK)==222 &&
                (rd_u16(CONTROL_RECORDS)&0x40u),"demo did not reach its active aircraft")) goto done;
    /* Source C1CCBC's actual selected-render list supplies the visible record.
     * C22AC0 uses its chosen-record offset, published by that traversal. The
     * player's cockpit view need not submit the player's own exterior model. */
    const int16_t index=rd_s16(0xc4e98au);
    if(!require(index>0 && index<16,"demo has no selected render record")) goto done;
    const uint16_t offset=(uint16_t)(index*512);
    const gaddr record=CONTROL_RECORDS+offset;
    if(!require(rd_u16(record)&0x40u,"render record is not active")) goto done;
    wr_u16(record,rd_u16(record)|0x200u);
    wr_s16(record+0x4c,-1);
    wr_u16(SELECTED_RECORD,offset);wr_u32(WARNING_CAUSES,0xffffffffu);
    const unsigned calls=game->model_calls;
    if(!require(native_scene_draw(game) && game->model_calls>calls,"scene did not render") ||
       !require(rd_u16(record+0x4c)==15 &&
                (rd_u16(record)&0x600u)==0x400u,"destruction did not start expiry") ||
       !require(rd_u16(SELECTED_RECORD)==0xffffu && rd_u32(WARNING_CAUSES)==0xffffbdffu &&
                rd_u16(MESSAGE_CODE)==0x4016u,"target cleanup/message did not execute")) goto done;
    wr_u16(record+0x4c,12);wr_u16(MESSAGE_CODE,0);
    if(!require(native_scene_draw(game),"second scene did not render") ||
       !require(rd_u16(record+0x4c)==12 && rd_u16(MESSAGE_CODE)==0,
                "repeated drawing restarted expiry or reposted the message")) goto done;
    puts("Native disk-backed scene starts record expiry, clears the selected target and preserves repeated-render timing");
    result=0;
done:
    native_replay_close(&replay);native_frontend_close(game);free(game);return result;
}
