"""Load the cruise-mission pilot earned by the normal-key rescue save."""
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
FIXTURE = ROOT / 'tools/native/fixtures/cruise-mission-pilot.json'


def load_cruise_pilot():
    metadata = json.loads(FIXTURE.read_text())
    config = bytes.fromhex(metadata['config_hex'])
    assert metadata['schema'] == 1 and len(config) == 78
    assert hashlib.sha256(config).hexdigest() == metadata['config_sha256']
    assert metadata['mission_mode'] == 7 and metadata['availability_earned_by_mode'] == 6
    assert not metadata['full_tour_earned'] and not metadata['native_flight_state_seeded']
    assert config[6] == 6 and config[24] == 1 and int.from_bytes(config[56:58], 'big') == 4
    assert hashlib.sha256((ROOT / 'local/media/fa18.adf').read_bytes()).hexdigest() == metadata['adf_sha256']
    for name, digest in metadata['input_sha256_lf'].items():
        assert hashlib.sha256((ROOT / name).read_bytes().replace(b'\r\n', b'\n')).hexdigest() == digest, name
    return config
