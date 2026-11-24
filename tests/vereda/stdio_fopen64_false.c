// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
// fgets through a stream from fopen64 (fopen under _FILE_OFFSET_BITS=64)
// may write the buffer.
#define _FILE_OFFSET_BITS 64
#include <stdio.h>
extern void __VERIFIER_assert(int);
int main(void) {
  char b[8] = {0};
  FILE *f = fopen("a", "r");
  if (f)
    fgets(b, 8, f);
  __VERIFIER_assert(b[0] == 0);
  return 0;
}
