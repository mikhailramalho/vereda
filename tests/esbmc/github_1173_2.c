#include <assert.h>

struct D {
  int dummy;
  char fmt[16];
};

int main()
{
  unsigned n = __VERIFIER_nondet_uint() % 1024 + 1;
  struct D data[n];
  data->dummy = 42;
  assert(data->dummy == 42);
}
// Imported from ESBMC regression/esbmc/github_1173_2
// EXPECT: ^VERIFICATION SUCCESSFUL$
