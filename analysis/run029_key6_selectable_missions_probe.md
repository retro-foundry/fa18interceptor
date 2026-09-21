# Run029 controlled key-6 selectable-missions probe

Authority: sealed `captures/run029/playback.e9k` plus a controlled derivative
that changes only its frame-266/269 key-5 rows (key/character 53) to digit 6
(key/character 54).  The original capture is unchanged.

## Proven transition

The baseline frame-100 screen is the top-level `SELECT:` menu.  In the
derivative, digit 6 is pressed at frame 266.  The bounded `$C1BD78` trace
takes its `D4=5` branch:

```text
$C1BE3E  D4 := $FF
$C1BDEC  byte[$C458A6] := D4
$C1BDF2  call $C3318E
```

The press-only trace reaches `$C3318E` after 20 instructions.  A normal replay
with the matching release at frame 269 and no later input reaches frame 400
with a visibly different screen:

```text
SELECTABLE MISSIONS
<REQUIRES QUALIFICATION>
F1 - VISUAL CONFIRMATION MISSION
ESC - TO MAIN MENU
```

This is scenario-backed evidence that top-level digit 6 enters the
selectable-missions screen in this initial state.  It does not prove the
complete state transition between `$C3318E` and the screen, F1 selection,
other mission availability, or persistence.

At the frame-328 `$C32D24` breakpoint, the live selector queue is exactly:

```text
$C4574A:  $0040, $806E, $0000
```

That is selector 64 followed by the negative-form selector 110 consumed for
the `ESC - TO MAIN MENU` instruction record.  The initially inconclusive CPU
write watchpoints are superseded by the bounded `$C1017E` trace: that callback
explicitly writes this pair at frame 270; see
`analysis/routines/c1017e_build_selectable_missions_queue.md`.

## F1 under the qualification gate

A second derivative appends native F1 at frame 450 (release at 454), after the
submenu is visible.  A matching no-F1 control and the F1 replay have identical
frame-700 video SHA-256:

```text
b8eebcaae837fc5b29a9cc5dd69f4871892920b5aa71c5972d87c34c2acff5d9
```

The frame-450 `$C1BD78` trace receives `D4` low byte 10, tests a zero word at
the `$C1AB74` record pointer, and calls `$C3318E` without writing `$C458A6`.
It reaches `$C3318E` after 23 instructions.  Thus, in this unqualified
state, the visible F1 label does not establish an accepted mission-selection
transition.  Slow RAM does differ from the no-F1 control, so this is not a
claim that the event has no internal side effect.

## Artifacts

- `build/run029_key6_mode_dispatch/`: frame-266 20-instruction mode trace.
- `build/run029_key6_probe_400/screen.png`: normal-replay visible submenu.
- `build/run029_key6_prefix.e9k`: derivative input through the key release.
- `build/run029_key6_then_f1_mode_dispatch/`: frame-450 F1 input trace.
- `build/run029_key6_control_700/` and `build/run029_key6_then_f1_700/`:
  matched visible-output comparison.
