# run075 frame-230 demonstration selection

Authority: sealed `captures/run075` (canonical restore SHA-256
`760d729341bebb9d6aa49450e7c7a6b760fd2d35321e9bc1e4c5f09c4015a4f4`),
bounded direct-core traces in `build/port_run075_c1bd78_trace/` and
`build/port_run075_c0fd10_trace/`, full-frame word samples in
`build/port_run075_menu_tick_words.json`, and the byte-exact
`source_amiga/observed/dispatch_top_level_menu_selection.asm`. The
host-scheduled screenshot oracle is `build/port_run075_demo_oracle/`.

## Input producer: `$C1BD78` family

The recording presses frontend key `1` at frame 230 (`F 230 K 49 49 16 1`)
and releases it at frame 234. The breakpoint at `$C1BD78` hits in direct-core
frame 230 with `D4.b=0`, `$C458A6=0`, `$C45792=0`, and longword
`$C4FDBC=$00C57288`. The observed branch passes the zero-mode and nonzero
`$C4FDBC` checks, stores `1` to `$C45792`, changes `D4` to `$7F`, and stores
`$7F` to `$C458A6` at `$C1BDEC`. It then calls `$C3318E`; the meaning and
side effects of that callee are not established by this bounded packet.

This proves one *key-1 to menu-mode-$7F* route in run075. It does not make
`$7F` a raw keycode, nor explain every `$C1BD78` branch.

`port/menu.c:fa18_select_run075_demo_mode` ports this bounded branch after
the input dispatcher has identified the recorded first menu command. It maps
the proven zero-context state to `selected_mode=0x7F` and
`selection_marker=1` in `FA18MenuState`. The raw frontend-key mapping,
record-enable predicate, and `$C3318E` side-effect packet are still separate
contracts and are deliberately not represented as Amiga storage.

## Consumer: `$C0FCB4` demo arm

At the `$C0FD10` breakpoint in direct-core frame 234, the callback has already
passed its positive-mode check and the `$C11312` / `$C2FD22` calls. The saved
mode is `$7F`; `$C4582B=0`; `$C45B5B=$F7` (bit 7 set). The exact bounded path
compares the saved mode with `$7F`, writes selector `101` at `$C4574A`, writes
zero at `$C4574C`, writes `$00D2` (210) to `$C45AD6`, and installs callback
`$00C0FECE` at `$C1820C`. The previous word at `$C45AD6` was `$EF0A`; the
previous callback was `$00C0FCB4`. The selected-mode byte remains `$7F`.
The adjacent static branch would select `$0096` when `$C4582B!=0` or bit 7 of
`$C45B5B` is clear; run075 does not exercise that branch.

`port/menu.c:fa18_schedule_demo_selection` ports exactly the observed
`$C0FD10-$C0FDCE` demo path after the two earlier helper calls. Its pre/post
fixture comes from the trace's Slow-RAM dumps. The native `FA18MenuState`
stores a selector queue, delay ticks, and a typed
`FA18_MENU_CALLBACK_DELAYED_TRANSITION` continuation; it does not expose
original storage or callback addresses as runtime state. The wider `$C0FCB4`
callback, including helper effects and non-demo modes, remains unported.

## Countdown and transition boundary

`$C0FECE` is entered at direct-core frame 235. Its byte-exact prefix reads
mode `$7F` and signed delay `$C45AD6`; while nonnegative it returns at
`$C1017A`. The shared `$C0F5F8` post-input tick is the documented decrement
producer. End-of-frame samples show `$00D1` at frame 234, `$00CC` at frame
235, `$0005` at frame 269 and `$FFFF` at frame 270. There are normally five
or six game ticks per captured video frame here; the delay is **not 210 video
frames**. The negative-delay body at `$C0FEEA` is reached at frame 270.

The longer bounded continuation in `build/port_run075_c0feea_25k_trace/`
reaches the `$C0FFDA` mode table. It sees saved mode `$7F`, finds the matching
table entry, and jumps to `$C1000A`. Before that jump the common path clears
`$C457D7`, calls `$C17B08` twice, writes `$003F0000` at `$C4FF26`, takes the
`$C45B5B` bit-4 helper route, writes `$00B3` at `$C45984`, resets message
state, writes delay 4, sets `$C45897=3`, `$C45898=2`, and calls `$C28722`
and `$C11B0E`. These writes/calls are observed; the helpers' complete game
meaning is not yet assigned. The common path installs `$C103E4` before the
table jump.

`fa18_prepare_run075_demo_transition` ports the bounded run075 state subset
before that table: it records row limit 179, delay 4, transition stage 3,
post-input phase 2, auxiliary flag 1, a cleared followup flag, and the typed
intermediate continuation. These are native `FA18MenuState` fields, not a
copy of the original addresses. Effects of `$C17B08`, `$C17E4A`, `$C11312`,
`$C28722`, and `$C11B0E`, plus the other common stores, remain separate
evidence boundaries.

At `$C1000A` in direct-core frame 271, a separate five-instruction fixture
`build/port_run075_c1000a_trace/` proves the demo-specific arm. Its prestate
has `$C457AE=0`, `$C1820C=$00C103E4`, `$C45AD6=4`, and mode `$7F`.
`$C1000E` writes `1` to `$C457AE`; `$C1001A` replaces the callback with
`$00C0FA04`; the delay and selected mode remain unchanged. Native
`port/menu.c:fa18_enter_demo_followup` maps this to the
`demo_followup_pending` field and typed `FA18_MENU_CALLBACK_DEMO_ENTRY`
continuation in the same `FA18MenuState`. The shared setup and later
`$C0FA04` direct expiry sequence are separate bounded contracts, documented
in `c0fa04_post_input_followup.md` and
`c0faa4_run075_scene_initialization.md`.

`port/demo.c` composes the proved branch fragments as `FA18DemoController`.
Starting after the recorded first menu command, it selects the demo mode,
schedules its selector/delay, applies 211 post-input ticks, executes the
bounded common setup, and reaches this demo-entry continuation. Its test
checks the state transition only; it does not turn tick count into a video
frame count or assign renderer/helper behavior that is not yet proved.

The host-scheduled visual oracle shows menu at frame 200, black transition at
300, and a cockpit view at 400. These are visible checkpoints, not proof of
which individual routine drew each pixel. Direct-core instruction stepping is
used only for the bounded CPU contracts above; its screenshots are not used
as visual authority.

Meaning levels: `$C1BD78` run075 branch **scenario**; `$C0FD10-$C0FDCE`
demo arm **port-contract**; `$C0FECE` countdown gate **scenario**;
`$C1000A-$C10020` demo followup arm **port-contract**; shared transition
helpers **dataflow**.

## Reproduce the bounded packets

Use the sealed `captures/run075/restored-state.bin` and
`captures/run075/playback.e9k` with `scripts/trace_replay_breakpoint.py`.
The entry/arm-frame/instruction triples for the saved packets are:

| Output directory | Breakpoint | Arm frame | Instructions |
| --- | ---: | ---: | ---: |
| `build/port_run075_c1bd78_trace` | `$C1BD78` | 225 | 140 |
| `build/port_run075_c0fd10_trace` | `$C0FD10` | 225 | 200 |
| `build/port_run075_c0feea_25k_trace` | `$C0FEEA` | 235 | 25,000 |
| `build/port_run075_c1000a_trace` | `$C1000A` | 235 | 5 |

All use `--frames 800` (a tighter bound is also sufficient). The frame-word
series was made with `scripts/sample_replay_memory.py --frames 280 --word
0xC45AD6 --word 0xC458A6 --word 0xC1820C --word 0xC1820E --sample-every 1
--sample-first 228 --sample-last 280 --input-kind K`.
