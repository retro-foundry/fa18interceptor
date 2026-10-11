## Findings from emulating F/A-18 Interceptor

Details that mattered when building a native A500 model for this game
(`port/machine/`); each was found by comparing against a UAE-based emulator.

- **Blitter modulos ignore bit 0.** Agnus stores BLTxMOD as `value & $FFFE`.
  The game writes odd modulos for its fill and copy blits; honouring bit 0
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
  timing. Keep the memory effect synchronous but hold BBUSY and the BLIT
  interrupt for the blit's real duration (UAE `blit_cycle_diagram`).
- **Slow RAM is on the chip bus.** On an A500, CPU accesses to `$C00000` RAM
  wait for bitplane, Copper and blitter DMA just like Chip RAM accesses. A
  model without that runs the CPU too fast.
- **Savestates** (UAE `ASF`) are uncompressed chunks: `CPU `, `CHIP` (custom
  registers, without the sprite block), `CIAA`/`CIAB`, `CRAM`/`BRAM` (RAM),
  `BORO`, `END `.
