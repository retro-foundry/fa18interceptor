#ifndef FA18_GLUE_CONTEXT_PUBLICATION_H
#define FA18_GLUE_CONTEXT_PUBLICATION_H
#include <stdint.h>
#define CONTEXT_PUBLICATION_ENTRIES(X) X(C1B7A6) X(C1BEE8) X(C1C214) X(C083A6) X(C09DD0)
#define DECLARE_PUBLICATION(entry) int glue_##entry(void); int glue_##entry##_step(void); int glue_##entry##_owns(uint32_t pc);
CONTEXT_PUBLICATION_ENTRIES(DECLARE_PUBLICATION)
/* Shared C083B6 body, after the caller's D4/D7/A1 save. */
int glue_selected_record_request_body(void);
#undef DECLARE_PUBLICATION
#endif
