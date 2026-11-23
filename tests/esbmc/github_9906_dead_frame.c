// #9906: f has returned, so its setjmp is no longer active
#include <assert.h>
#include <setjmp.h>
jmp_buf b;
void f(void)
{
  setjmp(b);
}
int main(void)
{
  f();
  longjmp(b, 1);
  assert(0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9906_dead_frame
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: longjmp to a jmp_buf whose setjmp is not active
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
