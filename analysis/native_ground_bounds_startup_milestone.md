# Ground-strip startup initialization — 2026-10-09

The native runtime omitted original startup C0F50E -> C2527C. This step walks
the disk-backed directory at C44880 and generates six corner words per strip
record using C2574A normalization and the original four coefficient pairs.
Those words start at zero in the relocated executable. The original has all
40 records initialized before gameplay; native previously left them zero.

This explains the first independently recorded missing ground segment.
Descriptor C22A48/C096CA selects the C4532C ground stream. Its polygon uses
generated corners, including C453A2-C453AC and C453B6-C453C0. With zero corners
it returns zero in native's fixture, although the actual original ground
fixture returns one. C1CCAA caches a negative result after a rejected draw;
subsequent visits then skip the descriptor until its countdown expires.
Ground strips can consequently disappear when their distance-dependent
stream changes. This is a missing startup producer, not justification for
disabling the original culling or retry rules.

## Connected change

`native_frontend_open` now calls `native_model_prepare_ground_bounds` alongside
the existing startup initialization, after disk hunks are relocated. The
renderer supplies the real endpoint deltas to the existing native C2574A
normalization and calls the existing `prepare_setup_bounds` implementation.
The original table chooses the records and coefficients. No captured vertices
are loaded into gameplay, no corners are hand-authored, and no placement gate
or clock is overridden. Emulator/ROM execution remains external evidence only.

The constructor -> generated asset corners -> C096CA -> command geometry ->
native raster -> display path is exercised. State migration remains outside
the active completion goal; this patch restores the existing producer.

## Validation and limits

The complete initializer matches original instructions across all non-stack
RAM. A live one-tick startup capture proves every one of the 40 records
already has its original corners, before any gameplay update or scene draw.
All 240 derived words are checked. Clearing just those generated words in an
external oracle fixture reproduces the omitted-initializer failure, which the
new regression rejects. The new `fa18_native_ground_bounds` CTest and its
artifact-cleanup fixture pass in Release and Debug. Three connected setup
checkpoints and their existing model/scene comparisons also pass.

The independently started successful mission-three replay passes every one
of its 4,967 gameplay observations and 79,472 complete aircraft record cores
in Release and Debug. Complete traces, final RAM, counters, outcomes and saves
agree between builds. Complete drawing matches improve from 267 to 287;
the first remaining strict page difference moves from 22,286 to 22,303.
Other drawing and clock/HUD differences remain explicit and unaccepted.

All eight complete pages at observations 22,285-22,288 now match the original;
the prior nine-pixel missing ground segment is resolved. The retained bounded
capture verifies every live trace field, every full aircraft core and every
complete page hash. The Free Flight Delete callback and Escape/restart check
passes 57 original input/stage intervals and 37 full-body comparisons, with
the existing explicitly reported scratch/voice/busy exclusions. Frontend,
title hold, save/reload and SDL presentation checks also pass.

This identifies and fixes a ground-geometry cause consistent with the user's
Free Flight report. It does not prove that every reported disappearing line
has the same cause or that every frame now matches the original. Full-flight
drawing work remains open.

The earlier drawing-window note misread the placement record's +18 byte pair
03EF as its result. The actual cached result at +20 is source 0001 versus
native FFFF. That interpretation is corrected in the note and checkpoint;
the recorded bytes and captures are unchanged.

## Reproduction

```powershell
python tools/native/check_ground_bounds.py --runner build/native-cmake/native/Release/fa18_native.exe
python tools/native/check_models.py --runner build/native-cmake/native/Release/fa18_native.exe
python tools/native/check_mode_two.py --mode 125 --callback --out build/native-flight/road-free-flight-callback
python tools/native/check_recorded_original_mission_trace.py --runner build/native-cmake/native/Release/fa18_native.exe --source-evidence build/native-flight/original-mission-three-patrol-runway --source-updates build/native-flight/patrol-entry-review --out build/native-flight/original-mission-three-ground-bounds-release
python tools/native/check_recorded_original_mission_trace.py --runner build/native-cmake/native/Debug/fa18_native.exe --source-evidence build/native-flight/original-mission-three-patrol-runway --source-updates build/native-flight/patrol-entry-review --out build/native-flight/original-mission-three-ground-bounds-debug
```

`analysis/figures/native_ground_bounds_startup_checkpoint.json` records the
binary/capture identities and the scoped results. Diagnostic `--ground-only`
compares the actual captured descriptor without re-running its placement
skip gate; `--placements-only` checks reached descriptors without unrelated
fuzz cases. Optional descriptor exports and command logs in external oracles
helped isolate the omitted producer. Passing raw RAM is discarded; reusable
original and failing pre-fix fixtures remain compressed and bounded.
