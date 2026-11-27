#include <stdio.h>

// A width-less wide-string conversion (%ls, and the XSI/glibc %S alias) reads an
// unbounded run of bytes into the caller's buffer, exactly like %s, so it must
// still be reported as a buffer overflow. Locks in the soundness edge of the
// #1470 follow-up fix: only %lc / %C (single wide char) are bounded, not %ls.
int main(void)
{
  char buf[4];
  scanf("%ls", buf);
  return 0;
}
// Imported from ESBMC regression/cstd/scanf_wide_string_specifier_overflow_bug
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check -m64 --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
