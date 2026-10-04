"""Capture original service entry/return state with unmodified Engine9000.

Snapshots and instruction traces are oracle evidence only. Hardware observations
are flushed at the next normal frame commit and retain the larger capture window;
they must be bounded to the service interval before use as ordered fixtures.
"""
import argparse
import ctypes as C
import hashlib
import json
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
from engine9000_bridge import Engine, ENGINE, U, P


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def snapshot(engine, output, label):
    files = {}
    for name, address in (("chip", 0), ("slow", 0xC00000)):
        path = output / f"{label}_{name}.bin"
        path.write_bytes(engine.memory(address, 0x80000))
        files[path.name] = sha(path)
    state = output / f"{label}_state.bin"
    state.write_bytes(engine.state())
    files[state.name] = sha(state)
    return {"registers": engine.regs(), "cycle": engine.core.e9k_debug_read_cycle_count(),
            "beam": engine.memory(0xDFF004, 4).hex(),
            "hardware_frame": engine.hardware_frame, "files": files}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--config", type=Path, required=True)
    parser.add_argument("--state", type=Path, required=True)
    parser.add_argument("--entry", type=lambda s: int(s, 0), required=True)
    parser.add_argument("--return-pc", type=lambda s: int(s, 0),
                        help="filter calls from other OS tasks sharing this service")
    parser.add_argument("--instructions", type=int, default=100000)
    parser.add_argument("--frames", type=int, default=100)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    output = args.output.resolve()
    if not any(output.is_relative_to(ROOT / name) for name in ("build", "local")):
        parser.error("binary oracle evidence must remain in build/ or local/")
    if not 0 <= args.entry <= 0xFFFFFF or args.instructions < 1 or args.frames < 1:
        parser.error("invalid entry or execution bound")
    output.mkdir(parents=True, exist_ok=True)
    engine = Engine(args.config.resolve(), output)
    try:
        engine.core.retro_run()
        # PUAE's synchronous restore runs machine time before returning.
        # Install the breakpoint first so short startup calls are not skipped.
        engine.core.e9k_debug_add_breakpoint(args.entry)
        raw = args.state.read_bytes()
        buf = C.create_string_buffer(raw)
        if not engine.core.retro_unserialize(buf, len(raw)):
            raise RuntimeError("Engine9000 rejected oracle start state")
        engine.bind("e9k_debug_set_debug_option", None, U, U, P)(38, 1, None)
        with (output / "hardware_window.jsonl").open("w", encoding="utf8") as hardware:
            engine.custom_log = hardware
            if not engine.core.e9k_debug_is_paused():
                engine.core.e9k_debug_resume()
            reached = False
            skipped = 0
            for _ in range(args.frames):
                if engine.core.e9k_debug_is_paused():
                    regs = engine.regs()
                    if regs["pc"] != args.entry:
                        raise RuntimeError("unexpected breakpoint")
                    candidate_ret = int.from_bytes(engine.memory(regs["a7"], 4), "big")
                    if args.return_pc is None or candidate_ret == args.return_pc:
                        reached = True
                        break
                    skipped += 1
                    engine.core.e9k_debug_step_instr()
                    engine.core.retro_run()
                    engine.core.e9k_debug_resume()
                engine.core.retro_run()
            if not reached:
                raise RuntimeError("service entry not reached within frame bound")
            engine.core.e9k_debug_remove_breakpoint(args.entry)
            before = snapshot(engine, output, "entry")
            sp = before["registers"]["a7"]
            ret = int.from_bytes(engine.memory(sp, 4), "big")
            trace = output / "instructions.jsonl"
            with trace.open("w", encoding="utf8") as out:
                for count in range(args.instructions):
                    regs = engine.regs()
                    if regs["pc"] == ret and regs["a7"] == sp + 4:
                        break
                    row = {"before": regs, "cycle_before": engine.core.e9k_debug_read_cycle_count(),
                           "instruction_window": engine.memory(regs["pc"], 10).hex()}
                    engine.core.e9k_debug_step_instr()
                    engine.core.retro_run()
                    row["after"] = engine.regs()
                    row["cycle_after"] = engine.core.e9k_debug_read_cycle_count()
                    out.write(json.dumps(row, separators=(",", ":")) + "\n")
                else:
                    raise RuntimeError("service did not return within instruction bound")
            after = snapshot(engine, output, "return")
            # Capture pending writes through an ordinary frame boundary. This
            # advances only the oracle after its return snapshot was sealed.
            engine.core.e9k_debug_resume()
            engine.core.retro_run()
            engine.custom_log = None
        hardware_rows = [json.loads(s) for s in (output / "hardware_window.jsonl").read_text().splitlines()]
        if any("error" in row for row in hardware_rows):
            raise RuntimeError("Engine9000 dropped hardware writes")
        manifest = {
            "schema": "amiga.service_contract.v1", "entry_pc": args.entry, "return_pc": ret,
            "other_callers_skipped": skipped,
            "entry": before, "return": after, "instructions": count,
            "clock_unit": "UAE get_cycles()/CYCLE_UNIT; OCS colour clocks in this A500 profile",
            "hardware_window_requires_interval_filter": True,
            "authority": {"state_sha256": sha(args.state), "config_sha256": sha(args.config),
                          "engine_sha256": sha(ENGINE / "system/ami9000.dll")},
            "trace_sha256": sha(trace), "hardware_sha256": sha(output / "hardware_window.jsonl"),
            "captured_ram_is_runtime_input": False,
        }
        (output / "contract.json").write_text(json.dumps(manifest, indent=2) + "\n")
        print(json.dumps({"entry": hex(args.entry), "return": hex(ret), "instructions": count,
                          "colour_clocks": after["cycle"] - before["cycle"]}), flush=True)
    finally:
        engine.custom_log = None
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
