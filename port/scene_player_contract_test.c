#include "scene_player_setup.h"
#include "post_input_tick.h"
#include <assert.h>
#include <string.h>

static void check_bytes(const uint8_t *bytes,size_t count,uint8_t value) {
    size_t i; for(i=0;i<count;++i) assert(bytes[i]==value);
}
int main(void) {
    static FA18NativeSceneRecords bank;
    FA18CommandInput commands={0}; FA18FlightCommandState flight={0};
    FA18ViewCommandState view={0}; FA18ContextCommandState context={0};
    FA18CommandQueue queue={0}; FA18CommandEffects effects={0};
    FA18PostInputTickState tick={0};
    uint8_t source[16*512],work[16*32],bytes[512],flags[6],mission_b=9,mission_c=8,selection=7;
    uint16_t limit=0,selected=0,shown=0x5555,marker=0;
    uint32_t warnings=0xffffffff,events=0xffffffff,position[3],event;
    uint8_t neighbors[FA18_COMMAND_QUEUE_NEIGHBORS],keys[128]={0};
    FA18NativeScenePlayerSetup setup={.records=&bank,.flight=&flight,.effects=&effects,
        .mission_flags_b=&mission_b,.mission_flags_c=&mission_c,.phase=&tick.phase,
        .selection_active=&selection,.limit=&limit,.selected_record=&selected,
        .message_shown=&shown,.message_marker=&marker,.warning_causes=&warnings,.event_bits=&events};
    unsigned i,row,column;
    memset(source,0x5a,sizeof source); memset(work,0xa5,sizeof work);
    assert(!fa18_import_native_scene_records(&bank,&commands,source,sizeof source-1,work,sizeof work));
    assert(!bank.input);
    assert(fa18_import_native_scene_records(&bank,&commands,source,sizeof source,work,sizeof work));
    for(i=0;i<16;++i) {
        assert(bank.geometry[i].command_record==bank.aircraft+i);
        assert(fa18_read_native_scene_record(bank.records+i,0,bytes,sizeof bytes));
        assert(!memcmp(bytes,source+i*512,512));
    }
    assert(bank.records[0].level==&commands.indexed.control_record_level && commands.indexed.control_record_level==0x5a);
    /* The packed view reflects actual command and geometry writes live. */
    bank.aircraft[0].flags=0x11c8; bank.geometry[0].position[0]=0xdeadbeef;
    assert(fa18_read_native_scene_record(bank.records,0,bytes,32));
    assert(bytes[0]==0x11 && bytes[1]==0xc8 && bytes[20]==0xde && bytes[23]==0xef);
    {
        const uint8_t packet[]={0x01,0x23,0x45,0x67};
        assert(fa18_write_native_scene_record(bank.records+2,0x14,packet,sizeof packet));
        assert(bank.geometry[2].position[0]==0x01234567);
        assert(fa18_read_native_scene_record(bank.records+2,0x14,bytes,4) && !memcmp(bytes,packet,4));
    }
    flight.commands=&commands; flight.player=bank.aircraft; flight.viewed=bank.aircraft+5;
    view.flight=&flight; context.view=&view; context.records=bank.geometry; context.record_count=16;
    effects.context=&context; effects.message_code=0x7777;
    memset(neighbors,0x5a,sizeof neighbors);
    assert(fa18_initialize_command_queue(&queue,&context,neighbors,sizeof neighbors,keys,sizeof keys));
    for(i=0;i<6;++i) { flags[i]=0xff; setup.player_flags[i]=flags+i; }
    assert(fa18_bind_native_scene_player(&setup,&queue));
    assert(tick.phase==0x5a && selection==0x5a);
    assert(fa18_prepare_native_scene_player(&setup));
    assert(bank.records[0].byte_21==0x5a && bank.aircraft[0].weapon_radar==0x0d);
    assert(bank.aircraft[0].flags==0x11c8 && bank.records[0].word_7e==0x1400);
    assert(bank.records[0].long_72==0x61a800 && bank.records[0].byte_5f==0x24 && bank.records[0].word_60==500);
    assert(flight.weapon_mode_redraws==3 && flight.weapon_redraws==3 && flight.chaff_count==16 && flight.flare_count==16);
    assert(!flight.mission_flags && !mission_b && !mission_c && !flight.ecm_enabled);
    check_bytes(flags,6,0); assert(limit==0x7fff && commands.indexed.player_ready==1);
    assert(bank.records[0].byte_71==0xff && selected==0xffff && !selection && tick.phase==4);
    for(i=1;i<4;++i) {
        assert(fa18_read_native_scene_record(bank.records+i,0,bytes,sizeof bytes));
        check_bytes(bytes,164,0); check_bytes(bytes+164,348,0x5a);
        assert(!bank.aircraft[i].flags && !bank.geometry[i].angle);
        for(row=0;row<3;++row) {
            assert(!bank.geometry[i].position[row]);
            for(column=0;column<3;++column) assert(!bank.geometry[i].inverse[row][column]);
        }
    }
    assert(bank.aircraft[4].flags==0x5a5a && flight.viewed==bank.aircraft+5);
    {
        FA18ContextCommandChildInput input={.record=bank.geometry+1,.local={11,0,104}};
        FA18ContextCommandChildResult result;
        assert(fa18_apply_context_control_child(&context,CONTEXT_COMMAND_LOCAL_TO_WORLD,&input,&result));
        assert(!result.position[0] && !result.position[1] && !result.position[2]);
    }
    bank.aircraft[0].flags|=0x8000;
    assert(fa18_reset_native_scene_player(&setup));
    assert(bank.aircraft[0].flags==0x11c8 && !bank.aircraft[0].stick && commands.indexed.control_record_level==9);
    assert(!bank.records[0].long_3e && !bank.records[0].long_56 && !bank.records[0].word_5a);
    assert(!warnings && !events && !effects.message_code && !shown && marker==0xffff);
    /* A real signed-index publication changes the phase consumed by setup. */
    queue.taken=0; queue.count=0; queue.write_index=(uint8_t)(0x37-128);
    assert(fa18_publish_native_command(&queue,0,&event) && !tick.phase);
    assert(fa18_prepare_native_scene_player(&setup) && !tick.phase);
    assert(fa18_native_scene_start_position(position));
    {
        FA18ContextCommandChildInput input={0}; FA18ContextCommandChildResult result;
        for(i=0;i<3;++i) input.position[i]=(int32_t)position[i];
        assert(fa18_apply_context_control_child(&context,CONTEXT_COMMAND_SET_OBSERVER,&input,&result));
        assert(context.origin_first==0x10c00000 && view.origin_middle==0x05000000 && context.origin_third==0x11400000);
        assert(!context.negated[0] && context.negated[1]==0xfb000000 && !context.negated[2]);
    }
    assert(fa18_clear_native_bootstrap_records(&bank));
    for(i=0;i<16;++i) {
        assert(fa18_read_native_scene_record(bank.records+i,0,bytes,sizeof bytes));
        check_bytes(bytes,164,0); check_bytes(bytes+164,348,0x5a);
        check_bytes(bank.work[i],32,0);
    }
    /* A missing later owner retains earlier real player/record stores. */
    setup.limit=NULL; bank.aircraft[0].flags=0xffff;
    assert(!fa18_prepare_native_scene_player(&setup));
    assert(bank.aircraft[0].flags==0x11c8);
    assert(!fa18_read_native_scene_record(bank.records,510,bytes,3));
    assert(!fa18_write_native_scene_record(bank.records,510,bytes,3));
    assert(!fa18_native_scene_start_position(NULL));
    return 0;
}
