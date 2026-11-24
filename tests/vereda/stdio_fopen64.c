// RUN: --unwind 2
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Under _FILE_OFFSET_BITS=64 glibc's <stdio.h> renames fopen, tmpfile and
// freopen to fopen64, tmpfile64 and freopen64, modelled as the plain ones.
#define _FILE_OFFSET_BITS 64
#include <stdio.h>
extern void __VERIFIER_assert(int);
int main(void) {
  FILE *in = fopen("a", "r"), *t = tmpfile();
  if (!in || !t)
    return 1;
  __VERIFIER_assert(in != t);
  FILE *r = freopen("b", "w", in);
  __VERIFIER_assert(!r || r == in);
  return 0;
}
