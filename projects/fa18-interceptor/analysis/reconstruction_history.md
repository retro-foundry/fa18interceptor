# Historical F/A-18 reconstruction results

Extracted from the original reverse-engineering guide. These are historical
observations and estimates, not qualification of the current framework or native
checkout. The runtime described in the earlier guide was retired; its recorded
proofs require the commits named in the project evidence.

The reusable workflow now lives in [the framework guide](../../../REVERSE_ENGINEERING.md).

## 0. What went wrong the first time

The first ~2,000 commits ported the game bottom-up, one routine slice at a
time, each with its own contract test. After about 400 C modules and 208
passing tests, the native build still drew zero pixels at the first cockpit
frame. The causes:

- **No end-to-end metric.** Tests and "bytes reconstructed" rose while
  "frames that match" stayed at zero.
- **Scaffolding was forbidden.** No placeholders, no emulator-derived state,
  no display until everything was finished, so the first visible result was
  the last thing built.
- **Understanding was a prerequisite for running.** Naming a routine,
  proving it and exporting its P-code came before any code executed.
- **Handoffs grew without limit** (5,000+ lines), so each session spent its
  attention on history.


Where this ended here: per-scene error 0.01-0.5%, and 10/10 exact frames in
the parity window. That is not enough for a 10,000-frame replay: a beam-wait
loop entered 10 colour clocks early takes one extra pass, and the game
state diverges from there. Exact long replays need the emulator's own
cycle-exact CPU and blitter arbitration ported whole. Decide early whether
exact long replays are a requirement; if so, port the emulator's timing
model instead of approximating it (lesson 12.8).


## Time budget

| Step | Target |
| --- | --- |
| 1-2. Emulator, determinism, recordings, formats | 1-2 days |
| 3. Executable loading, first memory map | 1 day |
| 4. Ghidra import and one P-code export | 1-2 days, then ongoing reading |
| 5. Machine layer, interpreter frames from a snapshot | 2-3 days |
| 6. Translation with fallback, first translated frame | 3-5 days |
| 7. Timing (to the level the recordings demand) | 1-2 weeks; port the emulator's model if exact replays matter |
| 8. Parity on the recordings, live window | alongside 7 |
| 9. Readable C | the bulk of the project, always shippable |
| 10-11. OS replacement, cold boot, native backend | after most routines are C |
