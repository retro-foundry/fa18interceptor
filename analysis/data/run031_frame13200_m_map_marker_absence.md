# Run031 Alcatraz-window M-map: conditional marker absence

Classification: **direct negative renderer observation; identity still
unassigned**.

The diagnostic Alcatraz-window M-map state was stepped for 60 normal frames
with a breakpoint at `$C2FA7E`. It emits 65 screen-line submissions, including
28 grid lines with `A5=$C4C59E`, but **zero** entries with `A5=$C4C598`.
`$C4C598` is the three-stroke black `MAP OBJECT ?` line context captured in
the run003/run024/run035 maps.

The map therefore does not draw that symbol as a fixed city, island, or
coastline feature in every map state. Its prior link to an aircraft-associated
mutable display context remains the strongest provenance evidence, but this
absence alone does not distinguish player aircraft, another aircraft, selected
object, or another conditional display object.

This capture was reached by clearing the upstream `$C4584B` UI mode latch;
that diagnostic state must not be used to assert a normal-game absence rule.
It does, however, rule out treating the observed black symbol as a universal
static map landmark.

Authority:
`build/run031_frame13200_alcatraz_m_map_marker_lines/blitter_line_entries.json`
from `build/run031_frame13200_alcatraz_m_map_mode_latch_zero/state.bin`.
