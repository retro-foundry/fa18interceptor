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
| `$20` | byte | observed bit 0 gate | `$C1342C` packet |
| `$26` | word | shared record word updated by parent delta | `apply_parent_delta_to_shared_word.asm` |
| `$28-$2A` | bytes | read by `$C1342C` into local signed words | `$C1342C` packet |
| `$2B` | byte | input comparison byte | `prepare_indexed_control_record_context.asm`, `$C1B27E` packet |
| `$39` | byte | masked low nibble used by `$C1B27E` | `$C1B27E` packet |
| `$62` | byte | high nibble is a normal-update record-type discriminator | `prepare_normal_update_state.asm` |
| `$65` | byte | record control byte | `prepare_indexed_control_record_context.asm`, `$C1B27E` packet |
| `$66` | word | input to the observed trigonometric table path | `update_indexed_shared_record_fields.asm` |
| `$6C` | word | input to the observed limit-table path | `update_indexed_shared_record_fields.asm` |
| `$6E` | word | input to the observed secondary limit-table path; in run062 the selected base record supplies `$0A1B` to the postflight flag path's `>$03C0` gate | `update_indexed_shared_record_fields.asm`; `data/run062_c26102_postflight_record_transition.md` |
| `$72` | long | nonzero gate in the indexed-control selector | `prepare_indexed_control_record_context.asm` |
| `$76` | word | output of the secondary limit-table path | `update_indexed_shared_record_fields.asm` |
| `$78` | word | output of the trigonometric/limit path | `update_indexed_shared_record_fields.asm` |
| `$7C` | byte | signed control byte tested by the trigonometric path | `update_indexed_shared_record_fields.asm` |
| `$7D` | byte | low nibble selects the arithmetic shift applied to three candidate component words | `check_candidate_shifted_component_bounds.asm`; `data/run062_c26102_postflight_record_transition.md` |
| `$A6/$AC/$B2` | words | candidate component inputs individually shifted by `+$7D` and added to the `+$10`-derived accumulator before the negative-candidate return | `check_candidate_shifted_component_bounds.asm`; run062 transition trace |

Do not treat absent offsets as unused or the entries above as a complete object
definition. They are a reusable naming contract for byte-exact reconstructions.
