#include <assert.h>

struct S
{
  int *p;
  char buf[8];
};

unsigned __VERIFIER_nondet_uint();
int *__VERIFIER_nondet_ptr();

// A byte written at a symbolic index may land in p, after which p is no longer
// the pointer stored there.
int main()
{
  int *q = __VERIFIER_nondet_ptr();
  struct S s;
  s.p = q;
  unsigned char *bytes = (unsigned char *)&s;
  bytes[__VERIFIER_nondet_uint() % sizeof(s)] = 0xff;
  assert(s.p == q);
}
// Imported from ESBMC regression/esbmc/pointer_repr_symbolic_byte_keep_fail
// EXPECT: ^VERIFICATION FAILED$
