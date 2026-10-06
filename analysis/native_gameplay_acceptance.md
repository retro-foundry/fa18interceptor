# Native gameplay-frame acceptance

Latest user clarification: gameplay frames are what must match. Intro, loading
and preflight duration may differ, including loading substantially faster.
Copper fade remains excluded. This overrides earlier whole-launch timing gates.

Compare corresponding gameplay phases and ordered gameplay drawing outputs,
with equivalent consumed control input. Preserve the actual game physics,
scene/HUD composition and gameplay time dependencies. Do not require matching
intro frame counts, global uptime or an arbitrary menu-relative update number.
Do not hide changed gameplay by searching for a visually similar later frame.
Any alignment offset must come from an equivalent source gameplay transition.

Current evidence is functional scenario outcomes **3/3**, and sampled assembled
frame-body comparisons **8/8** with zero compared state/display differences.
Those bodies start from the same captured state and receive the same timer
interval. They prove composition, not that independent runs reach the same
state or produce the same complete gameplay sequence. Complete gameplay-frame
acceptance remains **0/3**; no defensible whole-game parity percentage exists.

One independently aligned gameplay checkpoint now passes. Original
start-of-update 2401 and native C0EFEA in update 2364 have game tick 222,
C10DAE and identical view/page selectors. **Both 320x200 four-plane gameplay
pages are byte-identical**, as are player position/motion, rates, orientation
and matrices. Geometry is taken from the original ViewPort; every byte of its
two page buffers is compared, without a pixel similarity mask or fade colours.
Native export: `build/native-flight/input-callback-aligned-frame2364.before.dat`;
existing original export: `build/native-flight/reference-demo2401.dat`.
The native process starts from ADF/input, never the reference state. Two
bookkeeping bytes outside the named kinematic comparison differ (+04 flags
and +4D counter); whole-state/sequence acceptance is not established.

An initial comparison mistakenly used native end-of-2364, which stops during
that update's timer wait after its physics pass. This compared before-update
source state with after-update native state and falsely suggested motion drift.
The actual C0EFEA boundary resolves it. The 36-update lead at the old
menu-relative checkpoint is startup evidence, not a gameplay failure by itself.
This checkpoint therefore supports the user's gameplay-only scope; it does
not prove that a fixed offset accepts every later phase or recording.

Reproduce without rerunning the original recording:

```powershell
python tools/native/check_gameplay_checkpoint.py --source build/native-flight/reference-demo2401.dat --iteration 2364
```

One source dependency to investigate is microsecond clock state. C16D04
publishes the host timer fraction at C45AF6. The C28782/C28CAE branches in the
geometry owner use its low word C45AF8/C45AF9 to derive signed small placement
offsets stored at C45B18/C45B1A. Native currently acquires timer samples only on
20 ms PAL boundaries. This is source-backed evidence of a possible gameplay
input difference, not evidence of a failure at this accepted checkpoint.
Original parent state, recorder cursors and clock samples should be compared
when a subsequent equivalent gameplay boundary differs before choosing a fix.

The first consecutive independent window is now retained: original updates
2401..2528 and native C0EFEA/updates 2364..2491, game ticks 222..349. **128/128
phase/control and named player motion/pose/matrix comparisons match (100% of
this window's state scope)**. There are 128 distinct original motion and drawing
states. **53/128 complete two-page drawing comparisons match (41.4% of this
window)**. The first 51 boundaries agree; the first failure is source 2452 /
native 2415, tick 273. Only 20 bytes in plane 3 of page 1 differ, in the target
information line at y=192. The source displays HDG:166 while native still
displays ALT:5110. Later failures stay in that same information-line region;
the last two boundaries agree again. This is a real gameplay HUD difference,
not Copper fade, startup duration, a physics mismatch or a similarity mask.

C25312 / `begin_main_loop_timers()` decrements INFO_REQUEST (C45886) through
C25482 when sampled seconds change; `message_line.c:info_line()` cycles
INFO_PAGE (C459C4) on a negative request. The retained source samples advance
18,000 ms across the window, versus 8,560 ms native. Both execute the existing
timer/message owners, but independent frame pacing changes their seconds-based
HUD phase. This identifies the connected timing dependency; no guessed speed
factor, constant offset or capture-fed clock has been introduced. Original
rendering/poll/display duration must be understood before correcting cadence.
The possible C28782/C28CAE clock-dependent geometry offsets do not cause this
window's drawing mismatch.

Optional diagnostic ranges preserve actual runner boundaries:
`--frame-capture FIRST+COUNT PREFIX` writes native `PREFIX.ITERATION.before.dat`
and `.after.dat`; `FA18_LOOP_DUMP=FIRST+COUNT:PREFIX` writes original
`PREFIX.ITERATION.dat`. Single-capture filenames remain unchanged. The one
bounded 6,000-PAL original prefix produced RAM/registers identical to the
previous retained prefix, and its first boundary equals the old accepted
checkpoint. Captures observe execution; they never supply behavior or pixels.
Reuse them for subsequent native changes:

```powershell
python tools/native/check_gameplay_window.py --source-prefix build/native-flight/gameplay-window-source --source-first 2401 --native-first 2364 --count 128 --out build/native-flight/gameplay-window-comparison
```

This currently exits 1 and retains `comparison.json` with every failure.
The check compares complete source drawing geometry, phase, controls and the
same named player fields as the single checkpoint. Palette/audio/full-state
acceptance is separate. Full gameplay sequence acceptance remains **0/3**.

Next: extend independent gameplay comparisons through equivalent source
phases, using existing checkpoints or bounded source windows. Distinguish
input/record/state differences from clock-driven offsets if one fails, then
correct the connected owner. Avoid a guessed
fixed update count, fabricated delay, changed physics or source RAM seeded into
the playable runner. Use expensive complete original runs only when needed to
establish or finally accept the changed gameplay sequence.
