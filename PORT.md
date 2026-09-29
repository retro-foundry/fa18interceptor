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

## Stages

| Stage | What | State |
| --- | --- | --- |
| A | Whole-program translation, interpreter fallback | done (540 routines) |
| B | Machine layer | done; bus timing modelled to ~0.1-0.5% (STATUS.md, "Bus timing") |
| C | Frame parity with Engine9000 on every recording | run075 frames 393-402 exact; run060 game RAM identical through frame 93 |
| D | Readable C, routine by routine, proven | 180 routines |
| E | OS replacement (Kickstart calls), cold boot from the ADF | not started |
| F | Native backend: plain C memory, direct drawing and audio | not started |

## Recreating a routine (stage D)

1. Pick one: `python tools/recomp/port_candidates.py` lists routines whose
   callees are already C, ranked by glue burden.
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
5. Prove it: `sh scripts/build_recomp.sh && sh scripts/recomp_ports_check.sh`.
6. Commit the batch.

When every caller of a routine is C, its glue is no longer reached; delete it.

## The proof

- **Liveness** (`tools/recomp/liveness.py`, emitted as
  `generated/recomp_liveness.c`). For every return address: which registers
  (data registers as low and high words) and flags the caller can read
  before overwriting them. Interprocedural over static and observed call
  edges (`fa18_recomp --edges`). A return into code outside the translation,
  such as a ROM interrupt dispatcher, keeps everything live.
- **Shadow** (`--ports shadow`). On every call of a recreated routine, the
  generated routine runs first, then the glue on the same state. Compared:
  live registers and flags, every memory byte either run wrote (except the dead
  stack below the returned-to SP), and the exact sequence of custom-register
  writes. The game continues on the reference result.
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
| Frame parity | `python scripts/recomp_parity.py --start 392 --frames 10` | first diverging frame does not regress |
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
- The glue charges fixed instruction cycles in ON mode, not measured ones.
