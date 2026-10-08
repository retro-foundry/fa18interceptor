"""Load actual new-pilot saves admitting the later tour missions."""
import hashlib
import json
from pathlib import Path

from stolen_pilot_fixture import load_stolen_pilot

ROOT = Path(__file__).resolve().parents[2]
FIXTURE = ROOT / 'tools/native/fixtures/new-pilot-tour-availability.json'


def load_tour_pilot(mode):
    assert mode in (5, 6, 7, 8)
    if mode == 5:
        return load_stolen_pilot()
    metadata = json.loads(FIXTURE.read_text())
    assert metadata['schema'] == 1 and metadata['pilot_newly_enlisted']
    assert metadata['mission_availability_earned'] and not metadata['full_tour_earned']
    assert not metadata['native_flight_state_seeded']
    assert hashlib.sha256((ROOT / 'local/media/fa18.adf').read_bytes()).hexdigest() == metadata['adf_sha256']
    record = metadata['missions'][str(mode)]
    config = bytes.fromhex(record['initial_config_hex'])
    assert len(config) == 78 and config[:4] == b'\0\1\0\0' and config[6] == mode - 1
    assert config[21:18 + mode] == b'\1' * (mode - 3)
    assert int.from_bytes(config[56:58], 'big') == mode - 3
    assert hashlib.sha256(config).hexdigest() == record['initial_config_sha256']
    assert hashlib.sha256(load_tour_pilot(mode - 1)).hexdigest() == record['previous_initial_config_sha256']
    keys = (ROOT / record['previous_input']).read_bytes().replace(b'\r\n', b'\n')
    assert hashlib.sha256(keys).hexdigest() == record['previous_input_sha256_lf']
    return config


def tour_input(mode):
    assert mode in (5, 6, 7)
    metadata = json.loads(FIXTURE.read_text())
    return ROOT / metadata['missions'][str(mode + 1)]['previous_input']


def load_tour_result():
    metadata = json.loads((ROOT / 'tools/native/fixtures/new-pilot-tour-result.json').read_text())
    assert metadata['schema'] == 1 and metadata['pilot_newly_enlisted'] and metadata['full_tour_earned']
    assert not metadata['native_flight_state_seeded']
    assert hashlib.sha256(load_tour_pilot(8)).hexdigest() == metadata['initial_config_sha256']
    config = bytes.fromhex(metadata['config_hex'])
    assert len(config) == 78 and config[:4] == b'\0\1\0\0' and config[6:8] == b'\x08\x00'
    assert config[21:27] == b'\1' * 6 and int.from_bytes(config[56:58], 'big') == 6
    assert hashlib.sha256(config).hexdigest() == metadata['config_sha256']
    assert hashlib.sha256((ROOT / 'local/media/fa18.adf').read_bytes()).hexdigest() == metadata['adf_sha256']
    for field in ('input', 'wrap_input'):
        keys = (ROOT / metadata[field]).read_bytes().replace(b'\r\n', b'\n')
        assert hashlib.sha256(keys).hexdigest() == metadata[field + '_sha256_lf']
    return config, metadata
