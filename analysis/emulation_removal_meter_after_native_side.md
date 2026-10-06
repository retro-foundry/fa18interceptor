# Active-runner emulation meter

Scope: full fixed suite.

Raw CPU work removed (minimum): **38.4011%**. Output parity: **FAIL**. Deletable subsystems: **0/4**.

Memory cutover: **0%** (0 converted, 8988 remaining access sites). Native chipset: **0%**. Native boot: **0%**.

| Scenario | OFF instructions | ON instructions | CPU removed | Guest accesses reduced | Chipset ops reduced | OS steps reduced | Parity |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| demo01 | 185496477 | 26988616 | 85.4506% | -33.4739% | -1.4065% | 3.3084% | FAIL |
| qual_carrier_success | 102572568 | 29844910 | 70.9036% | -104.1336% | -63.8031% | -25.2459% | FAIL |
| qual_fail_crashes | 25578359 | 7738857 | 69.7445% | -26.7415% | -0.7610% | 7.2457% | FAIL |
| adf_gnu | 23703947 | 14601379 | 38.4011% | -12.9285% | 0.0717% | 1.2906% | FAIL |
| adf_msvc | 23703947 | 14601379 | 38.4011% | -12.9285% | 0.0717% | 1.2906% | FAIL |

- CPU share is executed work, not plan completion or whole-game coverage.
- Failed parity makes CPU shares raw observations, not accepted removal progress.
- Residual instructions include original-byte execution between labels and OS RTE opcode helpers.
- Hand-written instruction steps still depend on PC/registers/bus; port step counts expose that debt.
- Bus counts are top-level guest API accesses including instruction fetch/dispatch reads, not bus cycles.
- RAM page counts are overlapping access hits; DMA reads/writes are separate chipset operations.
- Direct DMA and host compatibility memory accesses are outside guest-API page counts; absence does not prove exclusive ownership.
- Traffic reductions do not establish converted memory or native IO/boot.
- Zero OFF dispatches give no service reduction denominator; guest boot is still required.
- Full mission outcomes, progression and active-flight teardown are still unverified.

Frame policy: Ignore source-table Copper fade; require identical selected indices and non-fade RGB.

| Scenario | First strict RGB difference | First non-fade difference | Fade pixels excluded | Final RAM seal | Iterations equal |
| --- | ---: | --- | ---: | --- | --- |
| demo01 | 565 | {'frame': 316, 'pixels': 64} | 5171713 | False | True |
| qual_carrier_success | 423 | {'frame': 374, 'pixels': 55901} | 2061205 | False | True |
| qual_fail_crashes | 263 | {'frame': 213, 'pixels': 1905} | 0 | False | True |
| adf_gnu | 1460 | {'frame': 1460, 'pixels': 26} | 626291 | None | False |
| adf_msvc | 1460 | {'frame': 1460, 'pixels': 26} | 626291 | None | False |
