#include <assert.h>

struct S
{
  int *p;
  char buf[8];
};

unsigned __VERIFIER_nondet_uint();
int *__VERIFIER_nondet_ptr();

// A byte written into buf at a symbolic index leaves the bytes of p alone, so
// p reads back as the pointer stored there, whatever it points to.
int main()
{
  int *q = __VERIFIER_nondet_ptr();
  struct S s;
  s.p = q;
  unsigned char *bytes = (unsigned char *)&s;
  bytes[sizeof(int *) + __VERIFIER_nondet_uint() % 8] = 'a';
  assert(s.p == q);
}
// Imported from ESBMC regression/esbmc/pointer_repr_symbolic_byte_keep
// EXPECT: ^VERIFICATION SUCCESSFUL$
