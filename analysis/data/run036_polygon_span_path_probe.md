# run036 finalized-polygon span-path probe

Classification: **scenario-backed renderer-path separation; one polygon is
excluded from the red Golden Gate landmark**.

The first `$C2FF48` entry reached at replay frame 7,000 was traced to its
actual caller return (`$C24D66`), with future recorded input deliberately
withheld while the debugger single-stepped.  The trace completes in 540
instructions.

Its four projected pairs at entry are:

```text
stored:    (97,127) (130,138) (74,145) (57,130)
unreflected screen coordinates: (222,52) (189,41) (245,34) (262,49)
```

The latter form uses the independently established pair conversion
`(x,y)=(319-stored_x,179-stored_y)`.  Its bounds are therefore `x=189..262,
y=34..64`, completely disjoint from run036's measured red Golden Gate raster
at `x=59..145, y=107..115`.  This specific finalized polygon cannot be the
red landmark.

## Observed renderer route

This invocation does **not** enter `$C2FA7E`, the known line emitter.  After
the `$C301F6` bounds pass it takes `$C302DE -> $C302EC`, prepares successive
edge-related state through `$C30324 -> $C305AA`, and programs direct blitter
jobs at `$C303D2-$C30404`, two `$C30466` calls, and `$C304B2`.  The trace
contains four `BLTSIZE` writes in these direct-job paths and zero `$C2FA7E`
entries.

This proves that a finalized projected polygon can take a separate,
span-style direct-blitter route rather than the ordinary line-emitter route.
It does **not** identify the exact fill rule from this one trace, and it does
not make a distance/LOD claim.  Most importantly, its direct-blitter route is
not evidence that the red bridge feature was filled: the one fully bounded
polygon is in another part of the viewport.

Authority: sealed `captures/run036`; the replay-preserved ignored trace
`build/run036_7000_c2ff48_submission_trace_no_future/` (entry snapshot SHA-256
`42e066a43e21d0225ea536c6217523789ac613a97277d6532956beb007dfcbb1`).

## Same-frame primitive census

A one-chipset-frame instruction trace from the real replay boundary provides a
non-collector cross-check.  Frame 7,000 contains exactly three `$C2FF48`
entries (trace indices 5,103, 6,464, and 8,120).  Each takes the same
`$C302DE -> $C302EC` direct-blitter route and has `A5=$FFFFF2`, rather than a
static `$C355xx` bridge control address.  Earlier in that *same* trace are
exactly eight `$C2FA7E` calls: four with `A5=$C35596` and four with
`A5=$C355CE`; their endpoints exactly reproduce the red-raster-correlated line
groups in the primitive-transition probe.

Thus the live frame has both renderer routes, but its bridge-correlated work is
the line route.  This still does not prove that no other off-trace polygon can
ever represent the bridge, nor does it establish LOD.

Reproduce:

```text
python scripts/trace_from_breakpoint.py \
  --restore captures/run036/initial_state.bin \
  --config captures/run036/config.uae \
  --playback captures/run036/playback.e9k \
  --address 0xC2FF48 --arm-frame 7000 --return-pc 0xC24D66 \
  --frames 7002 --max-instructions 10000 --ignore-future-input \
  --output build/run036_7000_c2ff48_submission_trace_no_future
```

The same-frame census is reproduced with:

```text
python scripts/engine9000_bridge.py \
  --restore captures/run036/initial_state.bin \
  --config captures/run036/config.uae \
  --playback captures/run036/playback.e9k \
  --frames 6999 --trace-frames 1 \
  --output build/run036_frame7000_instruction_trace
```
