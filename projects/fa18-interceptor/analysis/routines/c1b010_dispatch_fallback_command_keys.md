# `$C1B010` fallback command-key dispatch

Classification: **structural with byte-exact source**. This 40-byte fallback
table routes raw `$1B` and `$1A` to separate handlers, raw `$9B` and `$9A` to
the shared fire-request action tail, and raw `$19` to `$C1C06E`. Failure to
match falls directly into the nonzero-context dispatcher at `$C1B038`.

The source is `dispatch_fallback_command_keys.asm`. No semantic assignment is
made for the first two or `$19` routes without a bounded input trace.
