"""Rebase a closed Engine9000 input recording from a chosen source frame."""
import argparse
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--input', type=Path, required=True)
    parser.add_argument('--first-frame', type=int, required=True,
                        help='First source frame to retain; it becomes frame 1.')
    parser.add_argument('--last-frame', type=int,
                        help='Inclusive source-frame limit; omit for the remainder.')
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    if args.first_frame < 1:
        raise ValueError('--first-frame must be positive')
    if args.last_frame is not None and args.last_frame < args.first_frame:
        raise ValueError('--last-frame precedes --first-frame')
    lines = args.input.read_text(encoding='ascii').splitlines()
    if not lines or lines[0] != 'E9K_INPUT_V1':
        raise ValueError('Expected E9K_INPUT_V1 recording')
    retained = []
    origin = args.first_frame - 1
    for line in lines[1:]:
        fields = line.split()
        if len(fields) < 3 or fields[0] != 'F':
            raise ValueError(f'Invalid event: {line}')
        frame = int(fields[1])
        if frame < args.first_frame or (args.last_frame is not None and frame > args.last_frame):
            continue
        fields[1] = str(frame - origin)
        retained.append(' '.join(fields))
    args.output.write_text('E9K_INPUT_V1\n' + '\n'.join(retained) + ('\n' if retained else ''),
                           encoding='ascii')
    print({'events': len(retained), 'first_source_frame': args.first_frame,
           'last_source_frame': args.last_frame})


if __name__ == '__main__':
    main()
