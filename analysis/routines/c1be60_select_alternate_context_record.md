# `$C1BE60` alternate-context record selector

Classification: **structural with byte-exact source**. When bit 3 of `$C458CC` is clear, the routine scans `$C42A02` in 16-byte steps, skips negative records, publishes `$C45848`, and prepares the queue context subject to two state gates.

When that bit is set, the alternate entry either increments `D4` for mode 0 or 2, or queues directly. The retry entry at `$C1BEB2` updates `$C457A3` then either retries `$C1B684` or queues.
