// EXPECT: ^VERIFICATION SUCCESSFUL$
// Only a reached setjmp or longjmp gives UNKNOWN (#10): one in a function
// main never calls does not cost the verdict.
#include <setjmp.h>
extern void __VERIFIER_assert(int);
static jmp_buf env;

void never_called(void) { longjmp(env, 1); }

int main(void) {
  int x = 1;
  __VERIFIER_assert(x == 1);
  return 0;
}
