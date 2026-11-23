// #10135: the issue's reproducer; p.f can be 12345 after p.f &= m.
#include <assert.h>

extern unsigned long __VERIFIER_nondet_ulong(void);

struct s
{
  unsigned long f : 52;
};

int main(void)
{
  struct s p;
  p.f = __VERIFIER_nondet_ulong();
  unsigned long m = __VERIFIER_nondet_ulong();
  p.f &= m;
  assert(p.f != 12345);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10135_irep2_only_fail
// Needs: --clang-c-irep2-adjust-only
// RUN: --64 --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
