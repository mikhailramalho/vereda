// EXPECT: ^VERIFICATION SUCCESSFUL$
// A jmp_buf filled by an earlier run of the same setjmp in the same frame
// still resumes it, though the setjmp ran again since with another buffer.
#include <setjmp.h>
extern void __VERIFIER_assert(int);

jmp_buf b[2];

int main(void) {
  int r = 0;
  for (int i = 0; i < 2; i++) {
    r = setjmp(b[i]);
    if (r)
      break;
    if (i == 1)
      longjmp(b[0], 3);
  }
  __VERIFIER_assert(r == 3);
  return 0;
}
