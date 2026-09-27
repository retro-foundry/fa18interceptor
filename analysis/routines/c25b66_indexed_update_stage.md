# `$C25B66` indexed update stage

Classification: **behavioural indexed-update evidence**. The record type and
subsystem are not yet named.

## Runtime packet

- Restored state: `run001` frame 2,696 plus its sole rebased joystick press;
  no later input occurs while the interval is instruction-stepped.
- The live call edge is `$C22D88 -> $C25B66 -> $C22D8E` at replay frame 17.
- It completes at that real return boundary in 2,688 instructions.
- P-code: `pcode/raw/run001_c25b66_update_stage/`, 1,803 observed RAM starts,
  11,570 P-code operations, and 31 dynamically reached call-target entries.

## Observed sequence

The stage calls `$C13D84` at `$C25D7E`, giving the previously bounded
joystick-state consumer its direct enclosing invocation. Later it calls
`$C2D408`, whose observed route enters the existing matrix helpers
`$C2DD4E`, `$C2DEE0`, `$C2DF02`, `$C2D968`, and `$C2D996`. It also reaches
`$C149BE` and the downstream `$C2600E -> $C26EBE` path.

The packet therefore joins the verified joystick-state, indexed-record, and
matrix work inside one complete per-record update interval. It does not yet
prove that this is the player, camera, or a particular aircraft record.

## run075 zero-index fast return

`build/run075_c25b66_indexed_update/trace.jsonl` reaches `$C25B66` at local
frame 189. `$C459B4` is zero, so the entry takes `$C25B78`; it ORs the zero
selector with `$C457AE=1`, branches to the return at `$C25B64`, and returns to
`$C22D8E` after six instructions. This proves only the zero-index/nonzero-state
fast return. The nonzero-index route remains a separate unported boundary.

## run001 zero-index clear-state route

At the start of `build/run001_c25b66_update_stage/trace.jsonl`, `$C459B4` and
`$C457AE` are both zero. The entry therefore reaches `$C25B80`; with
`$C45785` also clear, it branches to `$C25C3E`. The record byte at `A1+2` has
bit 0 clear, so `$C25C44` branches to `$C25C54`.

This establishes the zero-index clear-state normal route and the next bounded
record flag gate. `$C25C3E-$C25C45` tests only bit 0 of the caller-owned
record byte at `+2`: clear reaches `$C25C54`, while set falls through to the
still-unported `$C25C46` continuation. It does not establish either
continuation's record semantics.

## control-word branch after the clear record flag

The same run001 trace reaches `$C25C54` with `$C458CC=$0840` and
`$C459B4=0`. The `$0040` mask is set, so `$C25C62-$C25C69` branches to the
bounded `$C25C70` control-stage call. The other source-defined outcomes are a
clear mask to `$C25D86` and a set mask with nonzero selected index falling
through at `$C25C6A`; neither assigns record semantics.

## control-stage return routing

`$C25C70-$C25C85` first calls `$C1B27E`, then sends a nonzero selected index
to `$C25D22`. With a zero index it reads the signed longword at selected-record
offset `+$42`: nonnegative reaches `$C25CCA`, while negative falls through to
the still-unported `$C25C86` continuation. The `$C1B27E` control-record body
remains a separately evidenced caller boundary.

## matrix-dispatch class and header gates

`$C25D86-$C25DA5` masks selected-record class byte `+$62`. A high nibble of
`$30` reaches the `$C2D408` matrix-dispatch call directly. Other classes test
header byte `+0` bit 7: clear also calls `$C2D408`; set continues at
`$C25DA6`. The call preserves the current record pointer around the child and
does not identify the record class's scene meaning.

## selected-record eligibility gate

`$C25D22-$C25D3F` compares `$C459B4` with `$C458DC`, then tests `$C45785`
and selected-record byte `+3` bit 0. An index mismatch, nonzero context, or
clear bit reaches `$C25D5E`; equal indices with clear context and the bit set
fall through to the still-unported `$C25D40` continuation.

## conditional selector call

`$C25D5E-$C25D85` tests selected-record `+$20` bit 1, class byte `+$62` high
nibble `$10`, and header byte `+0` bit 4. Only clear/set/set respectively
calls `$C13D84`; all outcomes then reach `$C25D86`. The selector retains its
separate established record-selection semantics.
