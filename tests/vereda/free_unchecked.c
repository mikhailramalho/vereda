// Without ESBMC's pointer check, or when only reach_error counts, an
// invalid free is not reported, and it does not cut the path short: the
// reach_error after it is still found.
// RUN: --no-pointer-check
// RUN: --property unreach-call
// EXPECT-NOT: non-dynamic
// EXPECT: ^  reach_error$
// EXPECT: ^VERIFICATION FAILED$
#include <stdlib.h>

void reach_error(void);

int main(void) {
  int x;
  free(&x);
  reach_error();
  return 0;
}
