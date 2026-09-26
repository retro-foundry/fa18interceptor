# Observed 512-byte control-record layout

The current control record is selected as `$C46184 + (index << 9)`. The index
source differs by route (`$C458DC` or `$C459B4`), while `$C18210` publishes the
selected base for downstream helpers. The fixed stride is evidence for a
512-byte record; the field names below only describe observed accesses.

| Offset | Width | Observed role | Evidence |
| ---: | --- | --- | --- |
| `$01` | byte | `$C0A2F0` requires bit 6 for its selected-record scheduler setup. In direct-core run060 analysis, `$C1CA8C` temporarily sets bit 4 and `$C1D65E` clears it again; neither observed write changes the required bit 6. | `data/run060_root_header_scheduler_operands.md`; `set_context_workspace_bit4.asm` |
| `$02` | word | `$C0A2F0` requires `(word & $C080) == $C080` for its selected-record scheduler setup. `$C1B630` is a byte-exact raw-$20 arrestor-hook producer that can toggle bit 15, but direct-core timing does not establish it as the native run060 writer. The independently required bit 7 has no identified producer. | `data/run060_root_header_scheduler_operands.md`; `toggle_arrestor_hook.asm` |
| `$03` | byte | flags, including observed bit 7 test | `update_indexed_shared_record_fields.asm` |
| `$04` | byte | flags, including observed bit 3 and bit 4 tests | `$C1342C`, `$C1B27E` packets |
| `$10` | long | copied into the accumulated candidate term before the shifted `+$A6/$AC/$B2` comparisons | `prepare_c27968_candidate_result.asm`; run062 transition trace |
| `$14` | long | lateral/horizontal-X candidate of the active qualification-flight pose tuple; committed by `$C25E6E` | `data/root_record_pose_candidate.md`; `data/run060_root_axis_orientation_inference.md`; `data/run060_root_pose_integrator.md` |
| `$18` | long | active flight vertical-Y altitude and cockpit display source: arithmetic `>>10`, then `*5`, with an alternate formatter route carrying literal `FT`; run060 root value converts to 145 and the deterministic end screen shows 145 FT; `$C14D32` commits traced descent updates | `format_record_offset18_with_optional_ft.asm`; `data/run060_root_altitude_formatter.md`; `data/run060_root_axis_orientation_inference.md`; `data/run060_root_pose_integrator.md` |
| `$1C` | long | initial-forward/horizontal-Z candidate of the active qualification-flight pose tuple; committed by `$C25E72` | `data/root_record_pose_candidate.md`; `data/run060_root_axis_orientation_inference.md`; `data/run060_root_pose_integrator.md` |
| `$20` | byte | observed bit 0 gate | `$C1342C` packet |
| `$26` | word | shared record word updated by parent delta | `apply_parent_delta_to_shared_word.asm` |
| `$28-$2A` | signed bytes | bounded three-axis control lanes. `$C1B410` updates them from separate two-bit fields in `+$65`; `$C1342C` uses `+$28` as a signed table lane. In the initial run060 turn, `+$28=-2` selects the first pitch-like target while the other two are zero. In run003 frame 5,360, comma's `$80` mask changes `+$29` from 0 to -1 while the other lanes remain zero. | `update_three_axis_control_bytes.asm`; `routines/c1b410_update_three_axis_control_bytes.md`; `routines/c1342c_matrix_side_leaf.md` |
| `$2B` | byte | input comparison byte | `prepare_indexed_control_record_context.asm`, `$C1B27E` packet |
| `$39` | byte | masked low nibble used by `$C1B27E` | `$C1B27E` packet |
| `$3E/$42/$46` | longs | signed scaled motion-delta terms; `+$42` is the active-route vertical displacement added to `+$18` | `data/run060_root_motion_delta_terms.md` |
| `$62` | byte | high nibble is a normal-update record-type discriminator | `prepare_normal_update_state.asm` |
| `$65` | byte | packed control byte. Its bits 5:4, 7:6, and 3:2 drive the signed `+$28`, `+$29`, and `+$2A` lanes respectively; low bits are used by other control routes. The initial run060 turn uses `$21`, whose `$20` field decrements `+$28`; run003 comma uses `$80`, which decrements `+$29`. | `update_three_axis_control_bytes.asm`; `routines/c1b410_update_three_axis_control_bytes.md`; `data/run060_control_mask_turn_timeline.md`; `routines/c1b58e_set_rudder_input_mask.md` |
| `$66/$68/$6A` | three words | active orientation-angle state: `$C2D94E` republishes the triple then `$C2E514` composes root `+$92..+$A2`; `$66` is pitch-like in the initial run060 basis and also feeds an observed trigonometric path | `publish_record_matrix_update_triple.asm`; `data/run060_root_attitude_matrix.md`; `data/run060_root_axis_orientation_inference.md` |
| `$6C` | word | input to the observed limit-table path | `update_indexed_shared_record_fields.asm` |
| `$6E` | word | input to the observed secondary limit-table path; in run062 the selected base record supplies `$0A1B` to the postflight flag path's `>$03C0` gate | `update_indexed_shared_record_fields.asm`; `data/run062_c26102_postflight_record_transition.md` |
| `$72` | long | nonzero gate in the indexed-control selector | `prepare_indexed_control_record_context.asm` |
| `$76` | word | output of the secondary limit-table path | `update_indexed_shared_record_fields.asm` |
| `$78` | word | output of the trigonometric/limit path | `update_indexed_shared_record_fields.asm` |
| `$7B` | byte | signed entry gate for `$C27968`: a negative value directly returns zero before the class/component checks; otherwise its low nibble must be zero to enter the three-component path. Run060 samples use `$FF` and bypass; run062's failure invocation uses zero and enters. | `prepare_c27968_candidate_result.asm`; `data/run060_run062_candidate_return_comparison.md`; `data/run062_c26102_postflight_record_transition.md` |
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
