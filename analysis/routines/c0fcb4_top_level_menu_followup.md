# `$C0FCB4`: top-level menu follow-up

Classification: **observed idle polling, static positive selection dataflow,
and one scenario-backed negative branch**.
`$C0FBE0` installs this callback immediately after populating the top-level
text-selector sequence.

The full callback is byte-exact source in two adjacent slices:
`source_amiga/observed/dispatch_top_level_menu_selection.asm`
(`$C0FCB4-$C0FDCF`) and
`source_amiga/observed/route_negative_menu_selection.asm`
(`$C0FDD0-$C0FE35`).

## Observed idle path

The flight-return trace enters this callback repeatedly.  At frame 19,425,
the `$C4582A` guard is zero and `$C458A6` reads as zero, so it follows:

```text
$C0FCF4  D0 := byte[$C458A6] = 0
$C0FCFC  signed D0 <= 0 -> $C0FDD0
$C0FDD0  D0 := byte[$C458A6] = 0
$C0FDD8  signed D0 >= 0 -> $C0FE2C
$C0FE2C  clear $C4582A
$C0FE32  return
```

This is a no-selection poll; it does not enter a mission/menu transition in
the captured window.

## Static positive-value branches

When `$C458A6` is positive, `$C0FD10-$C0FD98` resets the message-sequence
state and appends one selector code to `$C4574A`, followed by zero.  These
exact comparisons establish the following code-to-message relationships:

| `$C458A6` byte | Appended selector | Resolved payload family |
|---:|---:|---|
| `$7F` | 101 | Demo |
| `$01` | 102 | Free Flight |
| `$02` | 103 | Training: demo of maneuvers |
| `$7D` | 104 | Training: practice maneuvers |
| `$09` | 105 | Qualification |
| otherwise, if `$C45792 != 0` | 107 | Next active advanced mission |

After this append, it selects `$C0FECE` as the next callback and sets a
delay at `$C45AD6` to `$D2` or `$96` according to the static video/auxiliary
guard.  In particular, these bytes are not promoted to keyboard scancodes.

## Negative `$FF` selectable-missions branch

The signed-negative path at `$C0FDD0` resets message state and clears the
display.  When `$C458A6=$FF`, `$C0FDEA-$C0FDF8` installs `$C1017E` as the next
callback; `$C0FE24` then clears `$C458A6`.  The controlled run029 digit-6
trace proves this route: `$C1BD78` first stores `$FF`, then `$C1017E` executes
at frame 270 and writes selector queue `$0040,$806E,$0000`.  See
`analysis/routines/c1017e_build_selectable_missions_queue.md`.

The byte-exact `$C0FDD0-$C0FE35` branch is
`source_amiga/observed/route_negative_menu_selection.asm`.  Its non-`$FF`
fallback queues selector 87 before installing `$C0FE36`; that fallback has no
mission-semantic interpretation here.

One input producer is now bounded in the run024 qualification scenario:
frontend key `5` reaches `$C1BD78`, which writes `$C458A6=9`; this callback
then dynamically takes the `$09` branch and writes selector 105 at `$C4574A`.
See `analysis/run024_qualification_selection_chain.md`.  This narrow evidence
does not turn the other table values into frontend-key mappings.

The sealed run075 frame-230 key-1 path now proves the `$7F` demo arm at
`$C0FD10`: it writes selector 101, delay `$00D2`, and callback `$C0FECE`
in the frame-234 bounded trace. See
`analysis/routines/c0fcb4_run075_demo.md` for the producer, pre/post RAM,
countdown, and limits. Only that bounded branch has a native C port.
