"""Prove profiling leaves real runner state/output unchanged in all CPU modes."""
import argparse
import json
from pathlib import Path
import subprocess
import tempfile
import sys
from array import array

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
from compare_recomp_frames import compare_frames, PIXELS


def check_fade_policy(out):
    palette = [set((0, 0x111, 0x222)) for _ in range(16)]
    palette[5] = {0, 0x336, 0x447}
    rgb = array("H", [0] * PIXELS)
    indices = bytearray(PIXELS)
    rgb[0], indices[0] = 0x447, 5
    rgb[1], indices[1] = 0, 7
    rgb[2], indices[2] = 0x447, 19
    paths = [out / name for name in ("off.rgb", "on.rgb", "off.idx", "on.idx")]
    paths[0].write_bytes(rgb.tobytes())
    paths[2].write_bytes(indices)
    def compare(value, index, pixel=0):
        actual_rgb, actual_indices = array("H", rgb), bytearray(indices)
        actual_rgb[pixel], actual_indices[pixel] = value, index
        paths[1].write_bytes(actual_rgb.tobytes())
        paths[3].write_bytes(actual_indices)
        return compare_frames(*paths, palette)
    fade = compare(0x336, 5)
    assert fade["passed"] and fade["ignored_fade_pixels"] == 1
    assert not compare(0xD92, 5)["passed"], "unrelated palette change hidden"
    assert not compare(0x447, 9)["passed"], "drawing change with equal RGB hidden"
    assert not compare(0, 8, 1)["passed"], "black fade hid changed drawing"
    assert not compare(0x336, 19, 2)["passed"], "upper palette incorrectly faded"
    assert compare(0x447, 5)["passed"]
    paths[1].write_bytes(rgb.tobytes() * 2)
    paths[3].write_bytes(indices * 2)
    lengths = compare_frames(*paths, palette)
    assert not lengths["passed"] and not lengths["frame_counts_equal"]
    print("fade policy: source-table fade excluded; index/black-frame/non-fade colours still checked")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--runner", type=Path, default=ROOT / "build/recomp/fa18_recomp.exe")
    args = parser.parse_args()
    base = [str(args.runner.resolve()), "--state", str(ROOT / "captures/native/demo01/state.bin"),
            "--rom", str(ROOT / "local/system/kick13.rom")]
    with tempfile.TemporaryDirectory(prefix="meter-check-", dir=ROOT / "build") as temporary:
        out = Path(temporary)
        check_fade_policy(out)
        for name, options in (("off", ["--ports", "off"]), ("on", ["--ports", "on"]),
                              ("interpreter", ["--ports", "off", "--no-recomp"])):
            frames = 800 if name == 'on' else 120
            if name == 'on':
                options += ["--input", str(ROOT / "captures/native/demo01/input.fa18in")]
            results = []
            for enabled in (False, True):
                command = base + options + ["--frames", str(frames), "--ram-out", str(out / f"{enabled}.ram"),
                                             "--rgb444", str(out / f"{enabled}.rgb")]
                if enabled:
                    command += ["--profile", str(out / "profile.json"), "--index8", str(out / "indices.bin")]
                results.append(subprocess.run(command, cwd=ROOT, capture_output=True, check=True))
            assert results[0].stdout == results[1].stdout, name
            assert results[0].stderr == results[1].stderr, name
            for suffix in ("ram", "rgb"):
                assert (out / f"False.{suffix}").read_bytes() == (out / f"True.{suffix}").read_bytes(), (name, suffix)
            meter = json.loads((out / "profile.json").read_text())["_emulation"]
            index_data = (out / "indices.bin").read_bytes()
            assert len(index_data) == frames * PIXELS and 0 < max(index_data) < 32
            assert meter["frames"] == frames, meter["frames"]
            assert meter["instructions"]["interpreted"] > 0, name
            assert meter["bus"]["interpreted"]["reads"] > 0, name
            assert meter["chipset"]["bitplane_words"] > 0, name
            assert meter["ram_pages"], name
            if name == "interpreter":
                assert meter["instructions"]["generated"] == meter["instructions"]["residual"] == 0
                assert meter["bus"]["generated"]["reads"] == meter["bus"]["residual"]["reads"] == 0
            else:
                assert meter["instructions"]["generated"] > 0, name
            if name == "on":
                profile = json.loads((out / "profile.json").read_text())
                parent_edge = next(edge for edge in profile['_native_edges']
                                   if edge['caller'] == 'C25B66' and edge['callee'] == 'C2D408')
                assert parent_edge['calls'] > 0 and not profile.get('C2D408', 0)
                for callee in ('C1B27E', 'C13D84', 'C2C392', 'C28E28'):
                    edge = next(edge for edge in profile['_native_edges']
                                if edge['caller'] == 'C25B66' and edge['callee'] == callee)
                    assert edge['calls'] > 0 and not profile.get(callee, 0)
                for callee in ('C1342C', 'C2DD4E'):
                    edge = next(edge for edge in profile['_native_edges']
                                if edge['caller'] == 'C2D408' and edge['callee'] == callee)
                    assert edge['calls'] > 0 and not profile.get(callee, 0)
                assert meter["ports"]["calls"] + meter["ports"]["steps"] > 0
                assert meter["bus"]["port"]["reads"] > 0
            if name == "off":
                assert not json.loads((out / "profile.json").read_text())['_native_edges']
                assert meter["ports"]["calls"] == meter["ports"]["steps"] == 0
            print(f"{name}: profiling preserves stdout, stderr, all RGB frames and final RAM; origins exercised")


if __name__ == "__main__":
    main()
