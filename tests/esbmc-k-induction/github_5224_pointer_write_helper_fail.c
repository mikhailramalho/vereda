// Issue #5224: the helper writes a[3] through a pointer only in iteration 20.
// Unhavoced, a[3] would keep its initial 0 in the inductive step, which would
// prove the assertion; with a havoced the base case finds the violation.
extern unsigned char __VERIFIER_nondet_uchar(void);
extern int __VERIFIER_nondet_int(void);

static void setelem(unsigned char (*dest)[8], unsigned i)
{
  (*dest)[i] = __VERIFIER_nondet_uchar() | 1u; // array element written through a pointer
}

int main(void)
{
  unsigned char a[8] = {0};
  unsigned n = 0;

  while (__VERIFIER_nondet_int())
  {
    if (n == 20)
      setelem(&a, 3);
    n++;
  }
  __VERIFIER_assert(a[3] == 0); // a[3] is written in iteration 20
  return 0;
}
// Imported from ESBMC regression/k-induction/github_5224_pointer_write_helper_fail
// Needs: --k-induction --max-inductive-step 3
// RUN: --k-induction --max-inductive-step 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
