// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: unwinding assertion loop
// Each longjmp back to the setjmp is a loop iteration, bounded by --unwind.
#include <setjmp.h>

jmp_buf b;

void f(void) {
  longjmp(b, 1);
}

int main(void) {
  volatile int n = 0;
  setjmp(b);
  if (n < 3) {
    n++;
    f();
  }
  return 0;
}
