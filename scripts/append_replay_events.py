"""Append explicitly supplied, ordered rows to an E9K playback.

Use only for controlled replay probes: existing rows are preserved and each
new row must be a valid event later than the recording's final source frame.
"""

import argparse
from pathlib import Path


def parse_row(row):
    fields = row.split()
    if len(fields) < 3 or fields[0] != "F":
        raise ValueError(f"unrecognized playback row: {row!r}")
    frame = int(fields[1])
    if frame < 1:
        raise ValueError(f"nonpositive frame in row: {row!r}")
    for value in fields[3:]:
        int(value)
    return frame, " ".join(fields)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", type=Path)
    parser.add_argument("--row", action="append", required=True,
                        help="complete E9K event row, such as 'F 450 K 282 0 16 1'")
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    lines = args.input.read_text(encoding="ascii").splitlines()
    if not lines or lines[0] != "E9K_INPUT_V1":
        raise ValueError(f"{args.input} is not an E9K_INPUT_V1 playback")
    original = [parse_row(line) for line in lines[1:]]
    additions = [parse_row(row) for row in args.row]
    previous = original[-1][0] if original else 0
    for frame, _ in additions:
        if frame <= previous:
            raise ValueError("appended frames must be strictly later than existing rows and ordered")
        previous = frame
    args.output.write_text("E9K_INPUT_V1\n" + "\n".join(
        [row for _, row in original] + [row for _, row in additions]) + "\n",
        encoding="ascii")
    print(f"wrote {args.output}: appended {len(additions)} events")


if __name__ == "__main__":
    main()
