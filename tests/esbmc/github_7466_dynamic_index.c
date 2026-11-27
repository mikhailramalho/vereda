#include <assert.h>
#include <stddef.h>
struct S
{
  int a;
  int arr[4];
  struct
  {
    char x;
    int y[3];
  } in[2];
};
int __VERIFIER_nondet_int(void);
int main(void)
{
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 4);
  size_t o = offsetof(struct S, arr[i]);
  assert(o == 4 + 4 * (size_t)i);
  int j = __VERIFIER_nondet_int();
  __VERIFIER_assume(j >= 0 && j < 2);
  int k = __VERIFIER_nondet_int();
  __VERIFIER_assume(k >= 0 && k < 3);
  size_t o2 = offsetof(struct S, in[j].y[k]);
  assert(o2 == 20 + 16 * (size_t)j + 4 + 4 * (size_t)k);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7466_dynamic_index
// EXPECT: ^VERIFICATION SUCCESSFUL$
