# `enqueue_command_input_event` at `$C1C23C` (Hunk 5 +`$7794`)

Classification: **behavioural**. This exact routine is reached after the
run003 HUD, rudder, bracket, radar, ECM, and hook command paths. The isolated
HUD trace executes its queue path and returns to `$C0F45C`; several other
bounded traces independently reach the same return routine.

For a non-release raw key and an empty pending flag, it stores the raw byte in
the ten-entry `$C457E1` ring, translates it through byte table `$C331CE`,
stores the translated byte in `$C457EB`, advances the observed indexes/count,
and clears `$C45878-$C4587A`. The source is byte-exact and covers every static
branch; queue consumer semantics remain separate work.
