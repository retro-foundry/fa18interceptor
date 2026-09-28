#include "terrain_template_cursor_resolver.h"
enum { H65=65, O70=0x40,O80=0x50,OA0=0x70,OB0=0x80,OF0=0xc0,R0=0x11e,R1=0x380,R2=0xf62 };
static int w(const uint8_t*p){return (int16_t)((uint16_t)p[0]<<8|p[1]);}
int fa18_resolve_terrain_template_cursor(const FA18Hunks*h,const FA18TerrainTemplateCursorState*s,FA18TerrainTemplateCursor*o){
 FA18TerrainTemplateStaticData d; const uint8_t*b,*root,*gate; size_t z,go; int q,x,y,off; uint8_t sel,secondary,derived=0;
 if(!h||!s||!o||fa18_load_terrain_template_static_data(h,&d)||h->count<=H65)return -1; b=h->segments[H65].data;z=h->segments[H65].size;if(!b||z<0xf64)return -1;
 if(s->append_enable){root=b+(s->route_flag?R0:R1);go=0x200;gate=s->gate_c;z=s->gate_c_size;sel=s->selector_b;x=s->row_b;y=s->group_b;}
 else {root=b+R1;go=s->alternate_pack?0x100:0;gate=s->alternate_pack?s->gate_b:s->gate_a;z=s->alternate_pack?s->gate_b_size:s->gate_a_size;sel=s->selector_a;x=s->row_a;y=s->group_a;if(!s->alternate_pack&&s->guard_long>(int32_t)0xffff6000){root=b+R2;derived=1;}else if(s->route_flag)root=b+R0;}
 if(!gate||!z||sel>=16)return -1;q=b[O70+sel];if(q>3)return -1;d.control_translate=(const int8_t*)(b+OF0+q*24);d.control_translate_count=21;d.group_directory_offset=go;
 if(derived){off=b[OA0+sel];if(s->route_flag)off*=2;else {off*=16;off+=b[O80+q*8+s->map_selector];}}
 else {off=b[OA0+sel]*32;secondary=b[O70+sel]*16+s->selector_b; if(secondary>=64)return -1;off+=b[OB0+secondary]*2;}
 if(off<0||off+2>(int)(h->segments[H65].size-(root-b))||(w(root+off))<=0)return -1;d.band_control=root+w(root+off);d.band_control_size=h->segments[H65].size-(size_t)(d.band_control-b);o->data=d;o->gate=gate;o->gate_size=z;o->row_term=x;o->group_term=y;o->derived_root=derived;return 0;
}
