# Observed runtime memory map

This is the A500 PAL/OCS runtime map for the pinned Engine9000 configuration,
not a claim about every F/A-18 configuration. It is derived from the core's
memory descriptors and the exported post-load snapshots.

| Address range | Size | Classification | Evidence / use |
| --- | ---: | --- | --- |
| `$000000-$07FFFF` | 512 KiB | Chip RAM | Exported as `chip.bin`; DMA-addressable. Contains runtime code, data and display-related buffers. |
| `$BF0000-$BFFFFF` | 64 KiB decode | CIA I/O | Hardware region from `AMIGA.md`; not dumped as RAM. |
| `$C00000-$C7FFFF` | 512 KiB | Slow (Ranger) RAM | Exported as `slow.bin`; CPU-visible, not assumed DMA-addressable. The menu trace executes code here, including candidate entry `$C0EFD4`. |
| `$DFF000-$DFFFFF` | 4 KiB decode | Custom-chip I/O | Custom register base `$DFF000`. Writes are captured separately with beam position and CPU/Copper provenance. |
| `$FC0000-$FFFFFF` | 256 KiB | Kickstart 1.3 ROM | Pinned local ROM, SHA-256 recorded in `local/toolchain.json`; excluded from mutable runtime-bank exports. |

The Engine9000 descriptor table exposes exactly the two RAM banks above: starts
`$000000` and `$C00000`, each `0x80000` bytes. Snapshot manifests retain those
addresses, bank hashes, CPU registers, core configuration and start-state hash.

## Observed input fields

These are runtime observations from sealed human `run001`, rather than claims
about a complete control structure.

| Address | Width | Observed access / role |
| --- | ---: | --- |
| `$DFF00A` | word | `JOY0DAT`; CPU-read at `$C17196` in the frame-2,697 joystick callback. |
| `$C1AC06` | word | prior low JOY0DAT byte sample, read and written by `$C1718E`. |
| `$C1AC08` | word | prior high JOY0DAT byte sample, read and written by `$C1718E`. |
| `$C45776` | word | first bounded accumulator updated by that callback. |
| `$C45778` | word | second bounded accumulator updated by that callback. |
| `$C4577C` | word | companion control-stage field. The complete `$C13E10->$C25D84` packet reads `$C45778` and writes `$C4577C` at `$C14044`; its axis semantics remain unknown. |
| `$C459B4` | word | index used by `$C13D84` to select `$C46184 + ($C459B4 << 9)` as the current control-stage record. |
| `$C18210` | long | pointer to that indexed record, initialized by `$C13D84` and used by bounded children `$C14876` and `$C2641E`. |
| `$C4599A` | byte | command-request flags. Bit 0 is set by raw `$24` (`G`); bit 1 by raw `$20` (`A`); bit 4 by raw `$44` (Return) after its bounded child; bit 5 by raw `$12` (recorded Shift+E eject); bit 6 by raw `$13` (`R`) after its bounded child; bit 7 by raw `$14` (`T`) after its bounded child. Consumers are unknown. |
| `$C4599B` | byte | command-request flags. Bit 2 is set by raw `$33` (`C`) chaff; bit 3 by raw `$26` (`J`) ECM. |
| `$C4599C` | byte | bit 0 is set by raw `$37` (`M`) before its capped OS/display path. |
| `$C4599D` | byte | dispatcher request-flag byte. Static selector entries set bits 0–7 on distinct context routes; bounded per-route control meaning is incomplete. |
| `$C45792` | byte | observed mode-selector marker written 1 or 2 by `$C1BD78` routes. |
| `$C457A7` | byte | context command index written by `$C1B9CC`; static selector routes increment, decrement, or select values through `$0D`. |
| `$C457A8/$C457A9` | bytes | cleared to zero by context-command selector/publisher routes. |
| `$C458A6` | byte | context-mode byte read by the keyboard dispatcher. `$C1BD78` publishes validated direct, indexed, and special values here. |
| `$C458AD/$C458AE` | bytes | adjacent dispatcher gate and context-state bytes. They are deliberately distinct: `$C1AD74` tests `$C458AD` and loads `$C458AE` into `D3`. |
| `$C458B2` | byte | request-slot byte written with static selector values 0–9 at `$C1B77C`. |
| `$C45785` | byte | dispatcher/context selection byte. Static routes increment/decrement it and use it to select control-record update branches. |
| `$C45C42` | long | static context selection value. `$C1B7F0/$C1B890` step it by `$02000000` with lower `$01000000` and upper `$08000000` clamps. |
| `$C46200` | byte | bit 7 is toggled by the documented gear-command route; raw `$12` ORs bits 1 and 3 (`$0A`). Its low nibble gates the observed raw `$44` Return command update. |
| `$C45842` | byte | written `$08` by the bounded raw `$12` eject-command route. |
| `$C457AB` | byte | set to 1 by `$C1C214`, called by that eject-command route. |
| `$C4584C` | byte | decremented by that chaff handler; the exhausted branch clears it. |
| `$C4584E` | byte | written `$1E` on the observed non-exhausted chaff route. |
| `$C4588B` | byte | written 1 on the observed non-exhausted chaff route. |
| `$C4584D` | byte | decremented by the observed raw `$23` (`F`) flare route. |
| `$C4584F` | byte | written `$1E` on that flare route. |
| `$C457BA` | byte | written 1 by the observed raw `$40` (Space) helper when `$C461E7` has high nibble `$30` in stationary run004; other observed Space states do not write it. |
| `$C461E7` | byte | high nibble is tested by the raw `$40` Space helper. The observed raw `$44` Return route replaces it with the preceding `$10`-step value, wrapping to `$30`; the low nibble is preserved. Stationary run004 distinguishes `$10` and `$30` fire branches. |
| `$C458C6` | word | bit 3 is set by the observed raw `$40` Space helper when `$C461E7` has high nibble `$10` in stationary run004. It remains set when Space is artificially held in an isolated packet, and is clear after the corresponding full-run Space release; the frame-level clear path is unassigned. |
| `$C46986` | byte | bit 3 set by a static alternate branch in that helper. |
| `$C461E9` | byte | shared input mask. Low two bits are replaced with `$02` by raw `$0B` (`-`) and `$01` by raw `$0C` (`=`) in observed throttle routes. Raw `$38` (comma) replaces bits 7:6 with `$80`; raw `$39` (period) replaces them with `$40`. |
| `$C45870` | byte | function-key level byte: post-F10 state probes establish F1--F9 values `$0C,$18,…,$6C` and F10 `$79` through `$C1BD04`; it is cleared by `$C1B602`, called from both observed `=`/`-` throttle routes. |
| `$C457D8` | byte | non-zero enables the function-key level path that publishes scaled words at `$C45778` and `$C4577C`. |
| `$C457A1` | byte | two-state field toggled by raw `$25` (`H`) at `$C1B264`; it is the observed HUD-mode field. |
| `$C457B9` | byte | written one by raw `$14` (`T`) after its bounded child; observed target-selection pending field. |
| `$C461E6` | byte | raw `$20` (`A`) only enters its observed hook-toggle path when this equals `$11`. |
| `$C46186` | word | bit 15 is toggled by the observed raw `$20` hook path. |
| `$C45847` | byte | bit 7 is toggled by the observed raw `$20` hook path. |
| `$C45845` | byte | written three by the observed raw `$20` hook path. |
| `$C45843/$C45844` | bytes | written three by the observed raw `$44` Return weapon-cycle route. |
| `$C458B4` | byte | cleared by the observed raw `$44` Return weapon-cycle route. |
| `$C457B7` | byte | set to 1 by the bounded `$C2374C` consumer of the stationary run004 `$30` selected-fire request. |
| `$C45797` | byte | written 8 by that observed `$C2374C` selected-fire consumer when its selected record matches the active record offset. |
| `$C458B0` | byte | written `$FB` by that same observed selected-fire consumer. |
| `$C457C5` | byte | set to 1 by the observed `$C2374C` record-initialization route. |
| `$C45858` | byte | written `$8C` by that same observed selected-fire consumer. |
| `$C458DE` | word | byte offset added to `$C46184` by raw `$13` (`R`) to select the record containing its radar-range field. |
| `$C4583B` | byte | written three by the bounded raw `$13` radar-range path before common queueing. |
| `$C45840` | byte | written three by raw `$26` (`J`) before its bounded child; observed ECM command mode. |
| `$C458B5` | byte | toggled zero/one by raw `$26` (`J`) after its bounded child; observed ECM enable field. |
| `$C4582F` | byte | receives `$80` for comma and `$40` for period from the bounded rudder handler. |
| `$C45A42` | word | zoom scale. Raw `$1B` (`]`) arithmetic-shifts it right toward `$20`; raw `$1A` (`[`) shifts it left toward `$80`. |
| `$C457DD` | byte | bit 7 records whether the observed zoom scale equals `$80`. |
| `$C4583D` | byte | written three by both bounded bracket zoom paths before common display-update queueing. |
| `$C45891` | byte | written `$FF` by both bounded bracket zoom paths before common display-update queueing. |
| `$C45858` | byte | written `$FF` by a bounded zoom path when `$C457DD` bits 6:0 are zero. |
| `$C457A3` | byte | command-input pending flag set by `$C1C23C` before raw-event queueing. |
| `$C457E1` | byte array | ten-entry observed raw command-event ring written by `$C1C23C`. |
| `$C457EB` | byte array | translated command-event ring written by `$C1C23C` through table `$C331CE`. |
| `$C457F6` | byte | translated-event write index read by `$C1C23C`. |
| `$C457F7` | byte | raw-event write index advanced by `$C1C23C`. |
| `$C457F9` | byte | command-event count, capped at ten by `$C1C23C`. |
| `$C45878-$C4587A` | bytes | cleared by `$C1C23C` on every common command return. |
| `$C457C1` | byte | incremented by the observed `$C0F5F8` post-input tail when `$C4582C` is non-negative. |
| `$C45AD6` | word | decremented by that same observed post-input tail. |
| `$C1820C` | long | function pointer called by the observed post-input tail; resolves to `$C1075A` in training frame 600. |
| `$C182CA` | long | library base loaded into `A6` by `$C53F9C` before its observed `-$19E(A6)` vector call; API identity is unassigned. |
| `$C45890` | byte | periodic countdown decremented and capped/reloaded at eight by `$C11B44`. |
| `$C4588E` | byte | countdown-dependent notification code written by `$C11B44`; observed values include `$86`, `$06`, `$04`, and zero. |
| `$C458DC` | word | current control-record index. `$C12950` publishes `$C46184 + ($C458DC << 9)` to `$C18210`. |
| `$C458A6` | byte | compared with one by the parent-tail helper entry `$C2B3C2`; the training frame-600 packet takes its non-one early return. |
| `$C45891` | byte | nonzero bypasses `$C120B0 -> $C1B906` in the bounded normal-update preparation route. |
| `$C458C6` | word | bit 1 selects the observed early-return route of `$C12098`; bit 4 is conditionally cleared in its static postflight path. |
| `$C4584B` | byte | selects the observed `$C1B27E` control-record input branch; zero takes its local input-mask update route in run003 frame 6,000. |
| `$C459B6` | word | initial gate tested by `$C1B27E`; zero takes the observed control-record input route. |
| `$C45790` | byte | zero causes `$C25A6A` to take its observed immediate return; nonzero continuation is unobserved. |
| `$C458CC` | word | bit 6 is tested by `$C1342C` before its observed self-contained control-record route. |
| `$C461F6` | long | compared with static thresholds by `$C1342C`; its game meaning is unassigned. |
| `$C45B50` | long | mask-and-set field updated by static `$C1342C` paths; only its observed clear-mask route is retained. |
| `$C45B5E/$C45B60/$C45B62` | words | updated by `$C1342C` control-record branches; their coordinate or state meaning is unassigned. |
| `$C45984` | word | result code field written by static paths in `$C12098`; observed values include `$A7` and `$B3`, without assigned display meaning. |
| `$C45986` | word | preparation code field read, written, and scaled into `$C45988` by `$C12098`; observed default `$32`. |
| `$C45988` | word | receives `$C45986 << 4` in `$C12098`; observed default `$320`. |
| `$C45871` | byte | initial gate tested by the bounded `$C32CEE` parent-tail packet. |
| `$C4574A` | word array | read by `$C32CEE` using the byte offset at `$C457C6`; the observed selected word is written to `$C45772`. |
| `$C457C6` | byte | table byte offset used by `$C32CEE` to select a word from `$C4574A`. |
| `$C45772` | word | receives the `$C4574A` table word in the bounded `$C32CEE` packet. |
| `$C457F8` | byte | raw command-event array index read by `$C32CEE` to select a byte from `$C457E1`. |
| `$C081AC/$C081B0` | words | lower/upper bounds applied to `$C45776`. |
| `$C081AE/$C081B2` | words | lower/upper bounds applied to `$C45778`. |

The accumulator and limit fields have intentionally neutral names until their
control semantics are measured across more runs.

## Observed indexed control record

`$C46184 + (index << 9)` is a repeated runtime record layout. Its selected base
is published at `$C18210`; the observed offsets and their bounded access roles
are maintained in [`control_record_layout.md`](control_record_layout.md).

In the early `run001` interval, recorded `J 0 5 1` causes `$C45778` and
`$C4577C` to rise together in `$20`-word steps and saturate at `$03C0`, while
`$C45776` stays `$FEF0`. This is a frame-level scenario contract only; see
`analysis/run001_takeoff_control_samples.md`.

The isolated later `J 0 7` replay reaches the same callback but produces zero
`JOY0DAT` delta and leaves both accumulators unchanged. Do not associate that
recording code with `$C45776` or `$C45778`; see
`analysis/routines/c1718e_joy0dat_callback.md`.


### Observed joystick-state consumer

The isolated `run001` joystick replay reaches `$C13E10` at frame 17 and
returns through `$C25D84` after 301 instructions. It directly reads `$C45778`
and reaches the `$C14044` write to `$C4577C`. The static range
`$C13E10-$C1414E` contains additional reads and writes to both fields, but its
unexecuted branches remain static evidence only. See
`analysis/routines/c13e10_control_state_stage.md`.


## Observed blitter setup fields

| Address | Width | Observed access / role |
| --- | ---: | --- |
| `$DFF002` | byte-tested | Custom-register bit 6 is polled by `$C304B2` before setup writes. |
| `$DFF080-$DFF086` | Copper pointer registers | CPU writes COP1LC `$0000A400` in attract hardware frame 602; that Chip-RAM list is documented in `analysis/attract_copper_display.md`. |
| `$DFF0E0-$DFF0EE` | bitplane pointer registers | the frame-602 Copper list loads four pointers: `$00012BC0`, `$00014B00`, `$00016A40`, `$00018980`. |
| `$DFF040` | word | `BLTCON0`; `$C304B2` writes `$0D0C` on the observed path, with static `$0D3C` alternative. |
| `$DFF042` | word | `BLTCON1`; `$C304B2` writes `$0002`. |
| `$DFF04C` | long | `BLTCPT`; `$C304B2` writes its `D1` pointer. |
| `$DFF050` | long | `BLTBPT`; `$C304B2` writes its `D2` pointer. |
| `$DFF054` | long | `BLTAPT`; `$C304B2` writes its `D1` pointer. |
| `$DFF058` | word | `BLTSIZE`; `$C304B2` writes the word loaded from `$C4596E`. |
| `$C45960` | long | renderer setup source pointer loaded into `D2` by `$C304B2`. |
| `$C4596E` | word | renderer setup size word loaded into `D0` by `$C304B2`. |

These field names follow the OCS custom-register map. Their source-buffer and
primitive ownership remain unassigned.

## Mapped Hunk regions in this runtime

The first relocation-validated original-code mapping is CODE segment 36 at
`$C2F490-$C306B3`; its line emitter lies at `$C2FA7E`. Relocation equations also
identify segment 71 payload at `$C45630` and segment 72 at `$C48390`. See
`analysis/hunk_runtime_mapping.md`; do not generalize these allocation addresses
beyond this pinned runtime.

Rules for reconstruction:

- Treat a pointer used by bitplane, Copper, blitter or Paula DMA as a Chip-RAM
  candidate until its range is observed.
- Treat custom registers as I/O, not ordinary memory; preserve their set/clear
  semantics and write provenance.
- Keep OS/runtime code distinct from game ownership. A RAM address is merely
  observed until caller, bytes and behaviour identify it.
- Add finer regions only when an observed pointer, function contract or custom
  register trace supports the boundary.
| `$C45898` | byte | signed entry guard of static `$C0F5F8`; negative joins its observed tail. |
| `$C4582A/$C4582B` | bytes | static `$C0F5F8` phase and associated flag fields; phase values `$FF`, 1, 2, and 3 select documented routes. |
| `$C45904/$C45908/$C4590C/$C45910/$C45914` | longwords | static `$C0F5F8` offset inputs combined with `$C45AF2`; gameplay meaning unassigned. |
| `$C1AB74` | long pointer | static `$C0F5F8` reads this pointer and may add its calculated offset at `8(a0)`. |
| `$C457C5` | byte | set to one after the static `$C0F5F8` in-range offset route. |
| `$C4599E` | word | set to `$003F` before `$C0F5F8` calls `$C06C02` on an out-of-range calculated offset. |
| `$C458C0` | word | copied to `$C45AD6` by static `$C0F5F8` phase-three route. |
| `$C45660` | long pointer | destination of the 32-word static `$C0F812` table copy from `$C08510`. |
| `$C4564C/$C45650/$C45654` | words | static `$C0F812` inputs compared with `$C560`, `$7E70`, and `$4DE8`; a differing value feeds `$C0F56A`. |
| `$C3F040` | buffer | static `$C0F812` writes a byte field beginning at offset `$15`; rendering purpose unassigned. |
| `$C457AD/$C45857` | bytes | each set to `$FF` by static `$C0F812`. |
| `$C458A0/$C458A1` | bytes | compared by static callback `$C0F946`; equality with a negative `$C45AD6` advances the callback chain. |
| `$C458AC` | byte | cleared by static callback `$C0F920`. |
| `$C08182` | word | pending raw keyboard-event flag consumed and cleared by `$C16C56`. |
| `$C1ABC8` | word | pending raw keyboard-event value returned by `$C16C56` when `$C08182` is nonzero. |
| `$C0815C` | long | handle passed to `$C53C08` by `$C16BF2` before consuming an input event. |
| `$C1ABAC` | long pointer | descriptor read by `$C16BF2`; its word at offset six is consumed and cleared. |
| `$C08134` | long | handle passed to `$C53C8C` after `$C16BF2` consumes the descriptor word. |

## Cockpit display ownership

| Address or range | Width | Observed access / role |
| --- | ---: | --- |
| `$00012BC0-$00014AFF` | 8,000 bytes | Copper BPL1 display buffer during attract cockpit frame 1800. |
| `$00014B00-$00016A3F` | 8,000 bytes | Copper BPL2 display buffer during attract cockpit frame 1800. |
| `$00016A40-$0001897F` | 8,000 bytes | Copper BPL3 display buffer during attract cockpit frame 1800. |
| `$00018980-$0001A8BF` | 8,000 bytes | Copper BPL4 display buffer during attract cockpit frame 1800. |
| `$C456B6-$C456C5` | four long pointers | Active plane-pointer table read by `$C2FB7A` and the `$C2FD8C` blit packet. |
| `$C45984` | word | Line/packet sizing input: `$C2FD8C` shifts it by six and adds `$14` before triggering each of four plane blits. |

The Copper list at `$00A400` loads the visible pointers at `$00A468-$00A484`. The active plane buffers are mutable Chip RAM: deterministic captures establish that the cockpit panel persists across nearby frames while narrow HUD/scenery regions change. See `analysis/cockpit_bitplane_assets.md` and `analysis/routines/cockpit_renderer_writes.md`.

| `$C4567E-$C4568D` | four longwords | Attract cockpit active plane table selected by `$C2F558`: plane bases in order 4, 3, 2, 1 are `$018980`, `$016A40`, `$014B00`, `$012BC0`. `$C2FD8C` adds `$28` before its four blit submissions. |
