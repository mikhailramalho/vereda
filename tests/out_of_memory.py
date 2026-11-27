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

# The walk needs about 400 MB, so the limits below run out during it; a run
# that fits (if the walk ever needs less) must still end normally.
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
    out_of_memory = 0
    for mb in range(200, 370, 10):
        limit = mb << 20
        run = subprocess.run([binary, f"{source}.ll", "--no-summarize-array-loops",
                              "--no-incremental-bmc"],
                             capture_output=True, text=True, timeout=60, check=False,
                             preexec_fn=lambda limit=limit: resource.setrlimit(
                                 resource.RLIMIT_AS, (limit, limit)))
        failure = (mb, run.returncode, run.stderr[-300:])
        assert "terminate called" not in run.stderr, failure
        if run.returncode == 6:
            assert "vereda: out of memory" in run.stderr, failure
            out_of_memory += 1
        else:
            assert run.returncode in (0, 1, 2), failure
    assert out_of_memory, "no limit ran out of memory: raise the walk's size"
