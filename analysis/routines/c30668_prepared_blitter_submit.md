# `$C30668`: prepared blitter-job submission

This exact leaf is reached repeatedly from the bounded no-input cockpit trace. It waits for `DMACONR` bit 6, then uses caller-prepared registers to program a blitter job and starts it by writing `BLTSIZE`.

Observed returns are to `$C30328` and `$C30340`, both portions of the display-geometry code. During the ten-frame trace it produced destination writes in the active first cockpit plane, including `$013C50`, `$012C0F`, and `$012BE8`. The function itself does not choose those pointers: caller register `D7` supplies both `BLTAPT` and `BLTCPT`; the destination register is set by the surrounding caller state before this leaf's final start.

The symbol therefore describes the proved hardware action only. It does not claim a specific cockpit component, polygon, or asset role.
