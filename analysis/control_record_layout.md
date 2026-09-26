# Observed 512-byte control-record layout

The current control record is selected as `$C46184 + (index << 9)`. The index
source differs by route (`$C458DC` or `$C459B4`), while `$C18210` publishes the
selected base for downstream helpers. The fixed stride is evidence for a
512-byte record; the field names below only describe observed accesses.

| Offset | Width | Observed role | Evidence |
| ---: | --- | --- | --- |
| `$02` | word | shared record word; read/written by an observed parent-delta helper | `apply_parent_delta_to_shared_word.asm` |
| `$03` | byte | flags, including observed bit 7 test | `update_indexed_shared_record_fields.asm` |
| `$04` | byte | flags, including observed bit 3 and bit 4 tests | `$C1342C`, `$C1B27E` packets |
| `$10` | long | copied into the accumulated candidate term before the shifted `+$A6/$AC/$B2` comparisons | `prepare_c27968_candidate_result.asm`; run062 transition trace |
| `$14` | long | lateral/horizontal-X candidate of the active qualification-flight pose tuple; committed by `$C25E6E` | `data/root_record_pose_candidate.md`; `data/run060_root_axis_orientation_inference.md`; `data/run060_root_pose_integrator.md` |
| `$18` | long | active flight vertical-Y altitude and cockpit display source: arithmetic `>>10`, then `*5`, with an alternate formatter route carrying literal `FT`; run060 root value converts to 145 and the deterministic end screen shows 145 FT; `$C14D32` commits traced descent updates | `format_record_offset18_with_optional_ft.asm`; `data/run060_root_altitude_formatter.md`; `data/run060_root_axis_orientation_inference.md`; `data/run060_root_pose_integrator.md` |
| `$1C` | long | initial-forward/horizontal-Z candidate of the active qualification-flight pose tuple; committed by `$C25E72` | `data/root_record_pose_candidate.md`; `data/run060_root_axis_orientation_inference.md`; `data/run060_root_pose_integrator.md` |
| `$20` | byte | observed bit 0 gate | `$C1342C` packet |
| `$26` | word | shared record word updated by parent delta | `apply_parent_delta_to_shared_word.asm` |
| `$28-$2A` | bytes | read by `$C1342C` into local signed words | `$C1342C` packet |
| `$2B` | byte | input comparison byte | `prepare_indexed_control_record_context.asm`, `$C1B27E` packet |
| `$39` | byte | masked low nibble used by `$C1B27E` | `$C1B27E` packet |
| `$3E/$42/$46` | longs | signed scaled motion-delta terms; `+$42` is the active-route vertical displacement added to `+$18` | `data/run060_root_motion_delta_terms.md` |
| `$62` | byte | high nibble is a normal-update record-type discriminator | `prepare_normal_update_state.asm` |
| `$65` | byte | record control byte | `prepare_indexed_control_record_context.asm`, `$C1B27E` packet |
| `$66/$68/$6A` | three words | active orientation-angle state: `$C2D94E` republishes the triple then `$C2E514` composes root `+$92..+$A2`; `$66` is pitch-like in the initial run060 basis and also feeds an observed trigonometric path | `publish_record_matrix_update_triple.asm`; `data/run060_root_attitude_matrix.md`; `data/run060_root_axis_orientation_inference.md` |
| `$6C` | word | input to the observed limit-table path | `update_indexed_shared_record_fields.asm` |
| `$6E` | word | input to the observed secondary limit-table path; in run062 the selected base record supplies `$0A1B` to the postflight flag path's `>$03C0` gate | `update_indexed_shared_record_fields.asm`; `data/run062_c26102_postflight_record_transition.md` |
| `$72` | long | nonzero gate in the indexed-control selector | `prepare_indexed_control_record_context.asm` |
| `$76` | word | output of the secondary limit-table path | `update_indexed_shared_record_fields.asm` |
| `$78` | word | output of the trigonometric/limit path | `update_indexed_shared_record_fields.asm` |
| `$7C` | byte | signed control byte tested by the trigonometric path | `update_indexed_shared_record_fields.asm` |
| `$7D` | byte | low nibble selects the arithmetic shift applied to three candidate component words | `check_candidate_shifted_component_bounds.asm`; `data/run062_c26102_postflight_record_transition.md` |
| `$92-$A2` | nine words | active flight orientation-transform matrix: `$C2E514` composes it from root angle state and it transforms a selected seed before its three components are added to `+$14/+18/+1C` | `select_record_matrix_component_seed.asm`; `data/root_record_pose_candidate.md`; `data/run060_root_attitude_matrix.md` |
| `$A6/$AC/$B2` | words | candidate component inputs individually shifted by `+$7D` and added to the `+$10`-derived accumulator before the negative-candidate return | `check_candidate_shifted_component_bounds.asm`; run062 transition trace |

Do not treat absent offsets as unused or the entries above as a complete object
definition. They are a reusable naming contract for byte-exact reconstructions.
In particular, the root table slot `$C46184` is not established as the full
moving player position/orientation record. In run060 it does supply a
selected feet-valued cockpit-display field; see
`data/run060_root_altitude_formatter.md`.
