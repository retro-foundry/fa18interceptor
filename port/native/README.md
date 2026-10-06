# Native intro/menu runner

The 2026-10-06 user request starts `fa18_native`, reusing `port/game/` source
with a separate native entry point. This supersedes the earlier restriction
against starting a native runner. It does not restore the deleted `fa18_port`.

```powershell
python scripts/build_native.py
build/native/fa18_native.exe --adf local/media/fa18.adf
```

The default is an SDL window. A key acknowledges the credits. A first-time
pilot can enter a callsign, edit with Backspace, and confirm with Return.
The original numbered main menu is then displayed. Closing the window exits.
Launching gameplay from its choices is the next milestone, not implemented yet.

The supplied ADF is read-only. `--save-dir PATH` selects the native save overlay
(default `saves-native`). Its `config` retains the original 78-byte format.
Source audio suppression is enabled for this initial visual milestone.

```powershell
build/native/fa18_native.exe --headless --frames 1800 --ppm build/credits.ppm
python tools/native/check_frontend.py --runner build/native/fa18_native.exe
```

Build ownership is `port/recomp/CMakeLists.txt` -> `port/native/CMakeLists.txt`.
`FA18_NATIVE_ONLY=ON` returns before creating the CPU/machine targets. The
explicit native target links SDL and ordinary disk/Hunk loading, ILBM decoding,
game message sequencing, menu publication, and text plotting. It never links
Musashi, generated translations, game glue, or the chipset model. A link map
supports the omission check. No ROM, savestate, captured page, or frame replay
supplies game behavior or pixels.

The original ADF provides splash pixels, palette, fonts and text tables.
`advance_main_loop_message_sequence` calls native children with `MessageWorking`
values; `queue_top_level_menu_messages` publishes the existing menu selectors.
`plot_glyph8` writes ordinary bitplane buffers, which SDL presents directly.
Source data addresses currently index checked host storage; fully typed game
state is still future work. These data addresses never select executable code.

Authority: C0E2E8/C0E078 splash load, C0E53C/C0E78A busy delay, C11446/C11478
credits and acknowledgement, C115BA-C1175A tour/name flow, C0FBE0 menu,
C32CEE message sequencing, C330FE glyphs, and C1643A flight-log saving.
The source busy pause is converted to nominal PAL ticks once; DMA/loading,
CPU-paced message update frequency and fade timing are not reproduced.
Static settled splash, credits and menu comparisons are separate from timing
parity. Copper fade remains excluded from acceptance, per the user.
