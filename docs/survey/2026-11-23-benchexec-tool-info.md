# BenchExec tool-info

Recorded 2026-11-23.

`contrib/benchexec/vereda.py` is the module BenchExec needs to run Vereda
in SV-COMP; it goes into `benchexec/tools/` there. It runs vereda.py with
the task's property file, `-m32` or `-m64` from its data model and every
input file, reads the version from `vereda.py --version` and the verdict
from the last `TRUE`, `FALSE_REACH`, `FALSE_OVERFLOW` or `UNKNOWN` line;
no such line is an error. Checked with `python3 -m benchexec.test_tool_info`
(BenchExec 3.20) and on the output of the property tests. Left for the
release archive: clang next to the binary, and the program files list.
