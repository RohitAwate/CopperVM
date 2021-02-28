#!/usr/bin/env python3

"""
cutest
The Copper Test Harness

Runs the entire JS suite of tests unless any of them is
marked with a "cutest-ignore" comment at the very top of the file.
"""

from os import listdir
from os.path import isfile, join, realpath, dirname, abspath
import subprocess
from threading import Timer


CUTEST_IGNORE_COMMENT = "// cutest-ignore"

HARNESS_DIR = dirname(realpath(__file__))
JS_SUITE_DIR = join(HARNESS_DIR, "js_suite")
CU_EXE_PATH = abspath(join(HARNESS_DIR, "..", "build/cu"))

TEST_TIMEOUT_SECS = 1/10

def is_ignore_test(filepath: str) -> bool:
    with open(filepath) as fd:
        contents = fd.read().lstrip()

    return contents.startswith(CUTEST_IGNORE_COMMENT)


SUCCESS = 0
FAILED = 1
TIMEOUT = -1
IGNORED = 2


def run_test(filepath: str) -> int:
    if is_ignore_test(filepath=filepath):
        print(f"IGNORED: {filepath}")
        return IGNORED

    test_process = subprocess.Popen([CU_EXE_PATH, filepath])

    timer = Timer(TEST_TIMEOUT_SECS, test_process.kill)

    try:
        timer.start()
        test_process.communicate()
    finally:
        if test_process.returncode == SUCCESS:
            print(f"SUCCESS: {filepath}.")
            if timer.is_alive(): timer.cancel()
            return SUCCESS
        elif test_process.returncode == FAILED:
            print(f"FAILED: {filepath}.")
            if timer.is_alive(): timer.cancel()
            return FAILED
        else:
            print(f"TIMEOUT: {filepath} took too long.")
            return TIMEOUT


if __name__ == "__main__":
    run_stats = {
        SUCCESS: 0,
        FAILED: 0,
        TIMEOUT: 0,
        IGNORED: 0,
    }

    for filename in listdir(JS_SUITE_DIR):
        if isfile(join(JS_SUITE_DIR, filename)) and filename.endswith(".js"):
            returncode = run_test(filepath=join(JS_SUITE_DIR, filename))
            run_stats[returncode] += 1

    total_tests_run = sum(run_stats.values())
    print("\n\n")
    print(f"Total tests run: {total_tests_run}")
    print(f"{run_stats[SUCCESS]} x successful")
    print(f"{run_stats[FAILED]} x failed")
    print(f"{run_stats[TIMEOUT]} x timed-out")
    print(f"{run_stats[IGNORED]} x ignored")