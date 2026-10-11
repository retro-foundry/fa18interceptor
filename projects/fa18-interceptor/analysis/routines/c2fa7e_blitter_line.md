# `blitter_draw_line_to_enabled_planes` at `$C2FA7E`

Classification: **behavioural, hardware-level**. Authority is
`build/attract_focus_600/trace.jsonl`, `custom_writes.jsonl`, the matching slow
RAM snapshot, and the raw P-code export.

The routine is called at `$C2FA7E` in the in-flight packet and returns at
`$C2FD20`. It derives a row-relative pointer from register inputs, constructs
the Amiga blitter line-mode control values, waits for `DMACONR` bit 6 to clear,
then conditionally starts the same line operation for bits 0-3 of the byte at
`$C456E7`.

For each enabled bit it writes:

| Register | Observed value source |
| --- | --- |
| `BLTCON0` | computed control word |
| `BLTCON1` | computed line-mode control word |
| `BLTCPT`, `BLTDPT` | the same computed buffer address |
| `BLTADAT` | `$8000` |
| `BLTBMOD` | computed error term |
| `BLTSIZE` | computed line trigger |

The code also establishes `BLTAMOD`, `BLTDMOD`, `BLTCMOD`, full A masks and
`BLTBDAT` before the enabled-plane sequence. Repeated writes from `$C2FBD4`,
`$C2FBD8`, `$C2FC3C`, `$C2FC40`, `$C2FCA4`, `$C2FCA8`, `$C2FD0A` and
`$C2FD0E` match these operations in the custom log.

The byte at `$C456E7` is therefore provisionally named
`active_line_plane_mask`; its wider structure, the input-register calling
convention, the coordinate system and the visual primitive's owner remain
unknown. Do not call it an aircraft, terrain, HUD or polygon routine yet.

## Byte-exact source boundary

The complete body through its return at `$C2FD20` is now represented by three
contiguous byte-exact source slices:

- `prepare_blitter_line_parameters.asm`, `$C2FA7E-$C2FB4D`: CPU coordinate,
  direction, Bresenham, and trigger preparation.
- `blitter_line_setup.asm`, `$C2FB4E-$C2FB79`: initial blitter idle wait and
  shared modulo/mask setup.
- `submit_blitter_line_to_active_planes.asm`, `$C2FB7A-$C2FD21`: conditional
  bitplane 0--3 submissions and per-trigger busy waits.

The first slice's `$C2FA70` branch is now byte-exact
`handle_equal_line_axis.asm` (`$C2FA70-$C2FA77`). It increments both axis
registers, clears the line span word, and rejoins at `$C2FA9C`.

## run075 horizontal submission

`build/port_run075_c2fa7e_800_chip_600/` is a bounded replay trace from the
sealed run075 restore. It reaches `$C2FA7E` at frame 601 with
`(D0.w,D1.w)=(256,89)` and `(D2.w,D3.w)=(319,89)`, establishing the native
endpoint order as `(x0,y0) -> (x1,y1)` for this call. The equal-Y branch takes
`$C2FA70`, so the source increments the working row values before the common
line preparation path. All four active-plane bits are set. The completed
800-instruction CPU trace reaches each of the four `BLTSIZE` triggers and
returns from the emitter.

The before/after Chip-RAM snapshots have no direct byte delta because this
trace steps CPU instructions around an asynchronous blitter submission. It is
therefore evidence for the endpoint convention and hardware packet only. A
native line rasterizer needs a separate post-blit visual or memory oracle;
none is claimed or implemented yet.

## Historical lower-run fixture excluded from the port evidence set

The replay prefix used by
`build/port_run003_c2fa7e_300_settle1_m/` starts from
`build/run003_pre_m_2183/state.bin` and delivers
`build/run003_m_press_only.e9k`. It hits `$C2FA7E` in host frame 9 with
`(D0.w,D1.w)=(180,0)` and `(D2.w,D3.w)=(202,1)`. After 300 stepped CPU
instructions, one resumed frame lets the submitted blitter jobs complete.
The before/settled Chip-RAM delta is exactly:

| Chip address | Before | After |
| --- | ---: | ---: |
| `$04FAAE` | `$00` | `$0F` |
| `$04FAAF` | `$00` | `$FF` |
| `$04FAB0` | `$00` | `$FF` |
| `$04FAB1` | `$00` | `$E0` |

This is the working plane-1 base `$04FA70` plus byte offset `$003E`:
row 1 (`40` bytes) plus x-byte 22. It sets bits for x=180 through x=202,
inclusive, and no other working-plane bytes. The other three plane jobs leave
their bytes unchanged. Plane 2 is already set at these pixels, so
deplanarizing the bounded result changes index 4 to index 6 at all 23
positions. The submitted `BLTSIZE` is `$05C2`, with line control and error
values `$0051` and `$FFD4`.

The source increments the working first row before hardware setup and turns
the source vertical delta into a line-mode control value. Thus this fixture is
not a conventional diagonal Bresenham segment: drawing `(180,0)->(202,1)` as
a normal software line would alter two rows and contradict the original.
The native `port/line.c:fa18_draw_line` translates this nonclipped line-mode
contract to `FA18IndexedFrameBuffer`. It uses `FA18LineSegment` and
`FA18LineStyle`, including the source's active-plane and signed mode choice,
instead of recreating either the pointer table or custom-chip registers. The
line-mode recurrence is the documented `BLTAPTL`, `BLTBMOD`, and `BLTAMOD`
relationship (initial `4*minor - 2*major`, then `4*minor` or
`4*(minor-major)`) and is checked by `port/line_contract_test.c` against this
fixture. The original source-derived clipping path and additional octants need
separate fixtures before the routine can be called complete. The hardware
semantics are documented by the [Commodore Hardware Reference Manual line-mode
summary](https://amigadev.elowar.com/read/ADCD_2.1/Hardware_Manual_guide/node0129.html).

The next M-map submission is captured by
`build/port_run003_c2fa7e_second_300_settle1_m/`, using the replay tracer's
`--skip-hits 1` option. Its `(219,0)->(225,0)` entry reaches `BLTSIZE=$01C2`
and changes only working plane-1 bytes `$04FAB3..$04FAB4` from `$00,$00` to
`$1F,$C0`. This is exactly x=219..225 on row 1, and independently confirms
the native equal-Y path and inclusive major-axis count. It uses the same
plane-bit result, changing chunky index 4 to 6.
