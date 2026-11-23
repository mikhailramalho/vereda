// #9906: b was never set; its zero token must not resume at setjmp(c)
#include <setjmp.h>
jmp_buf b, c;
int main(void)
{
  longjmp(b, 1);
  setjmp(c);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9906_unset_buf
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: longjmp to a jmp_buf whose setjmp is not active
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
