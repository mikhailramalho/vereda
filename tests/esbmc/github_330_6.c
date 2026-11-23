void g(void *a, unsigned n) {
  free(a); // unsafe
}

void f (unsigned n)
{
  void *a [8];
  for (int i = 0; i != 8; ++i)
    a [i] = __builtin_alloca (n);

  g(a, n);
}

unsigned __VERIFIER_nondet_uint();
int main() {
  f(__VERIFIER_nondet_uint());
  return 0;
}
// Imported from ESBMC regression/esbmc/github_330_6
// Needs: --memory-leak-check
// RUN: --memory-leak-check -Wno-error=implicit-function-declaration
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
