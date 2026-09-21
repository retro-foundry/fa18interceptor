"""Write an E9K playback prefix containing events through a requested frame."""

import argparse
import re
from pathlib import Path


EVENT_FRAME = re.compile(r"^F\s+(\d+)\s+")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", type=Path)
    parser.add_argument("--through-frame", type=int, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()

    lines = args.input.read_text().splitlines()
    if not lines or lines[0] != "E9K_INPUT_V1":
        raise ValueError(f"{args.input} is not an E9K_INPUT_V1 playback")
    output = [lines[0]]
    kept = 0
    for line in lines[1:]:
        match = EVENT_FRAME.match(line)
        if not match:
            raise ValueError(f"unrecognized playback row: {line!r}")
        if int(match.group(1)) <= args.through_frame:
            output.append(line)
            kept += 1
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text("\n".join(output) + "\n")
    print(f"wrote {args.output}: {kept} events through frame {args.through_frame}")


if __name__ == "__main__":
    main()
