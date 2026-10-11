# `$C1B664` context-special request entry

Classification: **structural with byte-exact source**. This entry marks
`$C457A3` with 2 and branches on `D6`. The zero branch sets request bit 1,
updates observed state bytes, reads a word at `$68($C46184,D4.W)` using the
index at `$C458DE`, sets word bit 1 at `$C458C6`, and queues the command.

The nonzero `D6` branch enters the separate calculation route at `$C1B6C2`.
The state labels reflect only exact storage operations; a bounded input trace
is needed before associating this entry with a named control.
