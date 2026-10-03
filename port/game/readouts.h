#ifndef FA18_GAME_READOUTS_H
#define FA18_GAME_READOUTS_H

/* READOUT_VALUE = 800000 / READOUT_DIVISOR (9999 when the divisor exceeds
 * $7FFF). Only the divided path updates minimum/maximum; both paths latch
 * the next sample. Zero division needs the original exception service, which
 * the CPU adapter supplies through MainTimerHooks. */
void update_readout(void);

#endif
