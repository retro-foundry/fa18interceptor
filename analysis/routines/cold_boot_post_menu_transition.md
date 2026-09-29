# Cold-boot post-menu transition window

Classification: **scenario-backed negative scheduling evidence**.

Authority: the fresh cold-boot replay in `captures/uae/cold_boot_menu_init/`, with
the recorded Return press/release at frames 9282 and 9287.  The bounded
instruction trace was produced with:

```text
python scripts\engine9000_bridge.py --restore captures\uae\cold_boot_menu_init\initial_state.bin --config captures\uae\cold_boot_menu_init\config.uae --playback captures\uae\cold_boot_menu_init\playback.e9k --frames 9288 --trace-frames 12 --output build\cold_boot_post_menu_transition_trace_9288
```

It contains 106,244 instructions across chipset frames 9289--9300.  At frame
9291 the observed parent sequence reaches `$C0EFEA`, tests `$C45795`, branches
from `$C0EFF6` to `$C0F370`, and continues through `$C0F37E/$C0F380`.

The window has zero executed instructions in each of these ranges:

| Range | Why it was checked |
| --- | --- |
| `$C0924A-$C095BE` | root transition/placement |
| `$C2D99C-$C2DBFF` | renderer-matrix route |
| `$C1C54E-$C1C63D` | active-record projection publication |
| `$C279D0-$C27D0F` | Hunk-25 page renderer packet |

The final screenshot is black.  Therefore the observed Enter event has left
the top-level menu but has **not** reached the flight update/render owner by
frame 9300.  Do not wire the native menu transition directly to the projection
pipeline or use this interval to seed its record, matrix, page, or cadence.

The next trace must begin after this black transition has completed, in a
quiet input interval, and should first establish an executed `$C2D99C` or
`$C279D0` entry before port integration proceeds.
