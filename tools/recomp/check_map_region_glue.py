"""Independently prove the readable whole-call map and region C.

The shared verifier builds a temporary registry through the object cache and
checks all sealed recordings. Normal source registration and runner stay intact.
"""
from check_whole_call_glue import main


if __name__ == "__main__":
    main(("C2AA9C", "C2AB34", "C2AB5A", "C2B05A"))
