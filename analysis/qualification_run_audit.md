# Qualification-run audit

This audit separates a recorded Qualification menu selection from a successful
qualification outcome. A key-5 press is only a top-level menu request; success
requires a direct result-screen or persistent-state oracle.

| Run | Source status | Key-5 press | Last recorded frame | Selector-74 probe | Final rendered frame | Outcome supported? |
|---|---|---:|---:|---|---|---|
| run024 | sealed | 584 | 27,437 | not this audit's target | crash-result sequence already documented | failed/crash result only |
| run026 | raw recording | 301 | 7,030 | absent (five startup records) | active cockpit | outcome unknown |
| run027 | raw recording | 389 | 27,047 | absent (five startup records) | active cockpit | outcome unknown |
| run028 | raw recording | 377 | 10,526 | absent (five startup records) | active/near-blank cockpit | outcome unknown |
| run029 | sealed | 266 | 14,298 | absent in existing bounded monitors | active cockpit after reported relaunch | leading success candidate |
| run060 | sealed, deterministic boot-restore v1 | not re-audited here | 10,085 | not required for visual result | native frame 9,545: qualification-success text | **successful qualification** |

`run026`, `run027`, and `run028` were not sealed previously because they retain
their terminal `F <frame> C` close marker. `scripts/profile_window.py` now
ignores that marker as a non-input event, allowing deterministic read-only
replay directly from the raw recordings without modifying them.

Probe artifacts:

- `build/run026_selector74_probe/report.json`
- `build/run027_selector74_probe/report.json`
- `build/run028_selector74_probe/report.json`
- `build/run026_final_frame/frame_07030.png`
- `build/run027_final_frame/frame_27047.png`
- `build/run028_final_frame/frame_10526.png`

The selector result is bounded to the standard `$C32D24` message path. It does
not prove that success can never be drawn through another route. A final
cockpit frame is compatible with a post-success relaunch, so it cannot decide
the outcome.

Run060 supersedes run029 as the qualification-success scenario authority. Its
native Engine9000 replay restores the recorded state twice with the canonical
SHA-256 `760d729341bebb9d6aa49450e7c7a6b760fd2d35321e9bc1e4c5f09c4015a4f4`.
The native renderer's frame 9,545 directly shows `LANDING SUCCESSFUL` and
`YOU ARE NOW QUALIFIED FOR MISSIONS`. This establishes the scenario outcome,
not the text producer, qualification-status writer, or persistence rule; those
still require an exact native checkpoint and bounded code trace. See
`analysis/run060_qualification_success.md`.
