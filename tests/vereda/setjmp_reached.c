// RUN: --property-file %S/properties/unreach-call.prp
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^TRUE$
// Walked as bodyless calls, setjmp returned a nondet value and longjmp
// returned normally, so this program, which never calls reach_error, was
// reported FALSE (#10). The longjmp now resumes the setjmp with 1 (#11).
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
