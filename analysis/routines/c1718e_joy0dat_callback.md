# JOY0DAT delta callback at `$C1718E` (Hunk 0 +`$92DE`)

Classification: **behavioural**, bounded to the observed hardware-input
contract. The callback updates two constrained control accumulators from an
Amiga joystick register. Their flight/control names are not assigned here.

`source_amiga/observed/joystick_delta_callback.asm` is a byte-exact 712-byte
reconstruction of `$C1718E-$C17455` (Hunk 0 +`$92DE`). It includes the
unobserved static mode/update branches with neutral names. VASM canonicalizes
one `ASL.L #1,D1` as an add, so the source has a documented `DC.W` escape for
that one original opcode; the assembled binary is compared directly with the
runtime bytes.

## Runtime packet

- Restored state: frame 2,696 of sealed human `run001`.
- Replay input: only the frame-2,697 joystick press (`J 0 5 1`), rebased as
  frame 1. There is no later input in this trace artifact.
- CPU read watchpoint on `JOY0DAT` (`$DFF00A`) reaches `$C17196` in frame 1.
- Breakpoint `$C1718E` returns through the real callback return `$FC134C`.
- 83 instructions, complete at that return boundary.
- P-code: `pcode/raw/run001_joystick_callback/`, 83 observed RAM starts / 490
  operations, every start mapped to verified Hunk 0.

## Observed contract

The callback reads the 16-bit `JOY0DAT` register, stores its low and high
bytes as the current sample, subtracts prior samples at `$C1AC06` and
`$C1AC08`, and normalizes each signed delta into `[-128, 127]` by adding or
subtracting `$100` on byte-counter wraparound.

It adds the first delta to `$C45776` and subtracts the second from `$C45778`.
Each resulting word is constrained using the corresponding low/high word pairs
`$C081AC/$C081B0` and `$C081AE/$C081B2`. It stores the current samples back to
`$C1AC06/$C1AC08`, increments `$C45774`, and calls `$C24FE8`.

The observed path skips the optional `$C457D8` half-delta condition and the
mode/update block at `$C172DE-$C17442`; `$C24FE8` immediately returns because
`$C457D7` is zero. The callback returns `D0 = 0` via Kickstart, not directly
to a game `JSR`. An earlier callback-stack sample included `$C1CC86`, but the
executed indirect call at that address is now directly proven to target
`$C1EE14` in this scenario. Do not infer a direct `$C1CC86 -> C1718E` edge.

## `J 0 7` differential probe

An isolated frame-3,518 recorded `J 0 7 1` event was replayed from the saved
frame-3,517 state. It reaches `$C1718E`, completes the same 83-instruction
callback through `$FC134C`, and has its own P-code export at
`pcode/raw/run001_right_joystick_callback/` (83 starts / 490 operations).

On this packet, `JOY0DAT`'s low and high samples equal the prior values at
`$C1AC06/$C1AC08`, so both calculated deltas are zero and `$C45776` and
`$C45778` remain unchanged. Therefore `J 0 7` is not evidence that either
game accumulator represents that recording code; it is only evidence that the
event reached this Engine9000 replay state without a changed `JOY0DAT` sample.

## run003 `J 0 4` differential probe

The isolated frame-6,325 `J 0 4 1` event from the sealed run003 state also
reaches `$C1718E` and returns to `$FC134C` in 83 instructions. Its canonical
P-code is `pcode/raw/run003_joy4_callback/`. The prior samples remain
`$000F/$0022`; `$C45776`, `$C45778`, and `$C4577C` are unchanged, while only
the callback count at `$C45774` rises from `$2768` to `$2769`. This recording
code likewise has no demonstrated JOY0DAT delta or game-axis meaning.

## run060 frame-939 `J 0 5` check

The sealed successful-qualification recording contains `J 0 5 1` at replay
frame 939, ten frames before the first observed non-zero root angle tuple.
A `$C1718E` breakpoint armed for that frame reaches the callback and returns
through `$FC134C` after the normal 83 instructions.  Its before/after state
is:

```text
                    before    after
$C45774 count       $005F     $0060
$C45776              $00BF     $00BF
$C45778              $00C0     $00C0
$C4577C              $00C0     $00C0
$C1AC06/$C1AC08      $0003/$0018  $0003/$0018
```

The raw `JOY0DAT` sample has the same low/high bytes as the saved prior
samples, so both callback deltas are zero.  Consequently this recorded event
does reach the input callback but does **not** directly change either bounded
accumulator in that invocation.  A later run060 control-stage trace sees
`$C45778/$C4577C=$03C0`; the transition to that held state is not assigned to
the frame-939 joystick callback by this evidence.

