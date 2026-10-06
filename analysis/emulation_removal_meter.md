# Active-runner emulation meter

Scope: full fixed suite.

Raw CPU work removed (minimum): **38.3921%**. Output parity: **FAIL**. Deletable subsystems: **0/4**.

Memory cutover: **0%** (0 converted, 8988 remaining access sites). Native chipset: **0%**. Native boot: **0%**.

| Scenario | OFF instructions | ON instructions | CPU removed | Guest accesses reduced | Chipset ops reduced | OS steps reduced | Parity |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| demo01 | 185496477 | 26861532 | 85.5191% | -32.6418% | -0.8654% | 3.3650% | FAIL |
| qual_carrier_success | 102572568 | 30901194 | 69.8738% | -99.4079% | -56.4518% | -23.2096% | FAIL |
| qual_fail_crashes | 25578359 | 7728346 | 69.7856% | -25.8243% | -0.1949% | 7.5598% | FAIL |
| adf_gnu | 23703947 | 14603515 | 38.3921% | -12.9112% | 0.0619% | 1.1421% | FAIL |
| adf_msvc | 23703947 | 14603515 | 38.3921% | -12.9112% | 0.0619% | 1.1421% | FAIL |

- CPU share is executed work, not plan completion or whole-game coverage.
- Failed parity makes CPU shares raw observations, not accepted removal progress.
- Residual instructions include original-byte execution between labels and OS RTE opcode helpers.
- Hand-written instruction steps still depend on PC/registers/bus; port step counts expose that debt.
- Bus counts are top-level guest API accesses including instruction fetch/dispatch reads, not bus cycles.
- RAM page counts are overlapping access hits; DMA reads/writes are separate chipset operations.
- Traffic reductions do not establish converted memory or native IO/boot.
- Zero OFF dispatches give no service reduction denominator; guest boot is still required.
- Full mission outcomes, progression and active-flight teardown are still unverified.
