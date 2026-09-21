# Run003 human-coverage plan

Purpose: cover documented controls absent from sealed `run002` while preserving
separable event windows for bounded replay packets. The baseline is the same
post-load menu state used by earlier runs.

## Recording command

```powershell
python scripts/record_run.py --name run003
```

Choose Free Flight and take off. Leave roughly two seconds of ordinary flight
between each action group so the resulting key windows can be isolated. Close
Engine9000 normally when finished, then seal it with:

```powershell
python scripts/finalize_run.py captures/run003
```

## Action order

1. `H` HUD toggle twice, with a pause between taps.
2. `M` map toggle twice.
3. `R` radar-range cycle three times, separately.
4. `T` target selection twice.
5. `A` hook toggle twice.
6. `J` ECM once; after a pause, test `K` once because the manual/fan-sheet
   disagreement is unresolved.
7. Backspace airbrake twice.
8. Comma and period rudder taps, separately.
9. Each cursor direction as a brief, separate hold.
10. Bracket zoom in/out taps and a few distinct numeric-keypad camera views.
11. Optional: `P` pause then resume after several seconds of wall time.

Avoid Esc, Shift+Esc, Restore, Rewind, Reset, Warp, and configuration changes.
Do not repeat gear, throttle, weapon, chaff, flare, or eject: run002 already
provides clean packets for those. This plan records a coverage experiment, not
a claim that each documented key is valid until replay-traced.
