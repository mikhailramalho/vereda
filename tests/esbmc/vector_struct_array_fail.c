/* An array of structs holding a vector, written at a symbolic index. */
#include <assert.h>
typedef int v4i __attribute__((vector_size(16)));
struct S
{
  v4i v;
  int k;
};
int __VERIFIER_nondet_int();
int main()
{
  struct S a[2];
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 2);
  a[i].v = (v4i){1, 2, 3, 4};
  assert(a[i].v[2] == 4);
  return 0;
}
// Imported from ESBMC regression/esbmc/vector_struct_array_fail
// EXPECT: ^VERIFICATION FAILED$
