// EXPECT: ^VERIFICATION SUCCESSFUL$
// A longjmp in a function main never calls costs nothing: its jmp_buf was
// never set, but no check is reached.
#include <setjmp.h>
extern void __VERIFIER_assert(int);
static jmp_buf env;

void never_called(void) { longjmp(env, 1); }

int main(void) {
  int x = 1;
  __VERIFIER_assert(x == 1);
  return 0;
}
