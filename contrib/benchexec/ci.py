#!/usr/bin/env python3
"""Helpers for .github/workflows/benchexec.yml.

  ci.py dirs SVB CATEGORY...
      the sparse-checkout patterns of the sv-benchmarks directories the
      categories' set files name
  ci.py plan SVB TIMEOUT FILTER CATEGORY...
      splits each category's unreach-call tasks whose path (under c/) the
      regular expression FILTER matches into shards, writes each shard's
      tasks to SVB/c/vereda.<shard>.set and prints the shard names as JSON
  ci.py summary NEW [OLD]
      Markdown summary of the BenchExec results under NEW, and the tasks
      gained and lost against those under OLD; exits 1 on a wrong verdict
"""
import bz2
import collections
import glob
import json
import math
import os
import re
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
# vereda.xml's cpuCores), and at most 4.5 hours a shard even if every task
# runs to its time limit, within a job's 6.
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


def plan(svb, timeout, task_filter, categories):
    c = os.path.join(svb, "c")
    shards = []
    for category in categories:
        tasks = sorted(set(reach_tasks(svb, category, task_filter)))
        n = math.ceil(len(tasks) * timeout / PARALLEL / BUDGET)
        print(f"{category}: {len(tasks)} tasks, {n} shards", file=sys.stderr)
        for i in range(n):
            name = category if n == 1 else f"{category}.{i + 1}"
            with open(os.path.join(c, f"vereda.{name}.set"), "w", encoding="utf-8") as f:
                f.write("\n".join(tasks[i::n]) + "\n")
            shards.append(name)
    if not shards:
        sys.exit("no tasks to run")
    print(json.dumps(shards))


Run = collections.namedtuple("Run", "set category status expected cputime")


def load(directory):
    """{task: Run} for the result files under directory. A task is its path
    under sv-benchmarks' c/, its set the category of the shard it ran in."""
    runs = {}
    for path in glob.glob(os.path.join(directory, "**", "*.results.*.xml.bz2"), recursive=True):
        name = os.path.basename(path).split(".")[1]
        with bz2.open(path) as f:
            for run in ET.parse(f).getroot().iter("run"):
                columns = {c.get("title"): c.get("value") for c in run.iter("column")}
                task = re.sub(r"^.*?/c/", "", run.get("name"))
                runs[task] = Run(name, columns.get("category"), columns.get("status"),
                                 run.get("expectedVerdict"),
                                 float(columns.get("cputime", "0s").rstrip("s")))
    return runs


def kind(run):
    if run.category in ("correct", "wrong", "unknown"):
        return run.category
    # TIMEOUT, or TIMEOUT (true) when the tool printed a verdict too late.
    return "timeout" if (run.status or "").startswith("TIMEOUT") else "other"


def summary(new_dir, old_dir=None):
    new = load(new_dir)
    kinds = ["correct", "wrong", "unknown", "timeout", "other"]
    print("| Set | Tasks | " + " | ".join(k.capitalize() for k in kinds) + " | CPU time |")
    print("|---" * (len(kinds) + 3) + "|")
    for category in sorted({r.set for r in new.values()}) + ["All"]:
        runs = [r for r in new.values() if category in ("All", r.set)]
        counts = collections.Counter(kind(r) for r in runs)
        cputime = sum(r.cputime for r in runs) / 60
        print(f"| {category} | {len(runs)} | " + " | ".join(str(counts[k]) for k in kinds)
              + f" | {cputime:.1f} min |")
    print("\nOther is an error or out of memory.")

    wrong = sorted(t for t, r in new.items() if r.category == "wrong")
    print(f"\n### Wrong verdicts: {len(wrong)}\n")
    for t in wrong:
        print(f"- `{t}`: expected {new[t].expected}, got {new[t].status}")

    if old_dir:
        compare(new, load(old_dir))
    return 1 if wrong else 0


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
        plan(path, float(rest[0]), rest[1], rest[2:])
    elif command == "summary":
        return summary(path, *rest)
    else:
        sys.exit(__doc__)
    return 0


if __name__ == "__main__":
    sys.exit(main())
