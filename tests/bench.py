#!/usr/bin/env python3
"""Compares the time and memory of two vereda binaries, a base and a head, on
a list of regression tests (bench.txt next to this script by default). Each
test runs with its own // RUN: flags, as run.py runs it, once with each
binary; the two runs go side by side, so a busy machine slows both alike.

A test regresses when head's wall time is above RATIO times base's and at
least MIN_SECONDS more, when its peak memory is above RATIO times base's and
at least MIN_MB more, when the verdicts differ, when head times out or is
killed for lack of memory and base was not, or when head ends without a
verdict (a crash, a bad flag). A test that regresses on time or memory runs
once more and counts only if it regresses again, since shared CI machines
are noisy. Head's time limit is max(SLOWDOWN times base's wall time,
MIN_LIMIT seconds). A test missing from the base tree is skipped.

A test improves by the same margins the other way, or when base timed out or
was killed and head was not; an improvement is also measured twice.

Prints a Markdown table, also appended to $GITHUB_STEP_SUMMARY when set, and
exits 1 when some test regressed. --changes FILE writes a table of only the
tests that regressed or improved, and nothing when none did.

--stats BASE HEAD picks the tests instead of --list: each directory holds the
numbers run.py records under $REGRESS_STATS for a whole suite run, and the
tests whose numbers regress or improve between the two by the margins above
are measured again here, side by side, at most MAX_CANDIDATES of them,
largest change first. Two suite runs on different machines and loads are
too noisy to report on their own; the side-by-side run decides. When the
base's suite run timed out or ran out of memory, head runs alone, with the
base's time limit, and the base's result is the suite run's, marked so in
the table."""
import argparse
import contextlib
import glob
import json
import os
import resource
import signal
import subprocess
import sys
import tempfile
import threading
import time

import run

RATIO = 1.5
MIN_SECONDS = 2.0
MIN_MB = 200
SLOWDOWN = 10
MIN_LIMIT = 120
MAX_CANDIDATES = 40

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TIMEOUT = "TIMEOUT"
# A run killed by a signal it did not send itself (on Linux the kernel's OOM
# killer), or one that printed an allocation failure under --memory-limit,
# is out of memory, as a timeout is out of time. vereda.py exits with the
# binary's status, -9 as a byte.
KILLED = "KILLED"
KILLED_STATUS = (-signal.SIGKILL, 256 - signal.SIGKILL)
STOPPED = {TIMEOUT: "head timed out", KILLED: "head was killed, out of memory"}


def test_files(root, name):
    """The flags and source files of test suite/name in the tree at root,
    or None when it has no such test."""
    # tests/ since fcf278ec, regression/ in a base from before.
    for stem in (os.path.join(root, d, name) for d in ("tests", "regression")):
        for path in (stem + ".c", stem + ".ll", os.path.join(stem, "main.c")):
            if not os.path.isfile(path):
                continue
            desc = run.parse(path)
            extra = [os.path.join(os.path.dirname(path), f) for f in desc["FILES"].split()]
            return desc["RUN"].split(), [path] + extra
    return None


@contextlib.contextmanager
def started(root, binary, clang, test, tmp, memory_limit):
    """Starts vereda.py from root on test with binary, in directory tmp, and
    yields (process, output file, start time, waiter thread, end times);
    the waiter puts the end time in the list as the process ends, so that
    a run is timed right while the other one is waited for. Stops the run
    on exit."""
    flags, files = test
    cmd = [sys.executable, os.path.join(root, "vereda.py"), "--bin", binary,
           "--clang", clang] + flags + files
    os.mkdir(tmp)
    # The output goes to a file: a pipe left unread would stall one run
    # while the other is waited for. A session of its own, so that a
    # timeout stops the binary too and not only vereda.py. The suite's limit
    # on address space, so that two runs side by side cannot take all of the
    # runner's memory, which gets the whole job killed.
    def limit_memory():
        size = memory_limit << 20
        resource.setrlimit(resource.RLIMIT_AS, (size, size))

    # The other threads only wait for processes, so the child, which calls
    # nothing but setrlimit before exec, cannot block on a lock they hold.
    # pylint: disable=subprocess-popen-preexec-fn
    with open(os.path.join(tmp, "out"), "w+", encoding="utf-8", errors="replace") as out, \
         subprocess.Popen(cmd, cwd=tmp, stdout=out, stderr=subprocess.STDOUT,
                          start_new_session=True, preexec_fn=limit_memory,
                          env={**os.environ, "TMPDIR": tmp}) as proc:
        start, ended = time.monotonic(), []

        def wait():
            proc.wait()
            ended.append(time.monotonic())
        waiter = threading.Thread(target=wait, daemon=True)
        waiter.start()
        try:
            yield proc, out, start, waiter, ended
        finally:
            if waiter.is_alive():
                kill(proc)
                waiter.join()


def kill(proc):
    """Stops proc's session, which may have ended meanwhile."""
    with contextlib.suppress(ProcessLookupError):
        os.killpg(proc.pid, signal.SIGKILL)


def finish(started_run, deadline):
    """Waits for a started run until the monotonic deadline and returns
    (wall seconds, walk seconds, solve seconds, peak MB, verdict); a number
    the output does not give is None."""
    proc, out, start, waiter, ended = started_run
    waiter.join(max(0, deadline - time.monotonic()))
    if waiter.is_alive():
        kill(proc)
        waiter.join()
        wall, verdict = None, TIMEOUT
    else:
        wall, verdict = ended[0] - start, None
    out.seek(0)
    text = out.read()
    if verdict is None:
        verdicts = run.VERDICT.findall(text)
        if verdicts:
            verdict = verdicts[-1]
        else:
            stopped = proc.returncode in KILLED_STATUS or run.NO_MEMORY.search(text)
            verdict = KILLED if stopped else f"ERROR {proc.returncode}"
    return (wall, *run.numbers(text), verdict)


def measure(args, base_test, head_test, b=None):
    """Runs a test, as test_files gives it for each tree, with both binaries
    side by side and returns the base and head results, as finish gives
    them. Given b, the base's result from elsewhere, runs head alone."""
    with tempfile.TemporaryDirectory(prefix="vereda-bench-") as tmp, \
         (contextlib.nullcontext() if b else
          started(args.base_root, args.base, args.clang, base_test, tmp + "/base",
                  args.memory_limit)) as base, \
         started(ROOT, args.head, args.clang, head_test, tmp + "/head",
                 args.memory_limit) as head:
        b = b or finish(base, base[2] + args.timeout)
        limit = args.timeout if b[4] in STOPPED else max(SLOWDOWN * b[0], MIN_LIMIT)
        h = finish(head, head[2] + limit)
    return b, h


def regressions(b, h):
    """Why the head result h is worse than the base result b, or []."""
    if h[4] in STOPPED:
        return [] if b[4] in STOPPED else [STOPPED[h[4]]]
    # A run without a verdict crashed or could not start; when both did,
    # the setup is broken, and that must not pass as no regression.
    if h[4].startswith("ERROR"):
        return ["head gave no verdict"]
    if b[4] in STOPPED:
        return []
    why = []
    if h[0] > RATIO * b[0] and h[0] - b[0] >= MIN_SECONDS:
        why.append(f"wall time x{h[0] / b[0]:.1f}")
    if b[3] and h[3] and h[3] > RATIO * b[3] and h[3] - b[3] >= MIN_MB:
        why.append(f"memory x{h[3] / b[3]:.1f}")
    if b[4] != h[4]:
        why.append("verdict differs")
    return why


def improvements(b, h):
    """Why the head result h is better than the base result b, or []."""
    if b[4] in STOPPED:
        return [] if h[4] in STOPPED or h[4].startswith("ERROR") else [f"base {STOPPED[b[4]][5:]}"]
    if h[4] in STOPPED or h[4].startswith("ERROR") or b[4] != h[4]:
        return []
    why = []
    if b[0] > RATIO * h[0] and b[0] - h[0] >= MIN_SECONDS:
        why.append(f"wall time x{h[0] / b[0]:.2f}")
    if b[3] and h[3] and b[3] > RATIO * h[3] and b[3] - h[3] >= MIN_MB:
        why.append(f"memory x{h[3] / b[3]:.2f}")
    return why


def load(stats):
    """The results run.py recorded in the directory stats, by test name, as
    finish gives them."""
    runs = {}
    for path in glob.glob(os.path.join(stats, "*.json")):
        with open(path, encoding="utf-8") as f:
            r = json.load(f)
        runs[r["test"]] = (r["wall"], r["walk"], r["solve"], r["peak"],
                           KILLED if r["verdict"] == "MEMOUT" else r["verdict"])
    return runs


def candidates(base_dir, head_dir):
    """The tests whose recorded numbers changed between two suite runs,
    largest relative change in wall time or memory first, at most
    MAX_CANDIDATES of them."""
    base, head = load(base_dir), load(head_dir)
    found = []
    for name in base.keys() & head.keys():
        b, h = base[name], head[name]
        if b[4] == h[4] and not b[4].startswith(("SUCCESSFUL", "FAILED", "UNKNOWN")) \
                and b[4] not in STOPPED:
            continue  # no verdict on either side: a test of an error, not of speed
        if b[4] != h[4] and b[4] not in STOPPED and h[4] not in STOPPED:
            continue  # a changed verdict is tests.yml's to judge, often on purpose
        if regressions(b, h) or improvements(b, h):
            pairs = ((h[0], b[0]), (b[0], h[0]), (h[3], b[3]), (b[3], h[3]))
            ratio = max(((x or 0) + 0.1) / ((y or 0) + 0.1) for x, y in pairs)
            found.append((ratio, name))
    return [name for _, name in sorted(found, reverse=True)[:MAX_CANDIDATES]]


def cell(b, h, fmt):
    """A base -> head table cell; a missing number shows as '-'."""
    def show(x):
        return "-" if x is None else fmt.format(x)
    return f"{show(b)} → {show(h)}"


def main():  # pylint: disable=too-many-locals
    """Runs the benchmark and returns 1 when some test regressed."""
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--clang", required=True, help="path to clang")
    parser.add_argument("--base-root", default=ROOT,
                        help="source tree whose vereda.py and tests the base binary runs "
                        "(default: this one); a test missing there is skipped")
    parser.add_argument("--list", help="tests, one suite/name per line; # starts a comment")
    parser.add_argument("--stats", nargs=2, metavar=("BASE", "HEAD"),
                        help="pick the tests from two suite runs' $REGRESS_STATS directories")
    parser.add_argument("--timeout", type=float, default=180,
                        help="seconds before a base run is stopped (default %(default)s)")
    parser.add_argument("--memory-limit", type=int, default=4096,
                        help="MB of address space for each run, as the suite's "
                        "REGRESS_MEMORY_LIMIT (default %(default)s)")
    parser.add_argument("--changes", metavar="FILE",
                        help="write a table of only the tests that regressed or improved")
    parser.add_argument("base", help="base vereda binary")
    parser.add_argument("head", help="head vereda binary")
    args = parser.parse_args()
    if not args.list and not args.stats:
        parser.error("give --list or --stats")
    # vereda.py runs in a temporary directory.
    args.base_root, args.base, args.head, args.clang = map(
        os.path.abspath, (args.base_root, args.base, args.head, args.clang))
    suite = {}
    if args.stats:
        # A base that ran out of time or memory in its suite run would only
        # do so again here, at the cost of the whole limit.
        suite = {n: r for n, r in load(args.stats[0]).items() if r[4] in STOPPED}
        names = candidates(*args.stats)
        print(f"{len(names)} tests changed between the suite runs", flush=True)
    else:
        with open(args.list, encoding="utf-8") as f:
            names = [n for n in (line.split("#")[0].strip() for line in f) if n]

    lines = ["| test | walk s | solve s | wall s | peak MB | verdict | change |",
             "|---|---|---|---|---|---|---|"]
    print("\n".join(lines), flush=True)
    failed, changed = [], []
    for name in names:
        head_test = test_files(ROOT, name)
        if head_test is None:
            sys.exit(f"bench.py: no test {name}")
        base_test = test_files(args.base_root, name)
        if base_test is None:
            lines.append(f"| {name} | | | | | | not in base; skipped |")
            print(lines[-1], flush=True)
            continue
        b, h = measure(args, base_test, head_test, suite.get(name))
        why, better = regressions(b, h), improvements(b, h)
        if (why or better) and b[4] == h[4] and not h[4].startswith("ERROR"):
            b, h = measure(args, base_test, head_test, suite.get(name))
            why, better = regressions(b, h), improvements(b, h)
        if why:
            failed.append(name)
        verdict = b[4] if b[4] == h[4] else f"{b[4]} → {h[4]}"
        if name in suite:
            verdict = f"{b[4]} in the suite run → {h[4]}"
        row = (f"| {name} | {cell(b[1], h[1], '{:.2f}')} | {cell(b[2], h[2], '{:.2f}')} "
               f"| {cell(b[0], h[0], '{:.2f}')} | {cell(b[3], h[3], '{}')} "
               f"| {verdict} "
               f"| {'**' + ', '.join(why) + '**' if why else ', '.join(better)} |")
        if why or better:
            changed.append(row)
        lines.append(row)
        print(row, flush=True)
    report(lines, failed, changed, args.changes)
    return 1 if failed else 0


def report(lines, failed, changed, changes):
    """Ends the table with the regressed tests, writes the step summary, and
    writes the changed rows to the file changes when some test changed."""
    lines.append("")
    lines.append(f"Regressed: {', '.join(failed)}" if failed else "No regressions.")
    print(lines[-1])
    if changes and changed:
        with open(changes, "w", encoding="utf-8") as f:
            f.write("\n".join(lines[:2] + changed) + "\n")
    summary = os.environ.get("GITHUB_STEP_SUMMARY")
    if summary:
        with open(summary, "a", encoding="utf-8") as f:
            f.write("## Performance: base → head\n\n" + "\n".join(lines) + "\n")


if __name__ == "__main__":
    sys.exit(main())
