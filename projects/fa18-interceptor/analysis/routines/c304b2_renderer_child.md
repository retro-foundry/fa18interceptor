# Renderer child at `$C304B2`

Classification: **structural**. The direct run001 edge `$C3002A -> $C304B2 ->
$C3002E` completes in 14 instructions at frame 9, with no nested calls.

P-code: `pcode/raw/run001_c304b2_renderer_child/`, 14 observed starts /
78 operations. Its register and custom-register effects remain separate from
the sibling `$C30466` setup path until their shared caller contract is traced.


`source_amiga/observed/setup_blitter_operation.asm` is the byte-exact 72-byte
function `$C304B2-$C304F9`. It waits on custom-register bit 6, programs
`BLTCON0/1`, `BLTCPT`, `BLTBPT`, `BLTAPT`, and `BLTSIZE`, then returns.

## Port contract

The leaf does not select a display page or own a graphics resource. Its caller
has already produced the lane pointer and size word. The observed dataflow is:

1. Copy the size word from `$C4596E` and the lane pointer from `$C45960`.
2. Copy that pointer into the A, B, and D blitter channels.
3. Wait for the blitter idle bit, write `BLTCON0=$0D0C` and `BLTCON1=$0002`,
   then trigger with the caller supplied size.

The source contains a `$0D3C` control word after the `$0D0C` write, but an
unconditional branch skips it on the observed path. It is therefore retained
as an alternate source path in the evidence record, not selected by the native
port. Since the three channel pointers are equal at this leaf, their semantic
page roles cannot be inferred from C304B2 alone; that assignment belongs to the
caller and its selected renderer lane.

## run075 prepared-page caller witness

The return-bounded run075 capture at local replay frame 182 (the prepared-page
window) reaches this leaf with `BLTSIZE=$0E14` and the shared A/B/D lane
pointer `$000076EE`. That is a 20-word by 56-row operation. The saved stack
has `$C3002E` as the immediate return PC and `$C24D66` as its caller's return
PC, establishing the source path:

```text
$C24D60 polygon wrapper -> $C2FF48 -> $C2FF58 lane tail -> $C3002A -> $C304B2
```

This is distinct from the `$C279D0` Hunk-25 invocation sampled during the
same page-preparation interval: its two `$C2FF48` calls take the direct-line
continuation and do not reach this fill leaf. A return-bounded capture at the
first `$C24D60` entry in the same frame now identifies its caller return as
`$C2AFE8`. That is the `$C2AA9C-$C2AFF9` map-packet pass's display-stage
continuation, so this prepared-page fill comes from the static-map packet
transform -> `$C246A0` clip -> `$C24CFE` projection path, rather than an
unclassified generic `$C24D` producer. The source-backed native composition
exists in `map_packet_parent_pass`, `map_packet_polygon_display`, and the
polygon/page-blitter modules; its still-missing owner is the live parent state
and page handoff. The observed pointer and size are diagnostic evidence only,
not native runtime constants.
