/* Connected C10A24 location/aircraft selection sequence. */
#include "setup.h"
#include "clock.h"
#include "flight.h"
#include "../menu_context_finish.h"
#include "../matrix.h"
#include "../audio.h"
#include "../globals.h"
#include "../stages.h"
#include "../menu_setup.h"
#include "../cockpit.h"
#include "../target_heading.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct { NativeFrontend *game; uint32_t position[3]; } Setup;
static void position_result(void *context,uint32_t result[3]) {
    const Setup *setup=context;
    for(unsigned i=0;i<3;++i) result[i]=setup->position[i];
}
static int32_t child(void *context,enum MenuContextChild which) {
    Setup *setup=context;
    switch(which) {
    case MC_SMOOTH_RESET: case MC_RESTART_RESET: case MC_STAGE_RESET:
    case MC_VIEWPORT_RESET: reset_message_sequence(); return 0;
    case MC_REFRESH_VIEW: native_flight_reset_aircraft(setup->game); return 0;
    case MC_PRESET_POSITION: {
        const MenuContextHooks hooks={child,NULL,position_result,setup};
        load_menu_position_preset(&hooks); return 0;
    }
    case MC_POSITION_TRANSFORM: {
        /* C09194/C09196/C0919A supply (0,0,$E980) to C091E6. */
        int32_t position[3];
        local_to_world(CONTROL_RECORDS,CONTROL_RECORDS+RECORD_INVERSE,0,0,(int16_t)0xe980,position);
        for(unsigned i=0;i<3;++i) setup->position[i]=(uint32_t)position[i];
        return 0;
    }
    case MC_MESSAGE_TIME: case MC_STAGE_TIME: case MC_VIEWPORT_TIME: {
        const MenuContextHooks hooks={child,NULL,position_result,setup};
        read_menu_time_sample(&hooks); return 0;
    }
    case MC_TIMER_REQUEST:
        /* timer.device GetSysTime's seconds/microseconds result. The native
         * replay clock advances at the runner's existing 50 Hz PAL cadence. */
        native_clock_request(); return 0;
    case MC_EXPIRY_TONE: play_tone_2(); return 0;
    case MC_HEADING: return refresh_post_input_heading(); /* C25070 */
    case MC_STAGE_SETUP: free_all_voices(); return 0;
    case MC_STAGE_SOUND: start_sound_6(0x3f,0x78); return 0;
    case MC_STAGE_COMMAND: return filter_cockpit_message(0x4021,NULL);
    case MC_STAGE_FINISH: request_cockpit_redraw(); return 0;
    case MC_NOISE: play_noise(8); return 0; /* C17E4A */
    case MC_ENGINE: play_engine(0x300,8); return 0; /* C17CF6 */
    default: fprintf(stderr,"native setup child unavailable: %u\n",(unsigned)which); abort();
    }
}
int native_setup_stage(NativeFrontend *game,gaddr routine) {
    Setup setup={game,{0}};
    const MenuContextHooks hooks={child,NULL,position_result,&setup};
    switch(routine) {
    case 0xc10a24: follow_menu_smoothing(&hooks); return 1;
    case 0xc10ab2: queue_menu_smoothing_message(&hooks); return 1;
    case 0xc10ae6: restart_menu_smoothing(&hooks); return 1;
    case 0xc10b1e: reset_menu_smoothing_view(&hooks); return 1;
    case 0xc10c08: begin_menu_context(&hooks); return 1;
    case 0xc10c68: queue_menu_context_command(0,&hooks); return 1;
    case 0xc10cfe: finish_menu_context_message(&hooks); return 1;
    case 0xc10d8a: expire_menu_context(&hooks); return 1;
    case 0xc10dae: update_menu_context(0,&hooks); return 1;
    /* C11A26-C11A4E uses D0/D1/A0 only and calls no children. Its viewport
     * gate and message publication preserve the preceding domain output. */
    case 0xc11a26: queue_menu_viewport_message(&hooks); return NATIVE_SETUP_INPUT_PRESERVED;
    case 0xc11a50: finish_menu_viewport_message(&hooks); return 1;
    default: return 0;
    }
}
