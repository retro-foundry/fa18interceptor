# run075 active projection-seed record at `$C1C54E` (global frame 389)

Classification: **scenario-backed active-record decode**.

The replay trace in `build/run075_frame384_c1c54e_projection_seed/` reaches
`$C1C54E` at local frame 189 (global frame 389) through the normal
context-guard-clear route. Although the supplied return-PC cap did not match
the caller and the trace continued afterward, the publisher itself completes
at trace indices 0--58 and returns at index 58.

`A2` resolves to `$C46184` after the `ADDA.W $C458DE,A2` selection.  The
record's observed fields are:

| Field | Value |
| --- | ---: |
| `+$14` root X | `$11183E1C` |
| `+$18` root Y | `$00007708` |
| `+$1C` root Z | `$11199DAC` |
| `+$62` type | `$11` |
| seed selected | `(0,5,$14)` |

The matrix transform yields `(223,1280,5115)` before it is added to that root.
`$C1C5E0` then publishes low words `$E7C1,$FF83,$E64E` at `$C45A72`, with
the full shifted middle longword `$FFFFFF83` at `$C45A78`.  This agrees with
the independently sampled renderer packet values in the prepared-page handoff.

These values are a trace oracle only.  The native path decodes the same source
fields through `fa18_decode_scene_projection_seed_record`; it does not embed
this record or use a replay-frame condition.
