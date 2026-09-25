"""Write an E9K playback prefix or a frame-rebased suffix."""

import argparse
import re
from pathlib import Path


EVENT_FRAME = re.compile(r"^F\s+(\d+)\s+")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", type=Path)
    bounds = parser.add_mutually_exclusive_group(required=True)
    bounds.add_argument("--through-frame", type=int,
                        help="Keep events through this absolute source frame.")
    bounds.add_argument("--from-frame", type=int,
                        help="Keep later events and subtract this frame from their timestamps.")
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
        frame = int(match.group(1))
        if args.through_frame is not None:
            if frame <= args.through_frame:
                output.append(line)
                kept += 1
        elif frame > args.from_frame:
            output.append(EVENT_FRAME.sub(f"F {frame - args.from_frame} ", line, count=1))
            kept += 1
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text("\n".join(output) + "\n")
    description = (f"through frame {args.through_frame}" if args.through_frame is not None
                   else f"rebased after frame {args.from_frame}")
    print(f"wrote {args.output}: {kept} events {description}")


if __name__ == "__main__":
    main()
