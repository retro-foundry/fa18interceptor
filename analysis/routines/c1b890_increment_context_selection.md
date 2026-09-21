# `$C1B890` context selection increment

Classification: **structural with byte-exact source**. This complementary
route sets request bit 7, adds `$02000000` to `$C45C42` with an `$08000000`
upper clamp when its state flag is set, or increments `$C45785` with the
captured wrap and maximum behavior when it is clear.

The shared byte-store tail is at `$C1B860`; the raw-key association remains
unassigned pending bounded input evidence.
