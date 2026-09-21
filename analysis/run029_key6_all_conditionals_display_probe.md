# Run029 controlled selectable-label display probe

Authority: sealed `captures/run029/initial_state.bin` and the controlled
key-6 prefix `build/run029_key6_prefix.e9k`. No captured input or authority
file was modified.

At the `$C1017E` breakpoint on frame 270, debugger writes set only
`$C06AAD-$C06AB2` to one. The routine was instruction-stepped to its normal
return at `$C101FA`, producing this queue in the saved post-step Slow RAM:

```text
$C4574A:  $0040, $004D, $004E, $004F, $0050, $0051, $0424, $806E, $0000
```

The post-step `final_state.bin` was then resumed for 180 ordinary no-input
frames with the same Engine9000 configuration. Its screen is retained at
`build/run029_key6_all_conditionals_screen/screen.png` and visibly contains:

```text
SELECTABLE MISSIONS
<REQUIRES QUALIFICATION>
F1 - VISUAL CONFIRMATION MISSION
F2 - EMERGENCY DEFENSE OPERATION
F3 - INTERCEPT STOLEN AIRCRAFT
F4 - SEARCH AND RESCUE OPERATION
F5 - INTERCEPT INCOMING CRUISE MISSILE
F6 - CARRIER SUB MISSION
ESC - TO MAIN MENU
```

This is a controlled display/dataflow result. Relative to the unmodified
key-6 state, forcing condition bytes 3--7 makes the F2--F6 lines visible;
F1 remains independently visible. It does not establish a legitimate player
state, a qualification rule, normal reachability or meaning of the forced
`$0424` word, or any
mission-selection/outcome transition.

## Successive record-selection traces

Five no-input breakpoint continuations from each saved post-selector state
then reach `$C32D24` in this exact order:

| Record trace | Incoming `D0` | Static descriptor |
|---|---:|---|
| `first_record` | 77 | `$C3F46C` (F2) |
| `second_record` | 78 | `$C3F494` (F3) |
| `third_record` | 79 | `$C3F4BA` (F4) |
| `fourth_record` | 80 | `$C3F4E2` (F5) |
| `fifth_record` | 81 | `$C3F510` (F6) |

Each entry returns normally to `$C32F5C` after 43 instructions. This directly
proves the appended selector words' ordered record consumption in the
controlled state. It still does not establish the normal writer or meaning of
the forced condition bytes.

## Out-of-inventory sixth selector

The sixth continuation receives `D0=$0424` at `$C32D24` and also returns to
`$C32F5C` after 43 instructions. Its indexed table read is outside the
inventory's selectors 1--110: it reads relative word `$4353` at `$C3F550` and
forms `A2=$C4305D`. This proves only that this forced positive selector is
consumed by the same generic record path; it does not establish that `$0424`
is a valid normal selector, that `$C4305D` is mission text, or that this path
is safe/reachable in unmodified play.

## Native F2 probe with its label visible

From the same post-`$C1017E` state, a matched normal replay sends frontend F2
(`K 283 0 16 1` at frame 180; release at frame 184). At frame 360 its video
SHA-256 is identical to the no-input control:

```text
e7b6cbc8afb0e5fb5cac4613f3bc6dc705481bcd575398660ebd26a59c51d44a
```

The full press/release differential changes 95 Slow-RAM bytes, so it is not a
no-op. A press-only no-future-input trace reaches `$C1BD78` at frame 180 with
raw `D0=$51` and `D4` low byte `$0B`. It finds `$C458A6=0`, loads
`$C1AB74=$C06A98`, tests its zero word at offset zero, and branches to
`$C1BDF2 -> $C3318E` without storing `$C458A6`. The bounded route returns to
`$C1BDF8` after 1,946 instructions and includes `$C17EF2`.

Thus a visible forced F2 line does not, in this pointer-state configuration,
prove an accepted Emergency Defense mission selection. Its changed internal
state is preserved as a separate dataflow result.
