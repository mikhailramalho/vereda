#!/usr/bin/env python3
"""Exercise independent RUN executions and new-fixture performance comparisons."""
import contextlib
import io
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

import bench
import run


FRONTEND = '''import json, os, sys, time
from pathlib import Path
with open(os.environ["CONFIG_TEST_LOG"], "a") as log:
    log.write(json.dumps(dict(args=sys.argv[1:], cwd=os.getcwd())) + "\\n")
if "--slow" in sys.argv:
    time.sleep(2)
if "--oom-verdict" in sys.argv:
    print("VERIFICATION SUCCESSFUL")
    print("out of memory")
    sys.exit(6)
if "--oom" in sys.argv:
    print("out of memory")
    sys.exit(6)
if "--unsupported" in sys.argv or ("--base-unsupported" in sys.argv and
                                  Path(sys.argv[sys.argv.index("--bin") + 1]).name == "base-binary"):
    sys.exit(6)
print("[INFO] Walk time: 0.001s (1 check, 1 symbols, peak memory 20 MB)")
print("[INFO] Solving time: 0.002s (peak memory 30 MB)")
print("VERIFICATION FAILED" if "--fail" in sys.argv else "VERIFICATION SUCCESSFUL")
'''


class Configurations(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.log = self.root / "calls.jsonl"
        self.stats = self.root / "stats"
        self.frontend = self.root / "vereda.py"
        self.frontend.write_text(FRONTEND)
        self.test = self.root / "tests" / "suite" / "sample.c"
        self.test.parent.mkdir(parents=True)
        self.env = patch.dict(os.environ, CONFIG_TEST_LOG=str(self.log),
                              REGRESS_STATS=str(self.stats))
        self.env.start()
        self.addCleanup(self.env.stop)

    def execute(self, *extra):
        return subprocess.run([sys.executable, str(Path(run.__file__).resolve()),
                               "--vereda", str(self.frontend), "--bin", "unused",
                               "--clang", "unused", "--timeout", "0.5", *extra,
                               str(self.test)], capture_output=True, text=True, check=False)

    def calls(self):
        return [json.loads(line) for line in self.log.read_text().splitlines()]

    def test_parse_defaults_and_ir_runs(self):
        self.test.write_text("int main(void) {}\n")
        self.assertEqual(run.parse(self.test)["RUN"], [""])
        self.test.write_text("; RUN:\n; RUN: --incremental-solver\n; EXPECT: shared\n")
        desc = run.parse(self.test)
        self.assertEqual(desc["RUN"], ["", "--incremental-solver"])
        self.assertEqual(desc["EXPECT"], ["shared"])
        self.assertEqual(run.split_run("suite/file.with.dots@run12"),
                         ("suite/file.with.dots", 12))

    def test_stats_wall_stops_before_output_analysis(self):
        self.test.write_text("// EXPECT: ^VERIFICATION SUCCESSFUL$\n")
        output = "VERIFICATION SUCCESSFUL\n"
        clock = [100.0]

        def execute(*args):
            clock[0] += 2.0
            return output

        def memory_check(out):
            clock[0] += 10.0
            return False

        argv = [run.__file__, "--vereda", "unused", "--bin", "unused",
                "--clang", "unused", "--memory-limit", "4096", str(self.test)]
        with patch.object(sys, "argv", argv), \
             patch.object(run, "execute", side_effect=execute), \
             patch.object(run.time, "monotonic", side_effect=lambda: clock[0]), \
             patch.object(run, "out_of_memory", side_effect=memory_check), \
             contextlib.redirect_stdout(io.StringIO()):
            self.assertEqual(run.main(), 0)
        stat = json.loads((self.stats / "suite__sample.json").read_text())
        self.assertEqual(stat["wall"], 2.0)
        self.assertEqual(stat["verdict"], "SUCCESSFUL")

    def test_memory_error_overrides_printed_verdict(self):
        self.test.write_text("// RUN: --oom-verdict\n// EXPECT: ^VERIFICATION SUCCESSFUL$\n")
        result = self.execute("--memory-limit", "4096")
        self.assertEqual(result.returncode, run.OUT_OF_MEMORY, result.stdout + result.stderr)
        stat = json.loads((self.stats / "suite__sample.json").read_text())
        self.assertEqual(stat["verdict"], "MEMOUT")

    def test_near_memory_limit_retains_verdict_prefix_rules(self):
        self.test.write_text("// EXPECT: tail\n")
        argv = [run.__file__, "--vereda", "unused", "--bin", "unused",
                "--clang", "unused", "--memory-limit", "4096", str(self.test)]
        for prefix, status in (("", 0), ("prefix\n", 0),
                               ("prefix ", run.OUT_OF_MEMORY), ("\r", run.OUT_OF_MEMORY)):
            # The memory heuristic accepts any anchored VERIFICATION prefix,
            # even when the status is malformed and verdict_of returns none.
            with self.subTest(prefix=prefix), patch.object(sys, "argv", argv), \
                 patch.object(run, "execute", return_value=prefix + "VERIFICATION INVALID\ntail\n"), \
                 patch.object(run.resource, "getrusage") as usage, \
                 contextlib.redirect_stdout(io.StringIO()):
                usage.return_value.ru_maxrss = 4096 << 10
                self.assertEqual(run.main(), status)

    def test_all_runs_have_independent_commands_stats_and_cwd(self):
        self.test.write_text("// RUN:\n// RUN: --incremental-solver\n"
                             "// CWD: %S\n// EXPECT: ^VERIFICATION SUCCESSFUL$\n")
        result = self.execute()
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        calls = self.calls()
        self.assertEqual(len(calls), 2)
        self.assertNotIn("--incremental-solver", calls[0]["args"])
        self.assertIn("--incremental-solver", calls[1]["args"])
        self.assertTrue(all(c["cwd"] == str(self.test.parent) for c in calls))
        for suffix in ("", "@run2"):
            stat = json.loads((self.stats / f"suite__sample{suffix}.json").read_text())
            self.assertEqual(stat["test"], f"suite/sample{suffix}")
            self.assertEqual(stat["verdict"], "SUCCESSFUL")
            self.assertEqual((stat["walk"], stat["solve"], stat["peak"]), (0.001, 0.002, 30))

    def test_later_success_does_not_hide_failure(self):
        self.test.write_text("// RUN: --fail\n// RUN:\n"
                             "// EXPECT: ^VERIFICATION SUCCESSFUL$\n")
        result = self.execute()
        self.assertEqual(result.returncode, 1, result.stdout)
        self.assertEqual(len(self.calls()), 2)
        self.assertIn("unmet EXPECT", result.stdout)

    def test_select_run_and_reject_invalid_index(self):
        self.test.write_text("// RUN: --fail\n// RUN:\n"
                             "// EXPECT: ^VERIFICATION SUCCESSFUL$\n")
        self.assertEqual(self.execute("--run-index", "2").returncode, 0)
        self.assertEqual(len(self.calls()), 1)
        for index in ("0", "3"):
            self.assertEqual(self.execute("--run-index", index).returncode, 2)

    def test_timeout_is_recorded_without_losing_next_run(self):
        self.test.write_text("// RUN: --slow\n// RUN:\n"
                             "// EXPECT: ^VERIFICATION SUCCESSFUL$\n")
        result = self.execute()
        self.assertEqual(result.returncode, run.SKIPPED, result.stdout)
        self.assertEqual(len(self.calls()), 2)
        first = json.loads((self.stats / "suite__sample.json").read_text())
        second = json.loads((self.stats / "suite__sample@run2.json").read_text())
        self.assertEqual(first["verdict"], "TIMEOUT")
        self.assertIsNone(first["wall"])
        self.assertEqual(second["verdict"], "SUCCESSFUL")

    def test_knownbug_promotion_is_checked_for_each_run(self):
        self.test.write_text("// RUN: --fail\n// RUN:\n// MODE: KNOWNBUG\n"
                             "// EXPECT: ^VERIFICATION SUCCESSFUL$\n")
        result = self.execute()
        self.assertEqual(result.returncode, 1, result.stdout)
        self.assertIn("passed unexpectedly", result.stdout)

    def test_memory_failure_takes_precedence_for_marking(self):
        self.test.write_text("// RUN: --fail\n// RUN: --oom\n"
                             "// EXPECT: ^VERIFICATION SUCCESSFUL$\n")
        result = self.execute("--memory-limit", "512")
        self.assertEqual(result.returncode, run.OUT_OF_MEMORY, result.stdout)
        stat = json.loads((self.stats / "suite__sample@run2.json").read_text())
        self.assertEqual(stat["verdict"], "MEMOUT")

    def test_candidates_include_new_tests_and_configurations(self):
        base = self.root / "base-stats"
        base.mkdir()
        self.stats.mkdir()
        def write(directory, name, wall=1):
            (directory / (name.replace("/", "__") + ".json")).write_text(json.dumps(
                dict(test=name, wall=wall, walk=0.1, solve=0.1, peak=30, verdict="SUCCESSFUL")))
        write(base, "suite/unchanged")
        write(self.stats, "suite/unchanged")
        write(self.stats, "suite/unchanged@run2")
        write(self.stats, "suite/new")
        write(base, "suite/slower")
        write(self.stats, "suite/slower", wall=4)
        self.assertEqual(set(bench.candidates(base, self.stats)),
                         {"suite/new", "suite/unchanged@run2", "suite/slower"})

    def benchmark(self, name):
        base = self.root / "base"
        base.mkdir(exist_ok=True)
        (base / "vereda.py").write_text(FRONTEND)
        names = self.root / "list.txt"
        names.write_text(name + "\n")
        argv = ["bench.py", "--clang", "unused", "--base-root", str(base),
                "--list", str(names), "base-binary", "head-binary"]
        output = io.StringIO()
        with patch.object(bench, "ROOT", str(self.root)), patch.object(sys, "argv", argv), \
             contextlib.redirect_stdout(output):
            code = bench.main()
        return code, output.getvalue()

    def test_bulk_import_does_not_crowd_out_regressions(self):
        def result(wall):
            return (wall, 0.1, 0.1, 30, "SUCCESSFUL")
        base = {"suite/regressed": result(1), "suite/improved": result(4)}
        head = {"suite/regressed": result(4), "suite/improved": result(1)}
        head.update({f"suite/new{i}": result(1) for i in range(bench.MAX_CANDIDATES + 1)})
        with patch.object(bench, "load", side_effect=[base, head]):
            selected = bench.candidates("base", "head")
        self.assertEqual(len(selected), bench.MAX_CANDIDATES)
        self.assertEqual(selected[0], "suite/regressed")
        self.assertNotIn("suite/improved", selected)

    def test_new_fixture_uses_head_sources_flags_and_cwd_with_base_binary(self):
        self.test.write_text("// RUN:\n// RUN: --config %S/input\n"
                             "// FILES: helper.c\n// CWD: %S\n")
        helper = self.test.parent / "helper.c"
        helper.write_text("void helper(void) {}\n")
        code, output = self.benchmark("suite/sample@run2")
        self.assertEqual(code, 0, output)
        self.assertIn("base uses head fixture", output)
        self.assertNotIn("skipped", output)
        calls = self.calls()
        self.assertEqual(len(calls), 2)
        for call in calls:
            self.assertIn(str(self.test), call["args"])
            self.assertIn(str(helper), call["args"])
            self.assertIn(str(self.test.parent / "input"), call["args"])
            self.assertEqual(call["cwd"], str(self.test.parent))
        binaries = {Path(call["args"][call["args"].index("--bin") + 1]).name for call in calls}
        self.assertEqual(binaries, {"base-binary", "head-binary"})

    def test_added_run_uses_head_fixture_when_source_exists_in_base(self):
        base_suite = self.root / "base" / "tests" / "suite"
        base_suite.mkdir(parents=True)
        (base_suite / "sample.c").write_text("// RUN:\n")
        self.test.write_text("// RUN:\n// RUN: --incremental-solver\n")
        code, output = self.benchmark("suite/sample@run2")
        self.assertEqual(code, 0, output)
        self.assertIn("base uses head fixture", output)
        self.assertTrue(all("--incremental-solver" in c["args"] for c in self.calls()))

    def test_new_fixture_errors_are_reported(self):
        self.test.write_text("// RUN: --unsupported\n")
        code, output = self.benchmark("suite/sample")
        self.assertEqual(code, 1, output)
        self.assertIn("head gave no verdict", output)

    def test_base_rejecting_new_fixture_is_reported(self):
        self.test.write_text("// RUN: --base-unsupported\n")
        code, output = self.benchmark("suite/sample")
        self.assertEqual(code, 0, output)
        self.assertIn("ERROR 6 → SUCCESSFUL", output)
        self.assertIn("verdict differs; performance not compared", output)

    def test_regressions_and_improvements_for_error_outcomes(self):
        err6 = (0.1, None, None, None, "ERROR 6")
        err1 = (0.1, None, None, None, "ERROR 1")
        ok = (0.5, 0.1, 0.2, 30, "SUCCESSFUL")
        self.assertEqual(bench.regressions(err6, err6, mode="FUTURE"), [])
        self.assertEqual(bench.improvements(err6, err6, mode="FUTURE"), [])
        self.assertEqual(bench.regressions(ok, err6, mode="FUTURE"), ["head gave no verdict"])
        self.assertEqual(bench.regressions(err6, err1, mode="FUTURE"), ["head gave no verdict"])
        for mode in ("CORE", "THOROUGH", "KNOWNBUG"):
            with self.subTest(mode=mode):
                self.assertEqual(bench.regressions(err6, err6, mode=mode),
                                 ["head gave no verdict"])
        self.assertEqual(bench.regressions(err6, err6), ["head gave no verdict"])
        self.assertEqual(bench.regressions(err6, ok, mode="FUTURE"), [])
        self.assertEqual(bench.improvements(err6, ok, mode="FUTURE"), ["base gave no verdict"])

    def test_future_identical_errors_do_not_fail_benchmark(self):
        base_suite = self.root / "base" / "tests" / "suite"
        base_suite.mkdir(parents=True)
        content = "// RUN: --unsupported\n// MODE: FUTURE\n"
        (base_suite / "sample.c").write_text(content)
        self.test.write_text(content)
        code, output = self.benchmark("suite/sample")
        self.assertEqual(code, 0, output)
        self.assertIn("No regressions.", output)
        self.assertNotIn("head gave no verdict", output)

    def test_future_error_to_verdict_is_reported_as_improvement(self):
        base_suite = self.root / "base" / "tests" / "suite"
        base_suite.mkdir(parents=True)
        content = "// RUN: --base-unsupported\n// MODE: FUTURE\n"
        (base_suite / "sample.c").write_text(content)
        self.test.write_text(content)
        code, output = self.benchmark("suite/sample")
        self.assertEqual(code, 0, output)
        self.assertIn("ERROR 6 → SUCCESSFUL", output)
        self.assertIn("base gave no verdict", output)

    def test_knownbug_identical_errors_still_fail_benchmark(self):
        base_suite = self.root / "base" / "tests" / "suite"
        base_suite.mkdir(parents=True)
        content = "// RUN: --unsupported\n// MODE: KNOWNBUG\n"
        (base_suite / "sample.c").write_text(content)
        self.test.write_text(content)
        code, output = self.benchmark("suite/sample")
        self.assertEqual(code, 1, output)
        self.assertIn("head gave no verdict", output)

    def test_cmake_registers_runs_and_reconfigures_when_headers_change(self):
        # Exercise the real registration code with a tiny source-only project.
        (self.root / "CMakeLists.txt").write_text(
            "cmake_minimum_required(VERSION 3.20)\nproject(RunnerTests NONE)\n"
            "enable_testing()\nadd_executable(vereda IMPORTED)\n"
            'set_target_properties(vereda PROPERTIES IMPORTED_LOCATION "/unused/vereda")\n'
            "add_subdirectory(tests)\n")
        cmake = Path(__file__).with_name("CMakeLists.txt").read_text()
        # Mirror #249: no table source, libvereda or C++ compiler in this fixture.
        cmake = cmake.replace("  add_test(NAME vereda/banner",
                              "  add_executable(vereda-table-patterns table_patterns.cpp)\n"
                              "  target_link_libraries(vereda-table-patterns PRIVATE libvereda)\n"
                              "  add_test(NAME vereda/banner")
        self.assertIn("add_executable(vereda-table-patterns", cmake)
        (self.root / "tests" / "CMakeLists.txt").write_text(cmake)
        shutil.copy(Path(run.__file__), self.root / "tests" / "run.py")
        self.test.write_text("// RUN:\n// RUN: --incremental-solver\n")
        ir = self.test.with_name("ir.ll")
        ir.write_text("; RUN:\n; RUN: --unwind 2\n")
        build = self.root / "build"
        def command(*args):
            result = subprocess.run(args, capture_output=True, text=True, check=False, timeout=15)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            return result.stdout
        def registered():
            data = json.loads(command("ctest", "--test-dir", str(build), "--show-only=json-v1"))
            return {t["name"]: t for t in data["tests"] if t["name"].startswith("suite/")}
        command("cmake", "-S", str(self.root), "-B", str(build), "-G", "Ninja")
        cases = registered()
        self.assertEqual(set(cases), {"suite/sample", "suite/sample@run2", "suite/ir", "suite/ir@run2"})
        self.assertIn("--run-index", cases["suite/sample@run2"]["command"])
        self.test.write_text(self.test.read_text() + "// RUN: --unwind 3\n")
        command("cmake", "--build", str(build))
        self.assertIn("suite/sample@run3", registered())


if __name__ == "__main__":
    unittest.main()
