#!/usr/bin/env python3
"""Runs one regression test. The test is a C or IR file whose comment header
describes it:

    // RUN: <flags for vereda.py>            optional, default none; %S is the
                                             directory of the test
    // EXPECT: <regex>                       must match the output (re.MULTILINE)
    // EXPECT-NOT: <regex>                   must not match
    // MODE: CORE|THOROUGH|KNOWNBUG|FUTURE   optional, default CORE
    // FILES: a.c b.c                        extra sources, relative to the test

IR files use ';' instead of '//'. KNOWNBUG and FUTURE tests are expected to
fail; the run then passes, with exit code 77 so that ctest calls it skipped,
exactly when some expectation does not hold. A run past --timeout is skipped
too, whatever the mode: how long a test takes depends on the machine and its
load, so a timeout says nothing about the verdict. A run that goes past
--memory-limit fails, with exit code 3 so that --mark in import_esbmc.py
leaves the mode alone, except in a KNOWNBUG test, where it is the known bug:
CI runs several tests at once, and one that needs more memory than its share
would take down the runner.
The tool runs in a fresh temporary directory, so output files never land in
the source tree."""
import argparse
import os
import re
import resource
import signal
import subprocess
import sys
import tempfile

HEADER = re.compile(r"^\s*(?://|;)\s*(RUN|EXPECT|EXPECT-NOT|MODE|FILES):\s*(.*?)\s*$")
FAIL_MODES = ("KNOWNBUG", "FUTURE")
# Exit code of a KNOWNBUG or FUTURE test that fails as expected; ctest
# reports it as skipped (SKIP_RETURN_CODE in CMakeLists.txt).
SKIPPED = 77
# Exit code of a run that went past --memory-limit.
OUT_OF_MEMORY = 3
# What Vereda, clang and the solvers print when an allocation fails.
NO_MEMORY = re.compile(r"std::bad_alloc|[Oo]ut of memory|Cannot allocate memory|MemoryError")
# Some allocations fail without a message: the binary crashes instead. A run
# that ends with no verdict after reaching this share of the limit ran out.
NEAR_LIMIT = 0.9


def parse(path):
    """The header of the test at path, as a dict keyed by directive."""
    desc = {"RUN": "", "EXPECT": [], "EXPECT-NOT": [], "MODE": "CORE", "FILES": ""}
    with open(path, encoding="utf-8", errors="replace") as f:
        for line in f:
            m = HEADER.match(line)
            if not m:
                continue
            key, value = m.groups()
            if key in ("EXPECT", "EXPECT-NOT"):
                desc[key].append(value)
            else:
                desc[key] = value
    return desc


# Lines of a failing run's output shown from each end; a long unwinding or
# counterexample would otherwise fill the CI log.
CLIP = 100


def clip(out):
    """out, with all but its first and last CLIP lines replaced by a count."""
    lines = out.splitlines()
    if len(lines) <= 2 * CLIP + 1:
        return out
    return "\n".join(lines[:CLIP] + [f"[... {len(lines) - 2 * CLIP} lines ...]"] + lines[-CLIP:])


def main():
    """Runs the test and returns 0 when it behaves as its MODE says."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--vereda", required=True, help="path to vereda.py")
    parser.add_argument("--bin", required=True, help="path to the vereda binary")
    parser.add_argument("--clang", required=True, help="path to clang")
    parser.add_argument("--timeout", type=float, help="seconds before the run is stopped")
    parser.add_argument("--memory-limit", type=int,
                        help="MB of address space for the run and what it starts")
    parser.add_argument("test")
    args = parser.parse_args()

    test = os.path.abspath(args.test)
    desc = parse(test)
    files = [test] + [os.path.join(os.path.dirname(test), f) for f in desc["FILES"].split()]
    cmd = [sys.executable, args.vereda, "--bin", args.bin, "--clang", args.clang]
    cmd += desc["RUN"].replace("%S", os.path.dirname(test)).split() + files
    def limit_memory():
        if args.memory_limit:
            size = args.memory_limit << 20
            resource.setrlimit(resource.RLIMIT_AS, (size, size))

    with tempfile.TemporaryDirectory(prefix="vereda-regress-") as tmp:
        # A session of its own, so that a timeout stops the binary too and
        # not only vereda.py; TMPDIR, so that what a killed vereda.py leaves
        # goes with tmp.
        with subprocess.Popen(cmd, cwd=tmp, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                              text=True, start_new_session=True, preexec_fn=limit_memory,
                              env={**os.environ, "TMPDIR": tmp}) as proc:
            try:
                out = proc.communicate(timeout=args.timeout)[0]
            except subprocess.TimeoutExpired:
                os.killpg(proc.pid, signal.SIGKILL)
                proc.communicate()
                print(f"TIMEOUT after {args.timeout:g} s; skipped")
                return SKIPPED
            except BaseException:  # Ctrl-C does not reach the session
                os.killpg(proc.pid, signal.SIGKILL)
                raise

    # Peak resident memory, in KB, of the largest process the run waited for.
    peak = resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss
    near_limit = args.memory_limit and peak >= NEAR_LIMIT * (args.memory_limit << 10)
    no_verdict = not re.search(r"^VERIFICATION ", out, re.MULTILINE)
    if args.memory_limit and (NO_MEMORY.search(out) or (near_limit and no_verdict)):
        if desc["MODE"] == "KNOWNBUG":
            print(f"OUT OF MEMORY past {args.memory_limit} MB, a known bug; skipped")
            return SKIPPED
        print("+", " ".join(cmd))
        print(clip(out))
        print(f"OUT OF MEMORY: the run needed more than {args.memory_limit} MB")
        return OUT_OF_MEMORY

    unmet = [f"EXPECT: {r}" for r in desc["EXPECT"] if not re.search(r, out, re.MULTILINE)]
    unmet += [f"EXPECT-NOT: {r}" for r in desc["EXPECT-NOT"] if re.search(r, out, re.MULTILINE)]
    expected_to_fail = desc["MODE"] in FAIL_MODES
    ok = (not unmet) != expected_to_fail
    if not ok:
        print("+", " ".join(cmd))
        print(clip(out))
        if expected_to_fail:
            print(f"{desc['MODE']} test passed unexpectedly; promote it to CORE")
        for u in unmet:
            print("unmet", u)
    if not ok:
        return 1
    return SKIPPED if expected_to_fail else 0


if __name__ == "__main__":
    sys.exit(main())
