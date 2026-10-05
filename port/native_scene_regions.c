#include "native_scene_regions.h"
#include "two_angle_matrix.h"

int fa18_load_native_scene_region_assets(const FA18Hunks *hunks,
    FA18NativeSceneRegionAssets *assets,PortFieldWindow *parameters,
    PortFieldWindow *directory,size_t capacity,
    const FA18NativeRegionDescriptor *descriptors,size_t descriptor_count) {
    const FA18HunkSegment *source; size_t i;
    if(!hunks || !hunks->segments || hunks->count<=27 || !assets || !parameters ||
       !directory || !capacity || (descriptor_count && !descriptors)) return 0;
    source=hunks->segments+27;
    if(!source->data) return 0;
    *parameters=(PortFieldWindow){.bytes=source->data,.byte_count=source->size};
    *assets=(FA18NativeSceneRegionAssets){.parameters=parameters,.directory=directory,
        .descriptors=descriptors,.descriptor_count=descriptor_count};
    for(i=0;i<capacity;++i) {
        size_t at; uint32_t segment,offset; const FA18HunkSegment *target;
        if(i>(SIZE_MAX-0x140)/4) return 0;
        at=0x140+4*i;
        if(at>source->size || source->size-at<2) return 0;
        if(fa18_be16(source->data+at)&0x8000u) {
            directory[i]=(PortFieldWindow){0}; assets->directory_count=i+1;
            return fa18_load_native_trig_data(hunks,&assets->trig)==0;
        }
        if(at>UINT32_MAX || !fa18_hunk_pointer(hunks,27,(uint32_t)at,&segment,&offset) ||
           segment>=hunks->count) return 0;
        target=hunks->segments+segment;
        if(!target->data || offset>target->size) return 0;
        directory[i]=(PortFieldWindow){.bytes=target->data,.byte_count=target->size,.origin=offset};
    }
    return 0;
}

static int16_t region_signed(uint16_t bits) {
    return bits<0x8000u?(int16_t)bits:(int16_t)((int32_t)bits-0x10000);
}
static uint32_t region_extended(uint16_t bits) {
    return (uint32_t)(int32_t)region_signed(bits);
}
static uint16_t region_half(uint16_t bits) {
    return (uint16_t)((bits>>1)|(bits&0x8000u));
}
static int region_ready(const FA18NativeSceneRegions *s) {
    return s && s->records && s->records->input && s->view_work && s->assets &&
        s->pointer_groups && s->pointer_group_count>=16 && s->assets->parameters &&
        (!s->assets->directory_count || s->assets->directory) &&
        (!s->assets->descriptor_count || s->assets->descriptors) &&
        s->recorder_on && s->occupied && s->mode && s->admitted && s->active &&
        s->reuse_jitter && s->random_word && s->jitter_x && s->jitter_z;
}
static int region_byte(FA18NativeSceneRecord *r,unsigned offset,uint8_t value) {
    return fa18_write_native_scene_record(r,offset,&value,1);
}
static int region_word(FA18NativeSceneRecord *r,unsigned offset,uint16_t value) {
    uint8_t bytes[2]={(uint8_t)(value>>8),(uint8_t)value};
    return fa18_write_native_scene_record(r,offset,bytes,2);
}
static int region_long(FA18NativeSceneRecord *r,unsigned offset,uint32_t value) {
    uint8_t bytes[4]={(uint8_t)(value>>24),(uint8_t)(value>>16),(uint8_t)(value>>8),(uint8_t)value};
    return fa18_write_native_scene_record(r,offset,bytes,4);
}
int fa18_set_native_region_view(FA18NativeSceneRecord *r,
    const uint16_t coordinates[4],uint32_t altitude) {
    unsigned i;
    if(!r || !coordinates) return 0;
    /* Snapshot caller inputs: callers may pass fields overwritten by stores. */
    { uint16_t words[4];
      for(i=0;i<4;++i) words[i]=coordinates[i];
      for(i=0;i<4;++i) if(!region_word(r,0x2c+2*i,words[i])) return 0; }
    return region_long(r,0x34,altitude);
}
static int region_parameter_words(const FA18NativeSceneRegions *s,int16_t relative,
    uint16_t words[5],unsigned count,uint32_t *axis) {
    unsigned i;
    for(i=0;i<count;++i) {
        if(!port_field_window_u16(s->assets->parameters,(int32_t)relative+2*(int32_t)i,words+i)) return 0;
        if(i==2 && axis) *axis=region_extended(words[i]);
    }
    return 1;
}
static int region_parameters(const FA18NativeSceneRegions *s,uint16_t selector,
    uint16_t words[5],uint32_t *axis) {
    int16_t relative;
    return port_field_window_s16(s->assets->parameters,region_signed(selector),&relative) &&
        region_parameter_words(s,relative,words,5,axis);
}
static int region_replacement_view(const FA18NativeSceneRegions *s,
    FA18NativeSceneRecord *r,uint16_t selector) {
    int16_t relative; uint16_t word; unsigned i;
    if(!port_field_window_s16(s->assets->parameters,region_signed(selector),&relative)) return 0;
    for(i=0;i<4;++i)
        if(!port_field_window_u16(s->assets->parameters,(int32_t)relative+2*(int32_t)i,&word) ||
           !region_word(r,0x2c+2*i,word)) return 0;
    return port_field_window_u16(s->assets->parameters,(int32_t)relative+8,&word) &&
        region_long(r,0x34,region_extended(word));
}
static int region_descriptor(const FA18NativeSceneRegions *s,uint16_t selector,
    FA18NativeScenePointerGroup *group,uint32_t *axis) {
    size_t i;
    for(i=0;i<s->assets->descriptor_count;++i) {
        const FA18NativeRegionDescriptor *d=s->assets->descriptors+i;
        if(d->selector==region_signed(selector)) {
            if(!d->group || !d->group->data[1].carried_value_bound) return 0;
            *group=*d->group; *axis=group->data[1].carried_value; return 1;
        }
    }
    return 0;
}
static int region_kind(const FA18NativeScenePointerGroup *group,uint8_t *kind) {
    int16_t header;
    if(!port_field_window_s16(&group->data[0].data,0,&header)) return 0;
    if(header>=0 && !port_field_window_s16(&group->data[0].data,(header&0x4000)?2:4,&header)) return 0;
    if(!port_field_window_byte(&group->data[0].data,6+(header&0x0fff),kind)) return 0;
    *kind&=15; return 1;
}
static uint32_t region_position(uint16_t coarse,uint16_t fine) {
    uint32_t swapped=((uint32_t)coarse<<16)|(region_signed(coarse)<0?0xffffu:0u);
    return (swapped<<6)+(region_extended(fine)<<8);
}
int fa18_dispatch_native_region_records(FA18NativeSceneRegions *s,
    const PortFieldWindow *rows,uint16_t counter,uint32_t *source_index) {
    uint32_t row,count=(uint32_t)counter+1;
    if(!region_ready(s) || !rows || !source_index) return 0;
    for(row=0;row<count;++row) {
        uint16_t selector,literal,link,parameter,options,words[5]; int16_t parameter_offset;
        uint32_t operand; unsigned slot; uint8_t kind;
        FA18NativeScenePointerGroup group; FA18NativeSceneRecord *r;
        int32_t at=(int32_t)(row*10);
        if(!port_field_window_u16(rows,at,&selector) ||
           !port_field_window_u16(rows,at+2,&literal) ||
           !port_field_window_u16(rows,at+8,&options)) return 0;
        if((options&0x0f00u) && (uint8_t)((options>>8)&15u)!=*s->mode) goto skipped;
        if(!port_field_window_u16(rows,at+4,&link)) return 0;
        slot=link&127u; if(slot>=16) return 0;
        r=s->records->records+slot;
        if(r->aircraft->flags&0x40u) goto skipped;
        if(!region_descriptor(s,selector,&group,&operand)) return 0;
        s->pointer_groups[slot]=group;
        s->view_work->carried_axis=operand;
        if(!region_kind(s->pointer_groups+slot,&kind)) return 0;
        s->view_work->carried_axis=(s->view_work->carried_axis&0xffffff00u)|kind;
        if((int8_t)*s->admitted<=(int8_t)*s->active) goto skipped;
        if(r->aircraft->equipment_kind==21 && r->word_06) goto skipped;
        if(r->aircraft->flags&0x40u) goto skipped;
        if(!fa18_clear_native_scene_record(r)) return 0;
        r->byte_7d=(uint8_t)((r->byte_7d&240u)|kind);
        r->aircraft->equipment_kind=(uint8_t)literal;
        if(!region_byte(r,0x5e,(uint8_t)slot)) return 0;
        /* Read remaining row fields after clear: a field window can bind live
         * adjacent owners, and original stores may change those values. */
        if(!port_field_window_u16(rows,at+4,&link) ||
           !region_byte(r,0x3a,(uint8_t)((link&0x7f00u)>>8)) ||
           !port_field_window_u16(rows,at+6,&parameter) ||
           !port_field_window_s16(s->assets->parameters,region_signed(parameter),&parameter_offset) ||
           !port_field_window_u16(rows,at+8,&options)) return 0;
        if(options&0x8000u) r->aircraft->flags|=8;
        else {
            r->aircraft->flags&=0xfff7u;
            if((r->aircraft->equipment_kind&240u)==16) ++*s->active;
        }
        if(options&0x4000u) if(!region_byte(r,5,8)) return 0;
        if(options&0x2000u) r->aircraft->flags|=1;
        if(!region_parameter_words(s,parameter_offset,words,5,&s->view_work->carried_axis)) return 0;
        if(options&0x1000u) {
            uint16_t x,z;
            if(*s->reuse_jitter) {
                x=*s->jitter_x; z=*s->jitter_z;
                s->view_work->carried_axis=((uint32_t)(options&128u)<<16)|words[2];
            } else {
                x=(uint16_t)((*s->random_word>>1)&3u);
                if(*s->random_word&16u) x=(uint16_t)(0u-x);
                *s->jitter_x=x;
                z=(uint16_t)((*s->random_word>>2)&3u);
                if(*s->random_word&32u) z=(uint16_t)(0u-z);
                *s->jitter_z=z;
            }
            if(options&128u) { x=region_half(x); z=region_half(z); }
            words[0]=(uint16_t)(words[0]+x); words[1]=(uint16_t)(words[1]+z);
        }
        if(!region_byte(r,0x5d,(uint8_t)(*source_index+1u)) || !region_byte(r,0x7a,3)) return 0;
        if((r->aircraft->equipment_kind&240u)!=32) r->aircraft->flags|=0x1080u;
        if(link&0x8000u) {
            if(link&0x7f00u) {
                if(!region_replacement_view(s,r,(uint16_t)((link&0x7f00u)>>7))) return 0;
            }
            if(!region_byte(r,0x38,255)) return 0;
        } else {
            if(!fa18_set_native_region_view(r,words,region_extended(words[4])) ||
               !region_byte(r,0x38,(uint8_t)((link>>8)|128u))) return 0;
        }
        r->word_06=words[0]; r->word_08=words[1]; r->word_0c=words[2]; r->word_0e=words[3];
        r->long_10=region_extended(words[4]);
        s->view_work->carried_axis=region_extended(words[2])<<8;
        if(r->long_10) { r->byte_7c|=0xe0u; r->word_6c=0x2000; r->word_6e=0x2000; }
        r->aircraft->flags=(uint16_t)((r->aircraft->flags|0x140u)&0x7fffu);
        r->byte_5f=68; r->word_60=500; r->long_72=0x61a800;
        r->aircraft->weapon_radar=61; r->byte_71=255;
        if(!region_byte(r,0x64,6) || !region_byte(r,0x64,16)) return 0;
        r->geometry->position[0]=region_position(words[0],words[2]);
        r->geometry->position[1]=region_extended(words[4])<<8;
        r->geometry->position[2]=region_position(words[1],words[3]);
        { const uint16_t angles[3]={0,0,0};
          s->view_work->carried_axis=0;
          if(!fa18_publish_native_record_orientation_with_axis(r,angles,&s->assets->trig,
              &s->view_work->carried_axis)) return 0; }
        *source_index=0; continue;
skipped:
        *source_index=UINT32_MAX;
    }
    return 1;
}
int fa18_spawn_native_region_records(FA18NativeSceneRegions *s,
    const PortFieldWindow *region,uint32_t *source_index) {
    int16_t count; PortFieldWindow rows;
    if(!region_ready(s) || !region || !source_index) return 0;
    if((int8_t)*s->mode<=2 || *s->mode==125) return 1;
    if(!port_field_window_s16(region,8,&count)) return 0;
    if(count<=0) { *source_index=UINT32_MAX; return 1; }
    rows=*region;
    if(rows.origin>rows.byte_count || rows.byte_count-rows.origin<10) return 0;
    rows.origin+=10;
    return fa18_dispatch_native_region_records(s,&rows,(uint16_t)(count-1),source_index);
}
static int region_exit(FA18NativeSceneRegions *s,const PortFieldWindow *region) {
    int16_t count; int32_t row;
    if(!port_field_window_s16(region,8,&count)) return 0;
    for(row=0;row<count;++row) {
        uint16_t link,parameter,words[5]; int16_t parameter_offset; unsigned slot;
        FA18NativeSceneRecord *r;
        uint8_t byte;
        if(!port_field_window_u16(region,14+10*row,&link) ||
           !port_field_window_u16(region,16+10*row,&parameter) ||
           !port_field_window_s16(s->assets->parameters,region_signed(parameter),&parameter_offset) ||
           !region_parameter_words(s,parameter_offset,words,4,&s->view_work->carried_axis)) return 0;
        slot=link&127u; if(slot>=16) return 0;
        r=s->records->records+slot;
        if(!fa18_read_native_scene_record(r,0x38,&byte,1)) return 0;
        if((byte&128u) && (byte&127u)) continue;
        if((link&0xff00u) && (link&0x8000u)) {
            if(!(link&0x7f00u)) continue;
            if(!region_parameters(s,(uint16_t)((link&0x7f00u)>>7),words,&s->view_work->carried_axis) ||
               !fa18_set_native_region_view(r,words,region_extended(words[4])) ||
               !region_byte(r,0x38,255)) return 0;
            r->aircraft->flags&=0xfffeu;
            if(!region_byte(r,0x7a,3)) return 0;
        } else if((link&0xff00u) && !(link&0x8000u)) {
            unsigned target=(link&0xff00u)>>8;
            FA18NativeSceneRecord *other;
            if(target>=16) return 0;
            other=s->records->records+target;
            /* The source repurposes the fourth coordinate for link testing
             * before an inactive target branches back into release. */
            words[3]=(uint16_t)(link&0xff00u);
            if(!(other->aircraft->flags&64u)) goto release;
            if(!region_word(r,0x2c,other->word_06) || !region_word(r,0x2e,other->word_08) ||
               !region_word(r,0x30,other->word_0c) || !region_word(r,0x32,other->word_0e) ||
               !region_long(r,0x34,other->long_10)) return 0;
            r->aircraft->flags&=0xfffeu;
            if(!region_byte(r,0x38,(uint8_t)(target|128u)) || !region_byte(r,0x7a,3)) return 0;
        } else {
release:
            if(!fa18_read_native_scene_record(r,0x7a,&byte,1)) return 0;
            if(byte==3 || byte==4) if(!region_byte(r,0x7a,5)) return 0;
            if(!port_field_window_u16(s->assets->parameters,(int32_t)parameter_offset+8,words+4)) return 0;
            if(!fa18_set_native_region_view(r,words,region_extended(words[4])) ||
               !region_byte(r,0x38,128)) return 0;
            r->aircraft->flags&=0xfffeu;
        }
    }
    return 1;
}
int fa18_update_native_scene_regions(FA18NativeSceneRegions *s) {
    size_t cursor; uint32_t index=0;
    if(!region_ready(s)) return 0;
    if(*s->recorder_on) return 1;
    for(cursor=0;cursor<s->assets->directory_count;++cursor) {
        const PortFieldWindow *region=s->assets->directory+cursor;
        uint16_t box[4]; unsigned i; uint8_t mask=(uint8_t)(1u<<(index&7u));
        int16_t x=region_signed(s->records->records[0].word_06);
        int16_t z=region_signed(s->records->records[0].word_08);
        if(!region->bytes) return 1;
        for(i=0;i<4;++i) {
            if(!port_field_window_u16(region,2*(int32_t)i,box+i)) return 0;
            if(i==3) s->view_work->carried_axis=region_extended(box[i]);
        }
        if(x>=region_signed(box[0]) && x<=region_signed(box[1]) &&
           z>=region_signed(box[2]) && z<=region_signed(box[3])) {
            *s->occupied|=mask;
            if(!fa18_spawn_native_region_records(s,region,&index)) return 0;
        } else if(*s->occupied&mask) {
            *s->occupied&=(uint8_t)~mask;
            if(!region_exit(s,region)) return 0;
        }
        index=(index&0xffff0000u)|(uint16_t)(index+1u);
        if(region_signed((uint16_t)index)>=8) return 1;
    }
    /* No implicit terminator or invented directory row. */
    return 0;
}
