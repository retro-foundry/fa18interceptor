#ifndef FA18_GAME_DISPLAY_RECORDS_H
#define FA18_GAME_DISPLAY_RECORDS_H

/* $C0D74A/$C0D752: form four corner candidates, project their edges, and
 * select the display polygon. The hook preserves the nested call boundary
 * for 68000 register replay; NULL runs the same C path directly. */
typedef struct DisplayRecordHooks {
    void (*project_edges)(void *context);
    void *context;
} DisplayRecordHooks;

int prepare_display_records(int wide, const DisplayRecordHooks *hooks);

#endif
