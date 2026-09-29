#ifndef FA18_GAME_MESSAGE_LINE_H
#define FA18_GAME_MESSAGE_LINE_H

/* Draw the cockpit message line ($C322EE). While a record is selected (and
 * no posted or threat message holds the line) it shows the selected
 * record's type and one info page: altitude, heading or speed, stepped by
 * INFO_REQUEST. Otherwise the message entry shown this step (MESSAGE_SHOWN)
 * is copied in when it changes, or a blank line when the info goes away.
 * It is redrawn for two more passes after a change: in the plane its kind
 * (MESSAGE_FLAGS bits 6-7) selects, with the other two planes' cells
 * cleared while MESSAGE_REDRAWS runs; in a context only with TEXT_ALWAYS,
 * again below the view. */
void draw_message_line(void);

#endif
