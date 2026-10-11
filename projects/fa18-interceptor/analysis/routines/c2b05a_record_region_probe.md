# Record-region probe ($C2B05A)

Updated 2026-10-01. Authority: `tools/recomp/port_info.py C2B05A`, the
original instructions in `generated/recomp_002.c`, and the sealed native demo.

## Contract

The selected mutable record is `$C46184 + signed_word($C459B6)`. Two walks
update its flag byte at +4, preserving the other bits:

- `$C2B05A-$C2B24A` selects a signed offset from `$C42E6C` using the high
  bytes of record +$14 and +$1C. Each directory group supplies signed point
  pairs scaled by 4096. Adaptive signed division projects a segment crossing;
  strict endpoint bounds and odd crossing parity control bit 1.
- `$C2B24A-$C2B3B2` walks 16-byte placements at `$C42A96`. Five signed words
  select the origin, the `$C1D7E2` adjustment pair, and two local offsets. The
  scene pointer at `$C22188 + signed_selector + $10` supplies four-point
  polygons. Four signed cross products control bit 2; a negative product
  rejects that polygon, and a -1 first word terminates the polygon stream.

These are behavioral names for the calculations. The precise gameplay meaning
of the two region bits is still unassigned.

## C/glue separation

`port/game/record_region_probe.c` now owns the calculations and flag writes.
`RecordRegionProbeHooks` publishes mathematical results and walk observations;
it carries no data-register or address-register array. The optional hooks do
not own memory writes or repeat the walk.

`glue_record_region_probe.c` reconstructs live register effects. The shape walk
overwrites D0-D5, A0 and A3, so their intermediate directory values need no
replay at the routine's completed-call boundary. Retained directory values,
sign extension, partial-word writes and high words are replayed in glue.
`$C2B1CA` restores saved D2-D5 into D2/D4-D6: the last restored value is
endpoint Y, subsequently subject to the bound-order exchange.

The direct oracle exposed a pre-existing discrepancy at `$C2B1E4/$C2B1FE`:
the source jumps to `$C2B050` when an X endpoint is within $300, abandoning
the entire directory walk and clearing bit 1. The previous C merely returned
from the segment and continued the group. The C now follows the source exit.
The placement origin also retains the sign-extension bits left by SWAP and
ASL.L #8 for a negative word; it is not just the low byte shifted by 24.

## Direct validation

```powershell
python tools/recomp/check_record_region_probe.py --cases 4096 `
  --bash 'C:/Program Files/Git/bin/bash.exe'
```

The harness reads the sealed demo start state and uses isolated structural
fixtures. It compares the original translated instructions at their actual
entry label against the C/glue result: all 16 data/address registers, return
PC, Chip RAM and Slow RAM, excluding the dead stack. Flags are not compared
because no flag is live at the recorded caller. Inputs cover signed coordinates,
both outline orientations, sloped edges, endpoint proximity, rejected or
terminated placements, nonpositive directory offsets, absent polygon streams
and randomized register halves.
These are oracle inputs, not game content or defaults.

The restored 413-entry acceptance gate and the temporary three-entry bridge
comparison are recorded in `CURRENT_PORT_HANDOFF.md`. This routine remains
inactive until live timing and RGB parity are solved.
