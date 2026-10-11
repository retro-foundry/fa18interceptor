# run075 frame559 `$C2FD8C` active page packet

The run075 trace at `$C2FD8C` begins at engine frame 559. It loads the
selected four plane table through `$C456B6`; the table values are:

| Hardware lane | Plane base | Native plane |
| ---: | ---: | ---: |
| 0 | `$0538F0` | 3 |
| 1 | `$0519B0` | 2 |
| 2 | `$04FA70` | 1 |
| 3 | `$04DB30` | 0 |

The values are separated by `$1F40` bytes. The setup adds `$28` for the
current row before writing the destination pointer. `$C45984` supplies
`$0090`; the routine shifts it six places and adds `$14`, producing the
semantic blit extent of 20 words by 144 rows. The native port represents the
four destinations as one `FA18PlanarPage` and converts the lane order with
`fa18_visible_lane_plane`.

The trace proves the active page and packet geometry, while the temporary
workspace source coordinates remain a separate unresolved input to the copy.

Authority: `build/run075_frame559_c2fd8c_600/trace.jsonl` and its `slow.bin`
snapshot from the sealed run075 restore and playback.
