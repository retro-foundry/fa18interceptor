"""Exercise connected native menu selection, mission gates and pilot-log controls.

Settled RGB digests: source OFF ADF, Space 1800, digit 3000; mode banners
frame 3010, missions/log frame 3300. No full gameplay replay or timing claim.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]
PIXELS = {
    1: "c1e610ccd1baff42a58e619c77c3951900d7d530b8c10df88ab4368b3f4fce96",
    2: "4c4f5ad98c0f0df18098653bd10a43a7eb1952b21806291dce060a14f933d099",
    3: "a42c2ea9da088714344e52d955ea8303e48fd7431f92e77ca1b1b629dc02eddc",
    4: "10dc403a03b0f4db379a1b56d061b96e24e33698f264431c24299114325a1671",
    5: "940527968c30173f83a6eeb9305110539c00004076baa02f76e66c84308288d0",
    6: "17257c880c09f3b0b91d89853f3831857ae4fe354d1110dab3f867c2fc5c66c1",
    8: "fd8106dcfd44193f9091039faa0051417c46eecf13749101831bde510bf0a28f",
}
MENU = "6d32c81d93af3cbdaf91a36dac49b31bbaacc5e0d86ecc19784891b30c9497c7"

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--runner", type=Path, default=ROOT / "build/native/fa18_native.exe")
    args = parser.parse_args()
    runner = args.runner.resolve()
    adf = ROOT / "local/media/fa18.adf"
    seal = hashlib.sha256(adf.read_bytes()).digest()
    with tempfile.TemporaryDirectory(prefix="native-menu-", dir=ROOT / "build") as directory:
        work = Path(directory)
        def run(name, frames, events, config=None):
            save = work / name
            save.mkdir()
            if config is not None:
                (save / "config").write_bytes(config)
            replay = work / f"{name}.e9k"
            replay.write_text("E9K_INPUT_V1\n" + "".join(
                f"F {frame} K {key} 0 0 {down}\n" for frame, key, down in
                [(1800, 32, 1), (1802, 32, 0)] + events))
            ppm = work / f"{name}.ppm"
            result = subprocess.run([str(runner), "--adf", str(adf), "--save-dir", str(save),
                "--headless", "--frames", str(frames), "--replay", str(replay), "--ppm", str(ppm)],
                cwd=work, capture_output=True, text=True, timeout=30)
            assert result.returncode == 0, (name, result.stderr)
            stats = json.loads(result.stdout)
            assert not stats["cpu_emulation"] and not stats["chipset_emulation"]
            data = ppm.read_bytes()
            header = b"P6\n320 256\n255\n"
            assert data.startswith(header) and len(data) == len(header) + 320 * 256 * 3
            return stats, hashlib.sha256(data[len(header):]).hexdigest(), save
        def key(frame, value):
            return [(frame, value, 1), (frame + 2, value, 0)]
        modes = (127, 1, 2, 125, 9)
        for digit, mode in enumerate(modes, 1):
            stats, pixels, _ = run(f"mode-{digit}", 3140, key(3000, 48 + digit))
            assert stats["screen"] == "mode-intro" and stats["mode"] == mode, stats
            assert pixels == PIXELS[digit], (digit, pixels)
        print("Modes 1-5: source selections and all five transition banners match", flush=True)
        stats, _, _ = run("next-mission", 4000, key(3000, 55))
        assert stats["screen"] == "mode-intro" and stats["mode"] == 6, stats
        print("Next mission: source pilot-log field selects mode 6", flush=True)
        entry = key(2400, 112) + key(2440, 105) + key(2480, 108)
        entry += key(2520, 111) + key(2560, 116) + key(2680, 13) + key(3000, 50)
        stats, pixels, _ = run("first-tour-selection", 3140, entry, bytes(78))
        assert stats["screen"] == "mode-intro" and stats["mode"] == 1 and pixels == PIXELS[2], stats
        print("New pilot: Return release restores the source command gate; first selection works", flush=True)
        for digit, screen in ((6, "missions"), (8, "pilot-log")):
            stats, pixels, _ = run(f"menu-{digit}", 6000, key(3000, 48 + digit))
            assert stats["screen"] == screen and pixels == PIXELS[digit], (digit, stats, pixels)
            stats, pixels, _ = run(f"return-{digit}", 6500,
                key(3000, 48 + digit) + key(5000, 27))
            assert stats["screen"] == "menu" and pixels == MENU, (digit, stats, pixels)
        print("Missions/log: source pixels and Escape -> main menu match", flush=True)
        for number in range(4):
            stats, _, _ = run(f"mission-f{number + 1}", 4900,
                key(3000, 54) + key(4500, 282 + number))
            assert stats["screen"] == "mode-intro" and stats["mode"] == number + 3, stats
        # Actual saved-log bytes drive the source eligibility gate. A pilot
        # with no qualification word cannot select F1; no invented availability.
        unqualified = bytearray(78)
        unqualified[5] = 1
        unqualified[30:36] = b"PILOT\0"
        stats, _, _ = run("unqualified-f1", 4900,
            key(3000, 54) + key(4500, 282), unqualified)
        assert stats["screen"] == "missions" and stats["mode"] == 0, stats
        print("F1-F4 accept source-available missions; unqualified F1 remains gated", flush=True)
        reset = key(3000, 56) + [(5000, 304, 1), (5002, 50, 1),
            (5004, 50, 0), (5006, 304, 0)] + key(6000, 49)
        stats, _, save = run("reset-save", 6100, reset)
        saved = (save / "config").read_bytes()
        assert saved == bytes(78) and stats["screen"] == "enlistment", (stats, saved.hex())
        stats, _, _ = run("reset-reload", 2400, [], saved)
        assert stats["screen"] == "callsign", stats
        print("SHIFT-2 resets 39 words; 1 saves 78 exact zero bytes; reload requests a new callsign", flush=True)
        assert hashlib.sha256(adf.read_bytes()).digest() == seal
    print("Native menu checks pass; original ADF unchanged", flush=True)

if __name__ == "__main__":
    main()
