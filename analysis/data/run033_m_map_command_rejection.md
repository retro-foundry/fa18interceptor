# Run033 Golden Gate checkpoints: M-map command rejection

Classification: **two direct negative command probes**.

The sealed run033 replay contains the user-identified Golden Gate encounters.
To obtain an authoritative M-map landmark position, each saved bridge
checkpoint was restored and given the same 5-frame `M` key press that opens
the map in run003:

```text
F 1 K 109 109 16 1
F 6 K 109 109 16 0
```

Both probes continued for 160 normal frames.  Neither entered the M-map
display; each final screenshot retains the flight HUD/cockpit and horizon.

| restored bridge frame | result frame | video SHA-256 |
| ---: | ---: | --- |
| 5,250 (distant red Golden Gate span) | 5,410 | `e44bcfe4cf4c17fdb1822bb97f97cdeb48b1333e2ade4aa71077c8902d752d20` |
| 6,250 (tower passed) | 6,410 | `f735d70934f422c85df180bbea81410b63e13c4890105746f4b50b6fb5d943cf` |

This rules out transferring the known flight-view bridge location onto the
M-map from either state.  It does not identify the state gate or assert that
the bridge is absent from the M-map.  The existing M-map Golden Gate label
continues to rest only on the directly traced red map line contexts
`$C3559A/$C355D2` and their measured map pan.

Authorities: `build/run033_frame05250_m_press_probe/` and
`build/run033_frame06250_m_press_probe/`, restored from the corresponding
sealed `run033_frame{05250,06250}_checkpoint/state.bin` snapshots.
