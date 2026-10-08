"""Reproduce the region pilot through normal menus and earned mission saves."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import sys

from capture_workspace import CaptureWorkspace
from mission_source_comparison import compare_mission_boundaries

ROOT = Path(__file__).resolve().parents[2]
FIXTURE = ROOT / 'tools/native/fixtures/region-flight-pilot.json'


def load_region_pilot():
    metadata = json.loads(FIXTURE.read_text())
    config = bytes.fromhex(metadata['config_hex'])
    assert metadata['schema'] == 1 and len(config) == 78
    assert hashlib.sha256(config).hexdigest() == metadata['config_sha256']
    assert config[:4] == b'\0\1\0\0' and config[21] == 1
    for name, digest in metadata['input_sha256_lf'].items():
        assert hashlib.sha256((ROOT / name).read_bytes().replace(b'\r\n', b'\n')).hexdigest() == digest, name
    return config


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, default=ROOT / 'build/native/fa18_native.exe')
    parser.add_argument('--qualification-test', type=Path, default=ROOT / 'build/native-cmake/native/Release/fa18_native_qualification_sequence_test.exe')
    parser.add_argument('--mission-test', type=Path, default=ROOT / 'build/native-cmake/native/Release/fa18_native_mission_success_test.exe')
    parser.add_argument('--out', type=Path, default=ROOT / 'build/native-flight/region-pilot-progression')
    args = parser.parse_args()
    work = args.out.resolve()
    expected = load_region_pilot()
    metadata = json.loads(FIXTURE.read_text())
    adf = ROOT / 'local/media/fa18.adf'
    assert hashlib.sha256(adf.read_bytes()).hexdigest() == metadata['adf_sha256']
    report = {'native_flight_state_seeded': False, 'phases': {}}

    def run(name, command):
        result = subprocess.run(list(map(str, command)), cwd=ROOT, capture_output=True, text=True, timeout=90)
        (work / f'{name}.log').write_text(result.stdout + result.stderr)
        if result.returncode:
            raise RuntimeError(f'{name}: {result.stderr or result.stdout}')
        return [json.loads(line) for line in result.stdout.splitlines() if line.startswith('{')]

    def checked_boundaries(name, exports, prefix):
        output = work / name
        output.mkdir(exist_ok=True)
        entries = [item for item in exports if 'entry' in item]
        bodies = [item for item in exports if 'capture' in item]
        (output / 'captures.json').write_text(json.dumps(exports, indent=2) + '\n')
        writes = compare_mission_boundaries(prefix, entries, bodies, output)
        assert writes == 1, (name, writes)
        report['phases'][name] = {'input_stage_intervals': len(entries), 'sampled_bodies': len(bodies), 'original_config_writes': writes}
        print(f'{name}: {len(entries)} intervals/{len(bodies)} bodies match original; one config write', flush=True)

    with CaptureWorkspace(work) as ram:
        pilot = ram / 'pilot'
        pilot.mkdir()
        runner = args.runner.resolve()
        fixtures = ROOT / 'tools/native/fixtures'
        base = [runner, '--adf', adf, '--save-dir', pilot, '--headless']
        menu = run('enlist', [*base, '--frames', 9000, '--replay', fixtures / 'region-pilot-enlist.e9k'])[0]
        initial = (pilot / 'config').read_bytes()
        assert menu['screen'] == 'menu' and hashlib.sha256(initial).hexdigest() == metadata['initial_config_sha256']
        assert not menu['cpu_emulation'] and not menu['chipset_emulation']
        report['phases']['enlist'] = {'screen': menu['screen'], 'config_sha256': metadata['initial_config_sha256']}

        acknowledge = ram / 'acknowledge.e9k'
        acknowledge.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
        qualification_input = fixtures / 'carrier_qualification_game_input.fa18in'
        canonical = run('canonical-qualification', [*base, '--frames', 45000, '--replay', acknowledge,
            '--input', qualification_input, '--iterations', metadata['qualification_iterations']])[0]
        qualified = (pilot / 'config').read_bytes()
        assert canonical['replay_iterations'] == 8038 and canonical['stage'] == 'C10DAE' and not canonical['postflight_resets']
        assert hashlib.sha256(qualified).hexdigest() == metadata['qualification_config_sha256']

        qualification_pilot = ram / 'qualification-pilot'
        qualification_pilot.mkdir()
        # Replay from the actual menu-earned log, never a synthesized pilot.
        (qualification_pilot / 'config').write_bytes(initial)
        prefix = ram / 'qualification'
        exports = run('qualification', [args.qualification_test.resolve(), adf, qualification_pilot, qualification_input, prefix])
        assert (qualification_pilot / 'config').read_bytes() == qualified
        summary = [item for item in exports if item.get('sequence')]
        assert len(summary) == 1 and summary[0]['initial_qualification'] == 0 and summary[0]['success'] and summary[0]['restarted'] and summary[0]['reloaded']
        checked_boundaries('qualification', exports, prefix)

        prefix = ram / 'mission'
        consumed = ram / 'mission.e9k'
        exports = run('mission-three', [args.mission_test.resolve(), adf, pilot, consumed, prefix, '3-sequence'])
        actual = (pilot / 'config').read_bytes()
        assert actual == expected
        assert consumed.read_bytes().replace(b'\r\n', b'\n') == (fixtures / 'region-pilot-mission-three.e9k').read_bytes().replace(b'\r\n', b'\n')
        sequence = [item for item in exports if item.get('sequence')]
        finish = [item for item in exports if item.get('finished')]
        assert len(sequence) == len(finish) == 1 and sequence[0]['menu'] and sequence[0]['restarted']
        assert finish[0]['completions_before'] == 0 and finish[0]['completions_after'] == 1 and finish[0]['reloaded'] and not finish[0]['crash_resets']
        checked_boundaries('mission-three', exports, prefix)

        canonical_pilot = ram / 'canonical-mission-pilot'
        canonical_pilot.mkdir()
        (canonical_pilot / 'config').write_bytes(qualified)
        canonical = run('canonical-mission-three', [runner, '--adf', adf, '--save-dir', canonical_pilot,
            '--headless', '--frames', metadata['mission_three_menu_tick'], '--replay', consumed])[0]
        assert canonical['screen'] == 'menu' and canonical['stage'] == 'C0FCB4' and not canonical['postflight_resets']
        assert (canonical_pilot / 'config').read_bytes() == expected
        report['config_sha256'] = metadata['config_sha256']
        report['reference_scope'] = 'Original instructions from native before-states; canonical runner agrees on earned saves/menu return, not independent original whole-flight parity'
        (work / 'comparison.json').write_text(json.dumps(report, indent=2) + '\n')
    print('Canonical reset/qualification/mission-three saves reproduce the retained region pilot', flush=True)


if __name__ == '__main__':
    try:
        main()
    finally:
        subprocess.run([sys.executable, 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)
