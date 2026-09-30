#ifndef FA18_TARGET_HEADING_H
#define FA18_TARGET_HEADING_H

/* $C25070: find the first active class-$10 record in the post-input list,
 * aim the tracked heading at a point through it, and write three heading
 * characters. Returns -1 when the list has no matching record, 0 otherwise. */
int refresh_post_input_heading(void);

#endif
