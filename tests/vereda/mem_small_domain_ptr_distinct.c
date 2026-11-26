// EXPECT: ^VERIFICATION SUCCESSFUL$
// Reading a small bounded pointer array of distinct known objects preserves
// their known object IDs via an ite chain (#279).
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

struct container {
  unsigned char pad[4];
  int *ptrs[4];
};

int main(void) {
  int a = 10, b = 20, c = 30, d = 40;
  struct container box;
  box.pad[0] = 1;
  box.ptrs[0] = &a;
  box.ptrs[1] = &b;
  box.ptrs[2] = &c;
  box.ptrs[3] = &d;
  unsigned idx = __VERIFIER_nondet_uint();
  __VERIFIER_assume(idx < 4);
  int v = *box.ptrs[idx];
  __VERIFIER_assert(v == (int)((idx + 1) * 10));
  return 0;
}
