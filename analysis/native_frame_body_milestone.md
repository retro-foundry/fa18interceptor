# Native assembled gameplay-frame comparison

The playable caller is `port/native/main.c` -> `native_frontend_tick` ->
`native_flight_tick`. After input and its single stage callback, C0EFEA begins
notification, view/records, context refresh, scene, cockpit/HUD and frame-tail
work. C0F3C0 follows the final C32CEE message update, before outer display
publication. The native runner continues to omit CPU, translations, glue, bus
and chipset objects. This batch corrects a connected caller decision and adds
comparison evidence; the diagnostic hooks themselves remove no dependency.

The original C1C860 clears its inherited sort-all flag at C1C870. Nonzero
context requests set it at C1C98A after the template children; C1E328 otherwise
sorts one pending list per update. The native caller previously always selected
all lists. A real crash-flight frame differed at SORT_LIST_NEXT (C4585D): source
0, native 1. The caller now captures the original request decision before the
children consume the bits. Bootstrap, menu transition and ordinary gameplay
all use that same native context-refresh adapter. No CPU-stack state is kept.

`--frame-capture ITERATION PREFIX` exports the actual native before/after
storage at these two boundaries, including when timer polls span PAL frames.
It requires recorded `--input`; run through at least the following iteration
so the target body finishes. JSON reports both PAL ticks and the tick saved
before the stage callback. Normal execution has no observer. The source oracle
loads these exports into a separate reference process and executes original
bytes C0EFEA-C0F3C0. It does not copy the native frame body into the oracle.
Source timer requests receive the native interval; original viewport/fade
callbacks execute for its elapsed PAL ticks. Frame timing is supplied here,
not validated by this comparison.

Eight actual native frame bodies pass with zero compared state differences and
zero display-plane differences:

| Path | Input update | Saved game tick | PAL interval | Original instructions |
| --- | ---: | ---: | --- | ---: |
| Demo selection | 1525 | 0 | 3811..3811 | 5,282 |
| Viewport wait | 1750 | 0 | 4040..4040 | 69 |
| Active demo | 2401 | 259 | 5311..5313 | 74,492 |
| Periodic readout | 2405 | 263 | 5324..5326 | 59,868 |
| Crash qualification | 2000 | 114 | 4568..4571 | 78,607 |
| Carrier approach | 6289 | 1600 | 12386..12389 | 66,128 |
| Free Flight cockpit | 3000 | 167 | 6454..6456 | 69,683 |
| Free Flight map | 3200 | 367 | 7129..7131 | 126,687 |

This is **8/8 (100% of this sampled assembled-frame batch)**, not whole-game
completion or recording acceptance. Comparison covers every byte below the
reference ABI stack (C7FC00), with explicit exclusions for ordinary native
scratch (3F30..4B10 and 306A..3080), asynchronous loaded voice descriptors and
the four voice slots, and three blitter-busy counters. It reports each excluded
difference count. Every byte of both display pages is compared without a pixel
mask. Copper fade is excluded by comparing drawing planes rather than faded
RGB. Voice/sample callbacks have their separate complete source checks; they
are not validated by this assembled-frame fixture.

Reproduce all eight using existing consumed carrier-input evidence:

```powershell
python tools/native/check_frame_body.py --carrier-input build/native-flight/carrier-game-input.fa18in
```

Without that local carrier export the default CTest covers seven paths. Use
`--case crash-flight` or repeated `--case` arguments for affected paths only.
The reference process executes just each captured body; it never repeats an
original full recording. The native test still starts from the read-only ADF
and input, never from reference RAM or pixels.

Frontend settled pixels/save/SDL/link checks and twelve reference host/loader
contracts pass. The complete 4,892-update native demo, carrier success/save/
restart/reload and crash/menu/re-entry checks pass, with existing source
checkpoints reused for the demo and carrier comparisons. Review the actual
Free Flight cockpit at `build/native-flight/frame-body-freeflight6100.png`;
its 6,100-PAL run reaches C10DAE and draws 409 HUD/scene frames without CPU or
chipset emulation.
Startup still leads the existing demo checkpoint by 36 game ticks. Full
recorded frame parity remains **0/3 accepted**. Original WaitBOVP/task pacing
is the next identified timing dependency; complete mouse callback and exact
audio fetch/filter/alignment remain open. Other modes remain outside this
sampled scope. No guessed fixed pass count or fabricated delay was introduced.
