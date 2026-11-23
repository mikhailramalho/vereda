// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^Violated property:\n  file \S*longjmp_zero_false\.c line 13 column 3 function main\n  __VERIFIER_assert$
// longjmp(b, 0) resumes setjmp with 1, so the assertion is reached.
#include <setjmp.h>
extern void __VERIFIER_assert(int);

jmp_buf b;

int main(void) {
  int r = setjmp(b);
  if (r == 0)
    longjmp(b, 0);
  __VERIFIER_assert(r != 1);
  return 0;
}
