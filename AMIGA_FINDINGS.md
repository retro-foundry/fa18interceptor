# Amiga emulation findings

Reusable observations from A500 OCS comparisons against a UAE-based emulator.
These were first recorded during the [F/A-18 case study](projects/fa18-interceptor/README.md).
They describe that hardware/configuration scope; check the selected machine and
emulator revision before applying them to other chipsets or memory expansions.

- **Blitter modulos ignore bit 0.** Agnus stores BLTxMOD as `value & $FFFE`.
  Honouring bit 0 on odd fill/copy modulos
  drifts every row by a byte.
- **Line mode, ONEDOT.** The one-dot flag is cleared whenever the line steps
  to a new row, so each row gets exactly one pixel; area fills depend on it.
- **POTGOR idle value is `$5500`**, not `$FF00`: DATLY, DATLX, DATRY and DATRX
  read high when the buttons are released. Kickstart's input handler reads it
  every frame.
- **CIA-A TOD** counts the vertical sync pulse, a few lines after the VERTB
  interrupt, not at line 0.
- **UAE boot ROM area.** UAE maps a 64 KiB board at `$F00000` (`rtarea`) and
  can install interrupt servers there; savestates carry it in the `BORO`
  chunk.
- **Blitter timing.** Games poll BBUSY; an instant blitter changes their
  timing. A model with synchronous memory effects still needs busy/interrupt
  timing consistent with the observed DMA duration (UAE `blit_cycle_diagram`).
  That shortcut alone does not reproduce concurrent CPU/DMA memory effects.
- **Slow RAM is on the chip bus.** On an A500, CPU accesses to `$C00000` RAM
  wait for bitplane, Copper and blitter DMA just like Chip RAM accesses. A
  model without that runs the CPU too fast.
- **Savestates** (UAE `ASF`) in these captures used uncompressed chunks: `CPU `, `CHIP` (custom
  registers, without the sprite block), `CIAA`/`CIAB`, `CRAM`/`BRAM` (RAM),
  `BORO`, `END `.

  Compression flags are part of the format; the retained audio-state reader
  explicitly rejects compressed CIA/audio chunks rather than misreading them.
