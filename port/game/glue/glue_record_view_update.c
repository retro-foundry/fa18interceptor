/* Register bridge for the record-view update stage $C23CA6. */
#include "glue.h"
#include "ports_glue.h"

#include "control_records.h"

int glue_C23CA6(void) {
    RecordViewUpdateWork work = {0};
    update_record_view(A(1), A(3), D(4), &work);
    A(3) = work.viewer;
    if (work.dispatch_valid &&
        (work.route == RECORD_VIEW_UPDATE_EARLY ||
         work.route == RECORD_VIEW_UPDATE_LINKED))
        SET_W(D(1), work.dispatch_d1);
    D(0) = 1;
    flags_logic_l(D(0));
    return glue_return();
}
