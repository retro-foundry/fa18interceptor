# The C port

## Deliverable

Recreated, readable C source for the whole game: named functions and
parameters, named globals and structs, fixed-point types and comments on
intent, with no CPU emulator and no generated 68000 code in the final build.

Everything else is scaffolding with two jobs: keep the game running and
rendering at every step, and serve as the reference each hand-written routine
is proven against.

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
| D | Readable C, proven in related batches | 432 registered routines; see CURRENT_PORT_HANDOFF.md for the latest full gate and cold-entry structural evidence |
| F | Native backend: plain C memory, direct drawing and audio | not started |
| E | OS replacement (Kickstart calls), cold boot from the ADF | Last: assess which services remain necessary after D and F; existing C shims are verified on three native sessions |

The work order is D, then F, then only the necessary parts of E.

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
