# Native port runner

The 2026-10-06 user request starts `fa18_native`, reusing `port/game/` source
with a separate native entry point. This supersedes the earlier restriction
against starting a native runner. It does not restore the deleted `fa18_port`.

```powershell
python scripts/build_native.py
build/native/fa18_native.exe --adf local/media/fa18.adf
```

The default is an SDL window. A key acknowledges the credits. A first-time
pilot can enter a callsign, edit with Backspace, and confirm with Return.
The original numbered main menu is then displayed. Digits 1-5 select the source
mode and show its transition banner. Digit 6 opens the source-gated mission
list; F1-F4 select the missions enabled by the supplied disk's pilot record.
Digit 7 uses the original next-mission selector. Digit 8 opens flight-log
statistics. Escape returns from the mission list or log to the main menu.
In the log, SHIFT-2 resets the 39 words, and 1 saves the exact 78 bytes. A reset
pilot goes through enlistment/callsign entry again. Closing the window exits.
Free Flight (digit 2) now runs source scene-storage preparation, delayed scene
selection and viewport/message stages. It stops at `scene-setup` / C1072E;
location/aircraft controls and cockpit/world rendering are still pending.
Other selected modes stop at their transition banner. Audio remains suppressed.

The supplied ADF is read-only. `--save-dir PATH` selects the native save overlay
(default `saves-native`). Its `config` retains the original 78-byte format.
Source audio suppression is enabled for this initial visual milestone.

```powershell
build/native/fa18_native.exe --headless --frames 1800 --ppm build/credits.ppm
python tools/native/check_frontend.py --runner build/native/fa18_native.exe
python tools/native/check_menu.py --runner build/native/fa18_native.exe
python tools/native/check_flight_start.py --runner build/native/fa18_native.exe
```

Build ownership is `port/recomp/CMakeLists.txt` -> `port/native/CMakeLists.txt`.
`FA18_NATIVE_ONLY=ON` returns before creating the CPU/machine targets. The
explicit native target links SDL and ordinary disk/Hunk loading, ILBM decoding,
game message sequencing, command selection, mission/log menu owners and text
plotting. It never links
Musashi, generated translations, game glue, or the chipset model. A link map
supports the omission check. No ROM, savestate, captured page, or frame replay
supplies game behavior or pixels.

The original ADF provides splash pixels, palette, fonts and text tables.
`advance_main_loop_message_sequence` calls native children with `MessageWorking`
values; `queue_top_level_menu_messages` publishes the existing menu selectors.
`plot_glyph8` writes ordinary bitplane buffers, which SDL presents directly.
Source data addresses currently index checked host storage; fully typed game
state is still future work. Source callback identifiers describe the recreated
menu transitions; they never execute source instructions or CPU adapters.
`--data-out PATH` exports host buffers and source data for checkpoint inspection;
it supplies no runtime state. Initial player pose, camera and template-gate
banks match a focused original checkpoint. Full record state, update timing and
active gameplay remain unverified; see
[`../../analysis/native_flight_start_milestone.md`](../../analysis/native_flight_start_milestone.md).

Authority: C0E2E8/C0E078 splash load, C0E53C/C0E78A busy delay, C11446/C11478
credits and acknowledgement, C115BA-C1175A tour/name flow, C0FBE0 menu,
C32CEE message sequencing, C330FE glyphs, and C1643A flight-log saving.
Menu continuation reuses C1AD74/C1BC72/C1BD78 command selection,
C0FCB4 mode routing, C1017E available-mission queue, C24E8A/C24F76 summary
formatting, and C0FE36/C16406 log actions/reset. See
[`../../analysis/native_menu_milestone.md`](../../analysis/native_menu_milestone.md).
The source busy pause is converted to nominal PAL ticks once; DMA/loading,
CPU-paced message update frequency and fade timing are not reproduced.
Static settled splash, credits and menu comparisons are separate from timing
parity. Copper fade remains excluded from acceptance, per the user.
