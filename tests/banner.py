#!/usr/bin/env python3
"""The executable's banner comes before compiler output, exactly once."""
from pathlib import Path
import re
import subprocess
import sys
import tempfile

frontend, binary, clang = sys.argv[1:]
version = subprocess.check_output([binary, "--version"], text=True).strip()
assert re.fullmatch(r"Vereda \S+ \(LLVM \S+, camada \S+, commit (?:[0-9a-f]{8,40}|unknown)\)",
                    version), version


def check(command, status):
    result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                            text=True, check=False, timeout=30)
    assert result.returncode == status, result.stdout
    lines = result.stdout.splitlines()
    assert lines[0] == version, result.stdout
    assert sum(line.startswith("Vereda ") for line in lines) == 1, result.stdout
    return result.stdout


with tempfile.TemporaryDirectory() as tmp:
    tmp = Path(tmp)
    source = tmp / "main.c"
    helper = tmp / "helper.c"
    source.write_text("extern int helper(void);\n"
                      "int main(void) { __VERIFIER_assume(helper()); return 0; }\n")
    helper.write_text("int helper(void) { return 1; }\n")
    command = [sys.executable, frontend, "--bin", binary, "--clang", clang]
    for flags in ([], ["--verbose"]):
        output = check(command + flags + [str(source), str(helper)], 0)
        assert "warning:" in output and "Linking 2 modules" in output, output
    source.write_text("int main( {\n")
    check(command + [str(source)], 6)
    ir = tmp / "main.ll"
    ir.write_text("define i32 @main() { ret i32 0 }\n")
    check(command + [str(ir)], 0)
    check([binary, str(ir)], 0)
    check(command + ["--version"], 0)
    ir.write_text("invalid IR\n")
    check([binary, str(ir)], 6)
