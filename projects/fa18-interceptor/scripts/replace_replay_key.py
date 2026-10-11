"""Copy an E9K replay while replacing one keyboard key's events.

The tool is intended for controlled, single-input differential probes.  It
preserves every nonmatching row byte-for-byte after normal line splitting.
"""

import argparse
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", type=Path)
    parser.add_argument("--key", type=int, required=True,
                        help="existing libretro key code")
    parser.add_argument("--replacement-key", type=int, required=True)
    parser.add_argument("--replacement-character", type=int,
                        help="replacement character code; defaults to the replacement key")
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()

    lines = args.input.read_text(encoding="ascii").splitlines()
    if not lines or lines[0] != "E9K_INPUT_V1":
        raise ValueError(f"{args.input} is not an E9K_INPUT_V1 playback")
    result = [lines[0]]
    changed = 0
    for line in lines[1:]:
        fields = line.split()
        if len(fields) < 3 or fields[0] != "F":
            raise ValueError(f"unrecognized playback row: {line!r}")
        if fields[2] == "K":
            if len(fields) != 7:
                raise ValueError(f"unrecognized keyboard row: {line!r}")
            if int(fields[3]) != args.key:
                result.append(line)
                continue
            fields[3] = str(args.replacement_key)
            if int(fields[4]) == args.key:
                fields[4] = str(args.replacement_character
                                if args.replacement_character is not None
                                else args.replacement_key)
            changed += 1
        result.append(" ".join(fields))
    if changed == 0:
        raise ValueError(f"no keyboard rows use key {args.key}")
    args.output.write_text("\n".join(result) + "\n", encoding="ascii")
    print(f"wrote {args.output}: replaced {changed} keyboard events")


if __name__ == "__main__":
    main()
