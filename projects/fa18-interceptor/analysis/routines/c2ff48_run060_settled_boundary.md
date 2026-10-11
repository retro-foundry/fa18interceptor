# `$C2FF48` run060 settled boundary

The sealed run060 replay reaches `$C2FF48` in two captured packets:
`build/port_run060_c2ff48_1200_settle1/` and
`build/port_run060_c2ff48_skip1/`. Both resume for one full frame after the
bounded CPU trace and retain Chip RAM before and after that settling frame.

The first packet enters at debugger frame 8002 and the second reaches its
second invocation at frame 8003. Both execute the `$8400` `DMACON` enable and
call `$C301F6`, whose tuple walker reaches the known display submission path.
The settled Chip RAM differences are only 5 and 8 bytes respectively. They
are asynchronous blitter completion bytes, not a complete source polygon or
an independently identifiable fill shape.

This establishes that run060 supplies a valid post submission synchronization
boundary, but it does not yet justify a general chunky area fill. The native
port therefore keeps `$C2FF48` outside the live renderer until a settled
packet with decoded screen bounds and source color/mask fields is captured.
