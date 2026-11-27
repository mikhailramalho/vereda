#include <assert.h>

unsigned int calc_size(unsigned int size)
{
  return size*2;
}

void VLA_size(unsigned int size)
{
  __VERIFIER_assume(size == 100);
  int arr[size];
  arr[99] = 0; // Works
  size--;
  arr[99] = 0; // Doesn't work
}

int main()
{
  unsigned int foo = __VERIFIER_nondet_uint();
  __VERIFIER_assume(foo > 0);
  __VERIFIER_assume(foo < 100000);
  VLA_size(foo);
}
// Imported from ESBMC regression/esbmc/array_sideeffect1
// EXPECT: ^VERIFICATION SUCCESSFUL$
