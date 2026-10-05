# The C port

## Deliverable

Recreated, readable C source for the whole game: named functions and
parameters, named globals and structs, fixed-point types and comments on
intent, with no CPU emulator and no generated 68000 code in the final build.

Everything else is scaffolding with two jobs: keep the game running and
rendering at every step, and serve as the reference each hand-written routine
is proven against.

For the 2026-10-05 milestone the user accepts static recompilation of the
remaining entries, marked for later readable decompilation. All 85 deferred
translations now have direct native instruction-helper bindings in
`port/recomp/generated/recomp_static_deferred.c`; their per-entry debt is in
`recomp_deferred.json`. Shared CPU/machine state remains an interim dependency.
This does not change the final readable, CPU-free deliverable above.

The active 2026-10-05 goal now prioritizes that full emulation-free deliverable.
Native primary/secondary record placement now shares the startup descriptor
bank, records and mutable fields directly, with 16,384 original comparisons
at all 172 boundaries and no child contracts. Its original-disk asset loader
resolves the descriptors; indices reaching adjacent relocated words require
explicit field owners. Native finish and all post-flight modes now add 90,112
comparisons at all 371 boundaries, including actual selection release,
readiness and view restoration. Periodic and dispatch remain scheduler children;
actual view publication and complete startup field bindings remain required.
See `analysis/routines/native_record_action_placement.md`.
See also `analysis/routines/native_postflight.md`.
Reference `port/game` modules still use the machine bus; readable C alone does
not remove this dependency. Ordinary-state modules in `port/` must be composed
into the complete native game loop. The first current input component,
`indexed_controls.c`, passes 65,536 source comparisons with full branch coverage
and builds without CPU/machine dependencies. `command_input.c` now adds both
complete selection prefixes, 32,768 passing source comparisons, shared ordinary
state and indexed-action composition. The `fa18_command_input` library links
without CPU or machine sources. `flight_command_input.c` adds all 28 aircraft
actions and real axis/throttle-reset/space-release children, with 28,672 source
comparisons and complete component coverage. The native view/origin/zoom family
adds 16 actions and both actual zoom/redraw children, with 32,768 complete
comparisons. All five context actions and their actual geometry/observer
children add 20,480 comparisons. Native queue publication adds 73,728
comparisons covering all signed-index destinations and shared-field aliases.
Both complete native command parents add 16,384 comparisons, including eject's
nested publication and actual callback registration/removal bodies. The remaining
command message/status/voice/space/sweep game children add 12,288 comparisons
and cover all 278 original boundaries. Audio program/update/output/fade owners
add 16,384 comparisons at all 105 boundaries. Their portable `voice_program`
core builds independently for reuse; original selector/output/fade policy stays
in the F/A-18 adapter. The complete native mouse/viewport/fade callback adds
4,096 comparisons at all 198 original boundaries. It shares mouse Y with
throttle controls, attaches X/tick queue aliases and uses the same viewport
transition implementation as the older native wrapper.
The actual callback RGB4 backend now uses the independent ordinary-buffer
`amiga/rgb4` component shared with the reference host service. Another 4,096
callback comparisons and 16,384 frozen-service comparisons validate this
composition, including actual display-list changes after publication.
Viewport construction and merging now share the independent ordinary-buffer
`amiga/viewport_list` core, with 32,768 frozen-service comparisons and a native
list owner connected to callback RGB4 updates and renderer-buffer presentation.
The complete native outer display owner now shares those input/page/mode
objects, with 8,192 original comparisons covering all 80 source boundaries
and real 32-word RGB4 updates. Original synchronization services remain explicit.
Native graphics setup now allocates the original five-plane family, builds
both actual five/four-plane display pairs and shares the native display owner.
Its 5,120 original invocations cover all 194 reachable boundaries. Palette
imports from both original ILBM resources and Hunk 21 match all 320 words each.
Native menu sound selection now shares ordinary audio owners with command
effects and audio updates. Its independent `voice_selection` core is reused
by both sound-start paths; 8,192 original comparisons cover all 110 boundaries.
The complete native startup text/palette publisher and actual formatter add
8,192 comparisons at all 221 boundaries, with bootstrap explicitly contracted.
They bind the real mutable checksum descriptor and contiguous original seed;
full bootstrap, checksum production and installed-stage integration remain open.
The complete ten-stream renderer clear and three post-input display stages
add 16,384 comparisons at all 62 boundaries, using the existing native planes
and command/controller owners. The full scene initializer is explicitly
contracted and the tenth clear buffer requires its actual startup producer.
Both complete startup clear/enable leaves add 8,192 comparisons at all 21
boundaries, with all RAM compared and no child contracts. Their live queue
references share the independent `field_bytes.h` mechanism with command
publication; all 73,728 queue comparisons still pass. The later bootstrap
composition binds viewed-record identity to the actual shared aircraft bank.
Native scene records now bind the actual aircraft flags/position/heading/
inverse fields and indexed root level through one live data view. Complete
mission reset, preparation, transient reset and start-position leaves share
that bank; the existing observer is reused. Another 24,576 comparisons cover
all 98 reachable boundaries, including the actual all-record/workspace clear
block with preserved tails. This proves the specified leaves/block, with
the remaining native record consumers still pending.
The complete native bootstrap parent now composes these actual available
children and the complete gate builder around three required placement/update/
context child contracts. Another 16,384 comparisons cover all 291 boundaries,
ordered child-entry states and the carried placement word. The shared gate
builder preserves signed adjacent-field writes, odd/zero list lengths and
original fault returns. All 65,536 bit indices and all 1,848 Hunk-66 payload
bytes are checked; a bound stream change reaches the real child. Viewed
identity resolves to the live
aircraft pointer; whole logical-word writes extend the independently reusable
field component. The actual three child graphs and native main integration
remain pending.
The placement dependency `$C2D954/$C2D94E` now publishes both matrices into
actual shared native records, using live original trig data and required
adjacent field owners. Its 8,192 complete calls cover all 292 boundaries,
with four actual children and no contracts; all 65,536 lookup input words
and 1,802 original quarter-table bytes match. This corrects the older
packet's restore-order mistake and two inverse-matrix subtraction signs.
See `analysis/routines/native_record_orientation.md`. Full root placement,
record update and context refresh still require their actual native owners.
Host services, sample playback, original data loading and
runtime integration remain open; see `port/REUSABLE_COMPONENTS.md`,
`analysis/routines/native_input_callback.md`,
`analysis/routines/native_rgb4.md`,
`analysis/routines/native_viewport_list.md`,
`analysis/routines/native_outer_display.md`,
`analysis/routines/native_graphics_setup.md`,
`analysis/routines/native_audio_update.md`,
`analysis/routines/native_audio_selection.md`,
`analysis/routines/native_postflight_text.md`,
`analysis/routines/native_post_input_display.md`,
`analysis/routines/native_startup_ranges.md`,
`analysis/routines/native_scene_player.md`,
`analysis/routines/native_scene_bootstrap.md`,
`analysis/routines/native_command_effects.md`,
`analysis/routines/native_command_parent.md`, `analysis/routines/native_command_queue.md`,
`analysis/routines/native_context_command_input.md`, `analysis/routines/native_view_command_input.md`,
`analysis/routines/native_flight_command_input.md` and
`analysis/routines/native_command_input.md`.

## Architecture

```
            recorded input / live SDL input
                         |
   +---------------------v----------------------+
   | port/machine: A500 model                    |
   |   bus, Chip/Slow RAM, ROM, custom chips,    |
   |   blitter, Copper, display, CIAs, timeline  |
   +---------------------^----------------------+
                         | memory and register access
   +---------------------+----------------------+
   | CPU state (Musashi register file)          |
   |   Musashi interpreter: Kickstart ROM,      |
   |   undiscovered code                        |
   |   port/recomp/generated: translated game   |
   |   port/game: recreated C (via glue)        |
   +--------------------------------------------+
```

- **Machine layer** (`port/machine/`). Memory map with mirroring; custom
  registers; a synchronous blitter ported from UAE (area, fill, descending,
  line; BBUSY and the BLIT interrupt follow the real duration); Copper per
  scanline; bitplane fetch to a 320x256 RGB444 frame (image origin: DIW
  `$81`, beam line `$2A`); CIA timers, TOD and keyboard; interrupts. It loads
  UAE savestates, so a run can start at any recorded frame.
- **One timeline.** Chipset work (line ends, Copper, display, interrupt
  acceptance) happens only at instruction boundaries in
  `fa18_machine_service()`, which both the interpreter hook and translated
  code reach, so both take interrupts at the same instruction.
- **Translation** (`tools/recomp/recomp.py` into `port/recomp/generated/`).
  One C function per routine, labels at every leader, native control flow;
  each data operation runs Musashi's handler for that opcode. The translator
  and interpreter share one register file, so execution can move between them
  at any label. Writes to translated bytes invalidate the routine.
- **Recreated source** (`port/game/`). Hand-written C entered through a glue
  function registered in `port/game/glue/ports.c`.
  Event-bearing glue resumes at original instruction boundaries; instruction
  fixtures include DMA contention and live batches compare fresh source OFF
  frames. Exact isolated batches can still expose combined timing debt in
  other fixed-charge entries. Current timing evidence is in
  `analysis/routines/native_c_template_placements_domain.md`.
- **OS replacement** (`port/os/`). Source-backed C Kickstart services, with a
  temporary CPU bridge while the game still uses the original register file.

## Stages

| Stage | What | State |
| --- | --- | --- |
| A | Whole-program translation, interpreter fallback | done (624 routines) |
| B | Machine layer | done; bus timing modelled to ~0.1-0.5% (STATUS.md, "Bus timing") |
| C | Machine and frame parity with Engine9000 | historical emulator comparisons documented; current acceptance uses the sealed native recordings |
| D | Readable C, proven in related batches | 539 readable translated + 85 explicitly deferred static entries cover the seeded 624; 75 additional readable source-only entries, 543 readable entries with source timing. Original callback scope remains follow-up work; see CURRENT_PORT_HANDOFF.md |
| F | Native backend: plain C memory, direct drawing and audio | in progress; native components proven, full game integration and sample output pending |
| E | OS replacement (Kickstart calls), cold boot from the ADF | Last: assess which services remain necessary after D and F; existing C shims are verified on three native sessions |

The project work order is D, then F, then only the necessary parts of E.
The 2026-10-04 user instruction authorizes ROM independence now: a separate
`fa18_romfree` runner must start from the ADF without Kickstart or a savestate,
retaining the CPU and chipset model and preserving original behavior. The
Amiga SDK is reference-only. Loading and service facilities must be reusable
across games, with Interceptor-specific configuration kept separate. See the
active objective in CURRENT_PORT_HANDOFF.md; the older service deferral is
superseded.

Later 2026-10-04 steering prioritizes a playable, faster ROM-free build using
behavior-level host compatibility services. Exact OS timing/register side
effects are deferred; existing exact fixtures remain a later validation oracle.
See `analysis/routines/romfree_exact_followup.md` for the return work.

## Recreating game-source batches (stage D)

1. Pick a related batch: `python tools/recomp/port_candidates.py` lists
   routines whose callees are already C, ranked by glue burden. Also inspect
   indirect and table-dispatched families that this list cannot rank.
2. Read it: `python tools/recomp/port_info.py C2FA7E` prints its
   instructions, observed call sites, and the registers and flags live after
   it returns. Read its report in `analysis/routines/`, the memory map, and any
   earlier `port/*.c` module for the same address.
3. Write the C in the right `port/game/` file, as original source would be
   written (see Conventions).
4. Write the glue in `port/game/glue/`: read the inputs from registers and
   memory, call the C, rebuild every live register, flag and high word the
   original leaves, then `glue_return()`. Register it in `ports.c` with the
   cycles to charge.
5. Build and use short recording probes for the new routines while porting a
   substantial batch. Commit verified chunks as they are ready.
6. Run `sh scripts/recomp_ports_check.sh` over all native recordings after the
   larger batch, then update the proof counts and handoff notes.

When every caller of a routine is C, its glue is no longer reached; delete it.

## The proof

- **Liveness** (`tools/recomp/liveness.py`, emitted as
  `generated/recomp_liveness.c`). For every return address: which registers
  (data registers as low and high words) and flags the caller can read
  before overwriting them. Interprocedural over static and observed call
  edges (`fa18_recomp --edges`). A return into code outside the translation,
  such as a ROM interrupt dispatcher, keeps everything live.
- **Shadow** (`--ports shadow`). On every call of a recreated routine, the
  glue runs first, sandboxed (no chipset events, hardware blocked, custom
  writes held, all undone), then the generated routine live, servicing
  chipset work and stepping to the next label exactly as the dispatcher
  would. Compared: live registers and flags, every memory byte either run
  wrote (except the dead stack below the returned-to SP), and the exact
  sequence of custom-register writes. The game continues on the live run,
  so a shadow run ends byte-identical to a plain run. Calls with an
  interrupt or hardware access inside, or cut by a frame end, are not
  compared.
  A stepped bridge can opt into source-first DMACONR input replay when held
  BLTSIZE writes make the usual port-first input state differ. The source's
  ordered PC/value read stream supplies hardware inputs only; C still computes
  its own registers and writes on entry RAM. An extra/reordered read fails
  immediately, and missing reads fail comparison. The live source result is
  retained. Reports expose `busy_input_calls` and `busy_input_reads`.
  Full ON RGB/RAM parity and instruction/event traces independently establish
  live timing; this shadow input replay is not a timing proof.
- **Sandbox** (`--ports sandbox`). The older comparison: the generated
  routine first with events held off and its custom writes performed at its
  end, then the glue. It covers the calls shadow cannot (audio, joystick)
  but shifts events and blits, so it proves routines without keeping
  timing. `scripts/recomp_ports_check.sh` runs both. Chip bytes the
  blitter wrote during a live call are not compared: the sandboxed port's
  blits are held, so CPU writes over them differ for DMA reasons only.
- **Poison** (`--poison`). After every compared call, everything liveness
  declares dead is overwritten; all frames must still render identically.
- **ON mode** (`--ports on`). The recreated C runs the game; parity must stay
  the same.

## Conventions for `port/game/`

- Globals are named in `globals.h`, at their original addresses, each with the
  routine or capture that established it. Unknown meanings stay honest
  (`REC_FIELD_0C`) until evidence names them.
- Hardware registers, bits and minterms are named in `hardware.h`; write them
  with `custom_write()` and `custom_write_ptr()`, and wait with
  `wait_blitter()`.
- Game memory is reached with `rd_*`/`wr_*` on `gaddr` addresses
  (`memory.h`) while generated code shares it. They become plain C globals and
  pointers in stage F.
- Keep word-exact arithmetic where the original has it: `int16_t` casts for
  `.W` operations, arithmetic `>>` for `ASR`.
- Factor shared logic where it reads better (`setup_line` serves lines and
  polygon edges); behaviour must stay identical.
- No 68000 register or flag details in `port/game/`; those belong in the glue.

## Validation gates

| Check | Command | Must hold |
| --- | --- | --- |
| Recreated routines | `sh scripts/recomp_ports_check.sh` | 0 mismatches; poison frames identical |
| Live frame parity | `sh scripts/recomp_live_check.sh` (`PORTS_ONLY=LIST` for an isolated batch) | `--ports on` RGB444 frames identical to fresh `--ports off` source streams on every affected sealed recording; final RAM and blit totals alone are insufficient |
| Translation vs interpreter | `fa18_recomp ... --ram-out A` vs `--no-recomp --ram-out B` | identical |
| Machine vs emulator, first steps | `scripts/recomp_lockstep.py` | first divergence understood |
| Blitter | `build/recomp/blit_replay.exe CHIP WRITES OUT` | identical Chip RAM |

## Known limits

- **Timing.** Bus contention, CIA E-clock waits and the 68000's access
  order are modelled (`port/machine/bus.c`) and match cycle-exact UAE to
  ~0.1-0.5% per scene. Long recorded replays still drift (run060 from frame
  94); exact replays need UAE's cycle-exact CPU and blitter timing. Routine
  proofs do not depend on this: the shadow check compares every call.
- **Kickstart** runs on the interpreter; together with the few game
  instructions not yet translated it takes about 30% of CPU cycles. Stage E
  replaces the ROM calls the game uses.
- **Start state** is a savestate; cold boot from the ADF needs disk loading.
- **Audio** (Paula) is not modelled; sprites are not drawn.
- ON mode mixes source-timed bridges with remaining fixed-charge entries.
  These CPU bridges are transitional proof machinery; readable domain C and
  the final native backend remain the deliverable.
