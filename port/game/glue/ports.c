/* Registry of recreated routines (recomp_ports.h). One row per original
 * routine: its address, glue, the C function it calls, and the CPU cycles to
 * charge in ON mode: instruction time only; blitter waits are charged by
 * wait_blitter(). */
#include "recomp_ports.h"

#include "ports_glue.h"

const FA18Port fa18_ports[] = {
    /* Complete flight-record actions and control streams. */
    {0xC230E8, glue_C230E8, "select_flight_record_action", 0, 0, glue_C230E8_step, 0xC23174, 0, 0, glue_C230E8_owns},
    {0xC23116, glue_C23116, "select_flight_record_action", 0, 0, glue_C23116_step, 0xC23174, 0, 0, glue_C23116_owns},
    {0xC23186, glue_C23186, "queue_flight_record_action_sound", 0, 0, glue_C23186_step, 0xC231A2, 0, 0, glue_C23186_owns},
    {0xC23228, glue_C23228, "advance_flight_record_control", 0, 0, glue_C23228_step, 0xC23578, 0, 0, glue_C23228_owns},
    {0xC233AA, glue_C233AA, "advance_flight_record_stream", 0, 0, glue_C233AA_step, 0xC23578, 0, 0, glue_C233AA_owns},
    {0xC23578, glue_C23578, "select_next_flight_record_stream", 0, 0, glue_C23578_step, 0xC23622, 0, 0, glue_C23578_owns},
    {0xC236AA, glue_C236AA, "initialise_flight_record_manoeuvre", 0, 0, glue_C236AA_step, 0xC23A26, 0, 0, glue_C236AA_owns},
    {0xC23716, glue_C23716, "initialise_flight_record_release", 0, 0, glue_C23716_step, 0xC23A26, 0, 0, glue_C23716_owns},
    {0xC2377E, glue_C2377E, "try_flight_record_action", 0, 0, glue_C2377E_step, 0xC23A26, 0, 0xC23744, glue_C2377E_owns},
    {0xC257EC, glue_C257EC, "normalise_flight_record_direction", 0, 0, glue_C257EC_step, 0xC25864, 0, 0xC257DC, glue_C257EC_owns},
    /* Complete main-loop control, flight and message owners. */
    {0xC149BE, glue_C149BE, "advance_main_loop_flight_controls", 0, 0, glue_C149BE_step, 0xC15138, 0, 0, glue_C149BE_owns},
    {0xC23A7E, glue_C23A7E, "advance_main_loop_flight_record", 0, 0, glue_C23A7E_step, 0xC244E2, 0, 0xC23A7A, glue_C23A7E_owns},
    {0xC153FC, glue_C153FC, "advance_control_record_action", 0, 0, glue_C153FC_step, 0xC15688, 0, 0, glue_C153FC_owns},
    {0xC15688, glue_C15688, "initialise_control_record_action", 0, 0, glue_C15688_step, 0xC158D8, 0, 0, glue_C15688_owns},
    {0xC159AE, glue_C159AE, "aim_control_record_action", 0, 0, glue_C159AE_step, 0xC15AD4, 0, 0, glue_C159AE_owns},
    {0xC15AD4, glue_C15AD4, "publish_control_record_direction", 0, 0, glue_C15AD4_step, 0xC15BF6, 0, 0, glue_C15AD4_owns},
    {0xC181A0, glue_C181A0, "start_control_record_alert", 0, 0, glue_C181A0_step, 0xC181FA, 0, 0, glue_C181A0_owns},
    {0xC1518C, glue_C1518C, "advance_main_loop_control_records", 0, 0, glue_C1518C_step, 0xC153FC, 0, 0, glue_C1518C_owns},
    {0xC32CEE, glue_C32CEE, "advance_main_loop_message_sequence", 0, 0, glue_C32CEE_step, 0xC330FE, 0, 0xC32BD2, glue_C32CEE_owns},
    /* Complete main-loop timer/readout and setup-bounds owners. */
    {0xC2527C, glue_C2527C, "prepare_setup_bounds", 0, 0, glue_C2527C_step, 0xC25312, 0, 0xC2527A, glue_C2527C_owns},
    {0xC25312, glue_C25312, "advance_main_loop_timers", 0, 0, glue_C25312_step, 0xC25482, 0, 0, glue_C25312_owns},
    /* Complete gameport/text setup and outer display synchronization. */
    {0xC16D4C, glue_C16D4C, "open_gameport_device", 0, 0, glue_C16D4C_step, 0xC16EAE, 0, 0, glue_C16D4C_owns},
    {0xC16FF4, glue_C16FF4, "set_gameport_controller_type", 0, 0, glue_C16FF4_step, 0xC17066, 0, 0, glue_C16FF4_owns},
    {0xC17066, glue_C17066, "configure_gameport_events", 0, 0, glue_C17066_step, 0xC170B2, 0, 0, glue_C17066_owns},
    {0xC1787A, glue_C1787A, "load_setup_text_resources", 0, 0, glue_C1787A_step, 0xC17B08, 0, 0, glue_C1787A_owns},
    /* C1612C has complete domain/CPU/step source and exact temporary ON
     * replay, but is not activated: its original graphics wait cannot finish
     * inside the frozen-event comparison. Keep that failure explicit. */
    /* Complete game input callback and setup owners. */
    {0xC1718E, glue_C1718E, "advance_input_device_callback", 0, 0, glue_C1718E_step, 0xC17456, 2, 0, glue_C1718E_owns},
    {0xC17456, glue_C17456, "install_input_device_callback", 0, 0, glue_C17456_step, 0xC1748C, 0, 0, glue_C17456_owns},
    {0xC1748C, glue_C1748C, "remove_input_device_callback", 0, 0, glue_C1748C_step, 0xC174A0, 0, 0, glue_C1748C_owns},
    {0xC174A0, glue_C174A0, "prepare_input_device_port", 0, 0, glue_C174A0_step, 0xC174F4, 0, 0, glue_C174A0_owns},
    {0xC16CD8, glue_C16CD8, "open_input_device_timer", 0, 0, glue_C16CD8_step, 0xC16D04, 0, 0, glue_C16CD8_owns},
    {0xC16B8C, glue_C16B8C, "open_input_device_request", 0, 0, glue_C16B8C_step, 0xC16BF2, 0, 0, glue_C16B8C_owns},
    {0xC17104, glue_C17104, "set_input_device_bounds", 0, 0, glue_C17104_step, 0xC1712C, 0, 0, glue_C17104_owns},
    {0xC1712C, glue_C1712C, "initialise_input_device_counters", 0, 0, glue_C1712C_step, 0xC1715C, 2, 0, glue_C1712C_owns},
    /* Complete game-side mode-file callers; actual OS children remain. */
    {0xC0EF08, glue_C0EF08, "check_postflight_mode_file", 0, 0, glue_C0EF08_step, 0xC0EFD2, 0, 0, glue_C0EF08_owns},
    {0xC162E4, glue_C162E4, "refresh_postflight_mode_file", 0, 0, glue_C162E4_step, 0xC1631C, 0, 0, glue_C162E4_owns},
    {0xC1631C, glue_C1631C, "save_postflight_mode_file", 0, 0, glue_C1631C_step, 0xC16386, 0, 0, glue_C1631C_owns},
    {0xC16386, glue_C16386, "read_postflight_mode_file", 0, 0, glue_C16386_step, 0xC16406, 0, 0, glue_C16386_owns},
    /* Complete postflight message and text callback owners. */
    {0xC0F4D8, glue_C0F4D8, "initialise_postflight_text", 0, 0, glue_C0F4D8_step, 0xC0F56A, 0, 0, glue_C0F4D8_owns},
    {0xC0F812, glue_C0F812, "copy_postflight_text", 0, 0, glue_C0F812_step, 0xC0F920, 0, 0, glue_C0F812_owns},
    {0xC110A4, glue_C110A4, "prepare_postflight_messages", 0, 0, glue_C110A4_step, 0xC11312, 0, 0, glue_C110A4_owns},
    {0xC11350, glue_C11350, "record_postflight_outcome", 0, 0, glue_C11350_step, 0xC113E4, 0, 0, glue_C11350_owns},
    {0xC113E4, glue_C113E4, "queue_postflight_text_error", 0, 0, glue_C113E4_step, 0xC1141E, 0, 0, glue_C113E4_owns},
    {0xC1141E, glue_C1141E, "wait_postflight_text_error", 0, 0, glue_C1141E_step, 0xC11446, 0, 0, glue_C1141E_owns},
    {0xC11446, glue_C11446, "queue_postflight_intro", 0, 0, glue_C11446_step, 0xC11478, 0, 0, glue_C11446_owns},
    {0xC11478, glue_C11478, "accept_postflight_return", 0, 0, glue_C11478_step, 0xC114D2, 0, 0, glue_C11478_owns},
    {0xC114D2, glue_C114D2, "prepare_postflight_status", 0, 0, glue_C114D2_step, 0xC1159E, 0, 0, glue_C114D2_owns},
    {0xC1159E, glue_C1159E, "wait_postflight_status", 0, 0, glue_C1159E_step, 0xC115BA, 0, 0, glue_C1159E_owns},
    {0xC115BA, glue_C115BA, "prepare_postflight_retry", 0, 0, glue_C115BA_step, 0xC1169A, 0, 0, glue_C115BA_owns},
    {0xC1169A, glue_C1169A, "wait_postflight_retry_message", 0, 0, glue_C1169A_step, 0xC116B0, 0, 0, glue_C1169A_owns},
    {0xC116B0, glue_C116B0, "wait_postflight_retry_input", 0, 0, glue_C116B0_step, 0xC116CE, 0, 0, glue_C116B0_owns},
    {0xC116CE, glue_C116CE, "advance_postflight_retry", 0, 0, glue_C116CE_step, 0xC11738, 0, 0, glue_C116CE_owns},
    {0xC11738, glue_C11738, "finish_postflight_retry", 0, 0, glue_C11738_step, 0xC1175C, 0, 0, glue_C11738_owns},
    /* Complete postflight/reset/restart callbacks and root transform. */
    {0xC118FC, glue_C118FC, "follow_postflight_message", 0, 0, glue_C118FC_step, 0xC11934, 0, 0, glue_C118FC_owns},
    {0xC11934, glue_C11934, "clear_postflight_phase", 0, 0, glue_C11934_step, 0xC11958, 0, 0, glue_C11934_owns},
    {0xC1104C, glue_C1104C, "queue_postflight_end", 0, 0, glue_C1104C_step, 0xC11078, 0, 0, glue_C1104C_owns},
    {0xC091E6, glue_C091E6, "root_to_world", 0, 0, glue_C091E6_step, 0xC0924A, 0, 0, glue_C091E6_owns},
    /* Complete menu/context completion owners and source children. */
    {0xC10A24, glue_C10A24, "follow_menu_smoothing", 0, 0, glue_C10A24_step, 0xC10AB2, 0, 0, glue_C10A24_owns},
    {0xC10AB2, glue_C10AB2, "queue_menu_smoothing_message", 0, 0, glue_C10AB2_step, 0xC10AE6, 0, 0, glue_C10AB2_owns},
    {0xC10AE6, glue_C10AE6, "restart_menu_smoothing", 0, 0, glue_C10AE6_step, 0xC10B1E, 0, 0, glue_C10AE6_owns},
    {0xC10B1E, glue_C10B1E, "reset_menu_smoothing_view", 0, 0, glue_C10B1E_step, 0xC10B90, 0, 0, glue_C10B1E_owns},
    {0xC10CFE, glue_C10CFE, "finish_menu_context_message", 0, 0, glue_C10CFE_step, 0xC10D8A, 0, 0, glue_C10CFE_owns},
    {0xC10DAE, glue_C10DAE, "update_menu_context", 0, 0, glue_C10DAE_step, 0xC1104C, 0, 0, glue_C10DAE_owns},
    {0xC11A26, glue_C11A26, "queue_menu_viewport_message", 0, 0, glue_C11A26_step, 0xC11A50, 0, 0, glue_C11A26_owns},
    {0xC11A50, glue_C11A50, "finish_menu_viewport_message", 0, 0, glue_C11A50_step, 0xC11ACC, 0, 0, glue_C11A50_owns},
    {0xC09192, glue_C09192, "load_menu_position_preset", 0, 0, glue_C09192_step, 0xC091A8, 0, 0, glue_C09192_owns},
    {0xC16D04, glue_C16D04, "read_menu_time_sample", 0, 0, glue_C16D04_step, 0xC16D4C, 0, 0, glue_C16D04_owns},
    /* Complete menu/context return owners, including source-only callbacks. */
    {0xC1064C, glue_C1064C, "finish_menu_context_three", 0, 0, glue_C1064C_step, 0xC10678, 0, 0, glue_C1064C_owns},
    {0xC108FE, glue_C108FE, "return_only_menu_callback", 0, 0, glue_C108FE_step, 0xC10900, 0, 0, glue_C108FE_owns},
    {0xC10900, glue_C10900, "follow_menu_return_message", 0, 0, glue_C10900_step, 0xC10942, 0, 0, glue_C10900_owns},
    {0xC10970, glue_C10970, "follow_menu_return_context", 0, 0, glue_C10970_step, 0xC109AC, 0, 0, glue_C10970_owns},
    {0xC102D8, glue_C102D8, "begin_menu_context_ready", 0, 0, glue_C102D8_step, 0xC10302, 0, 0, glue_C102D8_owns},
    {0xC10942, glue_C10942, "start_menu_smoothing", 0, 0, glue_C10942_step, 0xC10970, 0, 0, glue_C10942_owns},
    {0xC109AC, glue_C109AC, "complete_menu_return_after_countdown", 0, 0, glue_C109AC_step, 0xC10A24, 0, 0, glue_C109AC_owns},
    {0xC10302, glue_C10302, "select_menu_return_message", 0, 0, glue_C10302_step, 0xC10362, 0, 0, glue_C10302_owns},
    {0xC10BAE, glue_C10BAE, "cancel_menu_return", 0, 0, glue_C10BAE_step, 0xC10C08, 0, 0, glue_C10BAE_owns},
    {0xC10362, glue_C10362, "leave_menu_return_on_key", 0, 0, glue_C10362_step, 0xC103E4, 0, 0, glue_C10362_owns},
    /* Complete delayed-menu/outcome owners and immediate continuations. */
    {0xC104C2, glue_C104C2, "select_delayed_menu_message", 0, 0, glue_C104C2_step, 0xC105A6, 0, 0, glue_C104C2_owns},
    {0xC105F4, glue_C105F4, "pause_menu_after_countdown", 0, 0, glue_C105F4_step, 0xC10626, 0, 0, glue_C105F4_owns},
    {0xC1078A, glue_C1078A, "finish_menu_outcome", 0, 0, glue_C1078A_step, 0xC108DA, 0, 0, glue_C1078A_owns},
    {0xC105A6, glue_C105A6, "leave_delayed_menu_message", 0, 0, glue_C105A6_step, 0xC105F4, 0, 0, glue_C105A6_owns},
    {0xC10626, glue_C10626, "start_menu_context_after_countdown", 0, 0, glue_C10626_step, 0xC1064C, 0, 0, glue_C10626_owns},
    {0xC108DA, glue_C108DA, "queue_menu_attempts_exhausted", 0, 0, glue_C108DA_step, 0xC108FE, 0, 0, glue_C108DA_owns},
    {0xC29368, glue_C29368, "select_origin_control_record", 0, 0, glue_C29368_step, 0xC2940A, 0, 0, glue_C29368_owns},
    /* Complete menu follow-ups and original table-file owner. Three callbacks
     * are source-only; C1643A is a translated entry and C10678 was registered. */
    {0xC1029E, glue_C1029E, "poll_menu_viewport", 0, 0, glue_C1029E_step, 0xC102D8, 0, 0, glue_C1029E_owns},
    {0xC10418, glue_C10418, "poll_menu_viewport", 0, 0, glue_C10418_step, 0xC10458, 0, 0, glue_C10418_owns},
    {0xC10458, glue_C10458, "follow_menu_key_or_countdown", 0, 0, glue_C10458_step, 0xC104C2, 0, 0, glue_C10458_owns},
    {0xC1643A, glue_C1643A, "load_menu_mode_file", 0, 0, glue_C1643A_step, 0xC16512, 0, 0, glue_C1643A_owns},
    /* Original cold callback/helper entries absent from the seeded translation.
     * Track these ten separately from the 624 translated-entry denominator. */
    {0xC0FE36, glue_C0FE36, "consume_menu_table_action", 0, 0, glue_C0FE36_step, 0xC0FECE, 0, 0, glue_C0FE36_owns},
    {0xC1017E, glue_C1017E, "queue_available_menu_modes", 0, 0, glue_C1017E_step, 0xC101FC, 0, 0, glue_C1017E_owns},
    {0xC10272, glue_C10272, "leave_menu_after_countdown", 0, 0, glue_C10272_step, 0xC1029E, 0, 0, glue_C10272_owns},
    {0xC103E4, glue_C103E4, "leave_menu_after_countdown", 0, 0, glue_C103E4_step, 0xC10418, 0, 0, glue_C103E4_owns},
    {0xC09120, glue_C09120, "set_menu_position_preset", 0, 0, glue_C09120_step, 0xC09192, 0, 0, glue_C09120_owns},
    {0xC09148, glue_C09148, "set_menu_position_preset", 0, 0, glue_C09148_step, 0xC09192, 0, 0, glue_C09148_owns},
    {0xC29490, glue_C29490, "load_origin_candidate_preset", 0, 0, glue_C29490_step, 0xC294AC, 0, 0, glue_C29490_owns},
    {0xC2949A, glue_C2949A, "load_origin_candidate_preset", 0, 0, glue_C2949A_step, 0xC294AC, 0, 0, glue_C2949A_owns},
    {0xC10B90, glue_C10B90, "refresh_menu_cockpit", 0, 0, glue_C10B90_step, 0xC10BAE, 0, 0, glue_C10B90_owns},
    {0xC16406, glue_C16406, "clear_menu_mode_table", 0, 0, glue_C16406_step, 0xC1643A, 0, 0, glue_C16406_owns},
    /* Complete menu setup and source-owned input/message helpers. */
    {0xC0FBE0, glue_C0FBE0, "start_top_level_menu", 0, 0, glue_C0FBE0_step, 0xC0FCB4, 0, 0, glue_C0FBE0_owns},
    {0xC1082C, glue_C1082C, "begin_menu_countdown", 0, 0, glue_C1082C_step, 0xC108DA, 0, 0, glue_C1082C_owns},
    {0xC11BB0, glue_C11BB0, "filter_cockpit_message", 0, 0, glue_C11BB0_step, 0xC11BFC, 0, 0, glue_C11BB0_owns},
    {0xC24FA4, glue_C24FA4, "queue_indexed_menu_message", 0, 0, glue_C24FA4_step, 0xC24FE4, 0, 0, glue_C24FA4_owns},
    /* Complete menu callback owners, including all six delayed table arms. */
    {0xC0FCB4, glue_C0FCB4, "follow_top_level_menu", 0, 0, glue_C0FCB4_step, 0xC0FE36, 0, 0, glue_C0FCB4_owns},
    {0xC0FECE, glue_C0FECE, "advance_delayed_menu", 0, 0, glue_C0FECE_step, 0xC1017E, 0, 0, glue_C0FECE_owns},
    {0xC0FFE2, glue_C0FFE2, "enter_menu_mode_nine", 0, 0, glue_C0FFE2_step, 0xC1017E, 0, 0, glue_C0FFE2_owns},
    {0xC1000A, glue_C1000A, "enter_menu_demonstration", 0, 0, glue_C1000A_step, 0xC1017E, 0, 0, glue_C1000A_owns},
    {0xC17C2A, glue_C17C2A, "start_menu_alert_pair", 0, 0, glue_C17C2A_step, 0xC17C62, 0, 0, glue_C17C2A_owns},
    {0xC24E8A, glue_C24E8A, "format_menu_summary", 0, 0, glue_C24E8A_step, 0xC24FA4, 0, 0, glue_C24E8A_owns},
    /* Complete command/context and selected-record publication owners. */
    {0xC1B7A6, glue_C1B7A6, "publish_context_detail_command", 0, 0, glue_C1B7A6_step, 0xC1C2B8, 0, 0, glue_C1B7A6_owns},
    {0xC1BEE8, glue_C1BEE8, "publish_context_record_command", 0, 0, glue_C1BEE8_step, 0xC1C2B8, 0, 0xC1B906, glue_C1BEE8_owns},
    {0xC1C214, glue_C1C214, "publish_context_toggle_command", 0, 0, glue_C1C214_step, 0xC1C2B8, 0, 0, glue_C1C214_owns},
    {0xC083A6, glue_C083A6, "set_selected_record_request", 0, 0, glue_C083A6_step, 0xC083E2, 0, 0, glue_C083A6_owns},
    {0xC09DD0, glue_C09DD0, "clear_matching_record_selection", 0, 0, glue_C09DD0_step, 0xC09E06, 0, 0, glue_C09DD0_owns},
    /* Complete postflight mode scheduler and source-owned shared tails. */
    {0xC09E06, glue_C09E06, "dispatch_postflight_mode", 0, 0, glue_C09E06_step, 0xC09E96, 0, 0, glue_C09E06_owns},
    {0xC09E98, glue_C09E98, "schedule_postflight_mode_three", 0, 0, glue_C09E98_step, 0xC0A3EA, 0, 0xC09E96, glue_C09E98_owns},
    {0xC09EC4, glue_C09EC4, "schedule_postflight_mode_four", 0, 0, glue_C09EC4_step, 0xC0A3EA, 0, 0xC09EC2, glue_C09EC4_owns},
    {0xC0A002, glue_C0A002, "schedule_postflight_mode_five", 0, 0, glue_C0A002_step, 0xC0A3EA, 0, 0, glue_C0A002_owns},
    {0xC0A12E, glue_C0A12E, "restore_postflight_record_parameters", 0, 0, glue_C0A12E_step, 0xC0A15A, 0, 0, glue_C0A12E_owns},
    {0xC0A15C, glue_C0A15C, "schedule_postflight_mode_six", 0, 0, glue_C0A15C_step, 0xC0A3EA, 0, 0xC0A15A, glue_C0A15C_owns},
    {0xC0A1E0, glue_C0A1E0, "schedule_postflight_mode_seven", 0, 0, glue_C0A1E0_step, 0xC0A3EA, 0, 0xC0A1DE, glue_C0A1E0_owns},
    {0xC0A334, glue_C0A334, "schedule_postflight_mode_125", 0, 0, glue_C0A334_step, 0xC0A3C6, 0, 0xC0A332, glue_C0A334_owns},
    {0xC0A364, glue_C0A364, "schedule_other_postflight_modes", 0, 0, glue_C0A364_step, 0xC0A3EA, 0, 0xC0A362, glue_C0A364_owns},
    {0xC0A3EA, glue_C0A3EA, "check_postflight_player_ready", 0, 0, glue_C0A3EA_step, 0xC0A42C, 0, 0, glue_C0A3EA_owns},
    {0xC1AC28, glue_C1AC28, "dispatch_pending_command", 0, 0, glue_C1AC28_step, 0xC1C2BE, 0, 0xC1AC18, glue_C1AC28_owns},
    {0xC1AD74, glue_C1AD74, "dispatch_keyboard_command", 0, 0, glue_C1AD74_step, 0xC1C2BE, 0, 0xC06BF0, glue_C1AD74_owns},
    {0xC16EAE, glue_C16EAE, "consume_external_input_event", 0, 0, glue_C16EAE_step, 0xC16F1C, 2},
    {0xC16BF2, glue_C16BF2, "read_keyboard_event_source", 0, 0, glue_C16BF2_step, 0xC16C3A},
    {0xC16C56, glue_C16C56, "poll_raw_keyboard_event", 0, 0, glue_C16C56_step, 0xC16CD8},
    {0xC13D34, glue_C13D34, "consume_changed_buttons", 0, 0, glue_C13D34_step, 0xC13D84},
    {0xC0EFD4, glue_C0EFD4, "run_game_update_sequence", 0, 0, glue_C0EFD4_step, 0xC0F3C4, 3},
    {0xC0F3C4, glue_C0F3C4, "process_pending_key_events", 0, 0, glue_C0F3C4_step, 0xC0F4A6, 2},
    {0xC0D730, glue_C0D730, "submit_update_display_buffers", 0, 0, glue_C0D730_step, 0xC0D74A, 1},
    {0xC29042, glue_C29042, "publish_selector_origin", 0, 0, glue_C29042_step, 0xC295D2, 0, 0xC29040},
    {0xC22C80, glue_C22C80, "update_control_records", 0, 0, glue_C22C80_step, 0xC230B0},
    {0xC1C63E, glue_C1C63E, "run_record_update_stage", 0, 0, glue_C1C63E_step, 0xC1C7F6},
    {0xC1C860, glue_C1C860, "refresh_context_packet", 0, 0, glue_C1C860_step, 0xC1CA2E, 0, 0xC1C85E},
    {0xC08F26, glue_C08F26, "bootstrap_scene", 0, 0, glue_C08F26_step, 0xC090C2},
    {0xC0F920, glue_C0F920, "reset_sequence_after_bootstrap", 0, 0, glue_C0F920_step, 0xC0F946},
    {0xC0F992, glue_C0F992, "begin_sequence_after_bootstrap", 0, 0, glue_C0F992_step, 0xC0FA04},
    {0xC0F5F8, glue_C0F5F8, "run_post_input_tick", 0, 0,
     glue_C0F5F8_step, 0xC0F812},
    /* Complete follow-up placements and signed workspace accumulation. */
    {0xC1CCBC, glue_C1CCBC, "visit_followup_placements", 0, 0,
     glue_C1CCBC_step, 0xC1D0A4},
    {0xC1D0A4, glue_C1D0A4, "accumulate_workspace_position", 0, 0,
     glue_C1D0A4_step, 0xC1D10C},
    /* scene_placements.c: both complete descriptor-dispatch parents */
    {0xC1CB14, glue_C1CB14, "visit_primary_scene_placements", 0, 0,
     glue_C1CB14_step, 0xC1CCBC},
    {0xC1CB26, glue_C1CB26, "visit_alternate_scene_placements", 0, 0,
     glue_C1CB26_step, 0xC1CCBC, 0, 0xC1CB14},
    /* template_placements.c: complete static-band/cache refresh */
    {0xC1D10C, glue_C1D10C, "refresh_template_placements", 0, 0,
     glue_C1D10C_step, 0xC1E328},
    /* placement_order.c: complete cache classification and partition */
    {0xC1E540, glue_C1E540, "order_placement_cache", 0, 0,
     glue_C1E540_step, 0xC1EBB0, 0, 0xC1E53C},
    /* control_records.c: workspace selector variants, complete shared tails */
    {0xC1EBB0, glue_C1EBB0, "read_workspace_record_fields", 0, 0,
     glue_C1EBB0_step, 0xC1EBE0},
    {0xC1EC84, glue_C1EC84, "add_workspace_cell_steps", 0, 0,
     glue_C1EC84_step, 0xC1ECD4},
    /* grid_projection_packet.c */
    {0xC279D0, glue_C279D0, "draw_grid_projection_packet", 0, 0,
     glue_C279D0_step, 0xC27D24},
    /* map_packet.c: shared normal/wide directory and projection walk */
    {0xC2AA9C, glue_C2AA9C, "run_map_packet_depth_stage", 0, 0,
     glue_C2AA9C_step, 0xC2AB34},
    {0xC2AB34, glue_C2AB34, "run_wide_map_packet_pass", 0, 0,
     glue_C2AB34_step, 0xC2AFFA},
    {0xC2AB5A, glue_C2AB5A, "run_normal_map_packet_pass", 0, 0,
     glue_C2AB5A_step, 0xC2AFFA},
    /* record_region_probe.c */
    {0xC2B05A, glue_C2B05A, "probe_record_regions", 0, 0,
     glue_C2B05A_step, 0xC2B3B4, 0, 0xC2B042},
    /* postflight tuple and fixed-point variants */
    {0xC3129A, glue_C3129A, "draw_postflight_tuple_variant", 0, 0,
     glue_C3129A_step, 0xC318F6, 0, 0xC31224},
    {0xC31312, glue_C31312, "draw_postflight_fixed_variant", 0, 0,
     glue_C31312_step, 0xC318F6, 0, 0xC31224},
    {0xC31226, glue_C31226, "dispatch_postflight_renderer", 0, 0,
     glue_C31226_step, 0xC318F6, 0, 0xC31224},
    /* render_polygon.c */
    {0xC30466, glue_C30466, "composite_polygon_plane", 0, 0, glue_C30466_step, 0xC304FA},
    {0xC304B2, glue_C304B2, "clear_polygon_mask", 0, 0, glue_C304B2_complete_step, 0xC304FA, 0, 0, glue_C304B2_owns},
    {0xC305AA, glue_C305AA, "draw_polygon_edge", 0, 0, glue_C305AA_step, 0xC306B4},
    /* render_line.c */
    {0xC2FA7E, glue_C2FA7E, "draw_line", 0, 0, glue_C2FA7E_complete_step, 0xC2FD22, 0, 0xC2FA70, glue_C2FA7E_owns},
    /* fixed_math.c */
    {0xC2E6DA, glue_C2E6DA, "sin_cos", 0, 0, glue_C2E6DA_step, 0xC2E750},
    /* audio.c */
    {0xC501E0, glue_C501E0, "set_voice_output", 0, 0, glue_C501E0_step, 0xC50212},
    {0xC24FE8, glue_C24FE8, "fade_master_volume", 0, 0, glue_C24FE8_step, 0xC2502E, 0, 0xC24FE6},
    /* text.c */
    {0xC330FE, glue_C330FE, "plot_glyph8", 0, 0, glue_C330FE_complete_step, 0xC3316A, 0, 0, glue_C330FE_owns},
    {0xC32806, glue_C32806, "plot_glyph3", 0, 0, glue_C32806_complete_step, 0xC328A6, 0, 0, glue_C32806_owns},
    /* numbers.c, input.c, render_page.c */
    {0xC25A08, glue_C25A08, "pack_display_value", 0, 0, glue_C25A08_step, 0xC25A3E},
    {0xC1715C, glue_C1715C, "read_mouse_buttons", 0, 0, glue_C1715C_step, 0xC1718E},
    {0xC2F558, glue_C2F558, "select_draw_page", 0, 0, glue_C2F558_step, 0xC2F582},
    /* notify.c */
    {0xC11B44, glue_C11B44, "tick_notification_cadence", 0, 0, glue_C11B44_step, 0xC11BB0},
    /* fixed_math.c */
    {0xC15138, glue_C15138, "attenuate_control_record_offset", 0, 0, glue_C15138_step, 0xC1518C, 0, 0, glue_C15138_owns},
    /* render_span.c */
    {0xC310E2, glue_C310E2, "bound_span", 0, 0, glue_C310E2_complete_step, 0xC31120, 0, 0, glue_C310E2_owns},
    /* control_records.c */
    {0xC1EBC0, glue_C1EBC0, "read_record_fields", 0, 0, glue_C1EBC0_step, 0xC1EBE0},
    {0xC230B0, glue_C230B0, "release_lost_selection", 0, 0, glue_C230B0_step, 0xC230E8},
    {0xC2DE96, glue_C2DE96, "settle_record", 50},
    /* screen_frame.c */
    {0xC0DAA0, glue_C0DAA0, "append_mirrored_points", 0, 0, glue_C0DAA0_step, 0xC0DAEE, 0, 0xC0D74A},
    {0xC0DAD0, glue_C0DAD0, "append_point", 0, 0, glue_C0DAD0_step, 0xC0DAEE, 0, 0xC0D74A},
    {0xC0DAD4, glue_C0DAD4, "append_point", 0, 0, glue_C0DAD4_step, 0xC0DAEE, 0, 0xC0D74A},
    {0xC0DADC, glue_C0DADC, "append_point", 0, 0, glue_C0DADC_step, 0xC0DAEE, 0, 0xC0D74A},
    {0xC0DAE6, glue_C0DAE6, "append_point", 0, 0, glue_C0DAE6_step, 0xC0DAEE, 0, 0xC0D74A},
    /* stages.c */
    {0xC25B1C, glue_empty_stage, "empty_stage", 16},
    {0xC25B1E, glue_empty_stage, "empty_stage", 16},
    {0xC31F4A, glue_empty_stage, "empty_stage", 16},
    {0xC25864, glue_C25864, "reset_list", 0, 0, glue_C25864_step, 0xC25876},
    {0xC25482, glue_C25482, "tick_timer", 30},
    {0xC08394, glue_C08394, "set_event_bit_and_clear_command_word_bit", 48},
    {0xC090C2, glue_C090C2, "clear_scene_startup_state", 0, 0, glue_C090C2_step, 0xC090F2},
    {0xC090F2, glue_C090F2, "enable_scene_record_updates", 0, 0, glue_C090F2_step, 0xC0910C},
    {0xC1B602, glue_C1B602, "reset_throttle_input_state", 42},
    {0xC0833E, glue_C0833E, "dispatch_space_command_effect", 110},
    {0xC133B2, glue_C133B2, "record_6e_step", 0, 0, glue_C133B2_complete_step, 0xC1342A, 0, 0xC133B2, glue_C133B2_owns},
    {0xC118A0, glue_C118A0, "queue_postflight_failure_message", 0, 0, glue_C118A0_step, 0xC118E6, 0, 0, glue_C118A0_owns},
    {0xC083E2, glue_C083E2, "begin_main_loop_mission_reset", 0, 0, glue_C083E2_step, 0xC08488, 0, 0, glue_C083E2_owns},
    {0xC25A00, glue_C25A00, "add_repeated_nibble_weight", 150},
    {0xC30AE2, glue_C30AE2, "draw_stores_icon_stream", 500},
    {0xC30A00, glue_C30A00, "draw_stores_icons", 900},
    {0xC17B96, glue_C17B96, "start_menu_sound_pair", 0, 0, glue_C17B96_step, 0xC17C2A, 0, 0, glue_C17B96_owns},
    {0xC2F1C0, glue_C2F1C0, "draw_filled_circle", 0, 0, glue_C2F1C0_step, 0xC2F482, 0, 0xC2F1B8},
    {0xC2EC90, glue_C2EC90, "project_and_plot_point", 0, 0, glue_C2EC90_complete_step, 0xC2ED6C, 0, 0xC2EC70, glue_C2EC90_owns},
    {0xC2EC94, glue_C2EC94, "project_and_plot_point", 0, 0, glue_C2EC94_complete_step, 0xC2ED6C, 1, 0xC2EC70, glue_C2EC94_owns},
    {0xC2EC9C, glue_C2EC9C, "project_and_plot_point", 0, 0, glue_C2EC9C_complete_step, 0xC2ED6C, 1, 0xC2EC70, glue_C2EC9C_owns},
    {0xC2ECA4, glue_C2ECA4, "project_and_plot_point", 0, 0, glue_C2ECA4_complete_step, 0xC2ED6C, 1, 0xC2EC70, glue_C2ECA4_owns},
    {0xC2ECA8, glue_C2ECA8, "project_and_plot_point", 0, 0, glue_C2ECA8_step, 0xC2ED6C, 1, 0xC2EC70, glue_C2ECA8_owns},
    {0xC32A44, glue_C32A44, "draw_scene_numeric_label", 0, 0, glue_C32A44_step, 0xC32BD2, 1, 0, glue_C32A44_owns},
    {0xC32AC8, glue_C32AC8, "draw_packed_numeric_readout", 0, 0, glue_C32AC8_step, 0xC32BD2, 1, 0, glue_C32AC8_owns},
    {0xC33F70, glue_C33F70, "draw_speed_readout_tick", 0, 0, glue_C33F70_step, 0xC33FB2, 1, 0, glue_C33F70_owns},
    {0xC33F8A, glue_C33F8A, "draw_altitude_readout_tick", 0, 0, glue_C33F8A_step, 0xC33FB2, 1, 0, glue_C33F8A_owns},
    {0xC33FB4, glue_C33FB4, "draw_bounded_readout_sweep", 0, 0, glue_C33FB4_step, 0xC34066, 1, 0xC33FB2, glue_C33FB4_owns},
    {0xC1FE24, glue_C1FE24, "draw_display_stream_point", 0, 0, glue_C1FE24_complete_step, 0xC1FE46, 1, 0, glue_C1FE24_owns},
    {0xC1FE46, glue_C1FE46, "draw_display_stream_point", 0, 0, glue_C1FE46_complete_step, 0xC1FE68, 1, 0, glue_C1FE46_owns},
    {0xC0DAEE, glue_C0DAEE, "draw_fixed_matrix_mark", 0, 0, glue_C0DAEE_complete_step, 0xC0DB42, 1, 0, glue_C0DAEE_owns},
    {0xC17F8C, glue_C17F8C, "start_sound_6", 1100},
    {0xC18108, glue_C18108, "start_sound_12", 1050},
    {0xC1B906, glue_C1B906, "start_view_mode_zero", 0, 0, glue_C1B906_step, 0xC1C2B8},
    {0xC0CFFA, glue_C0CFFA, "draw_scaled_view_circle", 0, 0, glue_C0CFFA_complete_step, 0xC0D048, 1, 0, glue_C0CFFA_owns},
    {0xC0CF98, glue_C0CF98, "draw_scaled_stream_circle", 0, 0, glue_C0CF98_complete_step, 0xC0CFB6, 1, 0, glue_C0CF98_owns},
    {0xC13176, glue_C13176, "dispatch_event_sound", 0, 0, glue_C13176_complete_step, 0xC131BE, 0, 0xC13176, glue_C13176_owns},
    {0xC12098, glue_C12098, "update_view_controls", 0, 0, glue_C12098_step, 0xC12242},
    {0xC1B27E, glue_C1B27E, "update_flight_input", 2400},
    {0xC3316E, glue_C3316E, "play_context_tone_4", 0, 0, glue_C3316E_step, 0xC331CE},
    {0xC244E2, glue_C244E2, "classify_selected_record_range", 0, 0, glue_C244E2_step, 0xC2467E},
    {0xC1342C, glue_C1342C, "update_matrix_side_record", 3500},
    {0xC2DD4E, glue_C2DD4E, "adjust_matrix_record_depth", 1300},
    /* render_line.c, render_state.c */
    {0xC2F490, glue_C2F490, "reset_line_style", 0, 0, glue_C2F490_step, 0xC2F49C},
    {0xC2F596, glue_C2F596, "clear_renderer_blocks", 500},
    {0xC1D722, glue_C1D722, "fill_column", 0, 0, glue_C1D722_step, 0xC1D764},
    {0xC30F56, glue_C30F56, "start_blit", 90},
    /* audio.c, view.c, control_records.c */
    {0xC4FFB4, glue_C4FFB4, "clear_voice_interrupt", 0, 0, glue_C4FFB4_step, 0xC4FFCA},
    {0xC08324, glue_C08324, "set_zoom_maximum", 0, 0, glue_C08324_step, 0xC0833E},
    {0xC095C0, glue_C095C0, "reset_player_record", 0, 0, glue_C095C0_step, 0xC09620},
    {0xC1C7F6, glue_C1C7F6, "classify_record_rate", 0, 0, glue_C1C7F6_step, 0xC1C85E},
    {0xC50212, glue_C50212, "step_voice_program", 0, 0, glue_C50212_step, 0xC5027C},
    {0xC4FFB0, glue_C4FFB0, "clear_voice_interrupt", 0, 0, glue_C4FFB0_step, 0xC4FFCA},
    {0xC13B5A, glue_C13B5A, "update_record_5a", 120},
    {0xC14876, glue_C14876, "ease_record_26", 100},
    /* Complete stream stores, numeric fields and bounded marker siblings. */
    {0xC308E2, glue_C308E2, "store_stream_cd", 0, 0, glue_C308E2_complete_step, 0xC308F4, 0, 0, glue_C308E2_owns},
    {0xC30904, glue_C30904, "store_stream_ad", 0, 0, glue_C30904_complete_step, 0xC30916, 0, 0, glue_C30904_owns},
    {0xC308D8, glue_C308D8, "submit_next_stream_cd", 0, 0, glue_C308D8_step, 0xC308F4, 1, 0, glue_C308D8_owns},
    {0xC308F4, glue_C308F4, "submit_table_stream_ad", 0, 0, glue_C308F4_step, 0xC30916, 1, 0, glue_C308F4_owns},
    {0xC30F46, glue_C30F46, "submit_other_stream_acd", 0, 0, glue_C30F46_step, 0xC30F70, 1, 0, glue_C30F46_owns},
    {0xC31B76, glue_C31B76, "draw_stream_numeric_fields", 0, 0, glue_C31B76_step, 0xC31C20, 1, 0xC31B74, glue_C31B76_owns},
    {0xC33AD6, glue_C33AD6, "draw_bounded_stream_marker", 0, 0, glue_C33AD6_step, 0xC33B06, 1, 0, glue_C33AD6_owns},
    {0xC33B06, glue_C33B06, "draw_bounded_stream_marker", 0, 0, glue_C33B06_step, 0xC33B36, 1, 0, glue_C33B06_owns},
    {0xC2DEA2, glue_C2DEA2, "mark_record_pending", 50},
    {0xC28F16, glue_C28F16, "set_record_view", 0, 0, glue_C28F16_step, 0xC28F2C},
    {0xC0FA4C, glue_C0FA4C, "await_viewport_match", 90},
    {0xC0FA80, glue_C0FA80, "complete_post_input", 80},
    {0xC1FE20, glue_C1FE20, "zero_result", 20},
    {0xC21960, glue_C21960, "zero_result", 24},
    {0xC25980, glue_C25980, "divide_rounded", 0, 0, glue_C25980_step, 0xC259C2},
    {0xC2E370, glue_C2E370, "y_rotation_matrix", 0, 0, glue_C2E370_step, 0xC2E38E},
    /* batch 8: depth sort, records, compass, cockpit, context stage */
    {0xC1E4A6, glue_C1E4A6, "sort_by_depth", 0, 0, glue_C1E4A6_step, 0xC1E504},
    {0xC1E328, glue_C1E328, "sort_display_list", 0, 0, glue_C1E328_step, 0xC1E504},
    {0xC1CA82, glue_C1CA82, "flag_all_records", 0, 0, glue_C1CA82_step, 0xC1CB14},
    {0xC1EC3A, glue_C1EC3A, "read_record_pair", 0, 0, glue_C1EC3A_step, 0xC1EC84},
    {0xC2DAF2, glue_C2DAF2, "update_view_matrix", 0, 0, glue_C2DAF2_step, 0xC2DB18},
    {0xC310AA, glue_C310AA, "update_compass", 300},
    {0xC082B8, glue_C082B8, "request_cockpit_redraw", 0, 0, glue_C082B8_step, 0xC08324},
    {0xC082B0, glue_C082B0, "finish_scene_setup", 0, 0, glue_C082B0_step, 0xC08324},
    {0xC10C08, glue_C10C08, "start_context_stage", 0, 0, glue_C10C08_step, 0xC10C68, 0, 0, glue_C10C08_owns},
    {0xC11B0E, glue_C11B0E, "clear_long_table", 0, 0, glue_C11B0E_step, 0xC11B42},
    /* batch 9: hex text, decay, nudge, random, voices, readout, mission, view pan */
    {0xC0F56A, glue_C0F56A, "format_hex", 0, 0, glue_C0F56A_step, 0xC0F5F8, 0, 0, glue_C0F56A_owns},
    {0xC13A2A, glue_C13A2A, "decay_toward_zero", 120},
    {0xC13CDE, glue_C13CDE, "nudge_outside_dead_zone", 120},
    {0xC13396, glue_C13396, "five_eighths", 0, 0, glue_C13396_complete_step, 0xC133B2, 0, 0xC13396, glue_C13396_owns},
    {0xC50AB4, glue_C50AB4, "random_bit", 0, 0, glue_C50AB4_step, 0xC50B02},
    {0xC17B08, glue_C17B08, "free_voice", 0, 0, glue_C17B08_step, 0xC17B2C},
    {0xC2548A, glue_C2548A, "update_readout", 0, 0, glue_C2548A_step, 0xC254E8, 0, 0, glue_C2548A_owns},
    {0xC0840E, glue_C0840E, "reset_mission_objects", 0, 0, glue_C0840E_step, 0xC08488},
    {0xC258C8, glue_C258C8, "pan_view_from_keys", 0, 0, glue_C258C8_step, 0xC25980},
    /* batch 10: decay, messages, lookups, cell steps, 2.8 matrix, cached display value */
    {0xC148A2, glue_C148A2, "decay_outside_limit", 130},
    {0xC11312, glue_C11312, "reset_message_sequence", 0, 0, glue_C11312_step, 0xC1134E},
    {0xC287DA, glue_C287DA, "mode_offset", 0, 0, glue_C287DA_step, 0xC28800},
    {0xC1FEF2, glue_C1FEF2, "skip_stream_records", 60},
    {0xC1ECFC, glue_C1ECFC, "cell_step", 0, 0, glue_C1ECFC_step, 0xC1ED2A},
    {0xC1ECD4, glue_C1ECD4, "cell_step", 0, 0, glue_C1ECD4_step, 0xC1ECFC},
    {0xC2E346, glue_C2E346, "y_rotation_matrix8", 0, 0, glue_C2E346_step, 0xC2E370},
    {0xC31C20, glue_C31C20, "display_value_to_draw", 0, 0, glue_C31C20_complete_step, 0xC31C5E, 1, 0, glue_C31C20_owns},
    /* batch 11: player setup, steering, random bits, channel stop, cockpit script */
    {0xC09620, glue_C09620, "prepare_player_record", 0, 0, glue_C09620_step, 0xC096AA},
    {0xC13BA0, glue_C13BA0, "steer_record_56", 220},
    {0xC13C64, glue_C13C64, "steer_record_5a", 240},
    {0xC50B02, glue_C50B02, "random_bits", 0, 0, glue_C50B02_step, 0xC50B36},
    {0xC180FC, glue_C180FC, "stop_channel_2", 0, 0, glue_C180FC_step, 0xC18108},
    {0xC1EC96, glue_C1EC96, "cell_step", 0, 0, glue_C1EC96_step, 0xC1ECD4},
    {0xC21916, glue_C21916, "skip_for_type_3_to_6", 70},
    {0xC21966, glue_C21966, "skip_counted_entries", 90},
    {0xC2198C, glue_C2198C, "skip_for_low_class", 70},
    {0xC218C8, glue_C218C8, "skip_to_type_block", 110},
    {0xC207FE, glue_C207FE, "viewed_record_flagged", 70},
    /* batch 12: view octant, paired records, attitude term */
    {0xC254E8, glue_C254E8, "update_view_octant", 0, 0, glue_C254E8_step, 0xC2554E},
    {0xC231A2, glue_C231A2, "paired_record_ready", 0, 0, glue_C231A2_step, 0xC23228},
    {0xC148E2, glue_C148E2, "attitude_term", 450},
    /* batch 13: decimal format, cockpit slide, position history */
    {0xC3267A, glue_C3267A, "format_decimal", 500},
    {0xC2559A, glue_C2559A, "step_cockpit_slide", 0, 0, glue_C2559A_step, 0xC2564E, 0, 0xC25592},
    {0xC2651E, glue_C2651E, "record_position_history", 0, 0, glue_C2651E_complete_step, 0xC265DC, 0, 0xC2651C, glue_C2651E_owns},
    /* batch 14: paired sin_cos, print_number, square_root */
    {0xC2E5F6, glue_C2E5F6, "sin_cos", 0, 0, glue_C2E5F6_step, 0xC2E6DA},
    {0xC24F76, glue_C24F76, "print_number", 0, 0, glue_C24F76_step, 0xC24FA4},
    {0xC2564E, glue_C2564E, "square_root", 0, 0, glue_C2564E_step, 0xC25704},
    /* batch 15: cell occupancy, record position, record 76/78 */
    {0xC1D520, glue_C1D520, "collect_records_in_cell", 0, 0, glue_C1D520_step, 0xC1D722, 0, 0xC1D3F4},
    {0xC1D0B6, glue_C1D0B6, "accumulate_record_position", 0, 0,
     glue_C1D0B6_step, 0xC1D10C, 0, 0xC1D0A4},
    {0xC26428, glue_C26428, "update_record_76_78", 500},
    /* batch 16: small text */
    {0xC32794, glue_C32794, "draw_small_text", 0, 0, glue_C32794_complete_step, 0xC32806, 1, 0, glue_C32794_owns},
    {0xC32662, glue_C32662, "draw_small_text", 0, 0, glue_C32662_complete_step, 0xC32806, 1, 0, glue_C32662_owns},
    {0xC3271A, glue_C3271A, "format_small_hex", 0, 0, glue_C3271A_complete_step, 0xC32806, 1, 0, glue_C3271A_owns},
    {0xC32736, glue_C32736, "format_small_hex", 0, 0, glue_C32736_complete_step, 0xC32806, 1, 0, glue_C32736_owns},
    /* batch 17: BCD unpack, sorted search, record 56/66 with alert */
    {0xC259C2, glue_C259C2, "unpack_display_value", 900},
    {0xC1D4E4, glue_C1D4E4, "find_sorted_word", 0, 0, glue_C1D4E4_step, 0xC1D722, 0, 0xC1D3F4},
    {0xC13A8E, glue_C13A8E, "update_record_56_from_66", 200},
    /* batch 18: joystick */
    {0xC16F1C, glue_C16F1C, "read_joystick", 0, 0, glue_C16F1C_step, 0xC16FF4},
    /* batch 19: rotation matrices and row scaling */
    {0xC2E47A, glue_C2E47A, "rotation_matrix", 0, 0, glue_C2E47A_step, 0xC2E5AC},
    {0xC2E38E, glue_C2E38E, "two_angle_matrix", 0, 0, glue_C2E38E_step, 0xC2E3DE},
    {0xC2E5AC, glue_C2E5AC, "scale_matrix_rows", 0, 0, glue_C2E5AC_step, 0xC2E5F6},
    /* batch 19: three-angle matrix variants */
    {0xC2E3DE, glue_C2E3DE, "rotation_matrix8", 0, 0, glue_C2E3DE_step, 0xC2E47A},
    {0xC2E514, glue_C2E514, "alternate_rotation_matrix", 0, 0, glue_C2E514_step, 0xC2E5AC},
    /* batch 20: inverse orientation, stream skip, attitude flags, vertex tail, divide, interrupt server */
    {0xC2D970, glue_C2D970, "inverse_orientation_matrix", 0, 0, glue_C2D970_step, 0xC2D99C},
    {0xC21940, glue_C21940, "skip_if_shown_record_flag", 90},
    {0xC122A2, glue_C122A2, "update_attitude_flags", 0, 0, glue_C122A2_step, 0xC123FA},
    {0xC0D384, glue_C0D384, "derive_vertex_tail", 1300},
    {0xC52EC8, glue_C52EC8, "long_divide", 0, 0, glue_C52EC8_complete_step, 0xC52F0A, 0, 0xC52EC8, glue_C52EC8_owns},
    {0xC06132, glue_C06132, "count_interrupt", 0, 0, glue_C06132_step, 0xC06178},
    /* batch 20: play_sound */
    {0xC17B2C, glue_C17B2C, "play_sound", 0, 0, glue_C17B2C_step, 0xC17B96, 0, 0xC17B08},
    /* batch 21: side-plane clips, view transform */
    {0xC2EA5A, glue_C2EA5A, "segment_crossing", 0, 0, glue_C2EA5A_complete_step, 0xC2EC68, 0, 0xC2EA5A, glue_C2EA5A_owns},
    {0xC2EAD0, glue_C2EAD0, "segment_crossing", 0, 0, glue_C2EAD0_complete_step, 0xC2EC68, 0, 0xC2EAD0, glue_C2EAD0_owns},
    {0xC2F0C6, glue_C2F0C6, "segment_crossing", 0, 0, glue_C2F0C6_complete_step, 0xC2F1B8, 0, 0xC2F0C6, glue_C2F0C6_owns},
    {0xC2F0F4, glue_C2F0F4, "segment_crossing", 0, 0, glue_C2F0F4_complete_step, 0xC2F1B8, 0, 0xC2F0F4, glue_C2F0F4_owns},
    {0xC1F2EE, glue_C1F2EE, "view_transform", 560},
    /* batch 22: magnitude */
    {0xC1D974, glue_C1D974, "magnitude3", 0, 0, glue_C1D974_step, 0xC1D9D8},
    /* batch 23: y-plane clips, sound routines, record orientation */
    {0xC2EB4C, glue_C2EB4C, "segment_crossing", 0, 0, glue_C2EB4C_complete_step, 0xC2EC68, 0, 0xC2EB4C, glue_C2EB4C_owns},
    {0xC2EBC2, glue_C2EBC2, "segment_crossing", 0, 0, glue_C2EBC2_complete_step, 0xC2EC68, 0, 0xC2EBC2, glue_C2EBC2_owns},
    {0xC2F156, glue_C2F156, "segment_crossing", 0, 0, glue_C2F156_complete_step, 0xC2F1B8, 0, 0xC2F156, glue_C2F156_owns},
    {0xC17CF6, glue_C17CF6, "play_engine", 0, 0, glue_C17CF6_step, 0xC17D6E},
    {0xC17DAA, glue_C17DAA, "slide_engine", 0, 0, glue_C17DAA_step, 0xC17E4A},
    {0xC17E4A, glue_C17E4A, "play_noise", 0, 0, glue_C17E4A_step, 0xC17EF2},
    {0xC17EF2, glue_C17EF2, "play_programmed_sound", 0, 0, glue_C17EF2_step, 0xC17F8C},
    {0xC18096, glue_C18096, "play_scripted_sound", 0, 0, glue_C18096_step, 0xC180FC},
    {0xC2D954, glue_C2D954, "set_record_orientation", 0, 0, glue_C2D954_step, 0xC2D99C},
    /* batch 24: local to world, shown vertices, normalize, slot scan */
    {0xC091E0, glue_C091E0, "local_to_world", 0, 0, glue_C091E0_step, 0xC0924A, 0, 0xC091A8},
    {0xC091CE, glue_C091CE, "local_to_world", 0, 0, glue_C091CE_step, 0xC0924A, 0, 0xC091A8},
    {0xC091A8, glue_C091A8, "local_to_world", 0, 0, glue_C091A8_step, 0xC0924A},
    {0xC0D334, glue_C0D334, "derive_shown_vertices", 2800},
    {0xC25754, glue_C25754, "normalise_main_loop_control_vector", 0, 0, glue_C25754_step, 0xC257DC, 0, 0, glue_C25754_owns},
    {0xC265E8, glue_C265E8, "flagged_slot_in_range", 3500},
    /* batch 25: main engine, tone, edge vertices, buffers, stage blit, grid position, list point */
    {0xC17C62, glue_C17C62, "play_main_engine", 0, 0, glue_C17C62_step, 0xC17CF6},
    {0xC17D6E, glue_C17D6E, "slide_main_engine", 0, 0, glue_C17D6E_step, 0xC17DAA},
    {0xC3316A, glue_C3316A, "play_tone", 0, 0, glue_C3316A_step, 0xC331CE},
    {0xC219AE, glue_C219AE, "derive_edge_vertices", 0, 0, glue_C219AE_complete_step, 0xC21A20, 1, 0, glue_C219AE_owns},
    {0xC2FD22, glue_C2FD22, "clear_render_buffers", 0, 0, glue_C2FD22_step, 0xC2FD8C},
    {0xC3040C, glue_C3040C, "blit_mask_between_planes", 0, 0, glue_C3040C_step, 0xC30466, 0, 0xC301F0},
    {0xC1EBE0, glue_C1EBE0, "grid_relative_position", 0, 0, glue_C1EBE0_step, 0xC1EC3A},
    {0xC25876, glue_C25876, "append_list_point", 0, 0,
     glue_C25876_step, 0xC258C8},
    /* batch 26: tones, page plane tops */
    {0xC33180, glue_C33180, "play_tone_2", 0, 0, glue_C33180_step, 0xC331CE},
    {0xC3318E, glue_C3318E, "play_status_tone", 0, 0, glue_C3318E_step, 0xC331CE, 0, 0xC33180},
    {0xC33186, glue_C33186, "play_status_tone_outside_context", 0, 0, glue_C33186_step, 0xC331CE, 0, 0xC33180},
    {0xC2F582, glue_C2F582, "clear_page_plane_tops", 600},
    /* batch 27: target point, record range, lane blit */
    {0xC1C2C8, glue_C1C2C8, "update_target_point", 0, 0, glue_C1C2C8_step, 0xC1C40C},
    {0xC24568, glue_C24568, "classify_main_loop_record_range", 0, 0, glue_C24568_step, 0xC2467E, 0, 0xC24566, glue_C24568_owns},
    {0xC304FA, glue_C304FA, "blit_lane", 0, 0, glue_C304FA_complete_step, 0xC305AA, 1, 0, glue_C304FA_owns},
    /* batch 28: post-input expiry, projection seed, condition tables */
    {0xC10D8A, glue_C10D8A, "check_post_input_expiry", 0, 0, glue_C10D8A_step, 0xC10DAE, 0, 0, glue_C10D8A_owns},
    {0xC1C54E, glue_C1C54E, "seed_projection", 0, 0, glue_C1C54E_step, 0xC1C63E},
    {0xC09AB8, glue_C09AB8, "condition_table_matches", 0, 0, glue_C09AB8_step, 0xC09B48},
    /* batch 29: component bound, repeated sum, view key */
    {0xC1FC42, glue_C1FC42, "component_beyond_bound", 0, 0, glue_C1FC42_step, 0xC1FCDE},
    {0xC1BA86, glue_C1BA86, "queue_view_key", 0, 0, glue_C1BA86_step, 0xC1C2B8},
    /* batch 30: audio interrupt, date line */
    {0xC50158, glue_C50158, "update_voices", 0, 0, glue_C50158_step, 0xC501E0},
    {0xC24E2C, glue_C24E2C, "format_date_line", 0, 0, glue_C24E2C_step, 0xC24E8A},
    /* batch 31: condition flags, lost selection */
    {0xC09A78, glue_C09A78, "update_condition_a", 0, 0, glue_C09A78_step, 0xC09B48},
    {0xC09A98, glue_C09A98, "update_condition_b", 0, 0, glue_C09A98_step, 0xC09B48},
    {0xC12242, glue_C12242, "drop_lost_selection", 700},
    /* batch 32: fault hook, level lists */
    {0xC06C02, glue_C06C02, "fault_hook", 0, 0, glue_C06C02_step, 0xC06C04},
    {0xC1D5D8, glue_C1D5D8, "file_records_by_level", 0, 0, glue_C1D5D8_step, 0xC1D722, 0, 0xC1D3F4},
    /* batch 33: pixel plots */
    {0xC2F5F4, glue_C2F5F4, "plot_pixel", 0, 0, glue_C2F5F4_complete_step, 0xC2F8D0, 0, 0, glue_C2F5F4_owns},
    {0xC2F60A, glue_C2F60A, "plot_pixel_pair", 0, 0, glue_C2F60A_complete_step, 0xC2F8D0, 0, 0, glue_C2F60A_owns},
    /* batch 34: polygon preparation */
    {0xC301F6, glue_C301F6, "prepare_polygon", 0, 0, glue_C301F6_complete_step, 0xC3040C, 0, 0, glue_C301F6_owns},
    /* batch 35: polygon submission */
    {0xC2FF48, glue_C2FF48, "draw_polygon", 0, 0, glue_C2FF48_step, 0xC30038, 0, 0xC2FF46},
    /* batch 36: clip stages */
    {0xC247C0, glue_C247C0, "clip_stage", 0, 0, glue_C247C0_step, 0xC24DA8, 0, 0xC24688},
    {0xC248B2, glue_C248B2, "clip_stage", 0, 0, glue_C248B2_step, 0xC24DA8, 0, 0xC24688},
    {0xC24996, glue_C24996, "clip_stage", 0, 0, glue_C24996_step, 0xC24DA8, 0, 0xC24688},
    /* batch 37: outer polygon clipper */
    {0xC2469E, glue_C2469E, "clip_and_draw_polygon", 0, 0, glue_C2469E_step, 0xC24DA8, 0, 0xC24688},
    {0xC246A0, glue_C246A0, "clip_and_draw_polygon", 0, 0, glue_C246A0_step, 0xC24DA8, 0, 0xC24688},
    /* batch 38: faces and view marks */
    {0xC09952, glue_C09952, "draw_indexed_face", 0, 0, glue_C09952_complete_step, 0xC099AA, 1, 0, glue_C09952_owns},
    {0xC099F6, glue_C099F6, "draw_outlined_face", 0, 0, glue_C099F6_complete_step, 0xC09A78, 1, 0, glue_C099F6_owns},
    {0xC332FE, glue_C332FE, "draw_view_marker", 0, 0, glue_C332FE_complete_step, 0xC33370, 1, 0xC332FC, glue_C332FE_owns},
    {0xC30918, glue_C30918, "draw_gauge_bar", 0, 0, glue_C30918_step, 0xC309A2, 0, 0xC30916},
    /* batch 39: cockpit messages */
    {0xC11BFC, glue_C11BFC, "update_message", 3000},
    /* batch 41: plane-side test */
    {0xC27456, glue_C27456, "faces_all_behind", 0, 0, glue_C27456_complete_step, 0xC27504, 0, 0xC27450, glue_C27456_owns},
    /* batch 42: direction tracking */
    {0xC123FA, glue_C123FA, "track_direction", 0, 0, glue_C123FA_step, 0xC12950},
    /* batch 43: normalize register entry */
    {0xC2574A, glue_C2574A, "normalize_vector", 0, 0, glue_C2574A_step, 0xC257DC},
    /* batch 44: view aiming */
    {0xC2D9BA, glue_C2D9BA, "aim_view", 0, 0, glue_C2D9BA_step, 0xC2DAF2, 0, 0xC2D9B0},
    /* batch 45: face toward eye */
    {0xC1FB8C, glue_C1FB8C, "face_toward_eye", 0, 0, glue_C1FB8C_step, 0xC1FC3A},
    /* batch 46: target distance */
    {0xC1D91A, glue_C1D91A, "target_distance", 0, 0, glue_C1D91A_step, 0xC1D9D8, 0, 0xC1D90A},
    /* batch 47: post-input stages, stick and throttle, flight recorder */
    {0xC0F946, glue_C0F946, "await_viewport_then_ready", 0, 0, glue_C0F946_step, 0xC0F974, 0, 0, glue_C0F946_owns},
    {0xC0F974, glue_C0F974, "mark_viewport_ready", 0, 0, glue_C0F974_step, 0xC0F992, 0, 0, glue_C0F974_owns},
    {0xC0FB70, glue_C0FB70, "choose_after_countdown", 0, 0, glue_C0FB70_step, 0xC0FBB6, 0, 0, glue_C0FB70_owns},
    {0xC0FBB6, glue_C0FBB6, "leave_on_key_or_message", 0, 0, glue_C0FBB6_step, 0xC0FBE0, 0, 0, glue_C0FBB6_owns},
    {0xC101FC, glue_C101FC, "reset_viewport_after_countdown", 0, 0, glue_C101FC_step, 0xC10228, 0, 0, glue_C101FC_owns},
    {0xC10228, glue_C10228, "enter_mode_four_when_ready", 0, 0, glue_C10228_step, 0xC10272, 0, 0, glue_C10228_owns},
    {0xC1072E, glue_C1072E, "queue_message_four", 0, 0, glue_C1072E_step, 0xC1075A, 0, 0, glue_C1072E_owns},
    {0xC1075A, glue_C1075A, "start_outcome_countdown", 0, 0, glue_C1075A_step, 0xC1078A, 0, 0, glue_C1075A_owns},
    {0xC11872, glue_C11872, "expire_to_fire_state", 0, 0, glue_C11872_step, 0xC118A0, 0, 0, glue_C11872_owns},
    {0xC118E6, glue_C118E6, "end_on_message", 0, 0, glue_C118E6_step, 0xC118FC, 0, 0, glue_C118E6_owns},
    {0xC11958, glue_C11958, "follow_message_or_phase", 0, 0, glue_C11958_step, 0xC119D4, 0, 0, glue_C11958_owns},
    {0xC119D4, glue_C119D4, "restart_after_countdown", 0, 0, glue_C119D4_step, 0xC11A26, 0, 0, glue_C119D4_owns},
    {0xC0A2F0, glue_C0A2F0, "begin_phase_three", 0, 0, glue_C0A2F0_step, 0xC0A3C6, 0, 0xC0A2EE, glue_C0A2F0_owns},
    {0xC1B4D0, glue_C1B4D0, "set_throttle_input", 60},
    {0xC1B4D4, glue_C1B4D4, "set_throttle_input", 60},
    {0xC1B4D8, glue_C1B4D8, "release_throttle_keys", 70},
    {0xC1B4DE, glue_C1B4DE, "set_throttle_input", 60},
    {0xC1B50C, glue_C1B50C, "set_stick_y", 80},
    {0xC1B510, glue_C1B510, "set_stick_y", 80},
    {0xC1B514, glue_C1B514, "set_stick_y", 80},
    {0xC1B558, glue_C1B558, "set_stick_x", 80},
    {0xC1B55C, glue_C1B55C, "set_stick_x", 80},
    {0xC1B560, glue_C1B560, "set_stick_x", 80},
    {0xC25A6A, glue_C25A6A, "record_flight_input", 250},
    {0xC33DA4, glue_C33DA4, "take_warning_events", 80},
    {0xC13C0A, glue_C13C0A, "ease_record_58", 400},
    {0xC21C4C, glue_C21C4C, "split_edge", 0, 0, glue_C21C4C_complete_step, 0xC21C86, 1, 0, glue_C21C4C_owns},
    {0xC1FED4, glue_C1FED4, "skip_word_for_mode_57", 50},
    {0xC345A0, glue_C345A0, "plot_ring", 0, 0, glue_C345A0_complete_step, 0xC347F2, 1, 0, glue_C345A0_owns},
    {0xC12950, glue_C12950, "update_control_record_readouts", 0, 0, glue_C12950_complete_step, 0xC13176, 0, 0xC12950, glue_C12950_owns},
    {0xC131BE, glue_C131BE, "scale_control_record_magnitude", 0, 0, glue_C131BE_complete_step, 0xC13396, 0, 0xC131BE, glue_C131BE_owns},
    {0xC2CB82, glue_C2CB82, "select_record_pitch_preserving_controls", 0, 0, glue_C2CB82_complete_step, 0xC2CBBC, 0, 0xC2CB82, glue_C2CB82_owns},
    {0xC2CBBC, glue_C2CBBC, "select_record_pitch_branch", 0, 0, glue_C2CBBC_complete_step, 0xC2CBBE, 0, 0xC2CB86, glue_C2CBBC_owns},
    /* batch 48: coloured face, stored-normal test, record steering, view rotation, edge split */
    {0xC099AA, glue_C099AA, "draw_coloured_face", 0, 0, glue_C099AA_complete_step, 0xC099F6, 1, 0, glue_C099AA_owns},
    {0xC1FB9C, glue_C1FB9C, "point_toward_eye", 0, 0, glue_C1FB9C_step, 0xC1FBD4},
    {0xC2CAA0, glue_C2CAA0, "select_record_neutral", 0, 0, glue_C2CAA0_complete_step, 0xC2CAB2, 0, 0xC2CAA0, glue_C2CAA0_owns},
    {0xC2CA92, glue_C2CA92, "select_record_roll", 0, 0, glue_C2CA92_complete_step, 0xC2CAB2, 0, 0xC2CA92, glue_C2CA92_owns},
    {0xC2CA26, glue_C2CA26, "select_record_turn", 0, 0, glue_C2CA26_complete_step, 0xC2CAB2, 0, 0xC2CA26, glue_C2CA26_owns},
    {0xC2CB86, glue_C2CB86, "select_record_pitch", 0, 0, glue_C2CB86_complete_step, 0xC2CBBC, 0, 0xC2CB86, glue_C2CB86_owns},
    {0xC2CE82, glue_C2CE82, "rotate_by_view_matrix", 0, 0, glue_C2CE82_complete_step, 0xC2CEBE, 0, 0xC2CE82, glue_C2CE82_owns},
    {0xC22C70, glue_C22C70, "return_render_hook", 0, 0, glue_C22C70_complete_step, 0xC22C72, 0, 0xC22C70, glue_C22C70_owns},
    {0xC20826, glue_C20826, "reject_edge_alignment", 0, 0, glue_C20826_complete_step, 0xC2082A, 0, 0xC20826, glue_C20826_owns},
    {0xC2058E, glue_C2058E, "reject_split_square", 0, 0, glue_C2058E_complete_step, 0xC20592, 0, 0xC2058E, glue_C2058E_owns},
    {0xC203CC, glue_C203CC, "reject_square_faces", 0, 0, glue_C203CC_complete_step, 0xC203D0, 0, 0xC203CC, glue_C203CC_owns},
    /* C200F6 restores two saved pointers before RTS. Its enclosing C20100
     * continuation already owns all three boundaries. A standalone stepped
     * call would mistake the saved A1 for a return address; retain the exact
     * whole C tail here (40 base cycles plus 16 observed MOVEM cycles). */
    {0xC200F6, glue_C200F6, "return_indexed_face_list", 40},
    {0xC2D3A4, glue_C2D3A4, "view_coordinate_distance", 0, 0, glue_C2D3A4_complete_step, 0xC2D3FC, 0, 0xC2D3A4, glue_C2D3A4_owns},
    {0xC2D082, glue_C2D082, "draw_view_layers", 0, 0, glue_C2D082_complete_step, 0xC2D16C, 0, 0xC2D080, glue_C2D082_owns},
    {0xC2CD94, glue_C2CD94, "draw_view_record_pairs", 0, 0, glue_C2CD94_complete_step, 0xC2CE5E, 0, 0xC2CD94, glue_C2CD94_owns},
    {0xC2CD28, glue_C2CD28, "draw_view_record_layers", 0, 0, glue_C2CD28_complete_step, 0xC2D16C, 0, 0xC2CCBA, glue_C2CD28_owns},
    {0xC2CCA0, glue_C2CCA0, "test_view_record", 0, 0, glue_C2CCA0_complete_step, 0xC2CD28, 0, 0xC2CCA0, glue_C2CCA0_owns},
    {0xC21C2E, glue_C21C2E, "split_record_and_stream_edges", 0, 0, glue_C21C2E_complete_step, 0xC21C86, 1, 0, glue_C21C2E_owns},
    /* batch 49: projected segment, top-plane crossing, in-sight flag, edge alignment */
    {0xC2ED70, glue_C2ED70, "segment_projected", 0, 0, glue_C2ED70_complete_step, 0xC2EE44, 0, 0xC2ED6C, glue_C2ED70_owns},
    {0xC2F128, glue_C2F128, "segment_crossing", 0, 0, glue_C2F128_complete_step, 0xC2F1B8, 0, 0xC2F128, glue_C2F128_owns},
    {0xC2436A, glue_C2436A, "update_main_loop_record_sight", 0, 0, glue_C2436A_step, 0xC243F2, 0, 0xC24368, glue_C2436A_owns},
    {0xC2084A, glue_C2084A, "edge_alignment", 0, 0, glue_C2084A_complete_step, 0xC208D4, 1, 0, glue_C2084A_owns},
    {0xC2082A, glue_C2082A, "edge_alignment_test", 0, 0, glue_C2082A_complete_step, 0xC208D4, 1, 0xC20826, glue_C2082A_owns},
    /* batch 50: symbol plot */
    {0xC348B2, glue_C348B2, "plot_symbol", 0, 0, glue_C348B2_complete_step, 0xC3494A, 1, 0xC348B0, glue_C348B2_owns},
    /* batch 51-52: clipped segment, ground points, voices, messages, observer, stages, long table, alert, start position, typed code */
    {0xC2EE4A, glue_C2EE4A, "segment_clipped", 0, 0, glue_C2EE4A_complete_step, 0xC2F0C6, 0, 0xC2EE44, glue_C2EE4A_owns},
    {0xC098C6, glue_C098C6, "transform_ground_points", 0, 0, glue_C098C6_complete_step, 0xC09952, 1, 0, glue_C098C6_owns},
    {0xC0F4A6, glue_C0F4A6, "free_all_voices", 0, 0, glue_C0F4A6_step, 0xC0F4D6},
    {0xC25704, glue_C25704, "post_message", 0, 0, glue_C25704_step, 0xC2574A},
    {0xC0915A, glue_C0915A, "set_observer_position", 0, 0, glue_C0915A_step, 0xC09192},
    {0xC11078, glue_C11078, "raise_event_after_countdown", 0, 0, glue_C11078_step, 0xC110A4, 0, 0, glue_C11078_owns},
    {0xC11ACC, glue_C11ACC, "load_long_table", 0, 0, glue_C11ACC_step, 0xC11B0E},
    {0xC1803C, glue_C1803C, "sound_chosen_record_alert", 0, 0, glue_C1803C_step, 0xC18096},
    {0xC10678, glue_C10678, "queue_mode_messages", 0, 0, glue_C10678_step, 0xC1072E, 0, 0, glue_C10678_owns},
    {0xC0910C, glue_C0910C, "start_position", 0, 0, glue_C0910C_step, 0xC09120},
    {0xC25246, glue_C25246, "check_typed_code", 300},
    /* batch 53: draw-stream commands */
    {0xC212B0, glue_C212B0, "draw_segment_pairs", 0, 0, glue_C212B0_complete_step, 0xC2131C, 1, 0, glue_C212B0_owns},
    {0xC2129C, glue_C2129C, "draw_segment_pairs_near", 0, 0, glue_C2129C_complete_step, 0xC2131C, 1, 0, glue_C2129C_owns},
    {0xC211DC, glue_C211DC, "draw_segment_run", 0, 0, glue_C211DC_complete_step, 0xC2122A, 1, 0, glue_C211DC_owns},
    {0xC2131C, glue_C2131C, "draw_offset_segments", 0, 0, glue_C2131C_complete_step, 0xC2139E, 1, 0, glue_C2131C_owns},
    {0xC20E4E, glue_C20E4E, "draw_parallelogram_face", 0, 0, glue_C20E4E_complete_step, 0xC20EC4, 1, 0, glue_C20E4E_owns},
    {0xC20E40, glue_C20E40, "draw_parallelogram_face_2", 0, 0, glue_C20E40_complete_step, 0xC20EC4, 1, 0, glue_C20E40_owns},
    {0xC21490, glue_C21490, "draw_parallelogram_face_near", 0, 0, glue_C21490_complete_step, 0xC21500, 1, 0, glue_C21490_owns},
    {0xC2139E, glue_C2139E, "draw_offset_face", 0, 0, glue_C2139E_complete_step, 0xC21412, 1, 0, glue_C2139E_owns},
    {0xC21412, glue_C21412, "draw_mixed_face", 0, 0, glue_C21412_complete_step, 0xC21490, 1, 0, glue_C21412_owns},
    {0xC20F10, glue_C20F10, "extend_parallelograms", 0, 0, glue_C20F10_complete_step, 0xC20F78, 1, 0, glue_C20F10_owns},
    {0xC20EC4, glue_C20EC4, "extend_parallelograms_scaled", 0, 0, glue_C20EC4_complete_step, 0xC20F78, 1, 0, glue_C20EC4_owns},
    /* batch 54: segment grids, block generators */
    {0xC20D68, glue_C20D68, "draw_segment_grid", 0, 0, glue_C20D68_complete_step, 0xC20E40, 1, 0, glue_C20D68_owns},
    {0xC20904, glue_C20904, "draw_segment_lattice", 0, 0, glue_C20904_complete_step, 0xC20A40, 1, 0, glue_C20904_owns},
    {0xC21A20, glue_C21A20, "offset_block_copies", 0, 0, glue_C21A20_complete_step, 0xC21B38, 1, 0, glue_C21A20_owns},
    {0xC217EA, glue_C217EA, "extend_block_scaled", 0, 0, glue_C217EA_complete_step, 0xC218C8, 1, 0, glue_C217EA_owns},
    /* batch 55: corner edges */
    {0xC2E758, glue_C2E758, "project_corner_edges", 0, 0, glue_C2E758_complete_step, 0xC2EA5A, 0, 0xC2E758, glue_C2E758_owns},
    /* batch 56: fixed-row line, text lines and digits */
    {0xC2FA78, glue_C2FA78, "draw_line_to_row", 0, 0, glue_C2FA78_complete_step, 0xC2FD22, 0, 0xC2FA70, glue_C2FA78_owns},
    {0xC32726, glue_C32726, "format_digits", 0, 0, glue_C32726_complete_step, 0xC32806, 1, 0, glue_C32726_owns},
    {0xC32AB4, glue_C32AB4, "draw_text_in_view", 0, 0, glue_C32AB4_complete_step, 0xC32BD2, 1, 0, glue_C32AB4_owns},
    {0xC32AA6, glue_C32AA6, "print_bcd_in_view", 0, 0, glue_C32AA6_complete_step, 0xC32BD2, 1, 0, glue_C32AA6_owns},
    {0xC32AA4, glue_C32AA4, "print_bcd_in_view", 0, 0, glue_C32AA4_complete_step, 0xC32BD2, 1, 0, glue_C32AA4_owns},
    /* batch 57-58: side face, quad list and strip, face grids and lattices */
    {0xC2159E, glue_C2159E, "draw_side_face", 0, 0, glue_C2159E_complete_step, 0xC2168A, 1, 0, glue_C2159E_owns},
    {0xC210E6, glue_C210E6, "draw_quad_strip", 0, 0, glue_C210E6_complete_step, 0xC211DC, 1, 0, glue_C210E6_owns},
    /* batch 59: cockpit readouts */
    {0xC2F5C0, glue_C2F5C0, "plot_pixel_in_view", 0, 0, glue_C2F5C0_complete_step, 0xC2F626, 0, 0, glue_C2F5C0_owns},
    {0xC2F5D4, glue_C2F5D4, "plot_pixel", 0, 0, glue_C2F5D4_complete_step, 0xC2F5F4, 0, 0, glue_C2F5D4_owns},
    {0xC31A64, glue_C31A64, "draw_record_class_digits", 0, 0, glue_C31A64_complete_step, 0xC32806, 1, 0, glue_C31A64_owns},
    {0xC31ACC, glue_C31ACC, "draw_record_scale_digits", 0, 0, glue_C31ACC_complete_step, 0xC32806, 1, 0xC31ACA, glue_C31ACC_owns},
    {0xC31F4C, glue_C31F4C, "draw_speed_readout", 0, 0, glue_C31F4C_complete_step, 0xC32806, 1, 0, glue_C31F4C_owns},
    {0xC3201A, glue_C3201A, "draw_altitude_readout", 0, 0, glue_C3201A_complete_step, 0xC32806, 1, 0, glue_C3201A_owns},
    {0xC3212A, glue_C3212A, "draw_record_72_readout", 0, 0, glue_C3212A_complete_step, 0xC32806, 1, 0, glue_C3212A_owns},
    {0xC32178, glue_C32178, "draw_record_2b_readout", 0, 0, glue_C32178_complete_step, 0xC32806, 1, 0, glue_C32178_owns},
    {0xC321D2, glue_C321D2, "draw_grid_z_readout", 0, 0, glue_C321D2_complete_step, 0xC32806, 1, 0, glue_C321D2_owns},
    {0xC32260, glue_C32260, "draw_grid_x_readout", 0, 0, glue_C32260_complete_step, 0xC32806, 1, 0, glue_C32260_owns},
    {0xC31EB6, glue_C31EB6, "draw_heading_readout", 0, 0, glue_C31EB6_complete_step, 0xC31F4A, 1, 0xC31EB4, glue_C31EB6_owns},
    {0xC31C60, glue_C31C60, "draw_weapon_readout", 0, 0, glue_C31C60_complete_step, 0xC31D16, 1, 0xC31C5E, glue_C31C60_owns},
    {0xC31D16, glue_C31D16, "draw_signed_readout", 0, 0, glue_C31D16_complete_step, 0xC31D64, 1, 0, glue_C31D16_owns},
    {0xC31E6C, glue_C31E6C, "draw_shoot_cue", 0, 0, glue_C31E6C_complete_step, 0xC31EB4, 1, 0, glue_C31E6C_owns},
    {0xC31D64, glue_C31D64, "draw_load_readout", 0, 0, glue_C31D64_complete_step, 0xC31E5E, 1, 0, glue_C31D64_owns},
    {0xC33F54, glue_C33F54, "draw_three_digits", 0, 0, glue_C33F54_complete_step, 0xC33FB2, 1, 0, glue_C33F54_owns},
    /* batch 59b: weapon status, threat lights */
    {0xC2F64E, glue_C2F64E, "plot_square", 0, 0, glue_C2F64E_complete_step, 0xC2F66E, 0, 0, glue_C2F64E_owns},
    {0xC2F63A, glue_C2F63A, "plot_square_in_view", 0, 0, glue_C2F63A_complete_step, 0xC2F66E, 0, 0xC2F622, glue_C2F63A_owns},
    {0xC328A8, glue_C328A8, "draw_weapon_status", 0, 0, glue_C328A8_complete_step, 0xC32A42, 1, 0xC32794, glue_C328A8_owns},
    {0xC3112A, glue_C3112A, "draw_bit_selected_points", 0, 0, glue_C3112A_complete_step, 0xC31224, 1, 0xC31128, glue_C3112A_owns},
    /* batch 60: cockpit bars and panel image */
    {0xC30CC4, glue_C30CC4, "fill_bar_words", 900},
    {0xC30B5C, glue_C30B5C, "draw_status_stream_display", 0, 0, glue_C30B5C_complete_step, 0xC30D22, 1, 0, glue_C30B5C_owns},
    {0xC30D34, glue_C30D34, "draw_record_stream_display", 0, 0, glue_C30D34_complete_step, 0xC30EAA, 1, 0xC30D32, glue_C30D34_owns},
    {0xC30EAA, glue_C30EAA, "blit_image", 3000},
    {0xC309B6, glue_C309B6, "draw_table_stream_display", 0, 0, glue_C309B6_complete_step, 0xC309E2, 1, 0, glue_C309B6_owns},
    /* batch 60b: compass tape */
    {0xC30F78, glue_C30F78, "draw_cached_stream_display", 0, 0, glue_C30F78_complete_step, 0xC310AA, 1, 0xC30F76, glue_C30F78_owns},
    /* batch 60c: panel frame */
    {0xC30764, glue_C30764, "draw_counter_stream_display", 0, 0, glue_C30764_complete_step, 0xC308D8, 1, 0xC30762, glue_C30764_owns},
    /* batch 61: HUD marks */
    {0xC34146, glue_C34146, "draw_hud_marks", 0, 0, glue_C34146_complete_step, 0xC342D0, 1, 0, glue_C34146_owns},
    {0xC34066, glue_C34066, "draw_tick_row", 0, 0, glue_C34066_complete_step, 0xC34144, 1, 0, glue_C34066_owns},
    /* batch 61b: target box */
    {0xC342D0, glue_C342D0, "draw_target_box", 0, 0, glue_C342D0_complete_step, 0xC34540, 1, 0, glue_C342D0_owns},
    /* batch 61d: ring point, pixel block */
    {0xC2F66E, glue_C2F66E, "plot_pixel_block", 0, 0, glue_C2F66E_complete_step, 0xC2FA70, 0, 0xC2F60A, glue_C2F66E_owns},
    {0xC347F2, glue_C347F2, "plot_ring_point", 0, 0, glue_C347F2_complete_step, 0xC348B0, 1, 0, glue_C347F2_owns},
    /* batch 61e: missile cue */
    {0xC33DC8, glue_C33DC8, "update_missile_cue", 0, 0, glue_C33DC8_complete_step, 0xC33F54, 1, 0xC33DA4, glue_C33DC8_owns},
    /* batch 62: message line, display list sort */
    {0xC322EE, glue_C322EE, "draw_message_line", 0, 0, glue_C322EE_complete_step, 0xC32806, 1, 0xC322EC, glue_C322EE_owns},
    /* batch 63: postflight HUD */
    {0xC33CD2, glue_C33CD2, "transform_postflight_record", 0, 0, glue_C33CD2_complete_step, 0xC33DA4, 1, 0, glue_C33CD2_owns},
    /* batch 63b */
    {0xC33B38, glue_C33B38, "draw_postflight_variant", 0, 0, glue_C33B38_complete_step, 0xC33CD2, 1, 0xC33B36, glue_C33B38_owns},
    /* batch 63c */
    {0xC33370, glue_C33370, "draw_postflight_tape", 0, 0, glue_C33370_complete_step, 0xC3395E, 1, 0, glue_C33370_owns},
    /* batch 63d: HUD stage */
    {0xC332BC, glue_C332BC, "draw_postflight_hud", 0, 0, glue_C332BC_complete_step, 0xC332FC, 1, 0xC332B4, glue_C332BC_owns},
    /* polygon to row C7 */
    {0xC301F0, glue_C301F0, "prepare_polygon_to_row", 0, 0, glue_C301F0_complete_step, 0xC3040C, 0, 0, glue_C301F0_owns},
    /* zone exit */
    {0xC28E28, glue_C28E28, "check_zone_exit", 0, 0, glue_C28E28_complete_step, 0xC28F16, 0, 0xC28E16, glue_C28E28_owns},
    /* shape */
    {0xC2D16C, glue_C2D16C, "draw_shape", 0, 0, glue_C2D16C_complete_step, 0xC2D3A4, 1, 0, glue_C2D16C_owns},
    /* block face */
    {0xC21500, glue_C21500, "draw_block_face", 0, 0, glue_C21500_complete_step, 0xC2159E, 1, 0xC214FC, glue_C21500_owns},
    /* offset run */
    {0xC2122A, glue_C2122A, "draw_offset_run", 0, 0, glue_C2122A_complete_step, 0xC2129C, 1, 0, glue_C2122A_owns},
    /* split square */
    {0xC20592, glue_C20592, "draw_split_square", 0, 0, glue_C20592_complete_step, 0xC20656, 1, 0xC2058E, glue_C20592_owns},
    /* side triangle */
    {0xC2168A, glue_C2168A, "draw_side_triangle", 0, 0, glue_C2168A_complete_step, 0xC217EA, 1, 0, glue_C2168A_owns},
    /* square faces */
    {0xC203D0, glue_C203D0, "draw_square_faces", 0, 0, glue_C203D0_complete_step, 0xC2058E, 1, 0xC203CC, glue_C203D0_owns},
    /* shadow */
    {0xC201A6, glue_C201A6, "draw_record_shadow", 0, 0, glue_C201A6_complete_step, 0xC203CA, 1, 0xC201A2, glue_C201A6_owns},
    /* mark polygon */
    {0xC3019C, glue_C3019C, "draw_mark_polygon", 0, 0, glue_C3019C_complete_step, 0xC301F0, 1, 0xC3019A, glue_C3019C_owns},
    /* face test dispatch */
    {0xC1FB82, glue_C1FB82, "face_toward_eye", 0, 0, glue_C1FB82_complete_step, 0xC1FCDE, 1, 0, glue_C1FB82_owns},
    /* bound points */
    {0xC1F99A, glue_C1F99A, "transform_bound_points", 0, 0, glue_C1F99A_complete_step, 0xC1FB82, 1, 0, glue_C1F99A_owns},
    /* draw_stream.c */
    {0xC1FF0A, glue_C1FF0A, "test_stream_face", 0, 0, glue_C1FF0A_complete_step, 0xC1FF46, 1, 0, glue_C1FF0A_owns},
    /* draw_stream.c */
    {0xC2005C, glue_C2005C, "draw_tested_face", 0, 0, glue_C2005C_complete_step, 0xC200F6, 1, 0, glue_C2005C_owns},
    {0xC20100, glue_C20100, "draw_indexed_face_list", 0, 0, glue_C20100_complete_step, 0xC201A0, 1, 0xC200F6, glue_C20100_owns},
    /* draw_stream.c face loops */
    {0xC21060, glue_C21060, "draw_quad_list", 0, 0, glue_C21060_complete_step, 0xC210E6, 1, 0, glue_C21060_owns},
    {0xC20C38, glue_C20C38, "draw_face_grid", 0, 0, glue_C20C38_complete_step, 0xC20D68, 1, 0, glue_C20C38_owns},
    {0xC20C22, glue_C20C22, "draw_face_grid_plain", 0, 0, glue_C20C22_complete_step, 0xC20D68, 1, 0, glue_C20C22_owns},
    {0xC20A52, glue_C20A52, "draw_face_lattice", 0, 0, glue_C20A52_complete_step, 0xC20C22, 1, 0, glue_C20A52_owns},
    {0xC20A40, glue_C20A40, "draw_face_lattice_plain", 0, 0, glue_C20A40_complete_step, 0xC20C22, 1, 0, glue_C20A40_owns},
    /* draw_stream.c */
    {0xC20002, glue_C20002, "draw_tested_parallelogram", 0, 0, glue_C20002_complete_step, 0xC200F6, 1, 0, glue_C20002_owns},
    /* control_records.c */
    {0xC1D3F4, glue_C1D3F4, "expand_cell_templates", 0, 0, glue_C1D3F4_step, 0xC1D722, 0, 0xC1D3F4},
    /* hud_bars.c */
    /* control_records.c */
    {0xC28800, glue_C28800, "aim_record_at_view", 0, 0, glue_C28800_step, 0xC288C6},
    /* scene_setup.c */
    {0xC0924A, glue_C0924A, "reset_scene_context", 0, 0, glue_C0924A_step, 0xC095C0},
    {0xC09266, glue_C09266, "reset_scene_recorder", 0, 0, glue_C09266_step, 0xC095C0},
    {0xC092A0, glue_C092A0, "place_scene_root", 0, 0, glue_C092A0_step, 0xC095C0},
    /* postflight scene callbacks (/) */
    {0xC11788, glue_C11788, "advance_postflight_reset", 0, 0, glue_C11788_step, 0xC11830, 0, 0, glue_C11788_owns},
    {0xC11830, glue_C11830, "restart_postflight_scene", 0, 0, glue_C11830_step, 0xC11872, 0, 0, glue_C11830_owns},
    /* panel mark drawing () */
    {0xC3003A, glue_C3003A, "draw_panel_mark", 0, 0, glue_C3003A_complete_step, 0xC3019A, 1, 0xC30038, glue_C3003A_owns},
    /* post-input heading formatter () */
    {0xC25070, glue_C25070, "refresh_post_input_heading", 0, 0, glue_C25070_step, 0xC25176, 0, 0xC2506C, glue_C25070_owns},
    /* scene record stream dispatch () */
    {0xC28B34, glue_C28B34, "dispatch_scene_records", 0, 0, glue_C28B34_complete_step, 0xC28E12, 0, 0xC28B34, glue_C28B34_owns},
    /* scene record initialization and aim () */
    {0xC28AFE, glue_C28AFE, "initialize_scene_record", 0, 0, glue_C28AFE_step, 0xC28E12},
    /* post-input context command and heading marker () */
    {0xC10C68, glue_C10C68, "queue_post_input_context_command", 0, 0, glue_C10C68_step, 0xC10CFE, 0, 0, glue_C10C68_owns},
    /* static template bit-gate builder () */
    {0xC1C40C, glue_C1C40C, "build_template_bit_gates", 20000},
    /* selected-fire record initializer () */
    {0xC2374C, glue_C2374C, "consume_selected_fire_request", 0, 0, glue_C2374C_step, 0xC23A26, 0, 0xC23744},
    /* scene stream selection and special scene record () */
    {0xC28722, glue_C28722, "initialize_scene_from_mode", 0, 0, glue_C28722_step, 0xC28E12, 0, 0xC28720},
    /* scene initialization and ordered child calls () */
    {0xC0FAA4, glue_C0FAA4, "initialize_scene_state", 0, 0, glue_C0FAA4_step, 0xC0FB28},
    /* timer-gated post-input scene transition () */
    {0xC0FA04, glue_C0FA04, "finish_post_input_followup", 0, 0, glue_C0FA04_step, 0xC0FA4C},
    /* display-record candidate and selector siblings (/) */
    {0xC0D74A, glue_C0D74A, "prepare_display_records_wide", 0, 0, glue_C0D74A_step, 0xC0DAEE},
    {0xC0D752, glue_C0D752, "prepare_display_records", 0, 0, glue_C0D752_step, 0xC0DAEE, 0, 0xC0D74A},
    /* signed matrix transform and three returned angles */
    {0xC2DEE0, glue_C2DEE0, "transform_record_matrix", 0, 0, glue_C2DEE0_step, 0xC2E346},
    /* active control-record matrix route */
    {0xC2DB18, glue_C2DB18, "update_control_record_matrix_route", 0, 0, glue_C2DB18_step, 0xC2DCC2},
    /* matrix route selector */
    {0xC2D99C, glue_C2D99C, "dispatch_matrix_route", 0, 0, glue_C2D99C_step, 0xC2D9B0},
    /* current record matrix update */
    {0xC2D408, glue_C2D408, "update_record_matrix", 9500},
    /* indexed control-record update */
    {0xC13D84, glue_C13D84, "update_indexed_record", 18000},
    {0xC26EBE, glue_C26EBE, "update_candidate_record", 0, 0, glue_C26EBE_complete_step, 0xC279C8, 0, 0xC26EB8, glue_C26EBE_owns},
    {0xC23CA6, glue_C23CA6, "update_record_view", 0, 0, glue_C23CA6_step, 0xC24368},
    {0xC0D04C, glue_C0D04C, "draw_history_projection", 0, 0, glue_C0D04C_complete_step, 0xC0D334, 1, 0xC0D048, glue_C0D04C_owns},
    /* selected projected segment */
    {0xC1FF9C, glue_C1FF9C, "segment_selected", 0, 0, glue_C1FF9C_complete_step, 0xC20002, 0, 0xC1FF9C, glue_C1FF9C_owns},
    /* planar lane mask handlers */
    {0xC2F826, glue_C2F826, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2F826_complete_step, 0xC2F830, 0, 0, glue_C2F826_owns},
    {0xC2F83A, glue_C2F83A, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2F83A_complete_step, 0xC2F844, 0, 0, glue_C2F83A_owns},
    {0xC2F844, glue_C2F844, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2F844_complete_step, 0xC2F84E, 0, 0, glue_C2F844_owns},
    {0xC2F84E, glue_C2F84E, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2F84E_complete_step, 0xC2F858, 0, 0, glue_C2F84E_owns},
    {0xC2F858, glue_C2F858, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2F858_complete_step, 0xC2F862, 0, 0, glue_C2F858_owns},
    {0xC2F862, glue_C2F862, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2F862_complete_step, 0xC2F86C, 0, 0, glue_C2F862_owns},
    {0xC2F86C, glue_C2F86C, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2F86C_complete_step, 0xC2F876, 0, 0, glue_C2F86C_owns},
    {0xC2F876, glue_C2F876, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2F876_complete_step, 0xC2F880, 0, 0, glue_C2F876_owns},
    {0xC2F880, glue_C2F880, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2F880_complete_step, 0xC2F88A, 0, 0, glue_C2F880_owns},
    {0xC2F88A, glue_C2F88A, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2F88A_complete_step, 0xC2F894, 0, 0, glue_C2F88A_owns},
    {0xC2F894, glue_C2F894, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2F894_complete_step, 0xC2F89E, 0, 0, glue_C2F894_owns},
    {0xC2F89E, glue_C2F89E, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2F89E_complete_step, 0xC2F8A8, 0, 0, glue_C2F89E_owns},
    {0xC2F8A8, glue_C2F8A8, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2F8A8_complete_step, 0xC2F8B2, 0, 0, glue_C2F8A8_owns},
    {0xC2F8B2, glue_C2F8B2, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2F8B2_complete_step, 0xC2F8BC, 0, 0, glue_C2F8B2_owns},
    {0xC2F8EA, glue_C2F8EA, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2F8EA_complete_step, 0xC2F904, 0, 0, glue_C2F8EA_owns},
    {0xC2F904, glue_C2F904, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2F904_complete_step, 0xC2F91E, 0, 0, glue_C2F904_owns},
    {0xC2F91E, glue_C2F91E, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2F91E_complete_step, 0xC2F938, 0, 0, glue_C2F91E_owns},
    {0xC2F96C, glue_C2F96C, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2F96C_complete_step, 0xC2F986, 0, 0, glue_C2F96C_owns},
    {0xC2F986, glue_C2F986, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2F986_complete_step, 0xC2F9A0, 0, 0, glue_C2F986_owns},
    {0xC2F9A0, glue_C2F9A0, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2F9A0_complete_step, 0xC2F9BA, 0, 0, glue_C2F9A0_owns},
    {0xC2F9BA, glue_C2F9BA, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2F9BA_complete_step, 0xC2F9D4, 0, 0, glue_C2F9BA_owns},
    {0xC2F9D4, glue_C2F9D4, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2F9D4_complete_step, 0xC2F9EE, 0, 0, glue_C2F9D4_owns},
    {0xC2F9EE, glue_C2F9EE, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2F9EE_complete_step, 0xC2FA08, 0, 0, glue_C2F9EE_owns},
    {0xC2FA08, glue_C2FA08, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2FA08_complete_step, 0xC2FA22, 0, 0, glue_C2FA08_owns},
    {0xC2FA22, glue_C2FA22, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2FA22_complete_step, 0xC2FA3C, 0, 0, glue_C2FA22_owns},
    /* selected clipped segment sibling */
    {0xC1FFA4, glue_C1FFA4, "segment_selected", 0, 0, glue_C1FFA4_complete_step, 0xC20002, 0, 0xC1FFA4, glue_C1FFA4_owns},
    {0xC2FD8C, glue_C2FD8C, "submit_active_planes", 0, 0, glue_C2FD8C_complete_step, 0xC2FF46, 1, 0, glue_C2FD8C_owns},
    {0xC26322, glue_C26322, "project_record_motion", 0, 0, glue_C26322_step, 0xC26352, 0, 0xC26322, glue_C26322_owns},
    {0xC26352, glue_C26352, "publish_motion_slot", 0, 0, glue_C26352_step, 0xC2639C, 0, 0xC26352, glue_C26352_owns},
    {0xC26C72, glue_C26C72, "project_scene_motion", 0, 0, glue_C26C72_step, 0xC26CC0, 0, 0xC26C72, glue_C26C72_owns},
    {0xC26CC0, glue_C26CC0, "test_component_motion", 0, 0, glue_C26CC0_step, 0xC26D8A, 0, 0xC26CC0, glue_C26CC0_owns},
    {0xC26D8A, glue_C26D8A, "test_face_motion", 0, 0, glue_C26D8A_step, 0xC26EAC, 0, 0xC26D8A, glue_C26D8A_owns},
    {0xC25B66, glue_C25B66, "advance_indexed_record_dynamics", 0, 0, glue_C25B66_step, 0xC26322, 0, 0xC25B22, glue_C25B66_owns},
    {0xC266AE, glue_C266AE, "collide_scene_motion", 0, 0, glue_C266AE_step, 0xC26C68, 0, 0xC266AC, glue_C266AE_owns},
    {0xC28996, glue_C28996, "update_scene_regions", 0, 0, glue_C28996_step, 0xC28AFC, 0, 0xC28994, glue_C28996_owns},
    {0xC28B16, glue_C28B16, "spawn_region_records", 0, 0, glue_C28B16_step, 0xC28E16, 0, 0xC28B14, glue_C28B16_owns},
    /* Complete grid, scene labels and record markers, including shared tails.
     * Drawing parents preserve their original children's DMACONR inputs. */
    {0xC2AFFA, glue_C2AFFA, "transform_marker_point", 0, 0, glue_C2AFFA_step, 0xC2B042, 0, 0xC2AFFA, glue_C2AFFA_owns},
    {0xC2B3C2, glue_C2B3C2, "draw_scene_position_labels", 0, 0, glue_C2B3C2_step, 0xC2B562, 1, 0xC2B3C0, glue_C2B3C2_owns},
    {0xC2B564, glue_C2B564, "draw_view_grid_and_markers", 0, 0, glue_C2B564_step, 0xC2B7E4, 1, 0xC2B562, glue_C2B564_owns},
    {0xC2B928, glue_C2B928, "draw_class_twenty_marker", 0, 0, glue_C2B928_step, 0xC2BAF0, 1, 0xC2B928, glue_C2B928_owns},
    {0xC2B952, glue_C2B952, "draw_record_position_marker", 0, 0, glue_C2B952_step, 0xC2BAF0, 1, 0xC2B950, glue_C2B952_owns},
    {0xC2F688, glue_C2F688, "plot_pixel_shared", 0, 0, glue_C2F688_complete_step, 0xC2FA70, 0, 0xC2F622, glue_C2F688_owns},
    {0xC2F8BC, glue_C2F8BC, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2F8BC_complete_step, 0xC2F8C6, 0, 0, glue_C2F8BC_owns},
    {0xC2F8C6, glue_C2F8C6, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2F8C6_complete_step, 0xC2F8D0, 0, 0, glue_C2F8C6_owns},
    {0xC2F8D0, glue_C2F8D0, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2F8D0_complete_step, 0xC2F8EA, 0, 0, glue_C2F8D0_owns},
    {0xC2F938, glue_C2F938, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2F938_complete_step, 0xC2F952, 0, 0, glue_C2F938_owns},
    {0xC2F952, glue_C2F952, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2F952_complete_step, 0xC2F96C, 0, 0, glue_C2F952_owns},
    {0xC2FA3C, glue_C2FA3C, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2FA3C_complete_step, 0xC2FA56, 0, 0, glue_C2FA3C_owns},
    {0xC2FA56, glue_C2FA56, "apply_planar_lane_masks", 0, 0xC2F764, glue_C2FA56_complete_step, 0xC2FA70, 0, 0, glue_C2FA56_owns},
    {0, 0, 0, 0}, /* sentinel; entries are added above it */
};
const int fa18_port_count = (int)(sizeof fa18_ports / sizeof fa18_ports[0]) - 1;
