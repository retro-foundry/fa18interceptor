# `$C3BE4C` run034 renderer-context comparison

Classification: **scenario-backed retreat-path source-to-face join plus a rejected far-path batch association**. It is not LOD evidence.

## Matched static input

No-input `$C0F090` windows restored from run034 checkpoints at replay frames 4,200 and 9,500 both execute `$C1F4AC` with the same immutable input `$C3BE4C = (576, 4224, -576)`, destination `$C48390`, and matrix `$C45BD8`. `$C48390` is a mutable transform workspace, so this does not export a terrain vertex or establish a world position.

## Retreat source-to-face join

The new source-interval trace begins exactly at the retreat `$C1F4AC` entry and records 4,061 instructions to the next matrix entry. It establishes this bounded path:

`$C3BE4C` → `$C3BE6A` at `$C1F6F8` → `$C3BBF4/$C3BBF6` renderer context → five `$C2005C` face preparations → three `$C2FF48` polygon submissions.

The three final submissions have static `A5=$C3BBF6` and are four-point polygons. This proves a partial immutable source/control/face path for the retreat checkpoint, not a complete standalone mesh.

## Transform-space gate before the control stream

The matched traces identify why the far sample lacks this source-to-face path.
Both runs transform `$C3BE4C` through `$C1F4AC-$C1F4F6`; both then test the
same live word at `$C54FE4` (`-$62(A6)`). Neither sampled path takes the
`$C1F4FC → $C1F524` branch. The next signed transformed-value branch differs:

| Checkpoint | `D4` at `$C1F500` | observed branch result |
| --- | ---: | --- |
| far 4,200 | `-2240` | `$C1F500 ble.b $C1F51E`, followed by return at `$C1F522`; no `$C1F6F8` source-control entry |
| retreat 9,500 | `+2176` | passes `$C1F500` and the following `$C1F50A-$C1F51C` comparisons; enters the multi-triple loop and then `$C1F6F8` |

Thus the different result occurs **before** the static `$C3BBF4/$C3BBF6`
control stream is selected. It is a transform-space inclusion/range gate for
this source, not evidence of a low-detail versus high-detail model replacement.

## Rejected far association

The far interval runs 50,944 instructions before the next matrix entry. Its first `$C1F6F8` belongs to `$C3515A`, and its later `$C1F6F8/$C1F910` records use `$C089A8`; neither follows `$C3BE4C`. The previous observation of three polygons and three line lists in that broad interval is therefore **not** a source-to-renderer association and must not be compared with the retreat `$C3BBF6` result.

## Consequence

The next useful work is to catalogue analogous source gates and their static
face/control streams across the flight checkpoints. Only a branch that selects
an alternate static model after a shared object has passed comparable
visibility conditions, plus an independent distance tie, could become LOD
evidence.

Authorities: `analysis/data/run034_{far,retreat}_matrix_inputs.json`; `build/run034_retreat_c3be4c_interval_trace/trace.jsonl`; and `build/run034_far_c3be4c_interval_trace/trace.jsonl`.
