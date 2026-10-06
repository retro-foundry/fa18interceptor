/* CPU and chipset are validation-only: run the exact native HUD owners
 * against original instructions and compare every non-stack RAM byte. */
#define main model_validation_main
#include "native_model_oracle.c"
#undef main
#include "graphics_glue.h"
#include "messages.h"
#include "notify.h"
#define FA18_NATIVE
#define draw_line host_draw_line
#define draw_line_to_row host_draw_line_to_row
#define draw_mark_polygon host_draw_mark_polygon
#define put hud_text_put
#define fill_bar host_fill_bar
#define fill_bar_words host_fill_bar_words
#define blit_image host_blit_image
#define draw_indicator_bars host_draw_indicator_bars
#define draw_mode_bar host_draw_mode_bar
#define draw_compass_tape host_draw_compass_tape
#define draw_panel_frame host_draw_panel_frame
#define draw_panel_image host_draw_panel_image
#define draw_panel_mark host_draw_panel_mark
#define draw_scale_readout host_draw_scale_readout
#define draw_zoom_readout host_draw_zoom_readout
#define draw_speed_readout host_draw_speed_readout
#define draw_altitude_readout host_draw_altitude_readout
#define draw_record_72_readout host_draw_record_72_readout
#define draw_record_2b_readout host_draw_record_2b_readout
#define draw_grid_z_readout host_draw_grid_z_readout
#define draw_grid_x_readout host_draw_grid_x_readout
#define draw_weapon_readout host_draw_weapon_readout
#define draw_signed_readout host_draw_signed_readout
#define draw_load_readout host_draw_load_readout
#define draw_shoot_cue host_draw_shoot_cue
#define draw_heading_readout host_draw_heading_readout
#define draw_weapon_status host_draw_weapon_status
#define draw_threat_lights host_draw_threat_lights
#define draw_three_digits host_draw_three_digits
#define draw_hud_marks host_draw_hud_marks
#define draw_tick_row host_draw_tick_row
#define draw_target_box host_draw_target_box
#define update_missile_cue host_update_missile_cue
#define draw_view_marker host_draw_view_marker
#define draw_gauge_bar host_draw_gauge_bar
#define plot_ring host_plot_ring
#define plot_ring_point host_plot_ring_point
#define plot_symbol host_plot_symbol
#define draw_postflight_tape host_draw_postflight_tape
#define draw_postflight_variant host_draw_postflight_variant
#define transform_postflight_record host_transform_postflight_record
#define draw_postflight_hud host_draw_postflight_hud
#define viewed_record hud_bars_record
#define divu_word hud_bars_divu
#define divs_word hud_bars_divs
#include "../../port/game/hud_bars.c"
#undef viewed_record
#undef divu_word
#undef divs_word
#define viewed_record hud_readouts_record
#define divu_word hud_readouts_divu
#define divs_word hud_readouts_divs
#include "../../port/game/hud_readouts.c"
#undef viewed_record
#undef divu_word
#undef divs_word
#define viewed_record hud_marks_record
#define divu_word hud_marks_divu
#define divs_word hud_marks_divs
#include "../../port/game/hud_marks.c"
#undef viewed_record
#undef divu_word
#undef divs_word
#define viewed_record view_marks_record
#define divu_word view_marks_divu
#define divs_word view_marks_divs
#include "../../port/game/view_marks.c"
#undef viewed_record
#undef divu_word
#undef divs_word
#define viewed_record postflight_hud_record
#define divu_word postflight_hud_divu
#define divs_word postflight_hud_divs
#include "../../port/game/postflight_hud.c"
#undef viewed_record
#undef divu_word
#undef divs_word

#define draw_postflight_tuple_pairs host_draw_postflight_tuple_pairs
#define draw_postflight_fixed_quad host_draw_postflight_fixed_quad
#define begin_postflight_variant_tail host_begin_postflight_variant_tail
#define select_postflight_variant_record host_select_postflight_variant_record
#define classify_postflight_variant_record host_classify_postflight_variant_record
#define submit_postflight_variant_record host_submit_postflight_variant_record
#define advance_postflight_variant_record host_advance_postflight_variant_record
#define process_postflight_variant_records host_process_postflight_variant_records
#define resolve_postflight_variant_status host_resolve_postflight_variant_status
#define scan_postflight_variant_records host_scan_postflight_variant_records
#define draw_postflight_tuple_variant host_draw_postflight_tuple_variant
#define draw_postflight_fixed_variant host_draw_postflight_fixed_variant
#define draw_postflight_tuple_variant_with_hooks host_draw_postflight_tuple_variant_with_hooks
#define draw_postflight_fixed_variant_with_hooks host_draw_postflight_fixed_variant_with_hooks
#define draw_postflight_renderer_dispatch host_draw_postflight_renderer_dispatch
#define draw_postflight_renderer_dispatch_with_hooks host_draw_postflight_renderer_dispatch_with_hooks
#define add_word postflight_add_word
#include "../../port/game/postflight_variants.c"
#undef add_word
#define draw_message_line host_draw_message_line
#undef draw_line
#define draw_line message_line_draw
#define divu_word message_line_divu
#include "../../port/game/message_line.c"
#undef divu_word
#undef draw_line
#define draw_line host_draw_line
#include "../../port/game/native/hud.c"
static int hud_original(uint32_t pc) {
    memset(REG_DA,0,sizeof REG_DA);REG_A[4]=rd_u16(LINE_LAST_ROW);
    REG_A[7]=0xc7ff00u;wr_u32(REG_A[7],0xc70000u);
    m68k_set_reg(M68K_REG_SR,0x2700);REG_PC=pc;
    fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
    for(unsigned step=0;step<2000000;++step) {
        if(REG_PC==0xc70000u && REG_A[7]==0xc7ff04u) {wait_blitter();return 1;}
        /* The native fixture has no graphics.library vector table. Route
         * the original C53F44 wrapper's WaitBlit JSR to its exact Kickstart
         * 1.3 ROM entry, then execute that ROM normally. */
        if(REG_PC==0xc53f4cu) {
            REG_A[7]-=4;wr_u32(REG_A[7],0xc53f50u);REG_PC=0xfc5a58u;continue;
        }
        int cycle_start=GET_CYCLES();uint16_t opcode=rd_u16(REG_PC);
        REG_PPC=REG_PC;REG_IR=opcode;REG_PC+=2;
        m68ki_instruction_jump_table[opcode]();USE_CYCLES(CYC_INSTRUCTION[opcode]);
        fa18_machine->cycle+=cycle_start-GET_CYCLES();
    }
    fprintf(stderr,"original HUD did not return at %06X\n",REG_PC);return 0;
}
static int hud_owners(void) {
    static const struct {uint32_t entry;void (*host)(void);} cases[]={
        {0xc30764,host_draw_panel_frame},{0xc309b6,host_draw_panel_image},
        {0xc332bc,host_draw_postflight_hud},{0xc3112a,host_draw_threat_lights},
        {0xc30f78,host_draw_compass_tape},{0xc31eb6,host_draw_heading_readout},
        {0xc31f4c,host_draw_speed_readout},{0xc32178,host_draw_record_2b_readout},
        {0xc3201a,host_draw_altitude_readout},{0xc3212a,host_draw_record_72_readout},
        {0xc30918,host_draw_gauge_bar},{0xc3003a,host_draw_panel_mark},
        {0xc328a8,host_draw_weapon_status},{0xc321d2,host_draw_grid_z_readout},
        {0xc32260,host_draw_grid_x_readout},{0xc31acc,host_draw_zoom_readout},
        {0xc31a64,host_draw_scale_readout},{0xc30b5c,host_draw_indicator_bars},
        {0xc30d34,host_draw_mode_bar},
        {0xc31226,host_draw_postflight_renderer_dispatch},{0xc322ee,host_draw_message_line},
        {0xc11bfc,update_message},{0xc11b44,tick_notification_cadence}
    };
    FA18Machine *saved=malloc(sizeof *saved),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);unsigned count=0;
    memcpy(saved,fa18_machine,sizeof *saved);
    for(unsigned variant=0;variant<5;++variant) {
        for(unsigned test=0;test<sizeof cases/sizeof cases[0];++test) {
            memcpy(fa18_machine,saved,sizeof *saved);
            const int16_t origins[]={0,-3,3,-20,20};
            wr_u16(SPAN_ORIGIN,(uint16_t)origins[variant]);
            wr_u16(SPAN_ORIGIN_Y,(uint16_t)(origins[variant]*16));
            wr_u16(REDRAW_STATE_WORD,0);wr_u32(REDRAW_STATE_LONG,0);
            wr_u8(REDRAW_FIRST,3);wr_u8(GAUGE_REFRESH,3);
            wr_u8(DISPLAY_UPDATE,3);wr_u8(WEAPON_REDRAWS,3);
            wr_u8(GRID_X_REDRAWS,3);wr_u8(GRID_Z_REDRAWS,3);
            wr_u8(BAR_REDRAWS_A,3);wr_u8(BAR_REDRAWS_B,3);wr_u8(BAR_REDRAWS_C,3);
            wr_u8(BAR_REDRAWS_D,3);wr_u8(BAR_REDRAWS_E,3);wr_u8(BAR_REDRAWS_F,3);
            if(cases[test].entry==0xc31226u) {
                wr_u8(GAUGE_REFRESH,(uint8_t)(variant%3));
                wr_u16(STREAM_SKIP,(uint16_t)(variant*4));
            }
            memcpy(before,fa18_machine,sizeof *before);
            cases[test].host();
            memcpy(expected,fa18_machine->chip,0x80000);memcpy(expected+0x80000,fa18_machine->slow,0x80000);
            memcpy(fa18_machine,before,sizeof *before);
            if(!hud_original(cases[test].entry)) {
                fprintf(stderr,"HUD case %06X variant %u stopped, preceding PC %06X opcode %04X\n",cases[test].entry,variant,REG_PPC,REG_IR);return 0;
            }
            unsigned differences=0;
            for(unsigned i=0;i<0xffc00;++i) {
                uint8_t actual=i<0x80000?fa18_machine->chip[i]:fa18_machine->slow[i-0x80000];
                if(actual!=expected[i]) {
                    if(differences<8) fprintf(stderr,"HUD %06X variant %u at %06X: source %02X host %02X\n",cases[test].entry,variant,i<0x80000?i:0xc00000+i-0x80000,actual,expected[i]);
                    ++differences;
                }
            }
            if(differences) {fprintf(stderr,"HUD %06X: %u non-stack differences\n",cases[test].entry,differences);return 0;}
            ++count;
        }
    }
    memcpy(fa18_machine,saved,sizeof *saved);free(saved);free(before);free(expected);
    printf("%u HUD instrument/panel cases match original non-stack RAM\n",count);return 1;
}
#ifndef FA18_HUD_ORACLE_LIBRARY
int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0;char error[256];
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=file_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=argc==2?file_bytes(argv[1],&nd):NULL;
    FA18Machine *m=calloc(1,sizeof *m);
    if(!state||!rom||!data||nd!=0x100000||!m || !fa18_os_wait_blit_signature_matches(rom)) return 1;
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) {fputs(error,stderr);return 1;}
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;
    memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
    return hud_owners()?0:1;
}
#endif
