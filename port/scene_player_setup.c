#include "scene_player_setup.h"

static int shared_records(const FA18NativeScenePlayerSetup *s) {
    return s && s->records && s->flight && s->flight->commands &&
        s->records->input==s->flight->commands &&
        s->flight->player==s->records->aircraft &&
        s->records->records[0].aircraft==s->flight->player;
}
int fa18_bind_native_scene_player(FA18NativeScenePlayerSetup *s,FA18CommandQueue *q) {
    if(!shared_records(s) || !q || q->commands!=s->flight->commands ||
       !s->phase || !s->selection_active) return 0;
    /* Validate both before importing either owner. */
    if(!port_field_byte_valid(q->slots+0x37) || !port_field_byte_valid(q->slots+0x107)) return 0;
    return fa18_bind_command_queue_byte(q,0x37,s->phase) &&
           fa18_bind_command_queue_byte(q,0x107,s->selection_active);
}
int fa18_reset_native_mission_objects(FA18NativeScenePlayerSetup *s) {
    FA18NativeSceneRecord *r; unsigned slot;
    if(!shared_records(s)) return 0;
    r=s->records->records;
    r->long_72=0x0061a800u; r->byte_5f=0x24; r->word_60=0x1f4;
    s->flight->weapon_mode_redraws=3; s->flight->weapon_redraws=3;
    s->flight->chaff_count=0x10; s->flight->flare_count=0x10;
    s->flight->mission_flags=0;
    if(!s->mission_flags_c) return 0;
    *s->mission_flags_c=0;
    if(!s->mission_flags_b) return 0;
    *s->mission_flags_b=0;
    s->flight->spawn_gate=0;
    for(slot=1;slot<4;++slot) if(!fa18_clear_native_scene_record(s->records->records+slot)) return 0;
    return 1;
}
int fa18_prepare_native_scene_player(FA18NativeScenePlayerSetup *s) {
    FA18NativeSceneRecord *r; unsigned i;
    if(!shared_records(s)) return 0;
    r=s->records->records;
    r->byte_21&=0xfe; r->aircraft->weapon_radar=0x0d;
    if(!fa18_reset_native_mission_objects(s)) return 0;
    r->aircraft->flags=0x148; r->aircraft->flags|=0x1080;
    r->word_7e=0x1400;
    for(i=0;i<6;++i) { if(!s->player_flags[i]) return 0; *s->player_flags[i]=0; }
    s->flight->ecm_enabled=0;
    if(!s->limit) return 0;
    *s->limit=0x7fff;
    s->flight->commands->indexed.player_ready=1;
    r->byte_71=0xff;
    if(!s->selected_record) return 0;
    *s->selected_record=0xffff;
    if(!s->selection_active) return 0;
    *s->selection_active=0;
    if(!s->phase) return 0;
    if(*s->phase) *s->phase=4;
    return 1;
}
int fa18_reset_native_scene_player(FA18NativeScenePlayerSetup *s) {
    FA18NativeSceneRecord *r;
    if(!shared_records(s)) return 0;
    r=s->records->records;
    r->word_6c=0; r->word_6e=0; r->word_78=0;
    r->long_3e=0; r->long_42=0; r->long_46=0; r->long_56=0;
    r->word_5a=0; r->long_50=0; r->word_54=0;
    r->aircraft->stick=0;
    s->flight->commands->indexed.control_record_level=9;
    r->aircraft->flags&=0x7fff;
    if(!s->warning_causes) return 0;
    *s->warning_causes=0;
    if(!s->event_bits) return 0;
    *s->event_bits=0;
    if(!s->effects) return 0;
    s->effects->message_code=0;
    if(!s->message_shown) return 0;
    *s->message_shown=0;
    if(!s->message_marker) return 0;
    *s->message_marker=0xffff;
    return 1;
}
int fa18_native_scene_start_position(uint32_t position[3]) {
    if(!position) return 0;
    position[0]=0x10c00000; position[1]=0x05000000; position[2]=0x11400000;
    return 1;
}
