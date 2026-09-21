# `H` input full-frame experiment

This experiment uses normal full-frame replay only. It deliberately does not
interpret instruction stepping as input evidence.

| Item | Evidence |
| --- | --- |
| Engine9000 recorded key representation | Native `WM_KEYUP` probe for virtual `H` recorded `K 104 104 16 0`. |
| Replayed input | `local/attract_hud_toggle.e9k`: mode-1 selection at 60/64, `H` down/up at 450/454. |
| Control | `local/start_demo.e9k`, same demo selection and no `H`. |
| Comparison point | Frame 460 from `captures/baseline_menu/state.bin`. |
| Effect | Final video hash is equal; Chip RAM is equal; Slow RAM, registers and cycle count differ. |
| Repeatability | Two independent full-frame `H` replays at frame 460 match Chip RAM, Slow RAM, registers, cycles, video and audio exactly. |

`GAME.md` documents `H` as HUD control, but this demo experiment does not show a
pixel change at frame 460. It proves only that this Engine9000 key representation
produces a deterministic, game-visible state change in this scenario. The
specific consumer and state-field meaning are unknown.

The comparison also tested an instruction-stepped replay of frames 450-460. It
matched Chip RAM and video but diverged in Slow RAM, registers and cycles, so it
is rejected as keyboard-handler evidence.

An Engine9000 write watchpoint armed at frame 450 on `$C457E4` did not fire,
even without an access-source filter, despite the final full-frame state having
`$25` there in the `H` replay. This means the final-memory diff is not currently
a direct 68000-write watchpoint anchor; it may be produced through the UAE input
path or a wider/unhooked state update. No CPU writer is inferred from it.
