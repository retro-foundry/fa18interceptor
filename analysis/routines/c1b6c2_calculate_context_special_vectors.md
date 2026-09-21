# `$C1B6C2` context-special vector calculation

Classification: **structural with byte-exact source**. The route derives an
indexed signed word from `$C42A02`. Negative values select one of two three-word
parameter sets and call `$C091E0`; nonnegative values load six words from
`$C42A54`, combine them with a two-word table at `$C1D7E2`, and call `$C0915A`.
It then updates observed command-state bytes and either retries `$C1B68C` or
queues the command.

A no-input frame-6000 breakpoint probe did not reach this entry within 120
frames. The arithmetic description is static until a bounded hit is captured.
