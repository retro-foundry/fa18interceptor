#ifndef FA18_PORTS_GLUE_H
#define FA18_PORTS_GLUE_H

/* Glue entry points, one per recreated routine, named by original address. */

/* render_polygon.c */
int glue_C30466(void);
int glue_C304B2(void);
int glue_C305AA(void);

/* render_line.c */
int glue_C2FA7E(void);

/* fixed_math.c, audio.c, text.c */
int glue_C2E6DA(void);
int glue_C501E0(void);
int glue_C24FE8(void);
int glue_C330FE(void);
int glue_C32806(void);
int glue_C25A08(void);
int glue_C1715C(void);
int glue_C2F558(void);

#endif
