// RUN: --dump-smt-only
// EXPECT-NOT: memory#
// EXPECT-NOT: havoc#
// Reading a bounded pointer array inside a mixed-field struct where every
// in-range entry holds the same pointer folds directly without flushing the
// struct or flattening globals into memory# (#279).
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

struct target { int value; };
struct container {
  unsigned char tag[4];
  struct target *ptrs[4];
};

static struct target g = { 42 };

int main(void) {
  struct container c;
  for (int i = 0; i < 4; ++i) {
    c.tag[i] = 0;
    c.ptrs[i] = &g;
  }
  unsigned idx = __VERIFIER_nondet_uint();
  __VERIFIER_assume(idx < 4);
  __VERIFIER_assert(c.ptrs[idx]->value == 42);
  return 0;
}
