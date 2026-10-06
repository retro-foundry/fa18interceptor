/* Both record-update owners now retain game C phases. Only the original
 * entry/stack compatibility remains; no source instruction stepping body. */
#include "glue.h"
#include "recomp_ports.h"
extern int fa18_write_log_active;
extern int glue_C22C80(void);
extern int glue_C1C63E(void);
extern int glue_schedule_control_records(void);
extern int glue_schedule_record_update_stage(void);
int glue_C22C80_step(void) {
    if(REG_PC!=0xc22c80u) return 0;
    if(fa18_write_log_active) { glue_C22C80(); return 1; }
    REG_PPC=REG_PC; return glue_schedule_control_records();
}
int glue_C1C63E_step(void) {
    if(REG_PC!=0xc1c63eu) return 0;
    if(fa18_write_log_active) { glue_C1C63E(); return 1; }
    REG_PPC=REG_PC; return glue_schedule_record_update_stage();
}
