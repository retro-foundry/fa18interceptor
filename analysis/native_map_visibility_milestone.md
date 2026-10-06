# Native terrain visibility and first takeoff checkpoint

The real caller is native flight -> shared scene sequence -> map packet pass
-> map detail fields. Native tick 7294 produced metric -6; the bounded earlier
reconstruction rejected its negative table index. C2AE88-C2AE8E actually
reads C2ADF8 plus a doubled signed word offset, including preceding image data.
The active map caller now supplies a checked resolver over the loaded original
control payload. It uses source word wrapping instead of clamping negative
indices or inventing limits.

C2AECA's X branch also preserves the compared coordinate's low word for the
later C2AF40 alternate-stream test. The previous normalized boolean could
choose a different stream when that word was zero. Y's branch and the forced
route retain the source's explicit selector value 1.

Validation:

- 384 original-instruction C2AE66-C2AEF8 cases match both coordinate terms,
  the exact selector word and visibility-limit result. Includes negative and
  wrapped indices, source image words, zoom gates and far-X zero-word cases.
- The actual 7294-tick native pullback checkpoint now completes with negative
  metric -6. Complete horizon/map plane buffers and 160 polygon cases match
  original instructions at that checkpoint.
- The 7150-tick checkpoint proves a short native takeoff: source ground bit
  7 of record +2 clears, height rises from 1800 to 264160, and source takeoff
  bookkeeping (+0x21 bit 0) is published. C12098/C1C63E match original
  non-stack RAM; positive strip rendering and all 29 reached descriptors pass.
- Native MSVC and GNU oracle builds pass. Reference MSVC rebuild and all twelve
  CTests pass. Native link still omits CPU/chipset/glue.
- `tools/native/check_map_limits.py` and `tools/native/check_strips.py` reproduce
  the reached terrain and takeoff checkpoints. Both are registered CTests.

This batch is complete. Rough Free Flight startup wiring estimate is now 99%
(previously 98%): source-controlled takeoff is demonstrated. This estimate
excludes whole-game completion and recorded-frame acceptance. The probe's
long pullback later returns to the ground; postflight reaches MC_STAGE_SETUP
(C0F4A6 free_all_voices), still unconnected in the native setup consumer.
Complete frame/Stores/end-of-frame ownership, alternate display pages,
postflight/restart and recorded-flight acceptance remain open. The runner's
historical screen label is still `scene-setup`; aircraft state demonstrates
flight independently of that UI label. Copper fade is excluded. No full sealed
replay repeated and no substitute gameplay added.
