"""Check that shadow busy-input replay still rejects wrong C outputs/reads.

Builds two intentionally incorrect copies of the plane bridge under build/,
leaving all game sources and the normal runner unchanged. The plane entry
must be registered with shadow_busy_reads enabled. Run from any directory.
"""
import argparse
import subprocess
from check_record_region_probe import ROOT, default_bash


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--bash", default=default_bash())
    args = parser.parse_args()
    source = (ROOT / "port/game/glue/glue_active_planes_step.c").read_text()
    variants = {
        "wrong_counter": (
            "step_write_long(m68ki_read_imm_32(), D(0)); flags_logic_l(D(0)); break;",
            "step_write_long(m68ki_read_imm_32(), pc == 0xC2FE28 ? D(0) ^ 1u : D(0)); flags_logic_l(D(0)); break;",
            b"mismatch: byte C4591F", 3),
        "extra_read": (
            "case 0xC2FD8C: A(2) =",
            "case 0xC2FD8C: (void)m68k_read_memory_8(0xDFF002u); A(2) =",
            b"unexpected shadow DMACONR read", None),
    }
    for name, (old, new, diagnostic, expected_code) in variants.items():
        if source.count(old) != 1:
            raise SystemExit(f"{name}: expected one mutation site")
        stem = f"build/recomp/busy_input_{name}"
        (ROOT / f"{stem}.c").write_text(source.replace(old, new))
        subprocess.run([
            "python", "scripts/build_recomp.py", "--output", f"{stem}.exe",
            "--replace-source", f"port/game/glue/glue_active_planes_step.c={stem}.c",
        ], cwd=ROOT, check=True)
        result = subprocess.run([
            str(ROOT / f"{stem}.exe"), "--state", "captures/native/demo01/state.bin",
            "--input", "captures/native/demo01/input.fa18in", "--rom", "local/system/kick13.rom",
            "--frames", "600", "--ports", "shadow", "--ports-only", "C2FD8C",
        ], cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        (ROOT / f"{stem}.log").write_bytes(result.stdout)
        if (result.returncode == 0 or diagnostic not in result.stdout or
                (expected_code is not None and result.returncode != expected_code)):
            raise SystemExit(f"{name}: proof did not reject mutation as expected; see {stem}.log")
        print(f"{name}: rejected (exit {result.returncode})", flush=True)


if __name__ == "__main__":
    main()
