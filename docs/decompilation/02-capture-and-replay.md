# 2. Capture scenarios and replay them

[Process manual](../../REVERSE_ENGINEERING.md) · [Previous](01-project-and-authority.md) · [Next](03-memory-and-hunks.md)

A recording makes a particular execution reproducible. It does not prove an
absence of randomness or identical encounters in independently started games.

## Choose scenarios that answer questions

Maintain a project scenario matrix covering startup/title dismissal, menus,
each gameplay mode, cameras, spawning, weapons/damage, success/failure, save
changes, scene restart and menu return. Actual controls, missions and success
conditions remain game-specific.

For every scenario record purpose, starting conditions, inputs, important
boundaries and required outputs. Include normal and edge paths for each
investigated subsystem. Isolate input changes enough to explain their effects.
A successful route does not exhaustively qualify random spawns or transitions.

Distinguish cold start, saved-game start and serialized emulator restore.
Captured reference state is an oracle input; using it to initialize an alleged
independent native game changes what the comparison establishes.

## Record through the GUI host

The current recorder uses Windows APIs and a compatible Engine9000 host with
the [boot-restore patch](../../patches/engine9000-v062-replay-snapshot.patch).
Supply the manifest's `toolchain` JSON, private config/media and serialized
initial state. The framework does not automatically build this host.

Replace `RESTORE_FRAME` with the observed boot frame at which restoration is
required:

```console
python -m amiga_decomp --project your-game run record_run --initial-state captures/baseline/state.bin --restore-frame RESTORE_FRAME --name menu-to-game
```

This creates `captures/uae/menu-to-game/`, isolates child preferences, disables
debugger function-key bindings, requests restoration and waits for its marker
and state dump. Play the scenario, then close that recording window normally.
Finalize after the process stops:

```console
python -m amiga_decomp --project your-game run finalize_run captures/uae/menu-to-game
```

Finalization excludes the setup prelude and close marker from `playback.e9k`,
checks monotonic labels and restore evidence, records hashes and marks the run
sealed. A rewind requires explicit segmentation. No post-setup input causes
rejection: this finalizer does not seal a timed no-input-only scenario. Use a
separately documented bounded capture for that investigation.

| Artifact | Meaning |
| --- | --- |
| `initial_state.bin` | Supplied baseline before host restoration. |
| `config.uae`, `toolchain.json` | Preserved configuration and tool identities. |
| `inputs.e9k` | Raw recording including setup/termination material. |
| `playback.e9k` | Finalized post-setup input stream. |
| `boot-restore.json`, `restored-state.bin` | Marker and canonical host-restored state. |
| `run.json` | Protocol, restore boundary, hashes, status and artifact names. |
| `saves/`, `appdata/` | Slot state and isolated preferences. |
| `stdout.log`, `stderr.log` | Recording diagnostics. |

Supplied state and canonical host-restored state are distinct artifacts.
Replay helpers compare the canonical restore dump with its recorded hash.

## Input and time coordinates

```text
E9K_INPUT_V1
F 120 K 13 13 0 1
F 124 K 13 13 0 0
```

This illustrates Return press/release, not game navigation. Keyboard fields
are frame, kind, key code, character, modifiers and pressed state. `J` uses
port/button/state, `m` uses port/relative motion, and `b` uses port/button/state.
Use recorded values and the host's event implementation for mappings.

| Coordinate | Meaning |
| --- | --- |
| GUI host frame | Recording/replay host label. |
| Core call | `retro_run` invocation; stepping can call it repeatedly. |
| Hardware/frame callback | Core timing callback observed by the bridge. |
| Game update | Program-specific update tick established by traces. |

`engine9000_bridge --start-frame N` labels the first executed frame N+1; it
does not advance to N. `profile_window --frame-offset N` and
`trace_from_breakpoint --playback-frame-offset N` similarly need a state for
that actual boundary. A restore can resume within a frame: record its routine,
instruction and scheduler boundary.

## Replay images and exact host checkpoints

```console
python -m amiga_decomp --project your-game run playback_run --run captures/uae/menu-to-game
python -m amiga_decomp --project your-game run render_run --run captures/uae/menu-to-game --every 5 --output build/menu-to-game-images
python -m amiga_decomp --project your-game run capture_run_checkpoint --run captures/uae/menu-to-game --frame FRAME --output build/menu-to-game-checkpoint
```

These use the host scheduler and copied mutable saves/preferences. They check
boot restoration and retain logs/derived config identity. `render_run` checks
cadence and coverage through the last input event; sparse images do not prove
every intermediate frame. `capture_run_checkpoint` rejects a frame after the
last input event. A later no-input tail needs a suitable recorded boundary or
separately bounded core run, not an invented host-helper tail argument.

Selected config bytes must equal the sealed config. `former_roots` relocates
matching leading path values only in derived UAE config; copied save wrappers
receive the derived identity while retaining serialized payload bytes. This
does not rewrite arbitrary internal core paths or bridge incompatible core
versions. Retain original and derived hashes.

## Direct-core diagnostics

```console
python -m amiga_decomp --project your-game run engine9000_bridge --restore captures/baseline/state.bin --playback captures/uae/menu-to-game/playback.e9k --frames 200 --output build/core-window
```

Pair input with a state for its actual start boundary. The bridge writes final
RAM/state, registers and hashes. It bypasses GUI host scheduling, so its images
and `capture_keyframes --direct-core` cannot establish sealed visual parity.

Never edit a sealed stream in place. `clip_replay`, `slice_playback`,
`replace_replay_key` and `append_replay_events` produce variants; record parent
hash, transformation and new hash. Text rebasing alone does not create a
matching starting state.

Acceptance: replay restores the canonical boundary, delivers input in its
documented coordinates and reaches the scenario end under the stated policy.
