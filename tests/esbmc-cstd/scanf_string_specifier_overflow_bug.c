#include <stdio.h>

// A width-less string conversion (%s) reads an unbounded run of bytes into the
// caller's buffer; this must still be reported as a buffer overflow. Guards
// against the #1470 follow-up fix over-suppressing genuine string overflows.
int main(void)
{
  char buf[4];
  scanf("%s", buf);
  return 0;
}
// Imported from ESBMC regression/cstd/scanf_string_specifier_overflow_bug
// RUN: --overflow-check -m64
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
