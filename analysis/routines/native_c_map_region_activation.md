# Map and region activation, 2026-10-01

The registered count increases from 414 to 418 of 624 game entries. This batch
activates existing source-backed map/region C and completes the small map
depth-stage parent. It introduces no new mechanics or placeholder behavior.

| Original entry | Readable C operation | Source timing bridge |
| --- | --- | --- |
| C2AA9C | `prepare_map_packet_depth`, shared `fa18_prepare_map_packet_depth_stage`, then normal/wide pass sequence | `glue_C2AA9C_step` |
| C2AB34 | `run_map_packet_pass`, wide directory/layout | `glue_C2AB34_step` |
| C2AB5A | `run_map_packet_pass`, normal directory/layout | `glue_C2AB5A_step` |
| C2B05A | `probe_record_regions`, directory ray crossings and placed polygons | `glue_C2B05A_step` |

The original routines are documented by `tools/recomp/port_info.py` and the
existing `c2aa9c_map_packet_depth_stage`, `c2ab34_wide_map_packet_directory_setup`
and `c2b05a_record_region_probe` reports. The map siblings share one timing
body; the parent retains LINK/UNLK and BSR boundaries. The region bridge
includes the C2B042 fault prefix and preserves the source's D2-D5 save followed
by the unusual D2/D4-D6 restore. Interrupts, nested renderer calls and frame
crossings resume at the original instruction boundary.

## Proof

GNU headless and MSVC Release builds pass. The final 418-entry full gate has
712,856 completed shadow matches and 1,126,474 sandbox matches, zero
mismatches, all three sealed final RAM hashes unchanged, and identical poison
frames. New parents absorb calls previously compared as separate children,
so aggregate call totals fall even though coverage increases.

| New entry | Completed shadow matches | Sandbox matches |
| --- | ---: | ---: |
| C2AA9C | 14 | 3,806 |
| C2AB34 | 259 | 35 |
| C2AB5A | 5 | 17 |
| C2B05A | 3 | 18 |

Interrupts prevent many live source calls from completing a shadow comparison;
they are retained as incomplete observations, not credited as matches. The
sandbox completes thousands of parent calls with events held, while the
separate live check covers their actual timing.

Fresh source OFF and isolated ON RGB444 streams match byte-for-byte for all
36,236 frames: demo01 (20,833), qual_carrier_success (12,353), and
qual_fail_crashes (3,050). Reproduce with:

```powershell
$env:PORTS_ONLY='C2AA9C,C2AB34,C2AB5A,C2B05A'
& 'C:\Program Files\Git\bin\bash.exe' scripts/recomp_live_check.sh
Remove-Item Env:PORTS_ONLY
```

The instruction oracle covers 735 new instructions (407 map, 328 region) and
23,520 fixtures. All registered timing groups together cover 1,967 instructions
and 62,944 fixtures, checking registers, full SR, PC, cycles and RAM. Both new
groups also pass with five-plane DMA contention enabled:

```text
python tools/recomp/check_active_planes_step.py --group all
python tools/recomp/check_active_planes_step.py --group map --bus
python tools/recomp/check_active_planes_step.py --group regions --bus
python tools/recomp/check_map_region_glue.py
```

The last command builds an isolated whole-call registry variant through the
shared object cache, leaving the normal registry and executable untouched.
It disables timing steps for these four entries so that the readable domain
code and register-observation hooks are independently exercised. It matches
281 shadow and 3,988 sandbox calls with zero mismatches.

## Bus-read regression

The first map bridge passed CPU-only instruction fixtures and a 500-frame
screen probe, but the wide pass first differed at demo frame 4,555 by 701
pixels. A 600-frame boundary trace found the first timing difference much
earlier: C2AD22 in frame 297, immediately after CLR.W at C2AD1E. The bridge
read the destination before clearing it; the authoritative runner's CLR
handler only writes it. Those extra bus accesses accumulated DMA delays.

Removing the extra reads made the 600-frame map boundary trace identical and
the complete live batch exact. DMA fixtures now vary the horizontal bus phase
across 32 cases. A deliberately incorrect bridge restoring the extra CLR read
is rejected at C2AB3A case 18: source remaining cycles 99,999,984 versus C
99,999,980. One fixed phase had missed that error; phase variation is required.

## Remaining work

The complete registered ON set still first differs at demo frame 416 by 361
pixels. This batch establishes isolated fidelity, not whole-game completion.
Prioritize new coverage next: C279D0, C1D10C, and explicit callback contracts
for C0F5F8 and C1CB14/C1CB26. Native backend work and deferred Kickstart work
remain in the order specified by CURRENT_PORT_HANDOFF.md. Replays clean their
large scratch streams; proof variants reuse compiler objects.
