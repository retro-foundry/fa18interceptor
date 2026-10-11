# Native PAL voice programs

The actual `fa18_native` path is `port/native/main.c` ->
`native_frontend_tick` -> `native_viewport_tick` -> `native_audio_tick` ->
`advance_voice_channels`. This removes the interrupt-server dependency for
the game's C50158 program/slide updater. It does not implement the separate
C500D8 sample-completion callback or audible playback.

## Original authority and ordering

C5034C-C50352 initializes the audio services and calls C5002A. C5002A installs
the C4FF10 interrupt node with Exec AddIntServer vector -$A8, interrupt number
5 (PAL vertical blank). The original hunk's node has priority $88 (-120) and
handler C50158. C17456 installs C1718E on the same interrupt with priority
zero. Thus the viewport/master fade runs before the four-voice update.
The loaded executable already supplies C4FE28's descriptor pointers and their
slot identities; no reference RAM or OS initialization is imported.

C50158 calls C50212's delay/program owner, then C501E0's output calculation,
then adds the period/volume slides and expires their independent countdowns.
An ending program clears its slot but still publishes and slides that voice
on its final tick. Empty slots retain the previous output. Native publication
resolves the four original descriptor identities to ordinary host channel
state; it does not write to a simulated register bank. The reference runner's
existing register sink uses the same shared calculation and update owner.

The callback executes once per host PAL tick before menu pauses, display
waits and timer polls can return. It does not depend on game update counts or
the still-open task/display pacing model.

## Validation

`python tools/native/check_audio.py` checks 145 source/native sequences of
50 callbacks each: **7,250 complete C50158 calls**, including the original
C50212/C501E0 children, all five original programs, idle/expiring/wrapped
delays and slides, loop counters, signed output limits, empty/aliased slots,
permuted descriptors and a real native checkpoint. Every non-stack RAM byte
below the fixture's C7FC00 stack and all four retained period/volume outputs
agree. CPU/ROM use is confined to the oracle.

The actual native selection at frames 3001/3002 has a suspended game update
while the callback count advances each PAL frame. The new faded master high
word appears in the engine outputs (31 -> 30), and the short tone's program
cursor advances. By frame 3008 its slot is free, delay zero and terminal
program position 96. The old native runner left this tone slot occupied.

Native/reference MSVC builds pass. Menu entry, frontend/menu, complete native
demo, carrier save/restart/reload, crash/re-entry and twelve focused reference
contracts pass. The affected scenarios reuse existing original evidence;
no full original recording replay is repeated. The native link map excludes
CPU, bus, chipset, translation and glue owners.

The demo checkpoint still has the same gameplay counters and 36-tick startup
lead. Its 62 changed RAM bytes are confined to loaded voice records and the
now-released tone slot. This is a sound-state fix, not a timing-parity claim.

## Scope and remaining work

The PAL voice-update connection is **1/1 complete (100% of this callback
scope)**. Sample progression/completion, repetition/chaining and audible
sample output remain open. Full recorded-frame parity remains **0/3 accepted**;
Copper fade remains excluded. The six identified missing drawing owners are
still connected, an inventory that does not measure whole-game completion.
