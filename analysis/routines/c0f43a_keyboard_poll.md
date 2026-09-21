# `keyboard_poll_and_dispatch` at `$C0F43A` (Hunk 0 +`$158A`)

Classification: **behavioural**. This is the exact runtime manifestation of
the WHDLoad `$1590` keyboard landmark: Hunk 0 payload base `$C0DEB0` plus
`$1590` is `$C0F440`.

The bounded loop calls `$C16C56`, receives an input-event value in `D0`, and
treats low-byte `$FF` as “no event.” Any other value is zero-extended and passed
to `$C1AD74`.

Full-frame Engine9000 evidence, armed at frame 450:

| Replay | `$C0F440` result | `$C1AD74` | Result |
| --- | --- | --- | --- |
| Documented `H` tap | `D0 = $30000025` (raw byte `$25`) | entered with `D0 = $25` | hit in frame 455 |
| Same demo, no `H` | `D0 = $000000FF` | not entered through frame 460 | control miss |

The native frontend records virtual `H` as `K 104 104 16`; the core presents it
to this game code as raw Amiga key byte `$25`. `$C1AD74` is therefore named
`dispatch_raw_key_event` provisionally. Its switch/table semantics and the
meaning of its downstream state changes are not yet recovered.

## Attract-mode dispatcher path for raw `$25`

The recorded `H` event was delivered during ordinary full-frame replay. Once
execution reached `$C1AD74`, a separate no-future-input trace stepped only to
the observed return PC `$C0F45C`. It contains 99 instructions and no calls;
all 99 starts map to verified Hunk 5 (payload base `$C1AC18`). The matching raw
P-code packet is `pcode/raw/h_key_dispatch/`.

This trace is valid only for this post-event window: playback has no recorded
event after the breakpoint. It proves the control flow below, but does not
validate held-input stepping generally.

At `$C1AD84`, the dispatcher reads `$C457D5`; its positive branch is taken. It
then reads `$C45785`, `$C45878`, and `$C458AE`. The byte at `$C4584B` is `3`,
so `$C1ADEC` branches to `$C1B030`. From there raw `$25` does not match any
observed comparison and reaches `$C1C23C`.

That fallback records the raw byte in bounded state at `$C457E1` and another
table-derived byte at `$C457EB`, increments `$C457F7` and `$C457F9`, then
clears `$C45878-$C4587A` before returning. These are literal observed memory
effects; field meanings remain unknown. The static `$25` compare at `$C1AEB8`
and its `$C1B264` target were not executed in this attract-mode path, so this
does not establish a HUD action in the demonstrated state.

Live caller addresses at the poll site include `$C15DA2` and `$C0EFE0`; these
are observed callstack entries, not main-loop names.

The direct static caller is `$C0F3C4` (verified Hunk 0); its bounded contract is
documented in [`c0f3c4_input_phase.md`](c0f3c4_input_phase.md). One level up,
`$C0EFD4` creates a stack local from `$C458DA`, calls `$C0F3C4` at `$C0EFE0`,
then calls `$C0F5F8` and `$C11B44`. It continues through many later calls when
`$C45795` is non-zero. A conservative trace from `$C0EFD4` did not return to
the `$C15DA2` call site within 10,000 instructions, so it remains a long-lived
structural caller rather than a bounded frame-loop proof.

## run002 `G` command

The sealed carrier-flight run supplies an independent raw-key case: frontend
`K 103` (`G`) reaches the dispatcher with `$24`, sets `$C4599A` bit 0, and
toggles `$C46200` bit 7. See
[`c1ad74_gear_dispatch.md`](c1ad74_gear_dispatch.md).
