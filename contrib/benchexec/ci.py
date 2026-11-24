#!/usr/bin/env python3
"""Helpers for .github/workflows/benchexec.yml.

  ci.py dirs SVB CATEGORY...
      the sparse-checkout patterns of the sv-benchmarks directories the
      categories' set files name
  ci.py plan SVB TIMEOUT FILTER TOOLS CATEGORY...
      splits each category's unreach-call tasks whose path (under c/) the
      regular expression FILTER matches into shards, each of which runs
      every tool of the space-separated TOOLS in turn, writes each shard's
      tasks to SVB/c/<tool>.<shard>.set for each tool and prints the shard
      names as JSON
  ci.py summary NEW [OLD]
      Markdown summary of the BenchExec results under NEW, per tool, the
      tools compared on the tasks they all answer correctly, and Vereda's
      tasks gained and lost against those under OLD; exits 1 on a wrong
      verdict of Vereda's
"""
import bz2
import collections
import glob
import json
import math
import os
import re
import statistics
import sys
import xml.etree.ElementTree as ET

import yaml

# SV-COMP's ReachSafety categories made of several set files, as ESBMC's
# scripts/competitions/svcomp/esbmc.xml defines them; any other category is
# the set file of its name.
CATEGORIES = {
    "Heap": ["Heap", "LinkedLists"],
    "Loops": ["Loops", "VerifyThis-Loops"],
    "Recursive": ["Recursive", "VerifyThis-Recursive"],
}
# Two tasks at a time on a runner's 4 cores (benchexec.yml's -N 2 and
# vereda.xml's cpuCores), and at most 4.5 hours a shard even if every tool
# runs every task to its time limit, within a job's 6.
PARALLEL = 2
BUDGET = 4.5 * 3600


def patterns(svb, category):
    for name in CATEGORIES.get(category, [category]):
        with open(os.path.join(svb, "c", name + ".set"), encoding="utf-8") as f:
            for line in f:
                line = line.strip()
                if line and not line.startswith("#"):
                    yield line


def dirs(svb, categories):
    names = {p.split("/")[0] for c in categories for p in patterns(svb, c)}
    print(" ".join(f"/c/{name}/" for name in sorted(names)))


def reach_tasks(svb, category, task_filter):
    c = os.path.join(svb, "c")
    for pattern in patterns(svb, category):
        for path in glob.glob(os.path.join(c, pattern)):
            task = os.path.relpath(path, c)
            if re.search(task_filter, task):
                with open(path, encoding="utf-8") as f:
                    properties = yaml.safe_load(f).get("properties", [])
                if any(p["property_file"].endswith("/unreach-call.prp") for p in properties):
                    yield task


def plan(svb, timeout, task_filter, tools, categories):
    c = os.path.join(svb, "c")
    shards = []
    for category in categories:
        tasks = sorted(set(reach_tasks(svb, category, task_filter)))
        n = math.ceil(len(tasks) * timeout * len(tools) / PARALLEL / BUDGET)
        print(f"{category}: {len(tasks)} tasks, {n} shards", file=sys.stderr)
        for i in range(n):
            name = category if n == 1 else f"{category}.{i + 1}"
            # A run definition's set file is named after its benchmark.
            for tool in tools:
                with open(os.path.join(c, f"{tool}.{name}.set"), "w", encoding="utf-8") as f:
                    f.write("\n".join(tasks[i::n]) + "\n")
            shards.append(name)
    if not shards:
        sys.exit("no tasks to run")
    print(json.dumps(shards))


Run = collections.namedtuple("Run", "set category status expected cputime memory")


def load(directory):
    """{tool: {task: Run}} for the result files under directory, the tool as
    BenchExec names it (Vereda, ESBMC). A task is its path under
    sv-benchmarks' c/, its set the category of the shard it ran in."""
    runs = collections.defaultdict(dict)
    for path in glob.glob(os.path.join(directory, "**", "*.results.*.xml.bz2"), recursive=True):
        name = os.path.basename(path).split(".")[1]
        with bz2.open(path) as f:
            root = ET.parse(f).getroot()
        for run in root.iter("run"):
            columns = {c.get("title"): c.get("value") for c in run.iter("column")}
            task = re.sub(r"^.*?/c/", "", run.get("name"))
            runs[root.get("tool")][task] = Run(
                name, columns.get("category"), columns.get("status"), run.get("expectedVerdict"),
                float(columns.get("cputime", "0s").rstrip("s")),
                int(columns.get("memory", "0B").rstrip("B")))
    return runs


# SV-COMP's score for each kind of answer.
SCORE = {"Correct TRUE": 2, "Correct FALSE": 1, "Wrong TRUE": -32, "Wrong FALSE": -16}
KINDS = list(SCORE) + ["Unknown", "Timeout", "Out of memory", "Other"]


def kind(run):
    status = run.status or ""
    if run.category in ("correct", "wrong"):
        return run.category.capitalize() + (" TRUE" if status == "true" else " FALSE")
    if run.category == "unknown":
        return "Unknown"
    # TIMEOUT, or TIMEOUT (true) when the tool printed a verdict too late.
    if status.startswith("TIMEOUT"):
        return "Timeout"
    return "Out of memory" if status.startswith("OUT OF MEMORY") else "Other"


def summary(new_dir, old_dir=None):
    new = load(new_dir)
    tools = sorted(new, key=lambda tool: tool != "Vereda")
    sets = sorted({r.set for runs in new.values() for r in runs.values()}) + ["All"]
    print("| Set | Tool | Tasks | " + " | ".join(KINDS) + " | Score | CPU time |")
    print("|---" * (len(KINDS) + 5) + "|")
    for category in sets:
        for tool in tools:
            runs = [r for r in new[tool].values() if category in ("All", r.set)]
            counts = collections.Counter(kind(r) for r in runs)
            score = sum(SCORE.get(k, 0) * n for k, n in counts.items())
            cputime = sum(r.cputime for r in runs) / 60
            print(f"| {category} | {tool} | {len(runs)} | "
                  + " | ".join(str(counts[k]) for k in KINDS) + f" | {score} | {cputime:.1f} min |")
    print("\nOther is an error.")
    if len(tools) == 2:
        both_correct(new, *tools, sets)

    print("\n### Wrong verdicts")
    for tool in tools:
        wrong = sorted(t for t, r in new[tool].items() if r.category == "wrong")
        print(f"\n{tool}: {len(wrong)}\n")
        for t in wrong:
            print(f"- `{t}`: expected {new[tool][t].expected}, got {new[tool][t].status}")

    if old_dir:
        compare(new["Vereda"], load(old_dir)["Vereda"])
    # ESBMC's wrong verdicts are listed, but only Vereda's fail the run.
    return 1 if any(r.category == "wrong" for r in new["Vereda"].values()) else 0


def both_correct(new, a, b, sets):
    """CPU time and peak memory of tools a and b on the tasks both answer
    correctly, each cell a's / b's."""
    def both(pairs, f, fmt):
        return " / ".join(fmt.format(f(column)) for column in zip(*pairs))

    def wins(pairs):
        return f"{sum(x < y for x, y in pairs)} / {sum(y < x for x, y in pairs)}"

    print(f"\n### On the tasks both answer correctly: {a} / {b}\n")
    print("| Set | Tasks | CPU time | Median CPU time | Faster on "
          "| Peak memory | Median peak memory | Less memory on |")
    print("|---" * 8 + "|")
    for category in sets:
        runs = [(r, new[b][t]) for t, r in new[a].items()
                if category in ("All", r.set) and r.category == "correct"
                and t in new[b] and new[b][t].category == "correct"]
        if not runs:
            continue
        cpu = [(x.cputime, y.cputime) for x, y in runs]
        memory = [(x.memory, y.memory) for x, y in runs]
        print(f"| {category} | {len(runs)} | {both(cpu, sum, '{:.0f} s')} "
              f"| {both(cpu, statistics.median, '{:.2f} s')} | {wins(cpu)} "
              f"| {both(memory, lambda m: sum(m) / 1e9, '{:.1f} GB')} "
              f"| {both(memory, lambda m: statistics.median(m) / 1e6, '{:.0f} MB')} "
              f"| {wins(memory)} |")


def compare(new, old):
    common = new.keys() & old.keys()
    print(f"\n### Against the earlier run ({len(common)} tasks in both)")
    now = {t for t in common if new[t].category == "correct"}
    before = {t for t in common if old[t].category == "correct"}
    for title, changed in (("Gained", now - before), ("Lost", before - now)):
        print(f"\n{title}: {len(changed)}\n")
        for t in sorted(changed):
            print(f"- `{t}`: {old[t].status} → {new[t].status}")


def main():
    command, path, *rest = sys.argv[1:]
    if command == "dirs":
        dirs(path, rest)
    elif command == "plan":
        plan(path, float(rest[0]), rest[1], rest[2].split(), rest[3:])
    elif command == "summary":
        return summary(path, *rest)
    else:
        sys.exit(__doc__)
    return 0


if __name__ == "__main__":
    sys.exit(main())
