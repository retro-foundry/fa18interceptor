"""Summarize native frame work separately from host pacing and scheduling.

Headless rows measure input/game/audio only. Window rows include conversion
and SDL submission/presentation; a hidden window does not measure compositor
visibility. Thresholds apply to measured work, not the intentional frame wait.
"""
import argparse
import csv
import json
import math
from pathlib import Path

PHASES = ('input_us', 'game_us', 'audio_us', 'convert_us', 'present_us', 'wait_us',
          'work_us', 'total_us', 'start_interval_us')


def summarize(rows, budget_us):
    def phase(name):
        values = sorted(float(row[name]) for row in rows)
        if not values:
            return None
        return {'mean_ms': round(sum(values) / len(values) / 1000, 4),
                'p95_ms': round(values[math.ceil(len(values) * .95) - 1] / 1000, 4),
                'p99_ms': round(values[math.ceil(len(values) * .99) - 1] / 1000, 4),
                'max_ms': round(values[-1] / 1000, 4)}
    return {'frames': len(rows), 'presented': sum(int(row['presented']) for row in rows),
            'over_work_budget': sum(float(row['work_us']) > budget_us for row in rows),
            'phases': {name: phase(name) for name in PHASES},
            'worst_work': sorted(rows, key=lambda row: float(row['work_us']), reverse=True)[:5]}


def report(path, budget_us):
    with path.open(newline='') as file:
        rows = list(csv.DictReader(file))
    if not rows or [int(row['frame']) for row in rows] != list(range(1, len(rows) + 1)):
        raise ValueError('Timing report must contain every frame, in order, starting at frame 1')
    for row in rows:
        for name in PHASES:
            value = float(row[name])
            if not math.isfinite(value) or value < 0:
                raise ValueError(f'Invalid {name} at frame {row["frame"]}')
        if row['presented'] not in ('0', '1') or row['scene_updated'] not in ('0', '1'):
            raise ValueError(f'Invalid presentation/scene flag at frame {row["frame"]}')
        if abs(sum(float(row[name]) for name in PHASES[:5]) - float(row['work_us'])) >= .01:
            raise ValueError(f'Work phase sum differs at frame {row["frame"]}')
        if abs(float(row['work_us']) + float(row['wait_us']) - float(row['total_us'])) >= .01:
            raise ValueError(f'Total phase sum differs at frame {row["frame"]}')
    scene = [row for row in rows if row['scene_updated'] == '1']
    return {'source': path.name, 'budget_ms': budget_us / 1000,
            'renderers': sorted({row['renderer'] for row in rows}),
            'all': summarize(rows, budget_us), 'scene_updates': summarize(scene, budget_us),
            'scene_views': {view: summarize([row for row in scene if row['view'] == view], budget_us)
                            for view in sorted({row['view'] for row in scene}, key=int)}}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('csv', type=Path)
    parser.add_argument('--output', type=Path)
    parser.add_argument('--budget-ms', type=float, default=20)
    parser.add_argument('--require-budget', action='store_true')
    parser.add_argument('--require-presentation', action='store_true')
    args = parser.parse_args()
    if not math.isfinite(args.budget_ms) or args.budget_ms <= 0:
        parser.error('--budget-ms must be finite and positive')
    try:
        result = report(args.csv, args.budget_ms * 1000)
        if args.output:
            args.output.write_text(json.dumps(result, indent=2) + '\n')
    except (OSError, ValueError, KeyError, TypeError) as error:
        parser.exit(1, f'Cannot summarize {args.csv}: {error}\n')
    totals = result['all']
    print(f"{totals['frames']} frames; {totals['presented']} presented; "
          f"work p99/max {totals['phases']['work_us']['p99_ms']:.4f}/"
          f"{totals['phases']['work_us']['max_ms']:.4f} ms; "
          f"{totals['over_work_budget']} over {args.budget_ms:g} ms")
    if args.require_budget and totals['over_work_budget']:
        parser.exit(1, 'Measured frame work exceeds the requested budget\n')
    if args.require_presentation and totals['presented'] != totals['frames']:
        parser.exit(1, 'The report does not show SDL presentation for every frame\n')


if __name__ == '__main__':
    main()
