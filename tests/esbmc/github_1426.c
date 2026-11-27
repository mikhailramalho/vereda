// based on esbmc/github_732-2-2

typedef union {
  int x[12000];
} t1;

typedef struct {
  int x;
  t1 y;
} t2;

t2 __VERIFIER_nondet_t2();

int main()
{
  t2 a = __VERIFIER_nondet_t2();
  int *b = &a.x;
  b[1] = a.x;
  assert(*b);
}
// Imported from ESBMC regression/esbmc/github_1426
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
