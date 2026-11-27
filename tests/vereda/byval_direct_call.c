// Direct, indirect, and recursive calls copy byval struct arguments into
// callee-owned objects, preserving caller state, provenance, and alignment.
// RUN: -m64
// RUN: -m32
// RUN: -m64 --ub-uninit-check
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <assert.h>
#include <stdint.h>

struct Big {
  long long a;
  long long b;
  long long c;
  int *p;
};

struct __attribute__((aligned(32))) AlignedBig {
  long v[4];
};

static void mutate(struct Big s, const struct Big *orig) {
  assert(&s != orig);
  assert(s.a == 1 && s.b == 2 && s.c == 3);
  assert(*s.p == 10);
  *s.p = 20;
  s.a = 99;
  s.b = 98;
  s.c = 97;
  s.p = 0;
  assert(s.a == 99 && s.b == 98 && s.c == 97 && s.p == 0);
}

static long recurse(struct Big s, int n) {
  if (n == 0)
    return s.a;
  s.a += 10;
  long sub = recurse(s, n - 1);
  assert(s.a == sub - 10 * (n - 1));
  return sub;
}

static void check_aligned(struct AlignedBig s) {
  assert(((uintptr_t)&s & 31) == 0);
  s.v[0] = 42;
  assert(s.v[0] == 42);
}

int main(void) {
  int target = 10;
  struct Big orig = {1, 2, 3, &target};

  mutate(orig, &orig);
  assert(orig.a == 1 && orig.b == 2 && orig.c == 3);
  assert(orig.p == &target && *orig.p == 20);

  target = 10;
  void (*fp)(struct Big, const struct Big *) = mutate;
  fp(orig, &orig);
  assert(orig.a == 1 && orig.b == 2 && orig.c == 3);
  assert(orig.p == &target && *orig.p == 20);

  long r = recurse(orig, 3);
  assert(r == 31);
  assert(orig.a == 1);

  struct AlignedBig al = {{1, 2, 3, 4}};
  check_aligned(al);
  assert(al.v[0] == 1);
  return 0;
}
