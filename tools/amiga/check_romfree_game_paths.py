"""Exercise original menus and flight-log persistence from isolated ADF launches.

These include qualification failure and active free flight; carrier success,
complete mission outcomes and exact timing remain unproved. No guest state is
injected; only frontend keys drive the original game.
"""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import struct
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]
ZERO_GUARD = dict(rom_reads=0, rom_instruction_fetches=0, unsupported_services=0)


def offset(address):
    assert 0xC00000 <= address < 0xC80000
    return address - 0xC00000 + 0x80000


def record(ram):
    address = struct.unpack_from(">I", ram, offset(0xC1AB74))[0]
    return ram[offset(address):offset(address) + 78]


def keys(*events):
    result = []
    for frame, key in events:
        result.extend([(frame, key, 1), (frame + 2, key, 0)])
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--runner", type=Path, default=ROOT / "build/recomp/fa18_romfree.exe")
    parser.add_argument("--modes", action="store_true", help="also exercise menu and mission selections")
    parser.add_argument("--outcomes", action="store_true", help="also replay qualification failure and persist its log")
    parser.add_argument("--flight", action="store_true", help="also verify free-flight startup and changing flight state/pixels")
    args = parser.parse_args()
    runner = args.runner.resolve()
    adf = ROOT / "local/media/fa18.adf"
    original_hash = hashlib.sha256(adf.read_bytes()).digest()
    with tempfile.TemporaryDirectory(prefix="romfree-game-") as directory:
        work = Path(directory)
        shutil.copy2(runner, work / "game.exe")
        shutil.copy2(adf, work / "original.adf")

        def run(name, frames, events, saves, minimum_pixels=1001):
            replay = work / f"{name}.e9k"
            replay.write_text("E9K_INPUT_V1\n" + "".join(
                f"F {frame} K {key} 0 0 {down}\n" for frame, key, down in events))
            output = work / f"{name}.ram"
            result = subprocess.run([
                str(work / "game.exe"), "--adf", "original.adf",
                "--save-dir", saves, "--frames", str(frames), "--replay", str(replay),
                "--ram-out", str(output), "--ppm", f"{name}.ppm", "--profile", f"{name}.calls.json"], cwd=work, capture_output=True, text=True)
            assert result.returncode == 0, (name, result.stderr, result.stdout)
            stats = [json.loads(line) for line in result.stdout.splitlines()]
            assert stats[-1] == ZERO_GUARD, (name, stats)
            assert stats[0]["iterations"] > 0 and stats[0]["nonblack_pixels"] >= minimum_pixels, (name, stats)
            ram = output.read_bytes()
            assert len(ram) >= 0x100000
            print(f"{name}: {frames} frames, mode={ram[offset(0xC458A6)]}, zero ROM/fault counters", flush=True)
            return ram, stats[0]

        # The flight-log screen labels 1 as update, SHIFT-2 as reset. The
        # original C16406 clears all 39 words; C1643A writes those 78 bytes
        # through an existing-file (1005) handle, leaving the ADF intact.
        events = keys((1800, 32), (2200, 56))
        events += [(2600, 304, 1), (2602, 50, 1), (2604, 50, 0), (2606, 304, 0)]
        events += keys((3000, 49))
        run("reset-save", 3700, events, "saves")
        saved = (work / "saves/config").read_bytes()
        assert saved == bytes(78), (len(saved), saved.hex())
        # Acknowledge the credits so the game actually reads the config, then
        # compare at its loaded-record callback before name/tour entry. Later
        # original C11720 increments the tour count at record+4 and accepts
        # a callsign at +30, so a later whole-record comparison is invalid.
        loaded, _ = run("reload", 1810, keys((1800, 32)), "saves", minimum_pixels=0)
        assert struct.unpack_from(">I", loaded, offset(0xC1820C))[0] == 0xC115BA
        assert record(loaded) == saved
        assert hashlib.sha256((work / "original.adf").read_bytes()).digest() == original_hash
        print("Original 78-byte reset/save/reload matches all bytes; ADF unchanged", flush=True)

        if args.flight:
            source = (ROOT / "tools/amiga/fixtures/freeflight_runway.e9k").read_text().splitlines()
            assert source[0] == "E9K_INPUT_V1"
            events = [(int(row[1]), int(row[3]), int(row[6])) for line in source[1:] if (row := line.split())]
            # Original UI: acknowledge credits, select free flight, confirm,
            # choose runway location and aircraft, then increase throttle.
            # C10DAE is the active post-input callback; C457B4/C457AD are
            # CONTEXT_STARTED/PAUSE_A. The player's record is C46184; the
            # original flight controls integrate its long coordinates +12,
            # +16, +20 (and ground height +24). Do not use mode/blit counts
            # alone as flight evidence: these also occur on menu maps.
            checkpoints = []
            for frame in (4900, 5300):
                name = f"free-flight-{frame}"
                ram, _ = run(name, frame, events, name)
                assert ram[offset(0xC458A6)] == 1
                assert ram[offset(0xC457B4)] == 1 and ram[offset(0xC457AD)] == 0
                assert struct.unpack_from(">I", ram, offset(0xC1820C))[0] == 0xC10DAE
                position = ram[offset(0xC46184 + 12):offset(0xC46184 + 24)]
                pixels = (work / f"{name}.ppm").read_bytes()
                checkpoints.append((position, pixels))
            assert checkpoints[0][0] != checkpoints[1][0], "player coordinates did not advance"
            assert checkpoints[0][1] != checkpoints[1][1], "flight image did not change"
            print("Free-flight runway/aircraft selections reach active cockpit; original player coordinates and pixels advance", flush=True)

        if args.outcomes:
            # Start a new tour using the reset record produced by the actual
            # game above. Adapt the sealed failure recording's frontend-frame
            # positions to this cold menu checkpoint, preserving its key order.
            (work / "qualification").mkdir()
            (work / "qualification/config").write_bytes(saved)
            source = (ROOT / "tools/amiga/fixtures/qualification_failure.e9k").read_text().splitlines()
            assert source[0] == "E9K_INPUT_V1"
            events = []
            for line in source[1:]:
                fields = line.split()
                if fields:
                    assert len(fields) == 7 and fields[0] == "F" and fields[2] == "K", line
                    events.append((int(fields[1]), int(fields[3]), int(fields[6])))
            # Continue in this same process into flight-log update. Replaying
            # the prefix in a second process can pick up the callsign save
            # already written by the original first-tour flow.
            failed, _ = run("qualification-failure", 6400, events, "qualification")
            calls = json.loads((work / "qualification-failure.calls.json").read_text())
            # These are real original restart/failure callbacks, not inferred
            # from reaching a menu or from a nonzero generic outcome flag.
            assert calls.get("C11788") == 3 and calls.get("C11830") == 2, calls
            assert calls.get("C118A0", 0) > 0 and calls.get("C118E6", 0) > 0, calls
            assert failed[offset(0xC458A6)] == 0
            assert struct.unpack_from(">I", failed, offset(0xC1820C))[0] == 0xC0FCB4
            log = record(failed)
            assert log[:2] == bytes(2) and log[30:36] == b"PILOT\0"
            assert struct.unpack_from(">H", log, 16)[0] == 3
            assert struct.unpack_from(">H", log, 70)[0] == 3
            saved_log = (work / "qualification/config").read_bytes()
            assert len(saved_log) == 78 and saved_log[:2] == bytes(2)
            assert saved_log[30:36] == b"PILOT\0"
            assert struct.unpack_from(">H", saved_log, 16)[0] == 3
            assert struct.unpack_from(">H", saved_log, 70)[0] == 3
            loaded, _ = run("failure-log-reload", 1810, keys((1800, 32)), "qualification", minimum_pixels=0)
            assert struct.unpack_from(">I", loaded, offset(0xC1820C))[0] == 0xC115BA
            assert record(loaded) == saved_log
            print("Qualification: three original reset passes, failure message path, main-menu return; full nonzero log save/reload matches", flush=True)

        if args.modes:
            for key, expected in ((2, 1), (3, 2), (4, 0x7D), (5, 9), (6, 0), (7, 6), (8, 0)):
                ram, stats = run(f"menu-{key}", 3600, keys((1800, 32), (2200, 48 + key)), f"mode-{key}")
                assert ram[offset(0xC458A6)] == expected, (key, expected)
                if expected:
                    assert stats["blits"] > 1000
            for index in range(1, 5):
                ram, stats = run(f"mission-{index}", 4200,
                    keys((1800, 32), (2200, 54), (2700, 281 + index)), f"mission-{index}")
                assert ram[offset(0xC458A6)] == index + 2 and stats["blits"] > 1000
            events = keys((1800, 32), (2200, 50))
            events += [(3000, 304, 1), (3001, 27, 1), (3003, 27, 0), (3004, 304, 0)]
            ram, _ = run("flight-return", 3800, events, "flight-return")
            assert ram[offset(0xC458A6)] == 0
            assert struct.unpack_from(">I", ram, offset(0xC1820C))[0] == 0xC0FCB4
    assert hashlib.sha256(adf.read_bytes()).digest() == original_hash


if __name__ == "__main__":
    main()
