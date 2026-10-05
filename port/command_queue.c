#include "command_queue.h"
#include <string.h>

enum { RAW_FIRST = 128, TRANSLATED_FIRST = 138 };

static void slot_write(FA18CommandQueueByte *slot, uint8_t value) {
    if (slot->byte) *slot->byte = value;
    else {
        uint16_t bits = (uint16_t)*slot->word;
        bits = (uint16_t)((bits & ~(0xffu << slot->shift)) | ((unsigned)value << slot->shift));
        *slot->word = (int16_t)(bits < 0x8000u ? (int32_t)bits : (int32_t)bits - 0x10000);
    }
}

static void bind_byte(FA18CommandQueue *q, unsigned offset, uint8_t *value) {
    q->slots[offset].byte = value;
    q->slots[offset].word = NULL;
    q->slots[offset].shift = 0;
}

static void bind_word(FA18CommandQueue *q, unsigned offset, int16_t *value) {
    unsigned i;
    *value = 0; /* Both bytes are imported below; avoid reading an unset word. */
    for (i = 0; i < 2; ++i) {
        q->slots[offset+i].byte = NULL;
        q->slots[offset+i].word = value;
        q->slots[offset+i].shift = i ? 0 : 8;
    }
}

int fa18_bind_command_queue_byte(FA18CommandQueue *q,unsigned offset,uint8_t *owner) {
    FA18CommandQueueByte *slot;
    uint8_t value;
    if(!q || !q->commands || !owner || offset>=FA18_COMMAND_QUEUE_NEIGHBORS) return 0;
    slot=&q->slots[offset];
    if(!slot->byte && !slot->word) return 0;
    value=slot->byte?*slot->byte:(uint8_t)((uint16_t)*slot->word>>slot->shift);
    *owner=value;
    bind_byte(q,offset,owner);
    return 1;
}

int fa18_initialize_command_queue(FA18CommandQueue *q,
                                  FA18ContextCommandState *context,
                                  const uint8_t *neighbors, size_t neighbor_count,
                                  const uint8_t *key_table, size_t key_count) {
    FA18CommandInput *c;
    FA18FlightCommandState *f;
    FA18ViewCommandState *v;
    unsigned i;
    if (!q || !context || !context->view || !context->view->flight ||
        !context->view->flight->commands || !neighbors || !key_table ||
        neighbor_count != FA18_COMMAND_QUEUE_NEIGHBORS || key_count != FA18_COMMAND_KEY_TABLE_SIZE)
        return 0;
    v = context->view; f = v->flight; c = f->commands;
    q->commands = c;
    memcpy(q->neighbors, neighbors, sizeof q->neighbors);
    memcpy(q->key_table, key_table, sizeof q->key_table);
    for (i = 0; i < FA18_COMMAND_QUEUE_NEIGHBORS; ++i)
        bind_byte(q, i, &q->neighbors[i]);

    /* Offsets within the original bounded neighboring-global window.
     * These references preserve aliases with the actual native owners. */
    bind_word(q, 0x17, &c->indexed.throttle);
    bind_word(q, 0x1b, &c->indexed.throttle_companion);
    bind_byte(q, 0x24, &c->origin_mode);
    bind_byte(q, 0x26, &c->indexed.mode_gate);
    bind_byte(q, 0x2f, &context->recorder_on);
    bind_byte(q, 0x30, &c->indexed.enable_gate);
    bind_byte(q, 0x31, &c->indexed.mode_request);
    bind_byte(q, 0x39, &f->weapon_pause);
    bind_byte(q, 0x40, &f->hud_mode);
    bind_byte(q, 0x42, &q->taken);
    bind_byte(q, 0x45, &context->track_started);
    bind_byte(q, 0x46, &v->mode);
    bind_byte(q, 0x47, &v->mode_auxiliary);
    bind_byte(q, 0x48, &v->mode_companion);
    bind_byte(q, 0x4a, &f->eject_flag);
    bind_byte(q, 0x4c, &f->pause);
    bind_byte(q, 0x4d, &c->indexed.origin_gate_a);
    bind_byte(q, 0x53, &f->context_started);
    bind_byte(q, 0x54, &c->indexed.origin_gate_b);
    bind_byte(q, 0x58, &f->next_target);
    bind_byte(q, 0x59, &f->space_command_latch);
    bind_byte(q, 0x5f, &v->redraw_keep_state);
    bind_byte(q, 0x72, &c->return_state);
    bind_byte(q, 0x74, &c->event_counter);
    bind_byte(q, 0x77, &c->indexed.player_ready);
    bind_byte(q, 0x7c, &v->zoom_flags);
    bind_byte(q, 0x7f, &c->message_state);
    for (i = 0; i < sizeof q->raw; ++i) bind_byte(q, RAW_FIRST+i, &q->raw[i]);
    for (i = 0; i < sizeof q->translated; ++i) bind_byte(q, TRANSLATED_FIRST+i, &q->translated[i]);
    bind_byte(q, 0x95, &q->translated_index);
    bind_byte(q, 0x96, &q->write_index);
    bind_byte(q, 0x98, &q->count);
    bind_byte(q, 0xc9, &f->sequence_phase);
    bind_byte(q, 0xcd, &f->stick_y);
    bind_byte(q, 0xce, &f->trim_input);
    bind_byte(q, 0xcf, &f->stick_x);
    bind_byte(q, 0xd2, &context->view_request);
    bind_byte(q, 0xd5, &v->redraw_first);
    bind_byte(q, 0xd6, &v->gauge_refresh);
    bind_byte(q, 0xd8, &v->grid_z_redraws);
    bind_byte(q, 0xd9, &v->grid_x_redraws);
    bind_byte(q, 0xda, &f->scale_redraws);
    bind_byte(q, 0xdb, &f->info_redraws);
    bind_byte(q, 0xdc, &v->display_update);
    bind_byte(q, 0xdd, &v->redraw_bar_a);
    bind_byte(q, 0xde, &f->redraw_b);
    bind_byte(q, 0xdf, &f->redraw_c);
    bind_byte(q, 0xe0, &v->redraw_bar_aux);
    bind_byte(q, 0xe1, &f->redraw_e);
    bind_byte(q, 0xe2, &f->weapon_mode_redraws);
    bind_byte(q, 0xe3, &f->weapon_redraws);
    bind_byte(q, 0xe4, &f->redraw_d);
    bind_byte(q, 0xe6, &f->script_count);
    bind_byte(q, 0xe7, &c->indexed.pose_entry);
    bind_byte(q, 0xe8, &c->indexed.enable_selection);
    bind_byte(q, 0xea, &c->indexed.recorder_mode);
    bind_byte(q, 0xeb, &f->chaff_count);
    bind_byte(q, 0xec, &f->flare_count);
    bind_byte(q, 0xed, &f->chaff_timer);
    bind_byte(q, 0xee, &f->flare_timer);
    bind_byte(q, 0xf7, &v->update_mask);
    for (i = 0; i < FA18_COMMAND_QUEUE_NEIGHBORS; ++i)
        slot_write(&q->slots[i], q->neighbors[i]);
    context->key_taken = &q->taken;
    return 1;
}

static int signed_byte(uint8_t value) {
    return value < 128 ? (int)value : (int)value - 256;
}

int fa18_publish_native_command(FA18CommandQueue *q, uint32_t event,
                                uint32_t *published_event) {
    uint8_t raw = (uint8_t)event;
    if (!q || !q->commands || !published_event) return 0;
    if (!q->taken && !(raw & 0x80u)) {
        q->taken = 1;
        if (signed_byte(q->count) < 10) {
            int index = signed_byte(q->write_index);
            uint8_t translated;
            if (index >= 10) index = 0;
            slot_write(&q->slots[RAW_FIRST+index], raw);
            translated = q->key_table[raw];
            q->write_index = (uint8_t)(index+1);
            /* Read after the raw write, as in the original. */
            q->count = (uint8_t)(q->count+1);
            index = signed_byte(q->translated_index);
            slot_write(&q->slots[TRANSLATED_FIRST+index], translated);
            event = (event & 0xffff0000u) | translated;
        }
    }
    q->commands->modifier = 0;
    q->commands->indexed.function_modifier = 0;
    q->commands->other_modifier = 0;
    *published_event = event;
    return 1;
}
