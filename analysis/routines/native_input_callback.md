# Native input callback and shared viewport transition

`port/input_callback.c/.h` implements the complete `$C1718E` callback with
ordinary C state, the existing command controls and actual master-volume
fade. The caller supplies sampled eight-bit counters, original bounds,
resolved display pairs and imported palette words. There are no CPU,
machine, bus, ROM or SDL dependencies in this implementation.

The callback subtracts each previous counter as a wrapped signed word, then
performs the original single correction by 256 outside -128..127. When the
player is unready, negative Y deltas use the original arithmetic half, rounded
down. X accumulates and Y subtracts with word wrap before signed upper/lower
clamps; inverted bounds preserve the source's ordering. Previous counters and
the tick word update before the viewport tail and actual `$C24FE8` fade.

Mouse Y is the existing indexed throttle word, and readiness is the existing
indexed readiness byte. Initialization attaches X and ticks to their actual
queue-reachable word owners using `fa18_bind_command_queue_word`. Both bytes
are read before attaching the owner, including when rebinding the same word.
Commands and callback updates therefore use the same canonical fields.

`viewport_transition.c/.h` contains the original transition sequence once.
Both the full callback and the older bounded `viewport_mode.c` adapter use it.
Signed mode/countdown tests, wrapped countdown decrement and asymmetric step
delays remain intact. Transition calls are load, publish, load, publish.
The signed pointer-pair index is captured after the first load and retained
across the second, while publication rereads the actual pair values. Both
loads retain the originally selected palette pointer even if a child changes
the current mode. Arrival copies sixteen words sequentially, including
overlap effects, and sets state to three. A stable nonzero state loads the
dynamic buffer; it does not select the original mode palette again.

`fa18_prepare_native_input_display` installs actual palette selection and
pair publication around the required native LoadRGB4 service. Signed modes
and pair indices resolve only within imported windows supplied by the owner.
Pairs contain actual resolved native object pointers. Missing data or a
failed service returns failure with preceding source writes preserved; no
substitute palette or fabricated pointer is supplied. The host palette
backend must still be integrated with the game's real display objects.

The older wrapper retains its bounded mode 0..15 asset importer, copper
streams and legacy page-publication metadata. Its stable branch now uses the
shared dynamic palette buffer, covered by a regression that changes that
buffer independently of the asset palette. Its bounds and metadata do not
establish complete display ownership for the new native callback.

Run `python tools/recomp/check_native_input_callback.py`. The validator seals
every reachable original instruction byte and executes the complete parent
and actual fade instructions. **4,096 calls cover all 198 source boundaries**
(178 parent and 20 fade), with 2,048 ordered palette-service boundaries.
Comparisons include every Chip/Slow RAM byte except the original CPU ABI stack
`$C7FD00..$C7FF00`, full RAM at each palette call, palette address/identity and
sixteen words, saved display-pair identities, argument count/viewport owner,
and final original return D0=0. Guest/native pointer mapping exists only in
the validation executable.

LoadRGB4 alone is a controlled host-service contract. Fixtures exercise shared
draw-page changes, pair-table changes including overlapping original tables,
palette-word changes, mode arrival and fade-gate changes. They do not change
the parent's CPU ABI locals. Signed modes, counter wrapping, signed/inverted
bounds, countdown delays, stable reloads, terminal overlapping copies and
extreme fade levels are included. This proves the game callback around a
required host service; it does not prove an implemented native palette driver.

GNU strict-warning callback/wrapper contracts and GNU symbol inspection pass
without CPU/bus/machine symbols. MSVC builds the native game and all thirteen
affected input/audio/viewport CTests pass. The unchanged native guard passes
439 files. Queue regression passes 73,728 full-RAM comparisons and all 28
boundaries. Both command parents still pass 16,384 calls, retaining their
985/1,104 parent and 125/125 actual-child coverage and separate checkpoint.
The checkpoint is `analysis/figures/native_input_callback_checkpoint.json`.

This callback and its dependencies are linked into the native game but are
not yet called by its incomplete loop. Full native input scheduling, display
services, asset loading, sample playback and gameplay composition remain
open. The playable reference still uses its CPU and machine model. Shared
viewport code avoids duplicate source rules in this port; its F/A-18 policy
remains game-specific. The independent voice-program library is the current
general component for other ports; see `port/REUSABLE_COMPONENTS.md`.
