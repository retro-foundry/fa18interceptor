# run075 frame 255/256 delayed menu trace

## Engine9000 replay evidence

The sealed `captures/run075/restored-state.bin` was replayed with
`captures/run075/playback.e9k`. Breakpoints were armed at Engine frame 255;
the first matching calls resumed at Engine frame 256 because the breakpoint
is installed at the start of the frame's execution window.

| Entry | First hit | Entry state evidence | Result |
| --- | ---: | --- | --- |
| `$C0F5F8` | 256 | `D0=0`, `D1=1`, `D2=$0FF000`, `D3=$F000`, `A0=$C0F82E`, `A1=$C0F832`, `A2=$C1131B`, `A4=$07FA` | Shared post-input/menu tick path executes |
| `$C0FECE` | 256 | `D0=$5D` at the signed mode/delay read, `D1=1`, `A0=$C10BCE`, `A1=$C0F832` | Delayed transition gate is evaluated |
| `$C2FD22` | not reached through 255 | — | No renderer work-buffer clear in this isolated window |
| `$C33058` | not reached through 255 | — | No static glyph submission in this isolated window |

The `$C0FECE` trace then reads `$C458A6` as `$7F` and proceeds through the
mode gate. The following instruction stream includes reads of `$C4582A`,
`$C45798`, and `$C45890`, plus a call to `$C11B44`; these are the active
transition fields for this window. The trace is in
`build/port_run075_frame255_arm_c0fece/trace.jsonl` and its entry snapshot is
in `report.json`.

## Port consequence

The unchanged frame-255 pixels do not prove an unchanged native state. The
native C loop must consume the deterministic replay event latch and execute
the proved menu tick/delay state update at the same fixed update boundary.
The existing `fa18_menu_post_input_tick` contract is the first candidate
consumer; the exact number and ordering of sub-ticks per displayed frame must
be measured from the Engine9000 trace before later frames are unlocked.

## First visible change

The first RGB444 change after the green `DEMO` label is frame 273. Its image
is an all-zero 320x200 chunky page. The next display submission breakpoint,
`$C2FF48`, is reached at Engine frame 274 with entry registers
`D0=319,D1=179,D2=0,D3=319,D4=179,D5=0,D6=1135,D7=$FFFFFFFF`; the bounded
trace branches through `$C301F6` and `$C305AA`. `$C2FD22` was not the entry
hit in this window. This identifies the visible frame-273 boundary as a
cleared page before the following renderer submission, without yet assigning
the complete transition state.
