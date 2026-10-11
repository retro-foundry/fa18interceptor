# Native disk-backed demonstration flight

The playable caller is `fa18_native` -> `native_frontend_tick` -> native menu
C0FCB4/C0FECE -> C0FA04/C0FAA4 -> C0FA4C/C0FA80 -> C10C08/C10C68/C10DAE.
The same runner now performs C1C63E/C22C80 NPC dispatch, C23A7E range/view
selection and C25B66/C2C392 guidance. This removes the native demo banner stop
and reached NPC/launch aborts; CPU, instruction adapters and chipset remain
absent from its link map and runtime.

Original C16518 loads `text/textply` and `text/textctl` into the recorder byte
and word buffers. Native startup now reads these files from the supplied ADF,
using the source capacity and four-times-capacity read limits. The 3,676 and
14,704 asset bytes match the sealed demo's initial recorder buffers exactly.
The native runner never loads reference RAM. Empty recorder storage was the
reason its first connected demo left the player stationary.

Native C23A7E children receive actual working arguments through the existing
`consume_values` contract. Flight-action children have the same direct argument
contract; reference adapters retain their existing observer/consumer route.
C2374C's selected-fire prefix clears SPACE_COMMAND_LATCH, sets
FIRE_RECORD_PENDING, consumes warning bit 14 and enters the shared C237B8
stores/motion body. It deliberately bypasses C2377E's secondary launch gates.
The common code retains original store consumption, record cloning, ammo
counters, template coefficients, inverse-matrix motion and direction scaling.

Validation completed:

- `check_demo.py` runs fresh native launches at recorded updates 2,000, 2,400,
  3,000 and 4,892. All two game keys are consumed; the aircraft moves with
  positive speed, NPCs update, recorded launches increment the pilot counter,
  and the complete native recording returns successfully. The final run
  executes 2,400 record updates, 2,398 scene/HUD passes and 47,367 descriptors.
- Fifteen original demo countdown/scene/viewport/heading parents and sixteen
  primary/secondary launch cases match every non-stack RAM byte.
- Original C12098/C1C63E parents match native memory at all four checkpoints
  and an independently reached original demo checkpoint. Reached end-state
  descriptor/geometry/parent comparisons pass.
- A single bounded original 6,000-frame run exports consumed keys and a
  start-of-update 2,401 checkpoint. Its two keys match the sealed recording's
  delivery rows. A one-frame original launch supplies the initial asset
  comparison. No full original replay suite was repeated.
- Native frontend/menu/save/SDL/link-omission regressions pass. Bounded source
  CPU/SR/RAM compatibility checks cover C23A7E and flight-action owners.
  Qualification success/save/restart and crash return/reselection regressions
  pass. Both MSVC reference runners build; their twelve focused CTests pass.

Functional scenario wiring is now **3/3 (100% of that checklist)**: crash return,
carrier success/restart and demonstration flight. This milestone is complete;
whole-game completion is not assigned that percentage. Full native frame parity
is still **0/3 accepted**. Copper fade remains excluded.

The bounded original demo checkpoint has game tick 222 versus native 319 at
corresponding input update 2,400. The original initial SOUND_FLAGS byte is F7;
native startup currently leaves it zero, selecting a 150-update banner delay
where the reference selects 210. Audio availability/initialization and remaining
message/viewport timing must be reconciled from their original owners. No
reference flags, flight state or recorder buffers are injected to hide this gap.
Other mission modes, remaining record/expiry children, physical gameport input,
remaining frame composition and actual audio output remain open.
