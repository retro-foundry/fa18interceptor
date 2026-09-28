#ifndef FA18_RECOMP_PORTS_H
#define FA18_RECOMP_PORTS_H

/* Stage D: hand-written C routines replacing generated ones.
 *
 * Each port has a glue function (port/game/glue/) that is entered exactly
 * where the original routine is: at its first instruction, just after a
 * JSR/BSR pushed the return address. The glue reads the routine's inputs from
 * the 68000 registers and game memory, calls the hand-written C in
 * port/game/, stores the outputs and register effects the original leaves,
 * then performs the RTS. It returns FA18_RET.
 *
 * Modes:
 *   OFF     generated code only (the reference).
 *   SHADOW  every call runs the generated routine and the glue on the same
 *           state and compares registers, flags and memory writes; the game
 *           continues on the generated result. Proof mode.
 *   ON      glue only.
 */

#include <stdint.h>

typedef int (*FA18PortGlue)(void);

typedef struct {
    uint32_t entry;       /* original routine address */
    FA18PortGlue glue;
    const char *name;     /* the C function it calls */
    int cycles;           /* CPU cycles charged in ON mode (measured in SHADOW) */
} FA18Port;

extern const FA18Port fa18_ports[];
extern const int fa18_port_count;

typedef enum { FA18_PORTS_OFF, FA18_PORTS_ON, FA18_PORTS_SHADOW } FA18PortMode;

void fa18_ports_init(FA18PortMode mode, const char *only);
/* Writes the per-port SHADOW results (calls, matches, mismatches, mean
 * cycles) as JSON; returns the number of mismatching calls. */
long fa18_ports_report(const char *path);

/* Per-routine call counts, for tools/recomp/inventory.py --profile. */
int fa18_recomp_write_profile(const char *path);

#endif
