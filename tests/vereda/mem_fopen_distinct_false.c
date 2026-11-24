// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
// Each stream fopen or tmpfile opens is an object of its own.
#include <stdio.h>
extern void __VERIFIER_assert(int);
int main(void) {
  FILE *in = fopen("a", "r"), *out = fopen("b", "w"), *t = tmpfile();
  if (!in || !out || !t)
    return 1;
  if (in == out || in == t || out == t)
    return 2;
  __VERIFIER_assert(0);
  return 0;
}
