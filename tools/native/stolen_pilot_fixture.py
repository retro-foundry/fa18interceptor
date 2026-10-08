"""Load stolen-aircraft mission availability earned by a newly enlisted pilot."""
import hashlib
import json
from pathlib import Path

from region_pilot_fixture import load_region_pilot

ROOT = Path(__file__).resolve().parents[2]
FIXTURE = ROOT / 'tools/native/fixtures/stolen-mission-pilot.json'


def load_stolen_pilot():
    metadata = json.loads(FIXTURE.read_text())
    config = bytes.fromhex(metadata['config_hex'])
    assert metadata['schema'] == 1 and len(config) == 78
    assert hashlib.sha256(config).hexdigest() == metadata['config_sha256']
    assert metadata['mission_mode'] == 5 and metadata['availability_earned_by_mode'] == 4
    assert metadata['mission_availability_earned'] and metadata['pilot_newly_enlisted']
    assert not metadata['full_tour_earned'] and not metadata['native_flight_state_seeded']
    assert config[:4] == b'\0\1\0\0' and config[6] == 4 and config[21:23] == b'\1\1'
    assert int.from_bytes(config[56:58], 'big') == 2
    assert hashlib.sha256(load_region_pilot()).hexdigest() == metadata['initial_config_sha256']
    assert hashlib.sha256((ROOT / 'local/media/fa18.adf').read_bytes()).hexdigest() == metadata['adf_sha256']
    for name, digest in metadata['input_sha256_lf'].items():
        assert hashlib.sha256((ROOT / name).read_bytes().replace(b'\r\n', b'\n')).hexdigest() == digest, name
    return config
