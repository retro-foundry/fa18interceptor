"""Exercise native startup -> Free Flight scene selection, not active flight.

Expected pose/camera values and gate digests come from the original ADF
runner with ports OFF at frame 3035 (Space 1800, digit 2 at 3000). Native
timing differs. No captures are loaded by the native executable; Copper fade
and unconnected record-update state are outside this checkpoint comparison.
"""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--runner", type=Path, default=ROOT / "build/native/fa18_native.exe")
    args = parser.parse_args()
    adf = ROOT / "local/media/fa18.adf"
    seal = hashlib.sha256(adf.read_bytes()).digest()
    with tempfile.TemporaryDirectory(prefix="native-flight-start-", dir=ROOT / "build") as directory:
        work = Path(directory)
        replay = work / "flight.e9k"
        replay.write_text("E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n"
                          "F 3000 K 50 0 0 1\nF 3002 K 50 0 0 0\n")
        output = work / "data.bin"
        def run(frames):
            result = subprocess.run([str(args.runner.resolve()), "--adf", str(adf),
                "--save-dir", str(work), "--headless", "--frames", str(frames),
                "--replay", str(replay), "--data-out", str(output)],
                capture_output=True, text=True, timeout=15)
            assert result.returncode == 0, result.stderr
            stats = json.loads(result.stdout)
            assert stats["mode"] == 1 and not stats["cpu_emulation"] and not stats["chipset_emulation"], stats
            data = output.read_bytes()
            assert len(data) == 0x100000
            return stats, data
        before, _ = run(3140)
        assert before["screen"] == "mode-intro" and before["stage"] == "C0FECE" and not before["scene_selected"], before
        after, data = run(4000)
        assert after["screen"] == "scene-setup" and after["stage"] == "C1072E" and after["scene_selected"], after
        def field(address, length):
            start = address - 0xC00000 + 0x80000
            return data[start:start + length]
        assert field(0xC46198, 12).hex() == "105459200000070810a404f0", "player placement"
        assert field(0xC461EA, 6).hex() == "000008c00000", "player angles"
        assert field(0xC45C3E, 12).hex() == "108000000300000010c00000", "camera preset"
        assert field(0xC45848, 2) == b"\x03\x11", "scene pose/input"
        assert field(0xC458A0, 1) == b"\x0f", "viewport mode"
        assert field(0xC458A1, 1) == b"\x0f", "viewport target"
        assert field(0xC4574A, 2) == b"\x00\x00", "mode message consumed"
        assert int.from_bytes(field(0xC4E988, 2), "big") > 0, "placement cache empty"
        for address, expected in (
            (0xC1929C, "420c6cfaac286552cdcc25972e16e78c69c1812aef71858ec83d877984025f77"),
            (0xC19A9C, "f64c0066a9be16d993eb039dd91c27a4d0842bdb2fb8d044b69c881abedf44c8"),
            (0xC1A29C, "f6f0f7b2d7f75d84945f8ef0909c2fdfa214e1f06079bc5ac97a48a8c0a1c038"),
        ):
            assert hashlib.sha256(field(address, 2048)).hexdigest() == expected, hex(address)
    assert hashlib.sha256(adf.read_bytes()).digest() == seal
    print("Native Free Flight: countdown -> pose/camera -> viewport -> mode messages executes")
    print("Source pose/camera checkpoint matches; active flight and full-state parity remain open")


if __name__ == "__main__":
    main()
