# Native menu sound resources and startup timing

The playable caller is `fa18_native` -> `native_frontend_open` ->
`native_audio_load_menu` -> `load_intro_sound_resources` (C17510) and
`load_menu_sound_resources` (C1756A). These execute before scene bootstrap.
The existing game owners consume the resulting availability bits through
C0FCB4/C0FECE and sound requests. This removes the native startup's empty
intro/menu voice table and its incorrect 150-update demo banner selection.
CPU, translated instructions, glue, OS allocation and chipset remain absent
from this runner's link.

Original C503E8/C5058E read complete sample files. Native startup reads the
same files from the read-only ADF, initializes C5046C's 64-byte descriptors,
and duplicates C50614's exact field set with shared sample pointers and the
high length bit. C17510 configures slots 35/36 and their periods. C1756A loads
seven files, duplicates the 22 slots 13..34, sets their original repetition
counts and next-voice links, and sets availability bit 7 only after all slots
exist. Its original all-or-release failure path is preserved. No source flag
is copied from a reference capture.

Sample/descriptor storage uses three explicitly reserved regions outside
the loaded Hunks, display pages, recorder, cockpit assets and rendering
workspaces: $008000..$011FFF, $C55000..$C7DFFF and $068000..$07FFFF.
Host frontend lifetime owns these buffers; no guest allocator is recreated.

Validation:

- All eight loaded sample files (text201, texti0a, texti0b, texti1..texti5)
  match the existing original initial checkpoint byte for byte, including the
  65,590-byte last file. Every duplicated voice shares the source samples and
  carries C50614's ownership bit.
- Fifteen original instruction cases cover intro success/load/duplicate
  failures, menu success and each of seven missing sample families, C5046C
  creation/gating, and C50614 duplication with dirty scalar fields. All
  non-stack RAM matches. Parent fixtures share the file/descriptor child
  boundaries; descriptor fixtures share only platform allocations. These
  tests do not claim OS allocator or audio.device parity.
- Native demo update 2,400 now has tick 259 versus the original start-of-2,401
  tick 222. Its lead falls from 97 to 37: exactly the original extra 60 banner
  updates are restored. About **62% of that measured startup tick gap** is
  removed, not 62% of the game or all timing work.
- All 4,892 native demo updates still complete with both recorded keys and
  recorded launches. Thirty-one startup/launch source cases and view/control/
  record parents at update 2,400 and the demo end pass.
- Native frontend/menu/save/SDL/link-omission regressions pass. Both MSVC
  reference runners build and their twelve focused CTests pass. Native MSVC
  and GNU fixture builds pass; the ADF remains unchanged.

Intro and menu sample initialization are connected (2/2 source parents).
The separate C1787A engine/alert/noise/programmed sample initialization remains
open: native SOUND_FLAGS is $80 versus the reference's $F7. In particular,
its C50A58 square-wave and C50B36 generated-noise consumers must execute from
their original owners, preserving the random seed. Per-tick voice programs,
sample playback and audible host output remain open. Remaining message/viewport
and result cadence also remain open; native full frame parity is still
**0/3 accepted**. Copper fade is excluded. Existing reference artifacts were
reused; no new full original replay was run.

Run `python tools/native/check_sound_resources.py`. Optional
`--source-initial build/native-flight/reference-demo-initial.dat` and
`--source-checkpoint build/native-flight/reference-demo2401.dat` reuse the
existing original artifacts for exact file and startup-gap comparisons.
