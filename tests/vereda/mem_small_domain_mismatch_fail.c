// EXPECT: ^VERIFICATION FAILED$
// Small-domain load folding must not over-fold when one element in the bounded
// range differs from the others (#279).
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

struct container {
  unsigned char pad[4];
  int vals[4];
};

int main(void) {
  struct container box;
  box.pad[0] = 1;
  box.vals[0] = 7;
  box.vals[1] = 7;
  box.vals[2] = 9;
  box.vals[3] = 7;
  unsigned idx = __VERIFIER_nondet_uint();
  __VERIFIER_assume(idx < 4);
  __VERIFIER_assert(box.vals[idx] == 7);
  return 0;
}
