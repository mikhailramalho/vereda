#include <stdbool.h>

int
main()
{
  int bees, fleas;
  unsigned int foo = (unsigned int*)&bees;
  unsigned int bar = (unsigned int*)&fleas;

  bool baz = __VERIFIER_nondet_bool();
  __VERIFIER_assume(baz == true);
  unsigned int qux = (baz) ? foo : bar;
  int *fin = (int *)qux;
  assert (fin == &bees);
  *fin = 0;

  assert(bees == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/38_nondet_int_ptr_casts
// RUN: -m32
// EXPECT: ^VERIFICATION SUCCESSFUL$
