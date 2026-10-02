"""Prove complete C1D10C domain memory against original execution.

The isolated recording registry deliberately checks only restored A6/A7;
this is NOT the normal CPU adapter or an instruction/event timing proof.
Captured entry fixtures additionally compare all Chip/Slow bytes outside
the source's bounded private stack, without a caller liveness exemption.
"""
import argparse
from concurrent.futures import ThreadPoolExecutor
import json
import os
from pathlib import Path
import re
import subprocess
from check_record_region_probe import ROOT, build_oracle, default_bash


def write_if_changed(path, text):
    if not path.exists() or path.read_text() != text:
        path.write_text(text)


def recorded_proof(capture_call=0, frames=0):
    output = ROOT / "build/recomp"
    output.mkdir(parents=True, exist_ok=True)
    registry = output / "template_memory_registry.c"
    write_if_changed(registry, '''/* Domain memory only; no CPU or timing claim. */
#include "recomp_ports.h"
#include "glue.h"
#include "template_placements.h"
#include "bus.h"
#include "machine.h"
#include <stdio.h>
#include <stdlib.h>
static int memory_only_call(void) {
    static unsigned calls;
    if (++calls == CAPTURE_CALL) {
        const char *path = getenv("FA18_TEMPLATE_ENTRY_DUMP");
        uint32_t registers[18];
        unsigned i;
        FILE *file = path ? fopen(path, "wb") : NULL;
        if (!file) { fputs("template entry capture: cannot open output\\n", stderr); abort(); }
        for (i = 0; i < 16; ++i) registers[i] = REG_DA[i];
        registers[16] = m68k_get_reg(NULL, M68K_REG_SR); registers[17] = REG_PC;
        if (fwrite(registers, sizeof registers, 1, file) != 1
            || fwrite(fa18_machine->chip, FA18_CHIP_SIZE, 1, file) != 1
            || fwrite(fa18_machine->slow, FA18_SLOW_SIZE, 1, file) != 1 || fclose(file)) {
            fputs("template entry capture: short write\\n", stderr); abort();
        }
    }
    refresh_template_placements(); return glue_return();
}
const FA18Port fa18_ports[] = {
    {0xC1D10C, memory_only_call, "refresh_template_placements_memory_only", 0},
    {0,0,0,0}
};
const int fa18_port_count = 1;
'''.replace("CAPTURE_CALL", str(capture_call)))
    # These are temporary domain-proof masks. Never alter the production
    # generated masks or claim that this validates any live CPU output.
    liveness = (ROOT / "port/recomp/generated/recomp_liveness.c").read_text()
    for address in ("C1C924", "C1C94A", "C1C982"):
        liveness, count = re.subn(
            r"\{0x" + address + r", 0x[0-9A-F]+, 0x[0-9A-F]+, 0x[0-9A-F]+\}",
            "{0x" + address + ", 0xC000, 0x00, 0x00}", liveness)
        if count != 1:
            raise RuntimeError(f"expected one original caller mask at {address}")
    masks = output / "template_memory_liveness.c"
    write_if_changed(masks, liveness)
    executable = output / "template_memory.exe"
    subprocess.run([
        "python", "scripts/build_recomp.py", "--output", str(executable.relative_to(ROOT)),
        "--replace-source", "port/game/glue/ports.c=" + str(registry.relative_to(ROOT)),
        "--replace-source", "port/recomp/generated/recomp_liveness.c=" + str(masks.relative_to(ROOT)),
    ], cwd=ROOT, check=True)

    def check(recording):
        results = {}
        for mode in ("shadow", "sandbox"):
            report = output / f"template_memory_{recording.name}_{mode}.json"
            environment = dict(os.environ)
            if capture_call:
                capture = output / f"template_{recording.name}_{mode}_call{capture_call}.bin"
                capture.unlink(missing_ok=True)
                environment["FA18_TEMPLATE_ENTRY_DUMP"] = str(capture)
            command = [str(executable), "--state", str(recording / "state.bin"),
                       "--input", str(recording / "input.fa18in"), "--rom", "local/system/kick13.rom",
                       "--ports", mode, "--ports-report", str(report)]
            command += ["--frames", str(frames)] if frames else ["--to-end"]
            run = subprocess.run(command, cwd=ROOT, env=environment,
                                 stdout=subprocess.DEVNULL, stderr=subprocess.PIPE, text=True)
            if run.returncode:
                raise RuntimeError(f"{recording.name} {mode}: {run.stderr.strip()}")
            rows = json.loads(report.read_text())
            if (len(rows) != 1 or rows[0]["entry"] != "C1D10C"
                    or rows[0]["mismatched"] or rows[0]["hardware"]):
                raise RuntimeError(f"{recording.name} {mode}: missing or mismatched domain comparison")
            if capture_call and capture.exists():
                print(f"captured entry: {capture.relative_to(ROOT)}", flush=True)
            results[mode] = rows[0]
            print(f"{recording.name} {mode}: {rows[0]['matched']} memory matches, "
                  f"{rows[0]['incomplete']} incomplete", flush=True)
        return results

    recordings = sorted(p.parent for p in (ROOT / "captures/native").glob("*/input.fa18in"))
    if not recordings:
        raise RuntimeError("no sealed recordings")
    with ThreadPoolExecutor(max_workers=3) as pool:
        results = list(pool.map(check, recordings))
    if not any(r[mode]["matched"] for r in results for mode in ("shadow", "sandbox")):
        raise RuntimeError("C1D10C: no completed recorded memory comparisons")
    print("C1D10C domain memory only; CPU/live timing NOT tested: " + ", ".join(
        f"{sum(r[mode]['matched'] for r in results)} {mode} matches"
        for mode in ("shadow", "sandbox")))
    return results


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--recorded", action="store_true")
    parser.add_argument("--capture-call", type=int, default=0)
    parser.add_argument("--frames", type=int, default=0, help="bounded recording probe; zero checks to end")
    parser.add_argument("--fixture", type=Path, help="compare one captured original entry")
    parser.add_argument("--cases", type=int, default=1, help="structural variations of the captured entry")
    args = parser.parse_args()
    if args.capture_call < 0 or (args.capture_call and not args.recorded):
        parser.error("--capture-call must be nonnegative and requires --recorded")
    if args.frames < 0 or args.cases <= 0:
        parser.error("--frames must be nonnegative and --cases positive")
    if not args.recorded and not args.fixture:
        parser.error("select --recorded or --fixture")
    if args.fixture:
        executable = build_oracle("template_placements_oracle",
                                  "tools/recomp/template_placements_oracle.c", default_bash())
        subprocess.run([str(executable), str(args.fixture.resolve()), str(args.cases)], cwd=ROOT, check=True)
    if args.recorded:
        recorded_proof(args.capture_call, args.frames)


if __name__ == "__main__":
    main()
