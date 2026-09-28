#ifndef FA18_GAME_READOUTS_H
#define FA18_GAME_READOUTS_H

/* READOUT_VALUE = 800000 / READOUT_DIVISOR (9999 when the divisor exceeds
 * $7FFF), tracking its minimum and maximum; then latch the next sample. */
void update_readout(void);

#endif
