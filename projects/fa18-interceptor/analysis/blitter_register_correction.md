# OCS blitter pointer-register correction

The OCS pointer layout is:

| Register | Custom offset |
| --- | ---: |
| `BLTCPT` | `$048` |
| `BLTBPT` | `$04C` |
| `BLTAPT` | `$050` |
| `BLTDPT` | `$054` |
| `BLTSIZE` | `$058` |

Several reconstructed source labels had shifted the B/A/D pointer names while
their assembled displacement bytes remained correct. The byte-exact source has
been relabelled without changing its emitted instructions; reconstruction
verification is the acceptance check.

## `$C30668` consequence

The observed `$C30668-$C306B3` leaf writes `D2` to `BLTAPTL` (`$052`) and
`D7` to both `BLTDPT` (`$054`) and `BLTCPT` (`$048`) before its `BLTSIZE`
write. It does not write `BLTBPT`. Thus, in the frame-1800 cockpit trace,
`D7` is the active-plane destination for the `$C306AE` jobs; it is not a
graphics-source pointer.

`scripts/inventory_blitter_jobs.py` reconstructs full channel state at every
CPU `BLTSIZE` write. Its attract output records 132 jobs, including 65 with an
active cockpit-plane channel, in
`analysis/attract_cockpit_blitter_jobs.json`. This is placement/dataflow
evidence only: inherited A/B/C registers and minterms still need a focused
producer trace before any source range is classified as an immutable graphic.
