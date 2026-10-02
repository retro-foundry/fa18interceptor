"""Compare complete placement-cache ordering writes with original instructions.

Default mode proves domain memory. --glue additionally proves the original
caller's live CPU outputs; --recorded keeps normal liveness in glue mode.
Instruction/event timing has a separate independent and live-frame gate.
"""
import argparse
from concurrent.futures import ThreadPoolExecutor
import json
import os
from pathlib import Path
import re
import subprocess
from check_record_region_probe import ROOT, build_oracle, default_bash


def recorded_proof(capture_call=0, glue=False):
    """Isolated whole-call registry; glue mode keeps original liveness."""
    output = ROOT / "build/recomp"
    proof = "adapter" if glue else "memory"
    registry = output / f"placement_{proof}_registry.c"
    registry.write_text('''/* Domain-memory proof only: no CPU-result claim. */
#include "recomp_ports.h"
#include "glue.h"
#include "placement_order.h"
extern int glue_C1E540(void);
#include <stdio.h>
#include <stdlib.h>
static int memory_only_call(void) {
    static unsigned calls;
    if (++calls == CAPTURE_CALL) {
        const char *path = getenv("FA18_PLACEMENT_ENTRY_DUMP");
        uint32_t registers[18];
        unsigned i;
        FILE *file = path ? fopen(path, "wb") : NULL;
        if (!file) { fputs("placement entry capture: cannot open output\\n", stderr); abort(); }
        for (i = 0; i < 16; ++i) registers[i] = REG_DA[i];
        registers[16] = m68k_get_reg(NULL, M68K_REG_SR); registers[17] = REG_PC;
        if (fwrite(registers, sizeof registers, 1, file) != 1
            || fwrite(fa18_machine->chip, FA18_CHIP_SIZE, 1, file) != 1
            || fwrite(fa18_machine->slow, FA18_SLOW_SIZE, 1, file) != 1 || fclose(file)) {
            fputs("placement entry capture: short write\\n", stderr); abort();
        }
    }
    order_placement_cache(); return glue_return();
}
const FA18Port fa18_ports[] = {
    {0xC1E540, memory_only_call, "order_placement_cache_memory_only", 0},
    {0,0,0,0}
};
const int fa18_port_count = 1;
'''.replace("CAPTURE_CALL", str(capture_call))
        .replace("order_placement_cache(); return glue_return();",
                 "return glue_C1E540();" if glue else "order_placement_cache(); return glue_return();")
        .replace("memory_only", "adapter" if glue else "memory_only")
        .replace("Domain-memory proof only: no CPU-result claim.",
                 "Normal caller liveness proof." if glue else "Domain-memory proof only: no CPU-result claim."))
    liveness = output / "placement_memory_liveness.c"
    executable = output / f"placement_{proof}.exe"
    build = [
        "python", "scripts/build_recomp.py", "--output", str(executable.relative_to(ROOT)),
        "--replace-source", "port/game/glue/ports.c=" + str(registry.relative_to(ROOT)),
    ]
    if not glue:
        source_liveness = ROOT / "port/recomp/generated/recomp_liveness.c"
        text, replacements = re.subn(
            r"\{0xC1C9AE, 0x[0-9A-F]+, 0x[0-9A-F]+, 0x[0-9A-F]+\}",
            "{0xC1C9AE, 0xC000, 0x00, 0x00}", source_liveness.read_text())
        if replacements != 1:
            raise RuntimeError("expected exactly one original C1E540 caller return mask")
        liveness.write_text(text)
        build += ["--replace-source", "port/recomp/generated/recomp_liveness.c=" + str(liveness.relative_to(ROOT))]
    subprocess.run(build, cwd=ROOT, check=True)

    def check(recording):
        results = {}
        for mode in ("shadow", "sandbox"):
            report = output / f"placement_{proof}_{recording.name}_{mode}.json"
            environment = dict(os.environ)
            if capture_call:
                environment["FA18_PLACEMENT_ENTRY_DUMP"] = str(
                    output / f"placement_{recording.name}_{mode}_call{capture_call}.bin")
            run = subprocess.run([
                str(executable), "--state", str(recording / "state.bin"),
                "--input", str(recording / "input.fa18in"), "--rom", "local/system/kick13.rom",
                "--to-end", "--ports", mode, "--ports-report", str(report),
            ], cwd=ROOT, env=environment, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE, text=True)
            if run.returncode:
                raise RuntimeError(f"{recording.name} {mode}: {run.stderr.strip()}")
            rows = json.loads(report.read_text())
            if len(rows) != 1 or rows[0]["entry"] != "C1E540" or rows[0]["mismatched"]:
                raise RuntimeError(f"{recording.name} {mode}: missing or mismatched {proof} comparison")
            results[mode] = rows[0]
            print(f"{recording.name} {mode}: {rows[0]['matched']} {proof} matches, "
                  f"{rows[0]['incomplete']} incomplete", flush=True)
        return results

    recordings = sorted(p.parent for p in (ROOT / "captures/native").glob("*/input.fa18in"))
    if not recordings:
        raise RuntimeError("no sealed recordings")
    with ThreadPoolExecutor(max_workers=3) as pool:
        results = list(pool.map(check, recordings))
    if not any(r[mode]["matched"] for r in results for mode in ("shadow", "sandbox")):
        raise RuntimeError(f"C1E540: no completed recorded {proof} comparisons")
    scope = "normal CPU/RAM outputs; live timing NOT tested" if glue else "domain memory; CPU/live timing NOT tested"
    print(f"C1E540 recorded {scope}: " + ", ".join(
        f"{sum(r[mode]['matched'] for r in results)} {mode} matches"
        for mode in ("shadow", "sandbox")))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases", type=int, default=2048)
    parser.add_argument("--glue", action="store_true", help="also prove the normal caller's live CPU outputs")
    parser.add_argument("--fixture", type=Path, help="replay a captured original entry instead of synthetic cases")
    parser.add_argument("--recorded", action="store_true",
                        help="also compare all sealed recordings; --glue retains normal CPU liveness")
    parser.add_argument("--capture-call", type=int, default=0,
                        help="save a bounded recorded entry fixture at this call for diagnosis")
    args = parser.parse_args()
    if args.cases <= 0:
        parser.error("--cases must be positive")
    if args.capture_call < 0 or (args.capture_call and not args.recorded):
        parser.error("--capture-call must be nonnegative and requires --recorded")
    executable = build_oracle("placement_order_oracle",
                              "tools/recomp/placement_order_oracle.c", default_bash())
    fixture_args = ["--fixture", str(args.fixture.resolve())] if args.fixture else [str(args.cases)]
    if args.glue:
        fixture_args.append("--glue")
    subprocess.run([str(executable), *fixture_args], cwd=ROOT, check=True)
    if args.recorded:
        recorded_proof(args.capture_call, args.glue)


if __name__ == "__main__":
    main()
