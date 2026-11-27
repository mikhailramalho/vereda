// #10383: VLAs of structs and of pointers with an empty initializer.
int __VERIFIER_nondet_int(void);
struct s
{
  int x;
  char *p;
};
int main(void)
{
  int n = __VERIFIER_nondet_int();
  __VERIFIER_assume(n > 0 && n < 4);
  struct s a[n] = {};
  int *b[n] = {};
  __VERIFIER_assert(a[n - 1].x == 0 && a[n - 1].p == 0); // struct zero
  __VERIFIER_assert(b[n - 1] == 0); // pointer null
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10383_struct
// RUN: --std c2x --solver z3 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
