"""Exercise the real original capture caller and reject incomplete/corrupt PCM."""
import json
import hashlib
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

from check_original_audio_capture import validate
from check_original_audio_events import validate_events

ROOT = Path(__file__).resolve().parents[2]


def main():
    with tempfile.TemporaryDirectory(prefix='original-pcm-',dir=ROOT/'build') as temporary:
        work = Path(temporary)
        base = [sys.executable,str(ROOT/'scripts/engine9000_bridge.py'),
                '--frames','32','--restore',str(ROOT/'captures/native/demo01/state.bin')]
        for name, extra in (('baseline',[]),('capture',['--wav']),
                            ('events',['--wav','--audio-events'])):
            result = subprocess.run([*base,'--output',str(work/name),*extra],cwd=ROOT,
                                    capture_output=True,text=True,timeout=20)
            assert result.returncode == 0, result.stderr
        result = validate(work/'capture',work/'baseline')
        assert result['replay_calls'] == result['chunks'] == 32
        assert result['recorded_audio']['sample_frames'] == 32*882
        events = validate_events(work/'events', work/'baseline')
        assert events['recorded_audio']['pcm_sha256'] == result['recorded_audio']['pcm_sha256']
        rows = [json.loads(line) for line in (work/'events/audio_events.jsonl').read_text().splitlines()]
        assert len([row for row in rows if row['kind'] == 'initial']) == 1
        assert [row['call'] for row in rows if row['kind'] == 'boundary'] == list(range(1,33))
        assert any(row['kind'] == 'write' and 0xdff0a0 <= row['address'] <= 0xdff0da for row in rows)
        assert events['audio_events']['led_interface']
        assert events['known_voice_boundaries'] == 32 and events['unknown_voice_boundaries'] == 0
        assert events['resolved_voice_layout']['voice_slots'] == 0xc4fe38
        assert sum(events['power_filter_boundary_counts'].values()) == 32
        assert any(row['kind'] == 'led' and row['led'] == 0 for row in rows)
        for change in ('missing-boundary', 'wrong-sample-boundary', 'wrong-led-boundary',
                       'missing-led-state', 'missing-led-notification', 'wrong-led-sample-offset',
                       'wrong-voice-layout', 'missing-voice-layout', 'wrong-voice-owner'):
            damaged = work/change
            shutil.copytree(work/'events', damaged)
            modified = [dict(row) for row in rows]
            index = next(i for i,row in enumerate(modified) if row['kind'] == 'boundary')
            if change == 'missing-boundary':
                del modified[index]
            elif change == 'wrong-sample-boundary':
                modified[index]['sample_frames'] += 1
            elif change == 'wrong-led-boundary':
                modified[index]['led_states'] = modified[index]['led_states'].copy()
                modified[index]['led_states'][0] ^= 1
            elif change == 'missing-led-state':
                del modified[index]['led_states']
            elif change == 'wrong-voice-layout':
                modified[index]['voice_layout'] = dict(modified[index]['voice_layout'],voice_slots=0xc50018)
            elif change == 'missing-voice-layout':
                del modified[index]['voice_layout']
            elif change == 'wrong-voice-owner':
                modified[index]['voice_layout'] = None
                modified[index]['voices'] = modified[index]['master_volume'] = None
            else:
                index = next(i for i,row in enumerate(modified) if row['kind'] == 'led')
                if change == 'missing-led-notification':
                    del modified[index]
                else:
                    modified[index]['sample_frames'] += 1
            log = damaged/'audio_events.jsonl'
            log.write_text(''.join(json.dumps(row)+'\n' for row in modified), encoding='utf8')
            report_path = damaged/'snapshot.json'
            report = json.loads(report_path.read_text())
            report['audio_events']['sha256'] = hashlib.sha256(log.read_bytes()).hexdigest()
            report['audio_events']['rows'] = len(modified)
            report_path.write_text(json.dumps(report))
            try:
                validate_events(damaged, work/'baseline')
            except (AssertionError, KeyError):
                pass
            else:
                raise AssertionError(f'Accepted damaged original events: {change}')
        for change in ('drop','offset','pcm'):
            damaged = work/change
            shutil.copytree(work/'capture',damaged)
            log = damaged/'audio_chunks.jsonl'
            rows = log.read_text().splitlines()
            if change == 'drop':
                del rows[10]
            elif change == 'offset':
                row = json.loads(rows[0]);row['first_sample'] += 1;rows[0]=json.dumps(row)
            else:
                wav = damaged/'original.wav'
                data = bytearray(wav.read_bytes());data[44] ^= 1;wav.write_bytes(data)
            log.write_text('\n'.join(rows)+'\n')
            try:
                validate(damaged,work/'baseline')
            except AssertionError:
                pass
            else:
                raise AssertionError(f'Accepted damaged original PCM: {change}')
        # Reusing an explicit retained WAV must not bypass PCM verification.
        validate_events(work/'events', work/'baseline', work/'capture/original.wav')
        try:
            validate_events(work/'events', work/'baseline', work/'pcm/original.wav')
        except AssertionError:
            pass
        else:
            raise AssertionError('Accepted corrupt reusable original PCM')
        for name, extra, message in (
            ('stepping',['--wav','--trace-frames','1'],'ordinary full-frame'),
            ('budget',['--wav','--capture-budget-mib','8'],'exceeds capture budget')):
            result = subprocess.run([*base,'--output',str(work/name),*extra],cwd=ROOT,
                                    capture_output=True,text=True,timeout=20)
            assert result.returncode and message in result.stderr, result.stderr
            assert not (work/name/'original.wav').exists()
    print('Real 32-frame original PCM/event/LED/loaded-voice tracing preserves complete state; corrupt PCM/events/LED/voice ownership/reference, stepping and budget failures rejected')


if __name__ == '__main__':
    try:
        main()
    finally:
        subprocess.run([sys.executable,str(ROOT/'scripts/prune_build_artifacts.py'),'--quiet'],cwd=ROOT,check=True)
