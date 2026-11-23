#include <assert.h>

struct D {
  int dummy;
  char fmt[16];
};

int main()
{
  unsigned n = __VERIFIER_nondet_uint() % 1024;
  struct D data[n];
  assert(data->fmt);
}
// Imported from ESBMC regression/esbmc/github_1173
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
