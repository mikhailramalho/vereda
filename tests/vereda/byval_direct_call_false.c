// Callee mutations to a byval struct parameter must not be visible in caller.
// RUN: -m64
// RUN: -m32
// EXPECT: ^VERIFICATION FAILED$
#include <assert.h>

struct Big {
  long long a;
  long long b;
  long long c;
  long long d;
};

static void mutate(struct Big s) {
  s.a = 42;
}

int main(void) {
  struct Big orig = {1, 2, 3, 4};
  mutate(orig);
  assert(orig.a == 42);
  return 0;
}
