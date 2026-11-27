#!/usr/bin/env python3
"""Running out of memory anywhere ends the run with "vereda: out of memory" and exit 6
(#384). Bitwuzla allocates its nodes with calloc and throws std::bad_alloc itself when
that fails, which the new-handler never sees: before the fix, some of these limits ended
with "terminate called after throwing an instance of 'std::bad_alloc'"."""
from pathlib import Path
import resource
import subprocess
import sys
import tempfile

frontend, binary, clang = sys.argv[1:]

# The walk needs about 400 MB, so each limit below runs out during it.
PROGRAM = """extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assert(int);
int main(void) {
  unsigned a[64], s = 0;
  for (int i = 0; i < 64; i++)
    a[i] = __VERIFIER_nondet_uint();
  for (unsigned n = 0; n < 100000; n++)
    s = s * 31 + a[(s ^ n) % 64];
  __VERIFIER_assert(s != 7);
  return 0;
}
"""

with tempfile.TemporaryDirectory() as tmp:
    source = Path(tmp) / "big.c"
    source.write_text(PROGRAM)
    subprocess.run([sys.executable, frontend, "--clang", clang, "--keep-ir", "--dump-ir-only",
                    str(source)], check=True, capture_output=True)
    for mb in range(200, 370, 10):
        limit = mb << 20
        run = subprocess.run([binary, f"{source}.ll", "--no-summarize-array-loops"],
                             capture_output=True, text=True, timeout=60,
                             preexec_fn=lambda: resource.setrlimit(resource.RLIMIT_AS,
                                                                   (limit, limit)))
        assert (run.returncode == 6 and "vereda: out of memory" in run.stderr
                and "terminate called" not in run.stderr), (mb, run.returncode, run.stderr[-300:])
