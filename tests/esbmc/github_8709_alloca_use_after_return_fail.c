#include <stddef.h>
void *alloca(size_t size);
int *f(void)
{
  int *p = alloca(sizeof(int));
  *p = 1;
  return p;
}
int main(void)
{
  int *p = f();
  return *p;
}
// Imported from ESBMC regression/esbmc/github_8709_alloca_use_after_return_fail
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\s*FAILED\s+\[main\.invalidated-dynamic-object\.1\]\s+line 12\s+dereference failure: invalidated dynamic object$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
