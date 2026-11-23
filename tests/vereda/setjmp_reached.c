// RUN: --property-file %S/properties/unreach-call.prp --witness -
// EXPECT: ^VERIFICATION UNKNOWN
// EXPECT: call to _?setjmp at
// EXPECT: ^UNKNOWN$
// EXPECT-NOT: VERIFICATION FAILED
// EXPECT-NOT: entry_type
// setjmp and longjmp have no model yet (#11). Walked as bodyless calls,
// setjmp returned a nondet value and longjmp returned normally, so this
// program, which never calls reach_error, was reported FALSE (#10). A reached
// call now gives UNKNOWN, with no witness.
#include <setjmp.h>
extern void reach_error(void);
static jmp_buf env;

int main(void) {
  int r = setjmp(env);
  if (r == 0)
    longjmp(env, 1);
  if (r != 1)
    reach_error();
  return 0;
}
