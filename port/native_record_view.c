#include "native_record_view.h"

static int get8(const FA18NativeSceneRecord *r,size_t at,uint8_t *v) {
    return fa18_read_native_scene_record(r,at,v,1);
}
static int put8(FA18NativeSceneRecord *r,size_t at,uint8_t v) {
    return fa18_write_native_scene_record(r,at,&v,1);
}
static int get16(const FA18NativeSceneRecord *r,size_t at,uint16_t *v) {
    uint8_t b[2]; if(!fa18_read_native_scene_record(r,at,b,2)) return 0;
    *v=(uint16_t)(((unsigned)b[0]<<8)|b[1]); return 1;
}
static int put16(FA18NativeSceneRecord *r,size_t at,uint16_t v) {
    uint8_t b[2]={(uint8_t)(v>>8),(uint8_t)v}; return fa18_write_native_scene_record(r,at,b,2);
}
static int put32(FA18NativeSceneRecord *r,size_t at,uint32_t v) {
    uint8_t b[4]={(uint8_t)(v>>24),(uint8_t)(v>>16),(uint8_t)(v>>8),(uint8_t)v};
    return fa18_write_native_scene_record(r,at,b,4);
}
static FA18NativeSceneRecord *record_offset(FA18NativeRecordView *s,uint16_t offset) {
    return offset%512u || offset/512u>=16?NULL:s->records->records+offset/512u;
}
static int fault(FA18NativeRecordView *s,uint16_t code) {
    *s->error_word=code;
    return s->ops && s->ops->fault && s->ops->fault(s->ops->context,s);
}
static int publish_view(FA18NativeSceneRecord *r,const uint16_t words[4],uint32_t height) {
    unsigned i;
    for(i=0;i<4;++i) if(!put16(r,0x2c+2*i,words[i])) return 0;
    return put32(r,0x34,height);
}
static int table_view(FA18NativeRecordView *s,FA18NativeSceneRecord *r,int32_t at) {
    uint16_t words[4]; int16_t height; unsigned i;
    if(at>INT32_MAX-8) return 0;
    for(i=0;i<4;++i) if(!port_field_window_u16(&s->assets->parameters,at+2*(int32_t)i,words+i)) return 0;
    if(!port_field_window_s16(&s->assets->parameters,at+8,&height)) return 0;
    return publish_view(r,words,(uint32_t)(int32_t)height);
}
static int refresh_table(FA18NativeRecordView *s,FA18NativeSceneRecord *r,FA18NativeRecordViewWork *w) {
    uint8_t mode,index; uint16_t distance,first[4]; int16_t relative,next; int32_t at; uint32_t a,b; unsigned i;
    if(!get8(r,5,&mode)) return 0;
    if(mode!=8 || (r->aircraft->secondary_flags&0x100u)) return 1;
    if(!get16(r,0x4a,&distance)) return 0;
    if((int16_t)distance>0x480) return 1;
    if(!get8(r,0x3a,&index) || !port_field_window_s16(&s->assets->parameters,2*index,&relative)) return 0;
    at=relative;
    for(i=0;i<4;++i) if(!get16(r,0x2c+2*i,first+i)) return 0;
    for(;;) {
        if(at>INT32_MAX-18) return 0;
        if(!port_field_window_u32(&s->assets->parameters,at,&a) ||
           !port_field_window_u32(&s->assets->parameters,at+4,&b)) return 0;
        if(a==((uint32_t)first[0]<<16|first[1]) && b==((uint32_t)first[2]<<16|first[3])) break;
        /* ADDA.W advances the native data cursor without wrapping the pointer. */
        { int32_t cursor;
          if(at>INT32_MAX-10) return 0;
          cursor=at+10;
          if(!port_field_window_s16(&s->assets->parameters,cursor,&next)) return 0;
          if(next<0) return fault(s,0x35);
          at=cursor; }
    }
    if(!port_field_window_s16(&s->assets->parameters,(int32_t)at+10,&next)) return 0;
    if(next<0) {
        uint16_t old=r->aircraft->flags; r->aircraft->flags&=0xff7f;
        if((old&0x80u) && r->aircraft->equipment_kind==0x15) r->aircraft->flags|=0x200;
        return 1;
    }
    if(at>INT32_MAX-18 || !port_field_window_s16(&s->assets->parameters,at+18,&next)) return 0;
    w->carried_axis=(uint32_t)(int32_t)next;
    return table_view(s,r,at+10) && put16(r,0x4a,0x7fff);
}
static int linked(FA18NativeRecordView *s,FA18NativeSceneRecord *r) {
    uint16_t offset,words[4],angle; FA18NativeSceneRecord *other;
    r->aircraft->flags&=0xfffe;
    if(!(r->aircraft->flags&8u)) return 1;
    offset=*s->selected_record;
    if(offset==0xffff || offset==*s->current_stride) {
        if(!put32(r,0x34,0)) return 0;
        angle=r->word_6c;
        if((int16_t)angle<=0x4200) { angle=(uint16_t)(angle+0x240u); if((int16_t)angle>0x4200) angle=0x4200; }
        else { angle=(uint16_t)(angle-0x240u); if((int16_t)angle<0x4200) angle=0x4200; }
        r->word_6c=angle; r->word_6e=angle; return 1;
    }
    other=record_offset(s,offset); if(!other) return 0;
    words[0]=other->word_06; words[1]=other->word_08; words[2]=other->word_0c; words[3]=other->word_0e;
    return publish_view(r,words,other->long_10) && put8(r,0x38,(uint8_t)((offset>>9)|0x80u));
}
static int zone_view(FA18NativeRecordView *s,FA18NativeSceneRecord *r) {
    const PortFieldWindow *list=&s->assets->primary_list; int32_t at=0; int16_t head,offset;
    uint16_t selector; uint8_t zone; int from_zone=0;
    if(!(r->aircraft->flags&0x1000u)) return 1; /* source C242DA skips pending clear */
    for(;;) {
        if(!port_field_window_s16(list,at,&head)) return 0;
        if(head<0) {
            if(from_zone) return 0;
            if(!get8(r,0x5d,&zone)) return 0;
            if((int8_t)zone<=0) {
                if(!fault(s,0x34)) return 0;
                *s->pending=0; return 1;
            }
            if(!s->assets->zones || (size_t)(zone-1)>=s->assets->zone_count) return 0;
            list=s->assets->zones+zone-1; at=10; from_zone=1;
            /* An absent source match loops forever. Native data exhaustion
             * fails; repeating the same exhausted zone is also explicit. */
            if(!port_field_window_s16(list,at,&head) || head<0) return 0;
        }
        if(!port_field_window_u16(list,at+4,&selector)) return 0;
        if((selector&255u)==*s->current_slot) {
            if(!port_field_window_s16(list,at+6,&offset) ||
               !port_field_window_s16(&s->assets->parameters,offset,&offset) || !table_view(s,r,offset)) return 0;
            if(!put8(r,0x38,0xff)) return 0;
            r->aircraft->flags&=0xfffe; *s->pending=0;
            return 1;
        }
        if(at>INT32_MAX-10) return 0;
        at+=10;
    }
}
static int in_sight(FA18NativeRecordView *s,FA18NativeSceneRecord *r,FA18NativeSceneRecord *viewer) {
    uint8_t cycle; uint16_t range; int32_t vector[3],product[3],first,sum; unsigned i;
    if(!get8(r,0x39,&cycle)) return 0;
    if((cycle&0xf0u)!=0x10) return 1;
    if(!viewer || !get16(viewer,0x4a,&range)) return 0;
    if((int16_t)range>0x3000) goto clear;
    for(i=0;i<3;++i) vector[i]=(int32_t)(viewer->geometry->position[i]-r->geometry->position[i])>>8;
    if(!s->ops || !s->ops->normalize || !s->ops->normalize(s->ops->context,s,0xc0,vector,s->normalized)) return 0;
    for(i=0;i<3;++i) product[i]=(int32_t)viewer->geometry->inverse[i][2]*s->normalized[i];
    first=(int32_t)((uint32_t)product[2]+(uint32_t)product[0]);
    sum=(int32_t)((uint32_t)first+(uint32_t)product[1]);
    if((int64_t)first+product[1]>=0 || sum>-0x2c0000) goto clear;
    for(i=0;i<3;++i) product[i]=(int32_t)viewer->geometry->inverse[i][2]*r->geometry->inverse[i][2];
    first=(int32_t)((uint32_t)product[2]+(uint32_t)product[0]);
    sum=(int32_t)((uint32_t)first+(uint32_t)product[1]);
    if((int64_t)first+product[1]<0 || sum<0xd000000) goto clear;
    r->byte_04|=0x20; return 1;
clear:
    r->byte_04&=0xdf; return 1;
}
static int finish(FA18NativeRecordView *s,FA18NativeSceneRecord *r,FA18NativeSceneRecord *viewer) {
    uint16_t range,delay,kind; uint8_t mode; int32_t at=0; int16_t difference; int negative,row;
    if(r->byte_20&2u) goto done;
    if(!in_sight(s,r,viewer) || !get8(r,5,&mode)) return 0;
    if(mode==6 || (mode==8 && (*s->mode!=5 || !*s->pending)) || !(r->byte_04&0x20u)) goto done;
    if(!viewer || !get16(viewer,0x4a,&range)) return 0;
    if((int16_t)range>0x300) {
        at=0x60;
        if((int16_t)range>0xc00) { at+=0x60; if((int16_t)range>0x1e00) { at+=0x60; goto row; } }
    }
    difference=(int16_t)(uint16_t)(r->word_6c-viewer->word_6c);
    negative=(int32_t)(int16_t)r->word_6c-(int16_t)viewer->word_6c<0;
    if(negative) difference=(int16_t)(uint16_t)(0u-(uint16_t)difference);
    if(difference>=0xc0) at+=negative?0x20:0x40;
    if((int16_t)range>0x300 && (int16_t)*s->selected_record>=0 &&
       *s->selected_record==*s->current_stride && *s->pending) { *s->pending=0; at+=0x10; }
row:
    row=(int8_t)*s->limit; if(row>3) row=3; at+=4*row;
    if(!port_field_window_u16(&s->assets->status,at,&delay) || !put16(r,0x4c,delay)) return 0;
    if(!port_field_window_u16(&s->assets->status,at+2,&kind)) return 0;
    if(mode==8 && (r->aircraft->flags&8u)) {
        r->aircraft->flags&=0xfff7; ++*s->created; ++*s->admitted;
    }
    if(!put8(r,5,(uint8_t)kind)) return 0;
done:
    *s->pending=0; return 1;
}
static int place(FA18NativeRecordView *s,FA18NativeSceneRecord *r,FA18NativeRecordViewWork *w) {
    FA18NativeSceneRecord *viewer=w->viewer; int16_t local[3]; uint32_t world[3],sum; uint16_t words[4];
    uint8_t control,first,second; unsigned i,j;
    if(r->aircraft->secondary_flags&1u) {
        if(!get8(r,0x64,&control)) return 0;
        local[0]=control&1u?0:control&2u?0xa8:-0x60;
        local[1]=control&1u?(control&2u?0x60:-0x60):0; local[2]=-0x30;
    } else {
        if(!(*s->tick_word&255u)) {
            if(!get8(viewer,0x28,&first)) return 0;
            if((int8_t)first<0) first=(uint8_t)(0u-first);
            if((int16_t)(uint16_t)((w->carried_axis&0xff00u)|first)>0x14) goto flag;
            if(!get8(viewer,0x2a,&second)) return 0;
            if((int8_t)second<0) second=(uint8_t)(0u-second);
            if((int8_t)second>0x14) {
flag:
                *s->view_flag=(uint8_t)(viewer->geometry->position[0]&4u);
            }
        }
        local[0]=local[1]=0; local[2]=-0x24;
    }
    for(i=0;i<3;++i) {
        sum=0;
        for(j=0;j<3;++j) sum+=(uint32_t)((int32_t)local[j]*viewer->geometry->inverse[i][j]);
        world[i]=(uint32_t)((int32_t)sum>>4)+viewer->geometry->position[i];
    }
    words[0]=(uint16_t)((int16_t)(world[0]>>16)>>6); words[1]=(uint16_t)((int16_t)(world[2]>>16)>>6);
    words[2]=(uint16_t)((world[0]>>8)&0x3fff); words[3]=(uint16_t)((world[2]>>8)&0x3fff);
    return publish_view(r,words,(uint32_t)((int32_t)world[1]>>8)) && finish(s,r,viewer);
}
int fa18_update_native_record_view(FA18NativeRecordView *s,unsigned slot,FA18NativeRecordViewWork *w) {
    FA18NativeSceneRecord *r,*root; uint8_t mode,selector; unsigned i; int32_t limit;
    if(!s || !s->records || !w || slot>=16 || !s->assets || !s->selected_record || !s->current_stride ||
       !s->current_slot || !s->tick_word || !s->error_word || !s->post_input_event || !s->mode ||
       !s->limit || !s->pending || !s->view_flag || !s->created || !s->admitted || !s->normalized) return 0;
    if(*s->post_input_event) return 1;
    r=s->records->records+slot; root=s->records->records;
    if(!refresh_table(s,r,w) || !get8(r,5,&mode)) return 0;
    if(mode==8) return finish(s,r,w->viewer);
    if(r->aircraft->flags&8u) return !(r->aircraft->flags&2u) || linked(s,r);
    if(!get8(r,0x38,&selector)) return 0;
    w->viewer=root;
    if(selector!=0xff) {
        w->viewer=record_offset(s,(uint16_t)((selector&0x7fu)<<9));
        if(!w->viewer || !get8(r,0x7a,&mode)) return 0;
        if(mode==5 && !put8(r,0x7a,3)) return 0;
        if(!(w->viewer->aircraft->flags&0x40u)) return zone_view(s,r);
        r->aircraft->flags|=1;
        if(w->viewer==root) return place(s,r,w);
    }
    if(!get8(r,5,&mode)) return 0;
    if(mode!=8) {
        w->carried_axis=root->geometry->position[1];
        limit=(int8_t)*s->limit>=3?0x300000:(int8_t)*s->limit>=2?0x240000:0x180000;
        for(i=0;i<3;++i) {
            uint32_t d=root->geometry->position[i]-r->geometry->position[i];
            if((int64_t)(int32_t)root->geometry->position[i]-(int32_t)r->geometry->position[i]<0) d=0u-d;
            if(i==1) w->carried_axis=d;
            if((int32_t)d>limit) return place(s,r,w);
        }
        if(!put8(r,0x38,0x80)) return 0;
    }
    return place(s,r,w);
}
