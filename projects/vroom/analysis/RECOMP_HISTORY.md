# Imported Vroom recompilation evidence

Historical narrative from `amiga-recomp` commit `0f71e52134ba64fa74975b6bbe75b19830b225c4`. Paths/counts refer to that checkout, not new qualification.

## Vroom (the real game)

`ports/vroom.toml` boots the real disk image; `ports/vroom-game.toml`
recompiles the decrunched result. Neither the ADF nor the snapshot is in this
repository.

What works:

1. `arecomp bootblock` reads the loader's two `trackdisk.device` reads off the
   boot block and emits the manifest fragment.
2. The recompiled boot stub kills the OS, installs a level 1 vector, triggers a
   software interrupt, measures the CPU speed against a CIA timer, seeks the
   drive and reads raw MFM through the chipset's disk DMA.
3. It decrunches the game to `$3658`; the capture hook writes memory and
   machine state there.
4. That snapshot recompiles to **1361 blocks / 17 KB of game code**, which runs
   natively at about 5.6x Amiga speed with **zero interpreter fallbacks**.

The title screen renders correctly in all 16 colours, pixel for pixel against
the real game, the frame counter advances, and input moves the game on to its
menu.

Current state: **converged**. Runs 400 million guest cycles with no faults,
**31.7 million native blocks**, 2877 interrupts, 52+ disk reads. Compiled and
interpreted runs produce **identical state digests over 6,687,000 blocks**.
`tools/converge.py` reports nothing further to add: the only unresolved
targets left are 104 addresses in $43xxx, which the game loads from disk at
run time and which are empty in the snapshot.

Six real bugs were found by getting this far, all now fixed:

1. **A 68000 decoder bug.** `cmpa.l a3,a6` ($bdcb) was decoded as
   `cmpm.l (a3)+,(a6)+`, because the CMPM pattern was matched without checking
   that the size field was a real size. That silently postincremented two
   registers the game only meant to compare, turning a loop-termination test
   into a runaway pointer. Regression test in tests/cpu/decoder_test.cpp.
2. **The wrong file in the drive.** A snapshot port passed argv[1] to
   insert_disk, so the game had its own 512 KB memory image in the floppy
   drive instead of the ADF.
3. **Chip state was never captured.** A snapshot is memory *and* machine state
   (section 39). Without the chip state the Copper resumed pointing at address
   zero, ran through low memory interpreting 68k code as a Copper list, and
   wrote `INTENA <- $4eb9`, clearing the master interrupt enable for good.
4. **The capture trigger did not force a block boundary**, so a trigger landing
   mid-block installed a hook that never fired.
5. **Input was encoded as one bit per direction.** A joystick presents two
   quadrature counters: right is bit 1, down is bit 1 xor bit 0, left is bit
   9, up is bit 9 xor bit 8. A press for right read as right+down and a press
   for down read as right. Port 0 was also being fed joystick directions when
   a game reads it as a mouse, which is what left the menu unusable.
6. **A write only retired the block at its first byte.** The picture is drawn
   by an unrolled bitplane conversion loop that patches its own shifts two at
   a time with one `move.l`. `write32`'s four-byte fast path reported only the
   address it started at, so the block two bytes later kept running the `nop`
   the capture held. Half the shifts never happened and the title screen came
   out in four colours. The same hole was open for DMA, which writes through
   `poke` and bypassed notification entirely.

Remaining: 64 unknown targets, all in $43xxx, which is **empty in the snapshot**
-- the game loads that code from disk at run time. Compiling it from the
snapshot translates zeros, which is the overlay case in section 31. It needs
either a later capture or per-region translation. Everything else runs
natively.

## Driving a game during bring-up

A hook is the simplest way to answer "is it stuck, or is it waiting for me?".
A game idling on its title screen and a game wedged in a loop look identical
from outside; a hook on the loop tells them apart, and can then supply the
input that moves it on.

```cpp
// The title screen's per-frame wait: it spins until the interrupt handler
// changes the frame counter. Watching that counter says whether the game is
// stuck or idling, and after a while we press fire to find out if it moves on.
runtime.hooks().add(0x00012f98, HookMode::Before, "title-probe",
    [](M68kState& cpu, Runtime& rt) -> uint32_t {
        static u32 visits = 0;
        const u32 counter = rt.memory().peek32(0x0000a884);
        if (++visits == 400) {
            if (Chipset* chipset = port::active_chipset())
                chipset->set_joystick(1, false, false, false, false, true);
        }
        return cpu.pc;
    });
```

That exact hook is how Vroom was shown to be running rather than hung: the
counter was advancing all along, and pressing fire moved it from the title
screen to the menu.

Hooks live in the generated port's `patch/` directory, which arecomp writes
once and never overwrites, so they survive regenerating the translation.
