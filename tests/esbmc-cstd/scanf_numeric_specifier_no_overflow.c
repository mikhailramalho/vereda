#include <stdio.h>

// A width-less numeric scanf conversion (%ld, %d) writes a single scalar and
// cannot overflow a buffer; it must NOT trip the "buffer overflow on scanf"
// unlimited-field check. (esbmc/esbmc#1470 scanf "%ld" false-alarm follow-up.)
int main(void)
{
  long data;
  int x;
  scanf("%ld", &data);
  scanf("%d", &x);
  return 0;
}
// Imported from ESBMC regression/cstd/scanf_numeric_specifier_no_overflow
// RUN: --overflow-check -m64
// EXPECT: ^VERIFICATION SUCCESSFUL$
