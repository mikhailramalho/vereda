// A struct holding a pointer, passed and returned by value: clang returns it
// as {ptr, i32} on x86-64 and through a hidden pointer on i386 (issue #17).
// The pointer keeps its object across both calls, so the deref reads x.
// RUN: -m64 --unwind 5
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);

struct s {
  int *p;
  int n;
};

static struct s make(int *p, int n) {
  struct s r = {p, n};
  return r;
}

static struct s bump(struct s v) {
  *v.p += v.n;
  v.n++;
  return v;
}

int main(void) {
  int x = 1;
  struct s v = bump(make(&x, 2));
#ifdef FAIL
  __VERIFIER_assert(*v.p == 4);
#else
  __VERIFIER_assert(*v.p == 3 && v.n == 3 && x == 3);
#endif
  return 0;
}
