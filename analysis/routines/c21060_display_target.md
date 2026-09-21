# Display-dispatch target at `$C21060` (Hunk 13 +`$378`)

Classification: **behavioural**, bounded to a display-only contract. The
record/object ownership and visual primitive meanings are unknown.

## Runtime packet

- State: `run001` flight frame 2,696 plus its isolated joystick press.
- Reached through the `$C1F942` record dispatcher; breakpoint `$C21060`, frame
  9; complete return to `$C1F944`.
- 7,254 instructions.
- P-code: `pcode/raw/run001_c21060_display_target/`, 1,136 observed RAM starts
  / 7,229 operations; 1,084 starts map to resolved Hunks and 52 are retained
  as unmapped.

## Observed display edge

The packet calls the known `$C2FA7E` blitter line emitter twice from `$C302B6`.
It also repeatedly enters helpers in the `$C246xx-$C249xx` range and the
Hunk-36 renderer range. This proves the target submits display work in this
flight packet. It does not prove a specific object, terrain, HUD, or camera
role.

## Verified record-walker entry

`source_amiga/observed/dispatch_offset_tuple_records.asm` is a byte-exact
134-byte reconstruction of the complete function `$C21060-$C210E5`. It reads
a signed-word sentinel list through `A2`; each nonnegative word is followed by
three offsets. The four offset-selected five-word entries from `$C48390` are
expanded at `$C4BF94`, then the function calls `$C246A0`, ORs its return into
its stack-local status word, and resumes the list. The negative sentinel
restores `A1/A5` and returns that status. The record and tuple meanings remain
unassigned.
