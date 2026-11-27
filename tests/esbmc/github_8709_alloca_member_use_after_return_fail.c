#include <stddef.h>
void *alloca(size_t size);
struct s
{
  void *m;
};
void *f(void)
{
  struct s v;
  v.m = alloca(sizeof(int));
  *(int *)v.m = 1;
  return v.m;
}
int main(void)
{
  int *p = f();
  return *p;
}
// Imported from ESBMC regression/esbmc/github_8709_alloca_member_use_after_return_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\s*FAILED\s+\[main\.invalidated-dynamic-object\.1\]\s+line 17\s+dereference failure: invalidated dynamic object$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
