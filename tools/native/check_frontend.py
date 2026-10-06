"""Exercise native intro/menu, editing/saving, presentation and omission build.

Settled pixel digests come from original-source OFF ADF runs, 2026-10-06:
splash frame 1000; credits 1800; menu 3000 after Space at 1800.
These are static-screen checks, not a claim about frame timing/fade parity.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]
SCREENS = {
    "splash": "b7fdc5a1957229042f733eb1030332145cb7ef7cfa5d7eed91af4a2e506d1e2e",
    "credits": "b6395078ddc314dc782613d7845bf3bd366b7aea27c292814b0073097d9e7f11",
    "menu": "6d32c81d93af3cbdaf91a36dac49b31bbaacc5e0d86ecc19784891b30c9497c7",
}

def digest(data):
    return hashlib.sha256(data).hexdigest()

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--runner", type=Path, default=ROOT / "build/native/fa18_native.exe")
    parser.add_argument("--map", type=Path, default=ROOT / "build/native-cmake/native/fa18_native.map")
    args = parser.parse_args()
    runner = args.runner.resolve()
    link_map = args.map.read_text(errors="replace")
    assert not re.search(r"m68ki?_|fa18_(?:bus_|machine_|recomp_)|glue_|custom_write|wait_blitter", link_map), "emulation symbol in native link"
    for symbol in ("native_frontend_tick", "advance_main_loop_message_sequence", "plot_glyph8", "queue_top_level_menu_messages"):
        assert symbol in link_map, symbol
    adf = ROOT / "local/media/fa18.adf"
    original_digest = digest(adf.read_bytes())
    with tempfile.TemporaryDirectory(prefix="native-frontend-", dir=ROOT / "build") as temporary:
        work = Path(temporary)
        existing = work / "existing"
        existing.mkdir()
        config = bytearray(78)
        config[5] = 1
        config[30:36] = b"PILOT\0"
        (existing / "config").write_bytes(config)
        def replay(name, events):
            path = work / f"{name}.e9k"
            path.write_text("E9K_INPUT_V1\n" + "".join(
                f"F {frame} K {key} 0 0 1\nF {frame+1} K {key} 0 0 0\n" for frame, key in events))
            return path
        acknowledge = replay("acknowledge", [(1800, 32)])
        def run(name, frames, save=existing, events=None, window=False):
            ppm = work / f"{name}.ppm"
            command = [str(runner), "--adf", str(adf), "--save-dir", str(save),
                       "--frames", str(frames), "--ppm", str(ppm)]
            if not window:
                command.append("--headless")
            if events:
                command += ["--replay", str(events)]
            env = os.environ.copy()
            env["SDL_VIDEODRIVER"] = "dummy"
            result = subprocess.run(command, cwd=work, env=env, capture_output=True, text=True, timeout=30)
            assert result.returncode == 0, result.stderr
            stats = json.loads(result.stdout)
            assert stats["frames"] == frames and not stats["cpu_emulation"] and not stats["chipset_emulation"]
            data = ppm.read_bytes()
            header = b"P6\n320 256\n255\n"
            assert data.startswith(header) and len(data) == len(header) + 320*256*3
            return stats, digest(data[len(header):])
        for screen, frames, events in (("splash", 1, None), ("credits", 1800, None), ("menu", 4000, acknowledge)):
            stats, pixels = run(screen, frames, events=events)
            assert stats["screen"] == screen and pixels == SCREENS[screen], (screen, stats, pixels)
            print(f"{screen}: original-source settled pixels match")
        first = work / "first-tour"
        first.mkdir()
        (first / "config").write_bytes(bytes(78))
        editing = replay("editing", [(1800, 32), (2400, 112), (2440, 105), (2480, 108),
                                      (2520, 111), (2560, 120), (2600, 8), (2640, 116), (2680, 13)])
        stats, pixels = run("first-tour", 4600, save=first, events=editing)
        saved = (first / "config").read_bytes()
        assert len(saved) == 78 and saved[4:6] == b"\0\1" and saved[30:36] == b"PILOT\0", saved.hex()
        assert stats["screen"] == "menu" and pixels == SCREENS["menu"]
        stats, pixels = run("reload", 4000, save=first, events=acknowledge)
        assert stats["screen"] == "menu" and pixels == SCREENS["menu"]
        assert (first / "config").read_bytes() == saved
        print("first tour: callsign/backspace/Return, exact 78-byte save and reload pass")
        stats, pixels = run("window", 2, window=True)
        assert pixels == SCREENS["splash"]
        print("SDL window presentation executes and exits at the frame limit")
        (first / "config").write_bytes(b"invalid")
        result = subprocess.run([str(runner), "--adf", str(adf), "--save-dir", str(first), "--headless", "--frames", "1"], capture_output=True, text=True)
        assert result.returncode != 0 and "invalid saved" in result.stderr
        result = subprocess.run([str(runner), "--adf", str(work / "missing.adf"), "--save-dir", str(existing), "--headless", "--frames", "1"], capture_output=True, text=True)
        assert result.returncode != 0 and "cannot open" in result.stderr
        assert digest(adf.read_bytes()) == original_digest
    print("Native link excludes CPU, translations, bus and chipset; ADF unchanged; malformed input fails")

if __name__ == "__main__":
    main()
