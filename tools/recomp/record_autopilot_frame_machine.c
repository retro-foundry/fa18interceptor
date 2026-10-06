/* Frame probe only: stop at the original autopilot return before unrelated
 * flight work. Chipset events keep running to the ordinary frame boundary. */
#define fa18_machine_service fa18_autopilot_original_service
#include "../../port/machine/machine.c"
#undef fa18_machine_service
extern int fa18_autopilot_frame_fixture;
int fa18_machine_service(void) {
    if(fa18_autopilot_frame_fixture && REG_PC==0xc25c70u && REG_A[1]==0xc65000u) {
        CPU_STOPPED|=STOP_LEVEL_STOP;
        SET_CYCLES(0);
        return 1;
    }
    return fa18_autopilot_original_service();
}
