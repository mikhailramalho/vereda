#!/usr/bin/env python3
"""Runs one regression test. The test is a C or IR file whose comment header
describes it:

    // RUN: <flags for vereda.py>            optional, default none; %S is the
                                             directory of the test
    // EXPECT: <regex>                       must match the output (re.MULTILINE)
    // EXPECT-NOT: <regex>                   must not match
    // MODE: CORE|THOROUGH|KNOWNBUG|FUTURE   optional, default CORE
    // FILES: a.c b.c                        extra sources, relative to the test
    // CWD: %S                               the directory to run in, default a
                                             fresh temporary one

IR files use ';' instead of '//'. KNOWNBUG and FUTURE tests are expected to
fail; the run then passes, with exit code 77 so that ctest calls it skipped,
exactly when some expectation does not hold, as long as the verdict is the one
--mark in import_esbmc.py recorded as `// Vereda: verdict X`: a different
verdict fails the run with exit code 4, saying whether it got worse (it was the
expected one), is now right (promote the test or re-mark it), or just changed.
Every run ends with a `verdict: X` line, which --mark reads, and with
REGRESS_REMARK set (as --mark sets it) the recorded verdict is not checked. A run past
--timeout is skipped
too, whatever the mode: how long a test takes depends on the machine and its
load, so a timeout says nothing about the verdict. A run that goes past
--memory-limit fails, with exit code 3 so that --mark in import_esbmc.py
leaves the mode alone, except in a KNOWNBUG test, where it is the known bug:
CI runs several tests at once, and one that needs more memory than its share
would take down the runner.
A test whose `// Needs:` line names an __ESBMC_ or __CPROVER_ builtin, which
Vereda does not support, is skipped without running it.
The tool runs in a fresh temporary directory, so output files never land in
the source tree.

When the environment sets REGRESS_STATS to a directory, the run also writes
<suite>__<name>.json there: the test's wall, walk and solve seconds, peak
memory in MB and verdict, which the performance check compares between a
pull request and its base."""
import argparse
import json
import os
import re
import resource
import signal
import subprocess
import sys
import tempfile
import time

HEADER = re.compile(r"^\s*(?://|;)\s*(RUN|EXPECT|EXPECT-NOT|MODE|FILES|CWD):\s*(.*?)\s*$")
# The verdict a marked test gave when --mark last ran it.
RECORDED = re.compile(r"^\s*(?://|;)\s*Vereda: verdict (\S+)\s*$")
# The Needs line import_esbmc.py writes for a test that calls such a builtin.
NEEDS_BUILTIN = re.compile(r"^\s*(?://|;)\s*Needs:\s*__(?:ESBMC|CPROVER)_\w")
# An expectation that is just the verdict, as test.desc files write it.
EXPECTED = re.compile(r"\^VERIFICATION (SUCCESSFUL|FAILED|UNKNOWN)\$?")
FAIL_MODES = ("KNOWNBUG", "FUTURE")
# Exit code of a KNOWNBUG or FUTURE test that fails as expected; ctest
# reports it as skipped (SKIP_RETURN_CODE in CMakeLists.txt).
SKIPPED = 77
# Exit code of a run that went past --memory-limit.
OUT_OF_MEMORY = 3
# Exit code of a KNOWNBUG or FUTURE test whose verdict is not the recorded one.
VERDICT_CHANGED = 4
# What Vereda, clang and the solvers print when an allocation fails.
NO_MEMORY = re.compile(r"std::bad_alloc|[Oo]ut of memory|Cannot allocate memory|MemoryError")
# Some allocations fail without a message: the binary crashes instead. A run
# that ends with no verdict after reaching this share of the limit ran out.
NEAR_LIMIT = 0.9

VERDICT = re.compile(r"^VERIFICATION (SUCCESSFUL|FAILED|UNKNOWN)\b", re.MULTILINE)
WALK = re.compile(r"^(?:\[INFO\] )?Walk time: ([\d.]+)s .*peak memory (\d+) MB", re.MULTILINE)
SOLVE = re.compile(r"^(?:\[(?:INFO|PROGRESS)\] )?Solving time: ([\d.]+)s \(peak memory (\d+) MB",
                   re.MULTILINE)


def numbers(out):
    """(walk seconds, solve seconds, peak MB) from a run's output, each None
    when the output does not give it. --incremental-bmc walks and solves
    once per bound, so the times add up."""
    walks = [(float(t), int(m)) for t, m in WALK.findall(out)]
    solves = [(float(t), int(m)) for t, m in SOLVE.findall(out)]
    return (sum(t for t, _ in walks) if walks else None,
            sum(t for t, _ in solves) if solves else None,
            max((m for _, m in walks + solves), default=None))


def record(test, start, out, verdict=None):
    """Writes the numbers of the test's run, started at the monotonic time
    start (None for a run that timed out), to $REGRESS_STATS when it is set."""
    stats = os.environ.get("REGRESS_STATS")
    if not stats:
        return
    wall = None if start is None else time.monotonic() - start
    stem = os.path.dirname(test) if os.path.basename(test) == "main.c" else test
    name = (os.path.basename(os.path.dirname(stem)) + "/"
            + os.path.splitext(os.path.basename(stem))[0])
    if verdict is None:
        verdicts = VERDICT.findall(out)
        verdict = verdicts[-1] if verdicts else "ERROR"
    walk, solve, peak = numbers(out)
    os.makedirs(stats, exist_ok=True)
    with open(os.path.join(stats, name.replace("/", "__") + ".json"), "w", encoding="utf-8") as f:
        json.dump({"test": name, "wall": wall, "walk": walk, "solve": solve, "peak": peak,
                   "verdict": verdict}, f)


def parse(path):
    """The header of the test at path, as a dict keyed by directive."""
    desc = {"RUN": "", "EXPECT": [], "EXPECT-NOT": [], "MODE": "CORE", "FILES": "", "CWD": "",
            "VERDICT": None, "BUILTIN": False}
    with open(path, encoding="utf-8", errors="replace") as f:
        for line in f:
            desc["BUILTIN"] |= bool(NEEDS_BUILTIN.match(line))
            if r := RECORDED.match(line):
                desc["VERDICT"] = r.group(1)
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


def verdict_of(out):
    """The run's verdict, or 'none' when it printed none."""
    verdicts = re.findall(r"^VERIFICATION (SUCCESSFUL|FAILED|UNKNOWN)\b", out, re.MULTILINE)
    return verdicts[-1] if verdicts else "none"


def changed(desc, out, cmd):
    """Whether a marked test's verdict is not the one --mark recorded;
    prints the run and how it changed if so."""
    now, then = verdict_of(out), desc["VERDICT"]
    if desc["MODE"] not in FAIL_MODES or not then or now == then:
        return False
    expected = next((m.group(1) for r in desc["EXPECT"] if (m := EXPECTED.fullmatch(r))), None)
    if then == expected:
        why = f"got worse: {then} when marked, {now} now"
    elif now == expected:
        why = (f"verdict now right ({now}, {then} when marked): promote the test to CORE "
               "if the other expectations hold, or re-mark it")
    else:
        why = f"verdict changed: {then} when marked, {now} now; re-mark it"
    print("+", " ".join(cmd))
    print(clip(out))
    print(f"{desc['MODE']} test {why}")
    return True


# The most a run may write to any file, its output included. A loop unrolled
# without --unwind prints a line per iteration; when nothing slows the walk
# down that is gigabytes a minute, which held in memory took CI's runner
# down. The run is stopped (SIGXFSZ) at the limit and counts as timed out: it
# would have run to the time limit anyway.
OUTPUT_LIMIT = 256 << 20
STOPPED_NOTE = f"\nrun stopped: it wrote {OUTPUT_LIMIT >> 20} MB of output\n"


def limits(memory_limit):
    """The resource limits a run gets, set in the child before it starts."""
    def apply():
        if memory_limit:
            size = memory_limit << 20
            resource.setrlimit(resource.RLIMIT_AS, (size, size))
        resource.setrlimit(resource.RLIMIT_FSIZE, (OUTPUT_LIMIT, OUTPUT_LIMIT))
    return apply


def stopped_note(path):
    """STOPPED_NOTE when the run that wrote path was stopped at OUTPUT_LIMIT,
    else ''."""
    return STOPPED_NOTE if os.path.getsize(path) >= OUTPUT_LIMIT else ""


def execute(cmd, timeout, memory_limit, cwd=None):
    """Runs cmd in cwd, else in a fresh temporary directory, under timeout seconds and
    memory_limit MB of address space, and returns its output, or None when
    it timed out."""
    with tempfile.TemporaryDirectory(prefix="vereda-regress-") as tmp:
        path = os.path.join(tmp, "vereda-regress-output")
        # The output goes to a file, which OUTPUT_LIMIT bounds. A session of
        # its own, so that a timeout stops the binary too and not only
        # vereda.py; TMPDIR, so that what a killed vereda.py leaves goes
        # with tmp. run.py starts no threads, so preexec_fn is safe.
        # pylint: disable=subprocess-popen-preexec-fn
        with open(path, "w+", encoding="utf-8", errors="replace") as out, \
             subprocess.Popen(cmd, cwd=cwd or tmp, stdout=out, stderr=subprocess.STDOUT,
                              start_new_session=True, preexec_fn=limits(memory_limit),
                              env={**os.environ, "TMPDIR": tmp}) as proc:
            try:
                proc.wait(timeout=timeout)
            except subprocess.TimeoutExpired:
                os.killpg(proc.pid, signal.SIGKILL)
                proc.wait()
                return None
            except BaseException:  # Ctrl-C does not reach the session
                os.killpg(proc.pid, signal.SIGKILL)
                raise
            out.seek(0)
            return out.read() + stopped_note(path)


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
    if desc["BUILTIN"]:
        print("needs an __ESBMC_ or __CPROVER_ builtin, which Vereda does not support; skipped")
        return SKIPPED
    files = [test] + [os.path.join(os.path.dirname(test), f) for f in desc["FILES"].split()]
    cmd = [sys.executable, args.vereda, "--bin", args.bin, "--clang", args.clang]
    cmd += desc["RUN"].replace("%S", os.path.dirname(test)).split() + files
    start = time.monotonic()
    out = execute(cmd, args.timeout, args.memory_limit,
                  desc["CWD"].replace("%S", os.path.dirname(test)) or None)
    if out is None or out.endswith(STOPPED_NOTE):
        why = (f"TIMEOUT after {args.timeout:g} s" if out is None
               else f"OUTPUT past {OUTPUT_LIMIT >> 20} MB, as a timeout")
        print(f"{why}; skipped")
        record(test, None, "", "TIMEOUT")
        return SKIPPED

    # Peak resident memory, in KB, of the largest process the run waited for.
    peak = resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss
    near_limit = args.memory_limit and peak >= NEAR_LIMIT * (args.memory_limit << 10)
    no_verdict = not re.search(r"^VERIFICATION ", out, re.MULTILINE)
    if args.memory_limit and (NO_MEMORY.search(out) or (near_limit and no_verdict)):
        record(test, start, out, "MEMOUT")
        if desc["MODE"] == "KNOWNBUG":
            print(f"OUT OF MEMORY past {args.memory_limit} MB, a known bug; skipped")
            return SKIPPED
        print("+", " ".join(cmd))
        print(clip(out))
        print(f"OUT OF MEMORY: the run needed more than {args.memory_limit} MB")
        return OUT_OF_MEMORY

    record(test, start, out)
    print(f"verdict: {verdict_of(out)}")
    if not os.environ.get("REGRESS_REMARK") and changed(desc, out, cmd):
        return VERDICT_CHANGED
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
