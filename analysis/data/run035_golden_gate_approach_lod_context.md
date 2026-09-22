# run035 Golden Gate approach context

Classification: **sealed visual approach with a bounded two-checkpoint source comparison**.
This is additional Golden Gate-range evidence, not a completed LOD test.

`captures/run035` is sealed with 392 input events through replay frame 8,855.
The supplied approach begins when the Golden Gate's red pixels are visible.
Deterministic keyframes at 2,500, 4,000, 5,500, and 7,000 retain a continuous
cockpit approach over the coast; the first sampled frame (1,000) is the map
display and is excluded from the flight comparison. The local contact sheet is
`build/run035_keyframes/contact_sheet_partial.png`.

## Matched source candidate

The static matrix source `$C3B0CE` occurs in no-input probes from both the
frame-5,500 and frame-7,000 checkpoints. It belongs to the established Golden
Gate batch in earlier captures, but its individual bridge-face ownership is
not proved. Its two bounded intervals both reach projection and final display
submission:

| checkpoint | `$C1F4AC` interval instructions | `$C24CFE` reached | `$C2FF48` reached |
| ---: | ---: | --- | --- |
| 5,500 | 1,433 | yes | yes |
| 7,000 | 6,621 | yes | yes |

The intervals do **not** expose `$C355D8` in their captured `A5` values. Thus
they cannot be used to claim that the same bridge face family persists or is
replaced across these two distances. Their different interval lengths may
reflect unrelated scene work, visibility, or control flow; it is not LOD
evidence.

Authority: sealed `captures/run035`, replay checkpoints
`build/run035_keyframes/frame_{5500,7000}/state.bin`, and bounded no-input
traces `build/run035_{5500,7000}_c3b0ce_interval/trace.jsonl`.

## Result

The capture is useful as a Golden Gate approach oracle and confirms that an
established batch member continues into projection at two later approach
checkpoints. It does not yet supply the required same-bridge-face,
distance-only comparison for LOD. The prior conclusion remains: no LOD is
currently demonstrated.
