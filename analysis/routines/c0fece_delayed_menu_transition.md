# `$C0FECE`: delayed menu-transition dispatcher

Classification: **scenario-backed transition routing with unknown gameplay
ownership**.  The static body is `$C0FECE-$C1017C`; the bounded qualification
scenario enters its negative-delay body at `$C0FEEA`.

## Run024 qualification route

`build/run024_qualification_transition_body_trace/` reaches `$C0FEEA` at
frame 627 after the previously proved qualification selector.  It sees the
saved mode value 9, performs its common transition setup, and scans the
eight-byte selector/jump table at `$C0FFDE`.  The matching zero-offset table
entry jumps through `$C0FFE2` to `$C10102`.

The observed branch at `$C10102`:

- sets `$C45848` to three;
- clears `$C458AE` and `$C457AD`;
- calls `$C28722`, `$C0924A`, `$C11312`, `$C082B0`, and `$C1C860`;
  the sealed run060 trace now establishes that `$C0924A` performs the early
  selected-root `+$14/+18/+1C` placement update; see
  `../data/run060_root_pose_initialization.md`;
- sets `$C458AD` and `$C45795` to one;
- clears `$C45986`, `$C45988`, and `$C45785`;
- installs `$C101FC` in callback slot `$C1820C`; its byte-exact countdown
  contract is documented in
  `analysis/routines/c101fc_advance_post_gate_callback_stage.md`.

The trace returns through `$C1017C` after 45,047 instructions.  `$C11312`
clears the selector-sequence head in this route, confirming that the earlier
qualification gate message is not replaced by another text queue here.

This establishes a real post-gate transition branch for selected mode 9.  It
does not prove the gameplay meaning of `$C10102`, the helper calls, or the
state fields above; in particular it does not identify qualification
persistence.

## run075 demo countdown boundary

The run075 demo selection installs this callback at direct-core frame 234
with mode `$7F` and `$C45AD6=$00D2`. The shared `$C0F5F8` tick decrements
the word five or six times per video frame in this interval. `$C0FECE`
reaches its signed-negative body at `$C0FEEA` in frame 270 with delay `$FFFF`;
the `$C0FFDA` table selects the mode-$7F `$C1000A` arm in the bounded
continuation. At frame 271 that arm writes `$C457AE=1` and callback `$C0FA04`
after the common setup has installed `$C103E4` and delay 4. The exact
pre/post branch contract is ported in `port/menu.c`; shared helpers and later
`$C0FA04` effects remain unresolved. See
`analysis/routines/c0fcb4_run075_demo.md`.

The entry gate itself is now native `fa18_menu_delay_expired`: its
`FA18MenuState.delay_ticks` field is `int16_t`, preserving the original signed
test. It returns false for zero and true for `-1`, exactly matching the
`BPL` branch at `$C0FEE6`. `fa18_menu_post_input_tick` now ports the proved
shared decrement itself with 16-bit wrap; callback dispatch and common helper
effects remain separate native contracts.
