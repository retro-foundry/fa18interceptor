# Active-runner emulation meter

Scope: partial discovery probe.

Raw CPU work removed (minimum): **0.4546%**. Output parity: **FAIL**. Deletable subsystems: **0/4**.

Memory cutover: **0%** (0 converted, 9135 remaining access sites). Native chipset: **0%**. Native boot: **0%**.

| Scenario | OFF instructions | ON instructions | CPU removed | Guest accesses reduced | Chipset ops reduced | OS steps reduced | Parity |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| demo01 | 5223176 | 5089217 | 2.5647% | -22.3116% | -0.0266% | -0.1508% | FAIL |
| qual_carrier_success | 3830290 | 3812879 | 0.4546% | -14.7364% | 0.0265% | -0.1481% | FAIL |
| qual_fail_crashes | 5571527 | 5434399 | 2.4612% | -24.6202% | -0.0815% | 0.5318% | FAIL |

- CPU share is executed work, not plan completion or whole-game coverage.
- Failed parity makes CPU shares raw observations, not accepted removal progress.
- Residual instructions include original-byte execution between labels and OS RTE opcode helpers.
- Source-instruction adapters still depend on PC/registers/bus and are included in CPU work.
- Schema 1 omitted adapter instructions; its CPU percentages are superseded and not comparable.
- Bus counts are top-level guest API accesses including instruction fetch/dispatch reads, not bus cycles.
- RAM page counts are overlapping access hits; DMA reads/writes are separate chipset operations.
- Direct DMA and host compatibility memory accesses are outside guest-API page counts; absence does not prove exclusive ownership.
- Traffic reductions do not establish converted memory or native IO/boot.
- Zero OFF dispatches give no service reduction denominator; guest boot is still required.
- Full mission outcomes, progression and active-flight teardown are still unverified.

Frame policy: Ignore source-table Copper fade; require identical selected indices and non-fade RGB.

| Scenario | First strict RGB difference | First non-fade difference | Fade pixels excluded | Final RAM seal | Iterations equal |
| --- | ---: | --- | ---: | --- | --- |
| demo01 | 619 | {'frame': 346, 'pixels': 128} | 0 | None | True |
| qual_carrier_success | 446 | {'frame': 446, 'pixels': 1631} | 0 | None | True |
| qual_fail_crashes | 263 | {'frame': 263, 'pixels': 1632} | 0 | None | True |
