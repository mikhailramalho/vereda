#!/usr/bin/env python3
"""Verify that Vereda's prepared IR passes LLVM's IR verifier."""
from pathlib import Path
import subprocess
import sys
import tempfile

binary, opt, *fixtures = sys.argv[1:]

for fixture in fixtures:
    with tempfile.TemporaryDirectory() as directory:
        dump = subprocess.run(
            [binary, "--dump-ir-only", "--dump-dir", directory, fixture],
            capture_output=True, text=True, timeout=30,
        )
        assert dump.returncode == 0, dump.stdout + dump.stderr
        ir = Path(directory) / "ir.ll"
        text = ir.read_text()
        assert "%res = phi i32" in text, text
        verify = subprocess.run(
            [opt, "-passes=verify", "-disable-output", str(ir)],
            capture_output=True, text=True, timeout=30,
        )
        assert verify.returncode == 0, verify.stdout + verify.stderr
