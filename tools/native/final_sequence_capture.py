"""Compare the same final flight in two partitions inside the 480 MiB cap."""
import json
from pathlib import Path
import subprocess

from mission_source_comparison import compare_mission_boundaries
from final_pilot_fixture import load_final_pilot

ROOT = Path(__file__).resolve().parents[2]


def collect_final_sequence(args, work, ram, env, initial=None, scenario='8-sequence'):
    if initial is None:
        initial = load_final_pilot()
    cases, partitions, writes = [], [], 0
    first_exports = first_saved = first_keys = first_wrap = None
    for index, (begin, end) in enumerate(((0, 23000), (23001, 0))):
        partition_work = work / ('combat' if index == 0 else 'return')
        partition_work.mkdir(exist_ok=True)
        pilot = ram / f'pilot-{index}'
        pilot.mkdir()
        (pilot / 'config').write_bytes(initial)
        prefix = ram / f'frame-{index}'
        keys = work / ('pilot.e9k' if index == 0 else 'repeat.e9k')
        run_env = dict(env, FA18_MISSION_END_TICK='40000',
                       FA18_MISSION_CAPTURE_FROM_TICK=str(begin), FA18_MISSION_CAPTURE_UNTIL_TICK=str(end))
        native = subprocess.run([str(args.test.resolve()), str(ROOT / 'local/media/fa18.adf'),
            str(pilot), str(keys), str(prefix), scenario], cwd=ROOT,
            env=run_env, capture_output=True, text=True, timeout=args.timeout)
        (partition_work / 'native.log').write_text(native.stdout + native.stderr)
        assert native.returncode == 0, f"Native exit {native.returncode}; see {partition_work / 'native.log'}: {native.stderr.strip() or native.stdout[-600:]}"
        exports = [json.loads(line) for line in native.stdout.splitlines() if line.startswith('{')]
        entries = [item for item in exports if 'entry' in item]
        bodies = [item for item in exports if 'capture' in item]
        assert entries and bodies and len(entries) + len(bodies) <= 240
        for item in entries:
            assert (begin <= item['tick'] <= end) if end else (item.get('wrap_probe') or item['tick'] >= begin), item
        for item in bodies:
            assert (begin <= item['before_tick'] <= end) if end else (item.get('wrap_probe') or item['before_tick'] >= begin), item
        saved = (pilot / 'config').read_bytes()
        wrap = Path(str(keys) + '.wrap.e9k').read_text()
        if index == 0:
            first_exports, first_saved, first_keys, first_wrap = exports, saved, keys.read_text(), wrap
        else:
            assert saved == first_saved and keys.read_text() == first_keys and wrap == first_wrap, 'Partition replay differs'
            for marker in ('finished', 'sequence', 'next_mission_wrap', 'final_mission_status'):
                assert [item for item in exports if item.get(marker)] == [item for item in first_exports if item.get(marker)], marker
        source_writes = compare_mission_boundaries(prefix, entries, bodies, partition_work)
        writes += source_writes
        cases.extend(item for item in exports if 'entry' in item or 'capture' in item)
        partitions.append({'from_tick': begin, 'until_tick': end, 'intervals': len(entries), 'bodies': len(bodies),
                           'original_config_writes': source_writes,
                           'wrap_intervals': sum(bool(item.get('wrap_probe')) for item in entries),
                           'wrap_bodies': sum(bool(item.get('wrap_probe')) for item in bodies)})
    assert len({item['body'] for item in cases if 'capture' in item}) == sum('capture' in item for item in cases), 'Duplicate capture body'
    exports = [item for item in first_exports if 'entry' not in item and 'capture' not in item] + cases
    (work / 'captures.json').write_text(json.dumps(exports, indent=2) + '\n')
    return exports, work / 'pilot.e9k', initial, first_saved, writes, partitions
