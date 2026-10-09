"""Exercise the playable audio observer without changing its PCM/game state."""
import argparse
import hashlib
import gzip
import json
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]


def read_audio_trace(path, stats, data=None):
    with gzip.open(path, 'rt') if path.suffix == '.gz' else path.open() as log:
        rows = [json.loads(line) for line in log]
    assert rows[0] == {'format': 'FA18_NATIVE_AUDIO_V1'}
    end = rows[-1]
    assert end['end'] and end['requests'] == stats['sample_requests']
    counts = {'request': 0, 'stop': 0, 'boundary': 0}
    per_channel = [0] * 4
    previous_sample = 0
    for row in rows[1:-1]:
        kind = row['kind']
        counts[kind] += 1
        if kind == 'boundary':
            assert row['tick'] == counts[kind]
            assert row['sample_frames'] == 960 * row['tick']
            assert len(row['voices']) == len(row['channels']) == 4
            for voice in row['voices']:
                assert (len(bytes.fromhex(voice['record'])) == 64) if voice['address'] else voice['record'] is None
            previous_sample = row['sample_frames']
            continue
        assert row['channel'] < 4
        assert previous_sample <= row['sample_frame'] <= previous_sample + 960
        if kind == 'request' and row['active']:
            per_channel[row['channel']] += 1
            assert row['bytes'] > 0 and len(row['sha256']) == 64
            if data is not None:
                address = row['samples'] & ~1
                offset = address if address < 0x80000 else address - 0xc00000 + 0x80000
                payload = data[offset:offset + row['bytes']]
                assert len(payload) == row['bytes']
                assert hashlib.sha256(payload).hexdigest() == row['sha256']
        elif kind == 'request':
            assert not row['bytes'] and not row['sha256']
    assert end['requests'] == counts['request'] and end['stops'] == counts['stop']
    assert end['boundaries'] == counts['boundary'] == stats['frames']
    return {'boundaries': counts['boundary'], 'requests': counts['request'],
            'stops': counts['stop'], 'active_requests': per_channel}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, required=True)
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix='native-audio-trace-', dir=ROOT/'build') as temporary:
        work = Path(temporary)
        replay = work/'intro.e9k'
        replay.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\nF 3000 K 49 0 0 1\nF 3002 K 49 0 0 0\n')
        records = []
        for name, extra in (('plain', []), ('trace', ['--audio-trace', str(work/'audio.jsonl')])):
            folder = work/name
            command = [str(args.runner.resolve()), '--headless', '--frames', '3100',
                '--replay', str(replay), '--save-dir', str(folder), '--wav', str(work/f'{name}.wav'),
                '--data-out', str(work/f'{name}.dat'), '--memory-report', str(work/f'{name}.json'), *extra]
            result = subprocess.run(command, cwd=ROOT, capture_output=True, text=True, timeout=30)
            assert result.returncode == 0, result.stderr
            records.append(json.loads(result.stdout))
            memory = json.loads((work/f'{name}.json').read_text())
            assert memory['project_gameplay_heap_violations'] == memory['sdl_failures'] == 0
        assert records[0] == records[1]
        assert (work/'plain.wav').read_bytes() == (work/'trace.wav').read_bytes()
        assert (work/'plain.dat').read_bytes() == (work/'trace.dat').read_bytes()
        read_audio_trace(work/'audio.jsonl', records[1], (work/'trace.dat').read_bytes())
        for name, extra, expected in (
            ('budget', ['--audio-trace', str(work/'budget.jsonl'), '--capture-budget-mib', '1'], 'exceeds'),
            ('joint', ['--audio-trace', str(work/'joint.jsonl'), '--flight-trace', str(work/'flight.jsonl')], 'own capture budget')):
            result = subprocess.run([str(args.runner.resolve()), '--headless', '--frames', '3100',
                '--save-dir', str(work/name), *extra], cwd=ROOT, capture_output=True, text=True, timeout=30)
            assert result.returncode and expected in result.stderr, result.stderr
        damaged = work/'truncated.jsonl'
        lines = (work/'audio.jsonl').read_text().splitlines()
        damaged.write_text('\n'.join(lines[:-1])+'\n')
        try:
            read_audio_trace(damaged, records[1])
        except (AssertionError, KeyError):
            pass
        else:
            raise AssertionError('Accepted a truncated native audio trace')
    print('Native audio trace preserves complete PCM/RAM/counters, exact payload hashes and the heap guard; truncation/budget/joint capture failures rejected')


if __name__ == '__main__':
    main()
