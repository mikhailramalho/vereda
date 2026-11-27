// fclose flushes into the buffer of an fmemopen stream, so a byte the
// program wrote there before the close may have changed: this fails only
// because fclose calls fflush on a stream that keeps buf.
// EXPECT: ^  FAILED       \[main\.assertion\.1\]  line 15  __VERIFIER_assert$
// EXPECT: ^VERIFICATION FAILED$
#include <stdio.h>
extern void __VERIFIER_assert(int);
int main(void) {
  char buf[8];
  FILE *m = fmemopen(buf, sizeof buf, "w");
  if (!m)
    return 0;
  buf[0] = 'y';
  fclose(m);
  __VERIFIER_assert(buf[0] == 'y');
  return 0;
}