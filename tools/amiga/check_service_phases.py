"""Differential proof of C service phases against the pinned Kickstart oracle.

The SDK is not consumed. Candidate runs have no ROM bytes and terminate on
every real ROM read/fetch. Cold phases, every CCR, nesting/queue boundaries,
and DMA contention are checked independently from whole-recording parity.
"""
import argparse
from pathlib import Path
import re
import subprocess

ROOT = Path(__file__).resolve().parents[2]
FAMILIES = [
    ("exec_glue.c", "fa18_os_exec_interrupt_step"),
    ("exec_glue.c", "fa18_os_exec_get_msg_step"),
    ("graphics_glue.c", "fa18_os_vbeam_step"),
    ("graphics_glue.c", "fa18_os_wait_blit_step"),
    ("graphics_wait_bovp.c", "fa18_os_wait_bovp_step"),
    ("graphics_blitter_ownership.c", "fa18_os_blitter_ownership_step"),
    ("potgo_glue.c", "fa18_os_potgo_step"),
    ("exec_task_lookup.c", "fa18_os_exec_find_task_step"),
    ("exec_task_lookup.c", "fa18_os_exec_find_name_step"),
    ("exec_lists_adapter.c", "fa18_os_exec_lists_step"),
    ("exec_task_services_adapter.c", "fa18_os_exec_messages_step"),
    ("exec_task_services_adapter.c", "fa18_os_exec_signals_step"),
    ("exec_task_services_adapter.c", "fa18_os_exec_task_protection_step"),
    ("exec_supervisor.c", "fa18_os_exec_supervisor_step"),
    ("exec_memory_adapter.c", "fa18_os_exec_memory_step"),
    ("exec_scheduler_adapter.c", "fa18_os_exec_scheduler_step"),
    ("exec_interrupt_adapter.c", "fa18_os_exec_irq_roots_step"),
    ("exec_interrupt_adapter.c", "fa18_os_exec_int_servers_step"),
    ("exec_interrupt_adapter.c", "fa18_os_exec_soft_interrupts_step"),
]


def write_cases(families=None):
    """Build the shared case registry even when a whole-call checker runs first."""
    rows = []
    for filename, function in FAMILIES if families is None else families:
        source = (ROOT / "port/os" / filename).read_text()
        body = source.split(f"int {function}(void) {{", 1)[1].split("\nint ", 1)[0]
        pcs = sorted(set(re.findall(r"case (0x[0-9A-F]+)u:", body)))
        rows.extend(f'{{{pc}u,{function},"{function}"}},' for pc in pcs)
    out = ROOT / "build/amiga"
    out.mkdir(parents=True, exist_ok=True)
    (out / "service_phase_cases.h").write_text(
        "static const struct { uint32_t pc; int (*step)(void); const char *name; } "
        "service_phase_cases[]={\n" + "\n".join(rows) + "\n};\n")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases", type=int, default=256)
    parser.add_argument("--family", action="append", choices=[function for _,function in FAMILIES],
                        help="check selected families while developing; omit for the complete gate")
    args = parser.parse_args()
    if args.cases < 256:
        parser.error("use at least 256 to exercise all boundary/CCR combinations")
    write_cases([row for row in FAMILIES if row[1] in args.family] if args.family else None)
    subprocess.run([
        "python", "scripts/build_recomp.py", "--main", "tools/amiga/service_phase_oracle.c",
        "--replace-source", "port/machine/machine.c=tools/amiga/service_phase_machine.c",
        "--output", "build/recomp/service_phase_oracle.exe",
    ], cwd=ROOT, check=True)
    subprocess.run([str(ROOT / "build/recomp/service_phase_oracle.exe"), str(args.cases)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
