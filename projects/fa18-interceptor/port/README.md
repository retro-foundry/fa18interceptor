# Historical port metadata

The playable native C port lives in
[fa18-interceptor-decomp](https://github.com/retro-foundry/fa18-interceptor-decomp).
The remaining port sources, original CPU/machine comparison runners and their
runtime test harness were retired here at the user's request on 2026-10-11.

This directory retains the historical `native_data_allowlist.txt` provenance
record. The protected legacy checks `scripts/check_native_build.py` and
`scripts/native_frame_count.py` are also retained unchanged for historical use.
The last checkout containing the comparison runtime is `b04eaa35`; archived
reports and their commands describe that earlier source, not an active build.

Original Engine9000 capture tools, disk/disassembly/Ghidra analysis and sealed
recordings remain in the reverse-engineering repository. Native builds and
gameplay changes belong to the standalone checkout.
